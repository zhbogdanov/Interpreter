#include "parsing.hpp"

/* 
  данный класс использует ПОЛИЗ, чтобы выполнять вычисления, считывание и вывод
 */

class Executer {
public:
    void execute(vector<Lex>& poliz);
};
 
/* 
  выполняем, пробегаясь по всему ПОЛИЗу
  получаем тип лексемы из ПОЛИЗа и при помощи switch выполняем правильное действие 
  добавляем в локальный стек интов соответсвущие значения переменных
  добвляем в стек строк соответсвующие значения переменных 
  flag отвечает за вычисления двух типов 
  flag == 0 - делаем вычисления для int flag == 1 - делаем вычисления для string 
  flag2 отвечает за тип строки (переменная или произвольно введенная строка)
  при строках мы добавляем в стек интов номер переменной из вектора vec_of_ident
  чтобы в дальнейшем присвоит значение
 */

void Executer::execute(vector<Lex>& poliz) {
  Lex pc_el;
  stack<int> args;
  stack<string> str_args;
  int i, j, index = 0, size = poliz.size(), flag = 0, flag2 = 1; 
  // flag == 0 (int) flag == 1 (string) flag2 == 1 (LEX_STRING) flag 2 == 2 (LEX_STR)
  string str, str2, buf;
  while (index < size) {
    pc_el = poliz[index];
    switch (pc_el.get_type()) {
    case LEX_TRUE: case LEX_FALSE: case LEX_NUM: case POLIZ_ADDRESS: case POLIZ_LABEL:
      flag = 0;
      args.push(pc_el.get_value());
      break;
 
    case LEX_STRING:
      flag = 1;
      flag2 = 1;
      str_args.push(pc_el.get_str_value());
      args.push(pc_el.get_value());
      break;

    case LEX_STR:
      flag = 1;
      flag2 = 2;
      str_args.push(pc_el.get_str_value());
      args.push(-1);
      break;

    /* 
      если мы получили переменную, то определяем к какому типу она относится и выставляем flag
     */

    case LEX_ID:
      i = pc_el.get_value();
      if (vec_of_ident[i].get_assign()) {
        if (vec_of_ident[i].get_str_value() != "") {
          flag = 1;
          str_args.push(vec_of_ident[i].get_str_value());
          args.push(pc_el.get_value());
        } else {
          flag = 0;
          args.push(vec_of_ident[i].get_value());
        }
        break;
      } else
        throw "POLIZ: indefinite identifier";
 
    case LEX_NOT:
      from_st(args, i);
      args.push(!i);
      break;
 
    case LEX_OR:
      from_st(args, i); 
      from_st(args,j);
      args.push(j || i);
      break;
 
    case LEX_AND:
      from_st(args, i);
      from_st(args,j);
      args.push (j && i);
      break;
 
    case POLIZ_GO:
      from_st(args, i);
      index = i - 1;
      break;
 
    case POLIZ_FGO:
      from_st(args, i);
      from_st(args,j);
      if (!j) index = i - 1;
      break;
 
    case LEX_PRINT:
      if (flag == 1) 
        from_st(str_args, str);
      from_st(args, j);
      cout << "\033[1;31m";
      if (flag == 1)
        cout <<str << endl;
      else 
        cout << j << endl;
      cout << "\x1B[37m";
      break;
 
    /* 
      достаем из стека номер переменной в векторе vec_of_ident
      определяем ее тип и в зависимости от типа считываем значени 
      далее помещаем значение в эту переменную с помощью одного из поля класса Ident 
     */

    case LEX_GET:
      int k;
      bool flag;
      from_st(args, i);
      if (flag == 1)
        from_st(str_args, str);
      if (vec_of_ident[i].get_type() == LEX_INT) {
        string numbb;
        flag = false;
        while (1) {
          cout << "Input int value for " << vec_of_ident[i].get_name() << endl;
          cin >> numbb;
          k = 0;
          while (numbb[k] != '\0') {
            if (numbb[k] > '9' || numbb[k] < '0') {
              flag = true;
              break;
            }
            ++k;
          }
          if (flag) {
            flag = false;
            cout << "Error in input:int" << endl;
            continue;
          }
          break;
        }
        k = stoi(numbb);
      } else if (vec_of_ident[i].get_type() == LEX_STRING) {
        cout << "Input string value for " << vec_of_ident[i].get_name() << endl;
        cin >> buf;
      } else {
        string j;
        while (1) {
          cout << "Input boolean value (true or false) for" << vec_of_ident[i].get_name() << endl;
          cin >> j;
          if (j != "true" && j != "false") {
            cout << "Error in input:true/false" << endl;
            continue;
          }
          k = (j == "true") ? 1 : 0;
          break;
        }
      }
      if (vec_of_ident[i].get_type() == LEX_STRING) 
        vec_of_ident[i].put_str_value(buf);
      else
        vec_of_ident[i].put_value(k);
      vec_of_ident[i].put_assign();
      break;
 
    /* 
      далее добавляем в стек интов логические значения операций как для строк, так и для интов 
      в стек строк добавляем значение операции
     */

    case LEX_PLUS:
      if (flag == 1) {
        from_st(str_args, str);
        from_st(str_args, str2);
        str_args.push(str2 + str);
        from_st(args, i);
        from_st(args,j);
        args.push(1);
      } else {
        from_st(args, i);
        from_st(args,j);
        args.push(i + j);
      }
      break;
 
    case LEX_TIMES:
      from_st(args, i);
      from_st(args,j);
      args.push(i * j);
      break;
 
    case LEX_MINUS:
      from_st(args, i);
      from_st(args,j);
      args.push(j - i);
      break;
 
    case LEX_SLASH:
      from_st(args, i);
      from_st(args,j);
      if (!i) {
        args.push(j / i);
        break;
      } else
        throw "POLIZ: divide by zero";
 
    case LEX_EQ:
      from_st(args, i);
      from_st(args, j);
      if (flag == 1) {
        from_st(str_args, str);
        from_st(str_args, str2);
        args.push(str == str2);
        flag = 0;
      } else 
        args.push(i == j);
      break;
 
    case LEX_LSS:
      from_st(args, i);
      from_st(args, j);
      if (flag == 1) {
        from_st(str_args, str);
        from_st(str_args, str2);
        args.push(str > str2);
        flag = 0;
      } else 
        args.push(i > j);
      break;
 
    case LEX_GTR:
      from_st(args, i);
      from_st(args, j);
      if (flag == 1) {
        from_st(str_args, str);
        from_st(str_args, str2);
        args.push(str < str2);
        flag = 0;
      } else 
        args.push(i < j);
      break;
 
    case LEX_LEQ:
      from_st(args, i);
      from_st(args, j);
      if (flag == 1) {
        from_st(str_args, str);
        from_st(str_args, str2);
        args.push(str <= str2);
        flag = 0;
      } else 
        args.push(i <= j);
      break;
 
    case LEX_GEQ:
      from_st(args, i);
      from_st(args, j);
      if (flag == 1) {
        from_st(str_args, str);
        from_st(str_args, str2);
        args.push(str >= str2);
        flag = 0;
      } else 
        args.push(i >= j);
      break;
 
    case LEX_NEQ:
      from_st(args, i);
      from_st(args, j);
      if (flag == 1) {
        from_st(str_args, str);
        from_st(str_args, str2);
        args.push(str != str2);
        flag = 0;
      } else 
        args.push(i != j);
      break;
 
    case LEX_ASSIGN:
      if (flag == 1) {
        from_st(str_args, str);
        from_st(str_args, str2);
      }
      from_st(args, i);
      from_st(args,j);
      if (flag == 1)
        vec_of_ident[j].put_str_value(str);
      if (flag == 0)
        vec_of_ident[j].put_value(i);
      vec_of_ident[j].put_assign(); 
      break;
 
    default:
      cout << pc_el << endl;
      throw "POLIZ: unexpected elem";
    }//end of switch
    ++index;
  };//end of while
  cout << "Finish of executing!!!" << endl;
}
