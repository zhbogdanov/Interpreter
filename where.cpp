#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

struct logic_expression;
struct expression;

void print_space() {
    cout << "--------------------------------------------";
}

void while_space(string& str, int& pos) {
    while ((str[pos] == ' ') || (str[pos] == '\t') || (str[pos] == '\n'))
        ++pos;
}

struct string_relation {
    string str1;
    string symbol;
    string str2;
};

struct long_relation {
    long lng1;
    string symbol;
    long lng2;
};

struct relation {
    int flag; // 1 - string 2 - long 
    string_relation *string_rel;
    long_relation *long_rel;
};

struct like_expression {
    string field_name;
    string str;
    int NOT;
};

struct in_expression {
    expression *expr;
    vector<long> vec;
    int NOT;
};

struct string_expression {
    string field_name;
    string str;
    int flag; // 1 - field 2 - str 
};

struct long_expression {
    string field_name;
    long lng;
    int flag; // 1 - field 2 - long
};

struct expression {
    string_expression *string_exp;
    long_expression *long_exp;
    int flag; // 1 - string 2 - long
};

struct operation {
    int flag;   // 1 - relation  2 - like 3 - in
    relation *rel;
    like_expression *like_exp;
    in_expression *in_exp;
};

struct logic_multiplier {
    int flag;  // 1 - logic multiplier 2 - logic expression 3 - operation
    int NOT;
    logic_multiplier *log_mul;
    logic_expression *log_exp;
    operation *oper;
};

struct logic_terms {
    vector<logic_multiplier*> vec;
};

struct logic_expression {
    vector<logic_terms*> vec;
};

struct where_struct {
    int ALL_flag;
    logic_expression *log_exp;
};

logic_expression *logic_exp_parsing(string&, int&);
logic_expression *logic_expression_parsing(string&, int&);
void print_logic_multiplier(logic_multiplier *);

long_relation *long_relation_parsing(string& str, int& pos) {
    long_relation *long_rel = new long_relation;
    string str_help = "";
    long lng = 0;
    while_space(str, pos);
    while ((str[pos] != ' ') && (str[pos] != '\t')) {
        if ((str[pos] > '9') || (str[pos] < '0'))
            return nullptr;
        lng = lng * 10 + str[pos++] - '0';
    }
    long_rel->lng1 = lng;
    while_space(str, pos);
    while ((str[pos] != ' ') && (str[pos] != '\t'))
        str_help += str[pos++];
    if ((str_help != "=") && (str_help != ">") && (str_help != "<") && (str_help != ">=") && (str_help != "<=") && (str_help != "!="))
        return nullptr;
    long_rel->symbol = str_help;
    lng = 0;
    while_space(str, pos);
    while ((str[pos] != ' ') && (str[pos] != '\t')) {
        if ((str[pos] > '9') || (str[pos] < '0'))
            return nullptr;
        lng = lng * 10 + str[pos++] - '0';
    }
    long_rel->lng2 = lng;
    return long_rel;
}

string_relation *string_relation_parsing(string& str, int& pos) {
    
    string_relation *string_rel = new string_relation;
    string str_help = "";
    while_space(str, pos);
    while ((str[pos] != ' ') && (str[pos] != '\t')) {
        str_help += str[pos++];
    }
    string_rel->str1 = str_help;
    str_help = "";
    while_space(str, pos);
    while ((str[pos] != ' ') && (str[pos] != '\t'))
        str_help += str[pos++];
    if ((str_help != "=") && (str_help != ">") && (str_help != "<") && (str_help != ">=") && (str_help != "<=") && (str_help != "!="))
        return nullptr;
    string_rel->symbol = str_help;
    str_help = "";
    while_space(str, pos);
    while ((str[pos] != ' ') && (str[pos] != '\t'))
        str_help += str[pos++];
    string_rel->str2 = str_help;
    return string_rel;
}

relation *relation_parsing(string& str, int& pos) {
    while_space(str, pos);
    //cout << "relation " << str[pos] << endl;
    if ((str[pos] == '\n') || (pos >= str.length() - 1))
        return nullptr;
    relation *rel_str = new relation;
    string_relation *str_rel = nullptr;
    long_relation *lng_rel = nullptr;
    int save_pos = pos;
    lng_rel = long_relation_parsing(str, pos);
    if (lng_rel != nullptr) {
        rel_str->flag = 2;
        rel_str->long_rel = lng_rel;
        return rel_str;
    }
    pos = save_pos;
    str_rel = string_relation_parsing(str, pos);
    if (str_rel != nullptr) {
        rel_str->flag = 1;
        rel_str->string_rel = str_rel;
        return rel_str;
    }
    pos = save_pos;
    return nullptr;
}

like_expression *like_expression_parsing(string& str, int& pos) {
    while_space(str, pos);
    if ((str[pos] == '\n') || (pos >= str.length() - 1))
        return nullptr;
    like_expression *like_exp = new like_expression;
    like_exp->NOT = 0;
    int save_pos = pos;
    string str_help = "";
    while_space(str, pos);
    //cout << "1 " << str << endl; 
    while ((str[pos] != ' ') && (str[pos] != '\t'))
        str_help += str[pos++];
    like_exp->field_name = str_help;
    str_help = "";
    while_space(str, pos);
    while ((str[pos] != ' ') && (str[pos] != '\t'))
        str_help += str[pos++];
    if (str_help == "NOT") {
        like_exp->NOT = 1;
        str_help = "";
        while_space(str, pos);
        while ((str[pos] != ' ') && (str[pos] != '\t'))
            str_help += str[pos++];
    }
    if (str_help != "LIKE") {
        pos = save_pos;
        return nullptr;
    }
    str_help = "";
    while_space(str, pos);
    while ((str[pos] != ' ') && (str[pos] != '\t'))
        str_help += str[pos++];
    like_exp->str = str_help;
    return like_exp;
}

long_expression *long_expression_parsing(string& str, int& pos) {
    while_space(str, pos);
    if ((str[pos] == '\n') || (pos >= str.length() - 1))
        return nullptr;
    long_expression *lng_exp = new long_expression;
    string str_help = "";
    long lng = 0;
    int flag = 0;
    int save_pos = pos;
    while_space(str, pos); 
    while ((str[pos] != ' ') && (str[pos] != '\t')) {
        if ((str[pos] > '9') || (str[pos] < '0')) {
            flag = 1;
            break;
        }
        lng = lng * 10 + str[pos++] - '0';
    }

    if (flag == 0) {
        lng_exp->flag = 2;
        lng_exp->lng = lng;
    } else {
        pos = save_pos;
        while_space(str, pos);
        while ((str[pos] != ' ') && (str[pos] != '\t'))
            str_help += str[pos++];
        lng_exp->field_name = str_help;
        lng_exp->flag = 1;
    }
    //cout << lng_exp->field_name << endl;
    return lng_exp;
}

string_expression *string_expression_parsing(string& str, int& pos) {
    while_space(str, pos);
    if ((str[pos] == '\n') || (pos >= str.length() - 1))
        return nullptr;
    string_expression *str_exp = new string_expression;
    string str_help = "";
    while_space(str, pos);
    while ((str[pos] != ' ') && (str[pos] != '\t'))
        str_help += str[pos++];
    str_exp->str = str_help;
    
    return str_exp;
}

expression *expression_parsing(string& str, int& pos) {
    expression *exp_str = new expression;
    string_expression *str_exp = nullptr;
    long_expression *lng_exp = nullptr;
    int save_pos = pos;
    lng_exp = long_expression_parsing(str, pos);
    if (lng_exp != nullptr) {
        //cout << lng_exp->field_name << endl;
        exp_str->flag = 2;
        exp_str->long_exp = lng_exp;
        return exp_str;
    } 
    pos = save_pos;
    str_exp = string_expression_parsing(str, pos);
    if (str_exp != nullptr) {
        cout << "STR" << endl;
        exp_str->flag = 1;
        exp_str->string_exp = str_exp;
        return exp_str;
    }
    pos = save_pos;
    return nullptr;
}

in_expression *in_expression_parsing(string& str, int& pos) {
    while_space(str, pos);
    if ((str[pos] == '\n') || (pos >= str.length() - 1))
        return nullptr;
    in_expression *in_exp = new in_expression;
    expression *exp_str = nullptr;
    string str_help = "";
    int save_pos = pos;
    long lng = 0;
    in_exp->NOT = 0;
    while_space(str, pos);
    exp_str = expression_parsing(str, pos);
    if (exp_str == nullptr) {
        pos = save_pos;
        return nullptr;
    }
    //cout << exp_str->long_exp << endl;
    in_exp->expr = exp_str;
    while_space(str, pos);
    while ((str[pos] != ' ') && (str[pos] != '\t'))
        str_help += str[pos++];
    if (str_help == "NOT") {
        in_exp->NOT = 1;
        str_help = "";
        while_space(str, pos);
        while ((str[pos] != ' ') && (str[pos] != '\t'))
            str_help += str[pos++];
    }
    if (str_help != "IN") {
        pos = save_pos;
        return nullptr;
    }
    str_help = "";
    while_space(str, pos);
    //cout << str[pos] << endl;
    if (str[pos] != '(') {
        pos = save_pos;
        return nullptr;
    }
    //cout << str[pos] << endl;
    while (str[pos] != ')') {
        ++pos;
        lng = 0;
        while_space(str, pos);
        while ((str[pos] != ',') && (str[pos] != ')')) {
            if ((str[pos] > '9') || (str[pos] < '0')) {
                pos = save_pos;
                return nullptr;
            }
            lng = lng * 10 + str[pos++] - '0';
        }
        in_exp->vec.push_back(lng);
    }
    ++pos;
    return in_exp;
}

operation *operation_parsing(string& str, int& pos) {
    operation *oper_str = new operation;
    relation *r_help = nullptr;
    like_expression *l_e_help = nullptr;
    in_expression *i_e_help = nullptr;
    int save_pos = pos;
    while_space(str, pos);
    l_e_help = like_expression_parsing(str, pos);
    if (l_e_help != nullptr) {
        oper_str->flag = 2;
        oper_str->like_exp = l_e_help;
        return oper_str;
    }
    pos = save_pos; 
    i_e_help = in_expression_parsing(str, pos);
    if (i_e_help != nullptr) {
        oper_str->flag = 3;
        oper_str->in_exp = i_e_help;
        return oper_str;
    }
    pos = save_pos;
    r_help = relation_parsing(str, pos);
    if (r_help != nullptr) {
        oper_str->flag = 1;
        oper_str->rel = r_help;
        return oper_str;
    }
    pos = save_pos;
    return nullptr;
}

logic_multiplier *logic_multiplier_parsing(string& str, int& pos) {
    logic_multiplier *logic_multiplier_str = new logic_multiplier;
    logic_multiplier *l_m_help = nullptr;
    logic_expression *l_e_help = nullptr;
    operation *oper_help = nullptr;
    int save_pos = pos;
    logic_multiplier_str->NOT = 0;
    string str_help = "";
    while_space(str, pos);
    if (str[pos] != '(') {
        oper_help = operation_parsing(str, pos);
        if (oper_help == nullptr) {
            pos = save_pos;
            return nullptr;
        } else {
            //cout << "OPERAYION PARSING " << pos << endl;
            logic_multiplier_str->flag = 3;
            logic_multiplier_str->oper = oper_help;
            return logic_multiplier_str;
        }
    }
    ++pos;
    while ((str[pos] != ' ') && (str[pos] != '\t'))
        str_help += str[pos++];
    if (str_help == "NOT") {
        l_m_help = logic_multiplier_parsing(str, pos);
        if ((l_m_help == nullptr) || (str[pos] != ')')) {
            pos = save_pos;
            return nullptr;
        } else {
            logic_multiplier_str->flag = 1;
            logic_multiplier_str->NOT = 1;
            logic_multiplier_str->log_mul = l_m_help;
            return logic_multiplier_str;
        } 
    } else 
        pos = save_pos;
    l_e_help = logic_expression_parsing(str, pos);
    if ((l_e_help  == nullptr) || (str[pos] != ')')) {
        pos = save_pos;
        return nullptr;
    } else {
        logic_multiplier_str->flag = 2;
        logic_multiplier_str->log_exp = l_e_help;
    }
    pos = save_pos;
    return logic_multiplier_str;
}

logic_terms *logic_terms_parsing(string& str, int& pos) {
    logic_terms *logic_terms_str = new logic_terms;
    logic_multiplier *log_mul_help;
    vector<logic_multiplier*> log_mul_vec;
    string str_help;
    int save_pos = pos;
    bool flag = true;
    while (flag) {
        str_help = "";
        while_space(str, pos);
        log_mul_help = logic_multiplier_parsing(str, pos);
        if (log_mul_help == nullptr) {
            pos = save_pos;
            return nullptr;
        }
        //cout << "\nKJNKJN  " << pos << endl;
        log_mul_vec.push_back(log_mul_help);
        while_space(str, pos);
        save_pos = pos;
        while ((str[pos] != ' ') &&  (str[pos] != '\t'))
            str_help += str[pos++];
        if (str_help != "AND") {
            pos = save_pos;
            logic_terms_str->vec = log_mul_vec;
            flag = false;//return logic_terms_str;
        } 
    }
    return logic_terms_str;
}

logic_expression *logic_expression_parsing(string& str, int& pos) {
    logic_expression *logic_expression_str = new logic_expression;
    logic_terms *log_ter_help;
    vector<logic_terms*> log_ter_vec;
    string str_help;
    int save_pos = pos;
    bool flag = true;
    while (flag) {
        str_help = "";
        //cout << "POS == " << pos << endl;
        while_space(str, pos);
        log_ter_help = logic_terms_parsing(str, pos);
        if (log_ter_help == nullptr) {
            pos = save_pos;
            return nullptr;
        }
        //cout << "POS2 == " << pos << endl;
        log_ter_vec.push_back(log_ter_help);
        while_space(str, pos);
        save_pos = pos;
        while ((str[pos] != ' ') &&  (str[pos] != '\t'))
            str_help += str[pos++];
        //cout << "njkcsk  " << str_help << endl;
        if (str_help != "OR") {
            pos = save_pos;
            logic_expression_str->vec = log_ter_vec;
            flag = false;//return logic__str;
        } 
        //if (flag)
            //cout << "\n rfjnrjfnr \n";
    }
    return logic_expression_str;
}

where_struct *where_parsing(string& str, int& pos) {
    where_struct *where_str = new where_struct;
    string str_help = "";
    int save_pos = pos;
    while_space(str, pos);
    while ((str[pos] != ' ') &&  (str[pos] != '\t'))
        str_help += str[pos++];
    if (str_help == "ALL") {
        where_str->ALL_flag = 1;
        return where_str;
    }
    pos = save_pos;
    where_str->ALL_flag = 0;
    logic_expression *log_exp = nullptr;
    log_exp = logic_expression_parsing(str, pos);
    if (log_exp != nullptr) {
        where_str->log_exp = log_exp;
        return where_str;
    }
    pos = save_pos;
    return nullptr;
}

void print_string_relation(string_relation *vec) {
    if (vec != nullptr) {
        cout << "String Relation" << endl;
        print_space();
        cout << endl;
        cout << vec->str1 << " " << vec->symbol << " " << vec->str2 << endl;
        print_space();
        cout << endl;
    }
    else 
        cout << "nullptr" << endl;
}

void print_long_relation(long_relation *vec) {
    if (vec != nullptr) {
        cout << "Long Relation" << endl;
        print_space();
        cout << endl;
        cout << vec->lng1 << " " << vec->symbol << " " << vec->lng2 << endl;
        print_space();
        cout << endl;
    }
    else 
        cout << "nullptr" << endl;
}

void print_relation(relation *vec) {
    if (vec != nullptr) {
        if (vec->flag == 1) {
            print_string_relation(vec->string_rel);
        }
        else if (vec->flag == 2)
            print_long_relation(vec->long_rel);
    } else 
        cout << "nullptr" << endl;
}

void print_string_expression(string_expression *vec) {
    if (vec != nullptr) {
        cout << "String Expression" << endl;
        print_space();
        cout << endl;
        cout << vec->str << endl;
        print_space();
        cout << endl;
    }
    else 
        cout << "nullptr" << endl;
}

void print_long_expression(long_expression *vec) {
    if (vec != nullptr) {
        cout << "Long Expression" << endl;
        print_space();
        cout << endl;
        if (vec->flag == 1) 
            cout << vec->field_name << endl;
        else if (vec->flag == 2)
            cout << vec->lng << endl;
        print_space();
        cout << endl;
    } else 
        cout << "nullptr" << endl;
}

void print_expression(expression *vec) {
    if (vec != nullptr) {
        if (vec->flag == 1) {
            print_string_expression(vec->string_exp);
        } else if (vec->flag == 2) {
            print_long_expression(vec->long_exp);
        }
    } else 
        cout << "nullptr" << endl;
}

void print_like_expression(like_expression *vec) {
    if (vec != nullptr) {
        cout << "Like Expression" << endl;
        print_space();
        cout << endl;
        cout << vec->field_name << ' ';
        if (vec->NOT == 1)
            cout << "NOT ";
        cout << vec->str << endl;
        print_space();
        cout << endl;
    } else 
        cout << "nullptr" << endl;
}

void print_in_expression(in_expression *vec) {
    if (vec != nullptr) {
        cout << "In Expression" << endl;
        print_space();
        cout << endl;
        print_expression(vec->expr);
        if (vec->NOT == 1)
            cout << "NOT ";
        for (const auto& elem : vec->vec)
            cout << elem << ' ';
        cout << endl;
        print_space();
        cout << endl;
    } else 
        cout << "nullptr" << endl;
}

void print_opeation(operation *vec) {
    if (vec != nullptr) {
        if (vec->flag == 1) {
            print_relation(vec->rel);
        } else if (vec->flag == 2) {
            print_like_expression(vec->like_exp);
        } else if (vec->flag == 3) {
            print_in_expression(vec->in_exp);
        }
    } else 
        cout << "nullptr" << endl;
}

void print_logic_terms(logic_terms *vec) {
    if (vec != nullptr) {
        for (auto& elem : vec->vec) {
            print_logic_multiplier(elem);
            cout << "  ";
        }
        cout << endl;
    } else 
        cout << "nullptr" << endl;
}

void print_logic_expression(logic_expression *vec) {
    if (vec != nullptr) {
        for (auto& elem : vec->vec) {
            print_logic_terms(elem);
        }
        cout << endl;
    } else 
        cout << "nullptr" << endl;
}

void print_logic_multiplier(logic_multiplier *vec) {
    if (vec != nullptr) {
        if (vec->flag == 3) {
            print_opeation(vec->oper);
        } else if (vec->flag == 1) {
            cout << "not logic mult" << endl;
            print_logic_multiplier(vec->log_mul);
        } else if (vec->flag == 2) {
            cout << "logic expression" << endl;
            print_logic_expression(vec->log_exp);
        }
    } else 
        cout << "nullptr" << endl;
}

void print_where(where_struct *vec) {
    if (vec != nullptr) {
        if (vec->ALL_flag == 1)
            cout << "ALL";
        else if (vec->ALL_flag == 0) 
            print_logic_expression(vec->log_exp);
    } else 
        cout << "nullptr" << endl;
}
/*
int main() {
    int pos = 0;
    string_relation *string_relation_ptr;
    long_relation *long_relation_ptr;
    relation *relation_ptr;
    string_expression *string_expression_ptr;
    long_expression *long_expression_ptr;
    expression *expression_ptr;
    like_expression *like_expression_ptr;
    in_expression *in_expression_ptr;
    operation *operation_ptr;
    logic_multiplier *logic_multiplier_ptr;
    where_struct *where_struct_ptr;
    string str, cmd;
    ifstream fin("test2.txt");
    try {
        
        getline(fin, str);
        //cout << str << endl;

        where_struct_ptr = where_parsing(str, pos);
        print_where(where_struct_ptr);

        //logic_multiplier_ptr = logic_multiplier_parsing(str, pos);
        //print_logic_multiplier(logic_multiplier_ptr);

        //operation_ptr = operation_parsing(str, pos);
        //print_opeation(operation_ptr);

        //in_expression_ptr = in_expression_parsing(str, pos);
        //print_in_expression(in_expression_ptr);

        //like_expression_ptr = like_expression_parsing(str, pos);
        //print_like_expression(like_expression_ptr);
        
        //relation_ptr = relation_parsing(str, pos);
        //print_relation(relation_ptr);

        //expression_ptr = expression_parsing(str, pos);
        //print_expression(expression_ptr);

        //long_expression_ptr = long_expression_parsing(str, pos);
        //print_long_expression(long_expression_ptr);

        //string_expression_ptr = string_expression_parsing(str, pos);
        //print_string_expression(string_expression_ptr);

        //relation_ptr = relation_parsing(str, pos);
        //print_relation(relation_ptr);

        //long_relation_ptr = long_relation_parsing(str, pos);
        //print_long_relation(long_relation_ptr);

        //string_relation_vec.push_back(string_relation_parsing(str, pos));
        //print_string_relation(string_relation_vec);
    }
    catch (exception& ex) {
        cout << ex.what() << endl;
    }
    return 0;
}
*/