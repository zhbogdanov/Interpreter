#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

/* Наш набор лексем */

enum type_of_lex {
  LEX_NULL,                                                                                   /* 0*/
  LEX_AND, LEX_BEGIN, LEX_BOOL, LEX_DO, LEX_ELSE, LEX_END, LEX_IF, LEX_FALSE, LEX_INT,        /* 9*/
  LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_GET, LEX_THEN, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_PRINT,   /*18*/
  LEX_STRING,                                                                                 /*19*/
  LEX_FIN,                                                                                    /*20*/
  LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS,   /*28*/
  LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ,              /*36*/
  LEX_NUM,                                                                                    /*37*/
  LEX_ID,                                                                                     /*38*/
  POLIZ_LABEL,                                                                                /*39*/
  POLIZ_ADDRESS,                                                                              /*40*/
  POLIZ_GO,                                                                                   /*41*/
  POLIZ_FGO,                                                                                  /*42*/
  LEX_LBRACE,                                                                                 /*43*/
  LEX_RBRACE,                                                                                 /*44*/
  LEX_STR
};

/* Перегрузка оператора вывода для отладки (выводит номер лексемы) */

ostream& operator << (ostream& s, type_of_lex l) {
  s << int(l);
  return s;
}
 
/////////////////////////  Класс Lex  //////////////////////////

/* 
  Класс для хранения лексем
  type_of_lex - тип лексемы 
  int - величина для значения типа int, либо номер в массивах CMD, SYMB, если лексема является 
  специальным символом или служебным словом, либо номер переменной (позиция в векторе vec_of_ident) 
  string - значение строки переменной типа string 
 */
 
class Lex {
  type_of_lex   type_lex;
  int           value_lex;
  string        str_value_lex;
  public:
  Lex(type_of_lex t_lex = LEX_NULL, int v_lex = 0, string str_v_lex = "") : type_lex(t_lex), 
                                                                            value_lex(v_lex), str_value_lex(str_v_lex) {}
  type_of_lex  get_type() const { 
    return type_lex; 
  }
  int get_value() const { 
    return value_lex; 
  }
  string get_str_value() const {
    return str_value_lex;
  }
  friend ostream& operator << (ostream& s, Lex l);
};
 
/////////////////////  Класс Ident  ////////////////////////////

/* 
  Данный класс отвечает за переменные 
  name - поля для хранения имени 
  declare - отвечает за уникальность имени переменной 
  type_of_lex - лексема переменной
  assign - пресвоено ли переменной значение
  value - значение для переменной типа int / bool
  string - значение для переменно типа string
 */
 
class Ident {
  string      name;
  bool        declare;
  type_of_lex type;
  bool        assign;
  int         value;
  string      str_value;
public:
  Ident() { 
    declare = false; 
    assign = false; 
  }
  Ident(const string n, const string str = "") {
    name = n;
    str_value = str;
    declare = false; 
    assign = false;
  }
  string get_name() const { 
    return name; 
  }
  bool get_declare() const { 
    return declare; 
  }
  type_of_lex get_type() const { 
    return type; 
  }
  bool get_assign() const { 
    return assign; 
  }
  int  get_value() const { 
    return value; 
  }
  string get_str_value() const {
    return str_value;
  }
  void put_declare() { 
    declare = true; 
  }
  void put_type(type_of_lex t) { 
    type = t; 
  }
  void put_assign() { 
    assign = true; 
  }
  void put_value(int v) { 
    value = v; 
  }
  void put_str_value(string str) {
    str_value = str;
  }
  bool operator == (const string& s) const { 
    return name == s; 
  }
};
