#include "base.hpp"
 
//////////////////////  vec_of_ident  ///////////////////////

/* 
  Вектор, использующийся для хранения переменных 
  insert возвращает место переменной в векторе, если она там есть и добавляет ее в противном случае
  для функции find был перегружен оператор == в классе Ident
 */
 
vector<Ident> vec_of_ident;
 
int insert(const string& buffer) {
  vector<Ident>::iterator it;
  if ((it = find(vec_of_ident.begin(), vec_of_ident.end(), buffer)) != vec_of_ident.end())
    return it - vec_of_ident.begin();
  vec_of_ident.push_back(Ident(buffer));
  return vec_of_ident.size() - 1;
}
 
/////////////////////////////////////////////////////////////////

/* 
  данный класс позволяет нам парсить файл на лексемы 
  метод check используется для поиска полученной лексемы в векторе служебных слов (команд) CMD
  или в векторе специальных символом SYMB, возвращается позиция в векторе, либо ноль в противном случае
  (точка проверяется иначе, поэтому стоит на нулевой позиции в массиве SYMB)
  метод get_lexem() возвращает текущую лексему
 */
 
class Scanner {
  FILE *fp;
  char   c;
  int check(const string buffer, const char **list) {
    int i = 0;
    while (list[i]) {
      if (buffer == list[i])
        return i;
      ++i;
    }
    return 0;
  }
  void gc() {
    c = fgetc(fp);
  }
public:
  static const char *CMD[], *SYMB[];
  Scanner(const char * program) {
    fp = fopen(program, "r");
  }
  Lex get_lexem();
};
 
const char *Scanner::CMD[] = {"", "and", "begin", "bool", "do", "else", "end", "if", "false", "int", 
                              "not", "or", "program", "get", "then", "true", "var", "while", "print", "string", NULL};
 
const char *Scanner::SYMB[] = {".", ";", ",", ":", ":=", "(", ")", 
                               "=", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=", NULL};
 
/* 
  для парсинга используется L-граф
 */

Lex Scanner::get_lexem() {
  enum    state {START, IDENT, NUMBER, COMMENT, ALE, NEQ, STR};
  int     num, pos;
  string  buf = "";
  state   STATE = START;
  do {
    gc();
    switch (STATE) {
      case START:
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t'); // игнорируем пустые символы
        else if (isalpha(c)) {   // в случае символа переходим в состояние ident 
          buf.push_back(c);
          STATE = IDENT;
        } else if (isdigit(c)) { // в случае цифры в состояние number
          num = c - '0';
          STATE = NUMBER;
        } else if (c == ':' || c == '<' || c == '>') { 
          buf.push_back(c);
          STATE = ALE; 
        } else if (c == '!') {
          buf.push_back(c);
          STATE = NEQ;
        } else if (c == '"')       // идентификатор строки
          STATE = STR;
        else if (c == '{') 
          return Lex(LEX_LBRACE);  // идентификатор начала оператора 
        else if (c == '}') 
          return Lex(LEX_RBRACE);  // идентификатор конца оператора
        else if (c == '[') 
          STATE = COMMENT;         // квадратная скобка обозначает начало комментария
        else if (c == '.')
          return Lex(LEX_FIN);     // точка - конец программы
        else {                     // если ничего не подошло, проверяем на спецаильные символы, иначе кидаем исключение 
          buf.push_back(c);
          if ((pos = check (buf, SYMB))) 
            return Lex((type_of_lex)(pos + (int)LEX_FIN), pos);
          else 
            throw c;
        }
        break;

      /* 
        собираем слова, проверяем есть ли оно в служебных, если есть, то возвращаем соответсвенно служебную лексемы
        иначе добавляем в вектор переменных и возвращаем лексему 
       */

      case IDENT:
        if (isalpha(c) || isdigit(c) || c == '_')
          buf.push_back(c); 
        else {
          ungetc(c, fp);
          if ((pos = check(buf, CMD))) {
            if (pos == int(LEX_STRING)) 
              return Lex((type_of_lex)pos, pos, " ");
            return Lex((type_of_lex)pos, pos);
          } else {
            pos = insert(buf);
            if (vec_of_ident[pos].get_type() == LEX_STRING)
              return Lex(LEX_ID, pos, vec_of_ident[pos].get_str_value());
            return Lex(LEX_ID, pos);
          }
        }
        break;

      /* собираем наше целочисленное число и возвращаем лексему */

      case NUMBER:
        if (isdigit(c)) 
          num = num * 10 + (c - '0'); 
        else {
          ungetc(c, fp);
          return Lex(LEX_NUM, num);
        }
        break;

      /* 
        игнорируем все символы до появления ], при появлении символа, отвечающего за начало нового
        комментария или отвечающего за окончание программы, кидаем исключение  
       */

      case COMMENT:
        if (c == ']') 
          STATE = START;
        else if (c == '.' || c == '[')
            throw c;
        break;

      /* возвращаем либо < > : либо <= >= := */

      case ALE:
        if (c == '=') {
          buf.push_back(c);
          pos = check(buf, SYMB);
          return Lex((type_of_lex)(pos + (int)LEX_FIN), pos);
        } else {
          ungetc(c, fp);
          pos = check(buf, SYMB);
          return Lex((type_of_lex)(pos + (int)LEX_FIN), pos);
        }
        break;

      /* если встретили символ окончания строки, возвращаем строку, иначе добавляем в строку очереднйо символ */

      case STR:
        if (c == '"') 
          return Lex(LEX_STR, 0, buf);
        else 
          buf.push_back(c);
        break;

      /* мы должны получить !=, иначе кидаем исключение */

      case NEQ:
        if (c == '=') {
          buf.push_back(c);
          pos = check(buf, SYMB);
          return Lex(LEX_NEQ, pos);
        } else
          throw '!';
        break;
    } //end switch
  } while (true);
}
 
/* перегрузка вывода для отладки кода */

ostream & operator << (ostream &s, Lex l) {
  string str;
  if (l.type_lex <= 19)
    str = Scanner::CMD[l.type_lex];
  else if (l.type_lex >= 20 && l.type_lex <= 36)
    str = Scanner::SYMB[l.type_lex-19];
  else if (l.type_lex == 37)
    str = "NUMB";
  else if (l.type_lex == 38)
    str = vec_of_ident[l.value_lex].get_name();
  else if (l.type_lex == 39)
    str = "Label";
  else if(l.type_lex == 40)
    str = "Addr";
  else if (l.type_lex == 41)
    str = "!";
  else if (l.type_lex == 42) 
    str = "!F";
  else if (l.type_lex == 43) 
    str = "LBRACE";
  else if (l.type_lex == 44) 
    str = "RBRACE";
  else if (l.type_lex == 45) 
    str = "STR";
  else
    throw l;
  s << '(' << l.type_lex << ", " << l.value_lex << ");" << endl;
  return s;
}
 
//////////////////////////  Класс Parser  /////////////////////////////////

/* 
  Главный класс для парсинга, описание методов буду приводить непосредственно перед очередным методом 
  используется шаблонный стек для хранения лексем
  метод gl() позволяет нам получить очередную лексему
  poliz - это наш полиз, который мы будем использовать для выполнения программы
 */

template <class T, class T_EL>
void from_st(T& st, T_EL& i) {
  i = st.top(); 
  st.pop();
}
 
class Parser {
  Lex           curr_lex;
  type_of_lex   cur_type;
  int           cur_value;
  Scanner       scan;
  stack<int>    st_int;
  stack<type_of_lex> st_lex;
  void  Program();
  void  VAR1();
  void  VAR();
  void  BEGIN();
  void  S();
  void  E();
  void  E1();
  void  T();
  void  F();
  void  dec(type_of_lex type);
  void  check_id();
  void  check_op();
  void  check_not();
  void  eq_type();
  void  eq_bool();
  void  check_id_in_get();
  void  gl() {
    curr_lex  = scan.get_lexem();
    cur_type  = curr_lex.get_type();
    cur_value = curr_lex.get_value();
  }
public:
  vector<Lex> poliz;
  Parser(const char *program) : scan(program) {}
  void  analyze();
};
 
/* Начало парсинга, первая лексема должна быть program, проверяем последнюю, если все хорошо, то выходим */

void Parser::analyze() {
  gl();
  Program();
  if (cur_type != LEX_FIN)
    throw curr_lex;
}
 
/* если первое слово program, берем очередную лексему и идем дальше */

void Parser::Program() {
  if (cur_type == LEX_PROGRAM)
    gl();
  else 
    throw curr_lex;      
  VAR1();
  BEGIN();
}
 
/*
   метод VAR1 проверяет наличие служебного слова var и проваливается дальше для получения переменных 
   до появления { - признака начала тела программы 
*/

void Parser::VAR1() {
  if (cur_type == LEX_VAR)   {
    gl();
    while (cur_type != LEX_LBRACE) {
      VAR();
      if (cur_type == LEX_SEMICOLON) // если после объявления встречается ; идем дальше, иначе исключение
        gl();
      else
        throw curr_lex;
    }
  }
  else
    throw curr_lex;
}
 
/* 
  lex_id - в случае неслужебного имени, т.е имени переменной
  добавляем в стек интов номер нашей переменной в векторе vec_of_ident для дальнейшей проверки на уникальность
  добавляем имена переменных, записанных через запятую (COMMA) до появления двоеточия (COLON) 
  далее получаем тип переменных и проверяем их имена на уникальность с помощью метода dec(), иначе исключение
 */

void Parser::VAR() {
  if (cur_type != LEX_ID)
    throw curr_lex;
  else {
    st_int.push(cur_value);
    gl();
    while (cur_type == LEX_COMMA) {
      gl();
      if (cur_type != LEX_ID)
        throw curr_lex;
      else {
        st_int.push(cur_value);
        gl();
      }
    }
    if (cur_type != LEX_COLON)
      throw curr_lex;
    else {
      gl();
      if (cur_type == LEX_INT) {
        dec(LEX_INT);
        gl();
      } else if (cur_type == LEX_BOOL) {
        dec(LEX_BOOL);
        gl();
      } else if (cur_type == LEX_STRING) {
        dec(LEX_STRING);
        gl();
      } else 
          throw curr_lex;
    } 
  }
}

/* 
  начало программы, если встретили { - проваливаемся в метод S(), иначе исключение
  далее, пока встречаем ; (SEMOCOLON) проваливаемся опять в S(), иначе проверяем лексему на окончание }
 */

void Parser::BEGIN() {
  if (cur_type == LEX_LBRACE) {
    gl();
    S();
    while (cur_type == LEX_SEMICOLON) {
      gl();
      if (cur_type != LEX_RBRACE)
        S();
    }
    if (cur_type == LEX_RBRACE)
      gl();
    else
      throw curr_lex;
  } else
    throw curr_lex;
}
 
/* 
  метод S() определяет следующее действие 
  обрабатывает if запрос, while запрос, get или print запрос
  так же выполняет присваивание переменной, если ничего из этого не было встречено, 
  вызывает метод BEGIN() 
 */

void Parser::S() {
  int pl0, pl1, pl2, pl3;
 
/* 
  if - проваливаемся в метод E() для проверки
  eq_bool() проверяет проверяет тип (он должен быть логическим для проверки условия if)
  запоминаем длину ПОЛИЗа, добавляем переход по лжи (FGO), иначе, если встретили { заходим в тело 
  внутри тела запоминаем размер полиза еще раз, добавляем переход по истине
  в место перехода по лжи добавляем данную позицию в ПОЛИЗе
  при встрече else добавляем в место перехода по истине данную позицию полиза
  внутри проводится проверка скобки и корректность операторов  
  аналогично с ПОЛИЗом и вызовом метода S() работает while 
  в случае get() проверяем левую и правую скобки и внутри на переменную
  если объявлена, помещаем в полиз, иначе исключение
  в случае print() все аналогично
  если встречаем переменную, сомтрим, надо ли что-то присвоить, иначе исключение
*/

  if (cur_type == LEX_IF) {
    gl();
    E();
    eq_bool();
    pl2 = poliz.size();
    poliz.push_back(Lex());
    poliz.push_back(Lex(POLIZ_FGO));
    if (cur_type == LEX_LBRACE) {
      gl();
      S();
      pl3 = poliz.size();
      poliz.push_back(Lex());
      poliz.push_back(Lex(POLIZ_GO));
      poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());
      if (cur_type == LEX_SEMICOLON)
        gl();
      if (cur_type != LEX_RBRACE)
        throw curr_lex;
      gl();
      if (cur_type == LEX_ELSE) {
        gl();
        if (cur_type == LEX_LBRACE) {
          gl();
          S();
          poliz[pl3] = Lex(POLIZ_LABEL, poliz.size());
        } else 
          throw curr_lex;
        if (cur_type == LEX_SEMICOLON)
          gl();
        if (cur_type == LEX_RBRACE)
          gl();
        else 
          throw curr_lex;
      } else
        throw curr_lex;
    } else
      throw curr_lex;
  } else if (cur_type == LEX_WHILE) {
      pl0 = poliz.size();
      gl();
      E();
      eq_bool();
      pl1 = poliz.size(); 
      poliz.push_back (Lex());
      poliz.push_back (Lex(POLIZ_FGO));
      if (cur_type == LEX_LBRACE) {
        gl();
        if (cur_type == LEX_ID)
        S();
        poliz.push_back(Lex(POLIZ_LABEL, pl0));
        poliz.push_back(Lex(POLIZ_GO));
        poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
      } else
        throw curr_lex;
      if (cur_type == LEX_SEMICOLON)
        gl();
      if (cur_type == LEX_RBRACE) {
        cur_type = LEX_SEMICOLON;
        gl();
      } else
        throw curr_lex;
  } else if (cur_type == LEX_GET) {
      gl();
      if (cur_type == LEX_LPAREN) {
        gl();
        if (cur_type == LEX_ID) {
          check_id_in_get();
          poliz.push_back(Lex(POLIZ_ADDRESS, cur_value));
          gl();
        } else
          throw curr_lex;
        if (cur_type == LEX_RPAREN) {
          gl();
          poliz.push_back(Lex(LEX_GET));
        } else
          throw curr_lex;
      } else  
        throw curr_lex;
  } else if (cur_type == LEX_PRINT) {
    gl();
    if (cur_type == LEX_LPAREN) {
      gl();
      E();
      if (cur_type == LEX_RPAREN) {
        gl();
        poliz.push_back(Lex(LEX_PRINT));
      } else
        throw curr_lex;
    } else
      throw curr_lex;
  } else if (cur_type == LEX_ID) { 
    check_id();
    if (vec_of_ident[cur_value].get_type() == LEX_STRING) 
      poliz.push_back(Lex(LEX_STRING, cur_value, vec_of_ident[cur_value].get_str_value()));
    else
      poliz.push_back(Lex(POLIZ_ADDRESS, cur_value));
    gl();
    if (cur_type == LEX_ASSIGN) {
      gl();
      E();
      eq_type();
      poliz.push_back(Lex(LEX_ASSIGN));
    } else
      throw curr_lex;
  } else
    BEGIN();
}
 
/* 
  в данном методе проверяем операнд = < > <= >= !=, складываем в стек лексем для дальнейших проверок
 */

void Parser::E() {
  E1();
  if (cur_type == LEX_EQ || cur_type == LEX_LSS || cur_type == LEX_GTR ||
      cur_type == LEX_LEQ || cur_type == LEX_GEQ || cur_type == LEX_NEQ) {
    st_lex.push(cur_type);
    gl(); 
    E1(); 
    check_op();
  }
}
 
/* 
  аналогично предыдущему методу проверяем операнд на + - OR, скалдывая в стек
 */

void Parser::E1() {
  T();
  while (cur_type == LEX_PLUS || cur_type == LEX_MINUS || cur_type == LEX_OR) {
    st_lex.push (cur_type);
    gl();
    T();
    check_op();
  }
}
 
/* 
  аналогично проверяем на * / AND и складываем в стек 
 */

void Parser::T() {
  F();
  while (cur_type == LEX_TIMES || cur_type == LEX_SLASH || cur_type == LEX_AND) {
    st_lex.push (cur_type);
    gl();
    F();
    check_op();
  }
}
 
/* 
  метод смотрит на текущую лексему 
  если переменная - проверяем, присвоено ли какое-нибудь значение ей с помощью check_id(), если да, помещаем в ПОЛИЗ
  иначе исключение
  если это число или строка - помещаем в стек лексем нужную лексему и добавляем значение в ПОЛИЗ
  аналогично с TRUE и FALSE
  если встретили NOT, вызываем в очередной раз F() и проверяем, для какой лексемы мы применяем отрицание
  если для булевской, то все хорошо, иначе исключение
  при встрече левой скобки проваливаемся в E() вычисляя выражение и проверяем наличие правой закрывающей скобки
 */

void Parser::F() {
  if (cur_type == LEX_ID) {
    check_id();
    poliz.push_back(Lex(LEX_ID, cur_value, vec_of_ident[cur_value].get_str_value()));
    gl();
  } else if (cur_type == LEX_NUM) {
    st_lex.push(LEX_INT);
    poliz.push_back(curr_lex);
    gl();
  } else if (cur_type == LEX_STR) {
    st_lex.push(LEX_STR);
    poliz.push_back(curr_lex);
    gl();
  } else if (cur_type == LEX_TRUE) {
    st_lex.push(LEX_BOOL);
    poliz.push_back(Lex(LEX_TRUE, 1));
    gl();
  } else if (cur_type == LEX_FALSE) {
    st_lex.push(LEX_BOOL);
    poliz.push_back(Lex(LEX_FALSE, 0));
    gl();
  } else if (cur_type == LEX_NOT) {
    gl(); 
    F(); 
    check_not();
  } else if (cur_type == LEX_LPAREN) {
    gl(); 
    E();
    if (cur_type == LEX_RPAREN)
      gl();
    else 
      throw curr_lex;
  } else 
    throw curr_lex;
}
 
////////////////////////////////////////////////////////////////
 
/* 
  метод, проверяющий уникальность имен переменныых
 */

void Parser::dec(type_of_lex type) {
  int i;
  while (!st_int.empty()) {
    from_st(st_int, i);
    if (vec_of_ident[i].get_declare()) {
      cout << "name: " << vec_of_ident[i].get_name() << endl;
      throw "twice";
    }
    else {
      vec_of_ident[i].put_declare();
      vec_of_ident[i].put_type(type);
    }
  }
}
 
/* 
  проверяем, присвоено ли какое-нибудь значение данной переменной
 */

void Parser::check_id() {
  if (vec_of_ident[cur_value].get_declare()) 
    st_lex.push(vec_of_ident[cur_value].get_type());
  else 
    throw "not declared";
}
 
/* 
  проверяем корректность операндов 
  для строк применимы толко +, < > <= >= !=
  если все хорошо - добавляем в стек лексем соотвествующую лексему и в полиз соотвествующий операнд
  иначе исключение
 */

void Parser::check_op() {
  type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;
 
  from_st(st_lex, t2);
  from_st(st_lex, op);
  from_st(st_lex, t1);
  if ((op == LEX_PLUS) && (((t2 == LEX_STR) || (t1 == LEX_STRING)))) {
    if ((t2 == t1) || ((t2 == LEX_STR) && (t1 == LEX_STRING))) {
      st_lex.push(LEX_STRING);
      poliz.push_back(Lex(op));
      return;
    } else 
      throw "wrong types are in operation0";
  }
  if ((t2 == LEX_STR) || (t1 == LEX_STRING) || (t1 == LEX_STR)) 
    if (op == LEX_EQ || op == LEX_LSS || op == LEX_GTR ||
              op == LEX_LEQ || op == LEX_GEQ || op == LEX_NEQ) {
      if ((t2 == t1) || ((t2 == LEX_STR) && (t1 == LEX_STRING)) || ((t2 == LEX_STRING) && (t1 == LEX_STR))) {
        st_lex.push(LEX_BOOL);
        poliz.push_back(Lex(op));
      } else 
        throw "wrong types are in operation1";
      return;
    }
  if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH)
    r = LEX_INT;
  if (op == LEX_OR || op == LEX_AND)
    t = LEX_BOOL;
  if (t1 == t2  &&  t1 == t) 
    st_lex.push(r);
  else
    throw "wrong types are in operation2";
  poliz.push_back(Lex(op));
}
 
/* 
  если NOT стоит не перед булевским выражением - кидает отрицание
 */

void Parser::check_not() {
  if (st_lex.top() != LEX_BOOL)
    throw "wrong type is in not";
  else  
    poliz.push_back(Lex(LEX_NOT));
}
 
/* 
  метод проверяет на эквивалентность двух типов, лежащих в стеке лексем, к которым будут применены операнды
 */

void Parser::eq_type() {
  type_of_lex t;
  from_st(st_lex, t);
  if ((t == LEX_STR) && ((st_lex.top() == LEX_STRING) || (st_lex.top() == LEX_STR)))
    st_lex.pop();
  else {
    if (t != st_lex.top())
      throw "wrong types are in :=";
    st_lex.pop();
  }
}
 
/* 
  проверяет, является ли выражение булевским 
 */

void Parser::eq_bool() {
  if (st_lex.top() != LEX_BOOL)
    throw "expression is not boolean";
  st_lex.pop();
}
 
/* 
  смотрит, если ли данная переменная для считывания в нее значения
 */

void Parser::check_id_in_get() {
  if (!vec_of_ident[cur_value].get_declare())
    throw "not declared";
}
