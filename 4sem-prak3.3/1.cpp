#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <cstdio>
#include <cctype>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

enum type_of_lex {
    LEX_NULL,                                                                                   /* 0*/
    LEX_AND, LEX_REAL, LEX_DO, LEX_INT,                                                         /* 4*/
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_WHILE, LEX_WRITE, LEX_BREAK,                    /*11*/
    LEX_STRING, LEX_IF, LEX_ELSE, LEX_FALSE, LEX_TRUE,
    LEX_FIN,                                                                                    /*18*/
    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS,   /*26*/
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ,              /*34*/
    LEX_START, LEX_FINISH,                                                                      /*36*/
    LEX_NUM,                                                                                    /*37*/
    LEX_ID,
    LEX_STR, LEX_REALBODY, LEX_REM,                                                             /*38*/
    POLIZ_LABEL,                                                                                /*39*/
    POLIZ_ADDRESS,                                                                              /*40*/
    POLIZ_GO,                                                                                   /*41*/
    POLIZ_FGO                                                                                   /*42*/
};

/////////////////////////  Класс Lex  //////////////////////////
class Lex {
    type_of_lex t_lex;
    int v_lex;
    string str_v_lex;
    double real_v_lex;
public:
    Lex(type_of_lex t = LEX_NULL, int v = 0, string s = "", double r = 0)
        : t_lex(t), v_lex(v), str_v_lex(s), real_v_lex(r) {}

    type_of_lex get_type() const {
        return t_lex;
    }

    int get_value() const {
        return v_lex;
    }

    string get_str_value() const {
        return str_v_lex;
    }

    double get_real_value() const {
        return real_v_lex;
    }

    void put_value(int n) {
        v_lex = n;
    }
    void put_r_value(double s) {
        real_v_lex = s;
    }

    friend ostream& operator<<(ostream& s, Lex l);
};

/////////////////////  Класс Ident  ////////////////////////////

class Ident {
    string name;
    bool declare;
    type_of_lex type = LEX_BREAK;
    bool assign;
    int value = 0;
public:
    Ident() {
        declare = false;
        assign = false;
    }

    bool operator==(const string& s) const {
        return name == s;
    }

    Ident(const string n) {
        name = n;
        declare = false;
        assign = false;
    }

    string get_name() const {
        return name;
    }

    bool get_declare() const {
        return declare;
    }

    void put_declare() {
        declare = true;
    }

    type_of_lex get_type() const {
        return type;
    }

    void put_type(type_of_lex t) {
        type = t;
    }

    bool get_assign() const {
        return assign;
    }

    void put_assign() {
        assign = true;
    }

    int get_value() const {
        return value;
    }

    void put_value(int v) {
        value = v;
    }
};

//////////////////////  TID  ///////////////////////

vector<Ident> TID;

int put(const string& buf) {
    vector<Ident>::iterator k;

    if ((k = find(TID.begin(), TID.end(), buf)) != TID.end())
        return int(k - TID.begin());
    TID.push_back(Ident(buf));
    return int(TID.size() - 1);
}

double string_to_double(const string& s) {
    istringstream i(s);
    double x;
    if (!(i >> x))
        return 0;
    return x;
}
/////////////////////////////////////////////////////////////////

class Scanner {
    FILE* fp;
    char c = '1';
    int look(const string buf, const char** list) {
        int i = 0;
        while (list[i]) {
            if (buf == list[i])
                return i;
            ++i;
        }
        return 0;
    }
    void gc() {
        c = fgetc(fp);
    }
public:
    static const char* TW[], * TD[];
    Scanner(const char* program) {
        if (!(fp = fopen(program, "r")))
            throw "can’t open file";
    }
    Lex get_lex();
};

const char*
Scanner::TW[] = { "", "and", "real", "do",
                 "int", "not", "or", "program",
                 "read", "while", "write", "break",
                 "string", "if", "else", "false", "true", NULL};

const char*
Scanner::TD[] = { "@", ";", ",", ":", "=", "(", ")", "==",
                 "<", ">", "+", "-", "*", "/", "<=", "!=",
                 ">=", "{", "}", "%", NULL};

Lex Scanner::get_lex() {
    enum state { H, IDENT, NUMB, COM, ALE, NEQ, STR, REAL };
    int d, j, t = 10;
    double dd;
    string buf;
    state CS = H;
    do {
        gc();
        switch (CS) {
        case H:
            if (c == ' ' || c == '\n' || c == '\r' || c == '\t');
            else if (isalpha(c)) {
                buf.push_back(c);
                CS = IDENT;
            }
            else if (c == '"') {
                buf.push_back(c);
                CS = STR;
            }
            else if (isdigit(c)) {
                d = c - '0';
                CS = NUMB;
            }
            else if (c == '<' || c == '>' || c == '=') {
                buf.push_back(c);
                CS = ALE;
            }
            else if (int(c) == EOF)
                return Lex(LEX_FIN);
            else if (c == '!') {
                buf.push_back(c);
                CS = NEQ;
            }
            else if ((c == '/') || (c == '%')) {
                gc();
                if (c == '*')
                    CS = COM;
                else {
                    ungetc(c, fp);
                    return Lex(LEX_SLASH, 13);
                }
            }
            else {
                buf.push_back(c);
                if ((j = look(buf, TD))) {
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                else
                    throw c;
            }
            break;
        case IDENT:
            if (isalpha(c) || isdigit(c)) {
                buf.push_back(c);
            }
            else {
                ungetc(c, fp);
                if ((j = look(buf, TW))) {
                    return Lex((type_of_lex)j, j);
                }
                else {
                    j = put(buf);
                    return Lex(LEX_ID, j);
                }
            }
            break;
        case NUMB:
            if (isdigit(c)) {
                d = d * 10 + (c - '0');
            }
            else if (c == '.') {
                dd = d;
                CS = REAL;
            }
            else {
                ungetc(c, fp);
                return Lex(LEX_NUM, d);
            }
            break;
        case REAL:
            if (isdigit(c)) {
                dd = dd + (1.0 * (c - '0')) / t;
                t *= 10;
            }
            else if (int(c) == EOF) {
                throw c;
            }
            else
                return Lex(LEX_REALBODY, 0, "", dd);
        case COM:
            if (c == '*') {
                gc();
                if ((c == '/') || (c == '%'))
                    CS = H;
                else if (int(c) == EOF)
                    throw c;
            }
            else if (int(c) == EOF)
                throw c;
            break;
        case STR:buf.push_back(c);
            if (c == '"')
                return Lex(LEX_STR, 0, buf);
            else if (int(c) == EOF)
                throw c;
            break;
        case ALE:
            if (c == '=') {
                buf.push_back(c);
                j = look(buf, TD);
                return Lex((type_of_lex)(j + (int)LEX_FIN), j);
            }
            else {
                ungetc(c, fp);
                j = look(buf, TD);
                return Lex((type_of_lex)(j + (int)LEX_FIN), j);
            }
            break;
        case NEQ:
            if (c == '=') {
                buf.push_back(c);
                j = look(buf, TD);
                return Lex(LEX_NEQ, j);
            }
            else
                throw '!';
            break;
        } //end switch
    } while (true);
}

ostream& operator<<(ostream& s, Lex l) {
    string t;
    if (l.t_lex < LEX_FIN)
        t = Scanner::TW[l.t_lex];
    else if (l.t_lex >= LEX_FIN && l.t_lex < LEX_NUM)
        t = Scanner::TD[l.t_lex - LEX_FIN];
    else if (l.t_lex == LEX_NUM)
        t = "NUMB";
    else if (l.t_lex == LEX_ID)
        t = TID[l.v_lex].get_name();
    else if (l.t_lex == LEX_STR) {
        t = "STR";
        s << '(' << t << ',' << l.str_v_lex << ");" << endl;
        return s;
    }
    else if (l.t_lex == LEX_REALBODY) {
        t = "REAL NUMB";
        s << '(' << t << ',' << l.real_v_lex << ");" << endl;
        return s;
    }
    else if (l.t_lex == LEX_START)
        t = "{";
    else if (l.t_lex == LEX_FINISH)
        t = "}";
    else if (l.t_lex == POLIZ_LABEL)
        t = "Label";
    else if (l.t_lex == POLIZ_ADDRESS)
        t = "Addr";
    else if (l.t_lex == POLIZ_GO)
        t = "!";
    else if (l.t_lex == POLIZ_FGO)
        t = "!F";
    else
        throw l;
    s << '(' << t << ',' << l.v_lex << ");" << endl;
    return s;
}

//////////////////////////  Класс Parser  /////////////////////////////////
template<class T, class T_EL>
void from_st(T& st, T_EL& i) {
    i = st.top();
    st.pop();
}

class Parser {
    Lex curr_lex;
    type_of_lex c_type;
    int c_val;
    string c_val_str;
    double c_val_real = 0.0;
    bool flag_block = true;
    bool flag_while = false;
    bool flag_no_eq = false;
    vector<int> gotoForTrNumbers;
    Scanner scan;
    stack<int> st_int;
    stack<type_of_lex> st_lex;
    vector<int> unusedLabel;
    vector<int> labels;
    stack<int> st_break;
    stack<int> break_flag;
    void P();
    void D1();
    void D();
    void O();
    void O1();
    void Oor();
    void Oeq();
    void Oand();
    void Oplusminus();
    void Oper();
    void Operand();
    void B();
    void V(int& flag_dec);
    void C();
    void S();
    void E();
    void E1();
    void T();
    void F();
    void dec(type_of_lex type);
    void check_id();
    void check_op();
    void check_not();
    void eq_type();
    void eq_bool();
    void check_id_in_read() const;
    void gl() {
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
        c_val = curr_lex.get_value();
        c_val_str = curr_lex.get_str_value();
        c_val_real = curr_lex.get_real_value();
    }
public:
    vector<Lex> poliz;
    explicit Parser(const char* program) : scan(program) {
        c_type = LEX_NULL;
        c_val = 0;
    }
    void analyze();
};

void Parser::analyze() {
    gl();
    P();
    if (c_type != LEX_FIN)
        throw curr_lex;
    for (Lex l: poliz)
        cout << l;
    cout << endl << "There are no problems" << endl;
}

void Parser::P() {
    if (c_type == LEX_PROGRAM) {
        gl();
    }
    else throw curr_lex;
    if (c_type == LEX_START)
        gl();
    else throw curr_lex;
    D();
    O();
    if (c_type == LEX_FINISH) {
        gl();
    }
    else throw curr_lex;
}

void Parser::D()
{
    D1();
    while (c_type == LEX_SEMICOLON) {
        gl();
        D1();
    }
}

void Parser::D1()
{
    int flag2 = 0;
    if (c_type == LEX_INT or c_type == LEX_STRING or c_type == LEX_REAL) {
        if (c_type == LEX_INT) {
            flag2 = 1;
            gl();
        }
        else if (c_type == LEX_STRING) {
            flag2 = 2;
            gl();
        }
        else if (c_type == LEX_REAL) {
            flag2 = 4;
            gl();
        }
        else {
            flag2 = 3;
            gl();
        }
        V(flag2);
        while (c_type == LEX_COMMA) {
            gl();
            V(flag2);
        }
    }
}

void Parser::V(int& flag_dec) {
    int tmp_value;
    if (c_type == LEX_ID) {
        tmp_value = c_val;
        st_int.push(c_val);
        switch (flag_dec) {
        case 1:
            dec(LEX_INT);
            break;
        case 2:
            dec(LEX_STRING);
            break;
        case 3:
            dec(LEX_STRING);
            break;
        case 4:
            dec(LEX_REAL);
        default:
            break;
        }
        gl();
        if (c_type != LEX_ASSIGN && c_type != LEX_SEMICOLON && c_type != LEX_COMMA) {
            throw curr_lex;
        }
        if (c_type == LEX_ASSIGN) {
            poliz.push_back(Lex(POLIZ_ADDRESS, tmp_value));
            if (TID[tmp_value].get_declare()) {
                st_lex.push(TID[tmp_value].get_type());
            }
            gl();
            C();
            eq_type();
            poliz.push_back(Lex(LEX_ASSIGN));
        }
    }
    else throw curr_lex;
}

void Parser::C() {
    int flag = 1;
    if (c_type == LEX_PLUS or c_type == LEX_MINUS) {
        if (c_type == LEX_MINUS) flag = -1;
        gl();
        if (c_type == LEX_NUM) {
            curr_lex.put_value(flag * curr_lex.get_value());
            st_lex.push(LEX_INT);
            poliz.push_back(curr_lex);
            gl();
        }
        else if (c_type == LEX_REALBODY) {
            curr_lex.put_r_value(flag * curr_lex.get_real_value());
            st_lex.push(LEX_REAL);
            poliz.push_back(curr_lex);
            gl();
        }
        else throw curr_lex;
    }
    else if (c_type == LEX_NUM) {
        st_lex.push(LEX_INT);
        poliz.push_back(curr_lex);
        gl();
        if (c_type == LEX_ASSIGN) {
            throw curr_lex;
        }
    }
    else if (c_type == LEX_REALBODY) {
        st_lex.push(LEX_REAL);
        poliz.push_back(curr_lex);
        gl();
    }
    else if (c_type == LEX_STR) {
        st_lex.push(LEX_STRING);
        poliz.push_back(curr_lex);
        gl();
    }
    else throw curr_lex;
}

void Parser::O() { //operators
    O1();
    while (flag_block) {
        O1();
    }
}

void Parser::O1() {
    int pl0, pl1, tmp;
    if (c_type == LEX_ID) {
        check_id();
        poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
        gl();
        if (c_type == LEX_ASSIGN) {
            gl();
            Oeq();
            eq_type();
            poliz.push_back(Lex(LEX_ASSIGN));
            while (c_type == LEX_ASSIGN) {
                gl();
                Oeq();
                eq_type();
                poliz.push_back(Lex(LEX_ASSIGN));
            }
            if (c_type == LEX_SEMICOLON) {
                gl();
            }
            else throw curr_lex;
        }
        else throw curr_lex;
    }
    else if (c_type == LEX_IF) {
        flag_no_eq = true;
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            Oeq();
            eq_bool();
        }
        else throw curr_lex;
        if (c_type == LEX_RPAREN) {
            flag_no_eq = false;
            gl();
            O1();
            if (!flag_block) 
                throw curr_lex;
        }
        else throw curr_lex;
        if (c_type == LEX_ELSE) {
            gl();
            O1();
            if (!flag_block) 
                throw curr_lex;
        }
    }
    else if (c_type == LEX_WHILE) {
        pl0 = int(poliz.size());
        flag_no_eq = true;
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            Oeq();
            eq_bool();
            pl1 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
        }
        else throw curr_lex;
        if (c_type == LEX_RPAREN) {
            flag_no_eq = false;
            flag_while = true;
            gl();
            O1();
            poliz.push_back(Lex(POLIZ_LABEL, pl0));
            poliz.push_back(Lex(POLIZ_GO));
            poliz[pl1] = Lex(POLIZ_LABEL, int(poliz.size()));
            if (!break_flag.empty()) {
                tmp = break_flag.top();
                break_flag.pop();
                poliz[tmp] = Lex(POLIZ_LABEL, int(poliz.size()));
            }
            flag_while = false;
            if (!flag_block) 
                throw curr_lex;
        }
    }
    else if (c_type == LEX_READ) {
        flag_no_eq = true;
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            if (c_type == LEX_ID) {
                check_id_in_read();
                poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
                gl();
            }
            else throw curr_lex;
            if (c_type == LEX_RPAREN) {
                poliz.push_back(Lex(LEX_READ));
                flag_no_eq = false;
                gl();
            }
            else throw curr_lex;
            if (c_type == LEX_SEMICOLON) {
                gl();
            }
            else throw curr_lex;
        }
        else throw curr_lex;
    }
    else if (c_type == LEX_WRITE) {
        flag_no_eq = true;
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            Oeq();
            while (c_type == LEX_COMMA) {
                gl();
                Oeq();
            }
            if (c_type == LEX_RPAREN) {
                poliz.push_back(Lex(LEX_WRITE));
                flag_no_eq = false;
                gl();
            }
            else throw curr_lex;
            if (c_type == LEX_SEMICOLON) {
                gl();
            }
            else throw curr_lex;
        }
        else throw curr_lex;
    }
    else if (c_type == LEX_BREAK) {

        if (!flag_while) {
            throw curr_lex;
        }
        break_flag.push(int(poliz.size()));
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_GO));
        gl();
        if (c_type == LEX_SEMICOLON) {
            gl();
        }
        else throw curr_lex;
    }
    else if (c_type == LEX_START) {
        gl();
        O1();
        flag_block = true;
        while (c_type != LEX_FINISH) {
            O1();
            if (!flag_block) {
                break;
            }
        }
        if (c_type == LEX_FINISH) {
            gl();
        }
        else throw curr_lex;
    }
    else {
        flag_block = false;
    }
}

void Parser::Oeq() {
    Oor();
    while (c_type == LEX_ASSIGN) {
        if (flag_no_eq) { throw curr_lex; }
        gl();
        Oor();
    }
}

void Parser::Oor() {
    Oand();
    while (c_type == LEX_OR) {
        st_lex.push(c_type);
        gl();
        Oand();
        check_op();
    }
}

void Parser::Oand() {
    E();
    while (c_type == LEX_AND) {
        st_lex.push(c_type);
        gl();
        E();
        check_op();
    }
}

void Parser::E() {
    Oplusminus();
    while (c_type == LEX_NEQ or c_type == LEX_LSS or c_type == LEX_GTR
        or c_type == LEX_GEQ or c_type == LEX_LEQ or c_type == LEX_EQ) {
        st_lex.push(c_type);
        gl();
        Oplusminus();
        check_op();
    }
}

void Parser::Oplusminus() {
    Oper();
    while (c_type == LEX_PLUS or c_type == LEX_MINUS) {
        st_lex.push(c_type);
        gl();
        Oper();
        check_op();
    }
}

void Parser::Oper() {
    Operand();
    while (c_type == LEX_SLASH or c_type == LEX_TIMES or c_type == LEX_REM) {
        st_lex.push(c_type);
        gl();
        Operand();
        check_op();
    }
}

void Parser::Operand() {
    int flag_minus = 1;
    if (c_type == LEX_PLUS or c_type == LEX_MINUS) {
        if (c_type == LEX_MINUS) {
            flag_minus = -1;
        }
        gl();
        if (c_type == LEX_NUM) {
            st_lex.push(LEX_INT);
            poliz.push_back(Lex(LEX_INT, c_val));
            gl();
            if (c_type == LEX_ASSIGN) {
                throw curr_lex;
            }
        }
        else if (c_type == LEX_REALBODY) {
            st_lex.push(LEX_REAL);
            poliz.push_back(Lex(LEX_REAL, 0, "", c_val_real));
            gl();
        }
        else throw curr_lex;
    }
    else if (c_type == LEX_ID) {
        check_id();
        poliz.push_back(Lex(LEX_ID, c_val, c_val_str, c_val_real));
        gl();
    }
    else if (c_type == LEX_STR) {
        st_lex.push(LEX_STRING);
        poliz.push_back(curr_lex);
        gl();
    }
    else if (c_type == LEX_NUM) {
        st_lex.push(LEX_INT);
        poliz.push_back(curr_lex);
        gl();
        if (c_type == LEX_ASSIGN) {
            throw curr_lex;
        }
    }
    else if (c_type == LEX_NOT) {
        gl();
        Operand();
        check_not();
    }
    else if (c_type == LEX_LPAREN) {
        gl();
        Oor();
        if (c_type == LEX_RPAREN) {
            gl();
        }
        else throw curr_lex;
    }
    else throw curr_lex;
}

void Parser::check_op() {
    type_of_lex t1, t2, op, t = LEX_INT, r = LEX_REAL, s = LEX_STRING;
    from_st(st_lex, t2);
    from_st(st_lex, op);
    from_st(st_lex, t1);
    if ((op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH) and (t1 != s or t2 != s)) {
        r = LEX_INT;
        if (t1 == LEX_REAL or t2 == LEX_REAL)
            r = LEX_REAL;
    }
    else if (t1 == t2 and t1 == s and (op == LEX_PLUS or op == LEX_ASSIGN)) {
        r = LEX_STRING;
    }
    else if (t1 == t2 and t1 == s and (op == LEX_EQ or op == LEX_NEQ or op == LEX_LSS or op == LEX_GTR)) {
        r = LEX_INT;
    }
    else if (t1 == t2 and t1 == s and (op == LEX_EQ or op == LEX_NEQ or op == LEX_LSS or op == LEX_GTR)) {
        r = LEX_INT;
    }
    else if ((t1 == LEX_REAL or t1 == LEX_INT) and (t2 == LEX_REAL or t2 == LEX_INT) and (op == LEX_EQ or op == LEX_NEQ or op == LEX_LSS or op == LEX_GTR or op == LEX_LEQ or op == LEX_GEQ)) {
        r = LEX_INT;
    }
    else if (t1 == LEX_INT and t1 == t2 and (op == LEX_AND or op == LEX_OR)) {
        r = LEX_INT;
    }
    else if (op == LEX_NOT and t1 == LEX_INT)
        r = LEX_INT;
    else if (op == LEX_ASSIGN) {
        if (t1 == t2 and t1 == LEX_INT)
            r = LEX_INT;
        if (t1 == t2 and t1 == LEX_REAL)
            r = LEX_REAL;
        if (t1 == LEX_INT and t2 == LEX_REAL)
            r = LEX_INT;
        if (t1 == LEX_REAL and t2 == LEX_INT)
            r = LEX_REAL;
    }
    else
        throw "Wrong types for the operation";
    st_lex.push(r);
    poliz.push_back(Lex(op));
}

void Parser::dec(type_of_lex type) {
    int i = 0;
    string s;
    while (!st_int.empty()) {
        from_st(st_int, i);
        if (TID[i].get_declare()) {
            s = "Redeclaration of the variable!";
            s += TID[i].get_name();
            throw s;
        }
        else {
            TID[i].put_declare();
            TID[i].put_type(type);
        }
    }
}

void Parser::eq_type() {
    type_of_lex t;
    from_st(st_lex, t);
    if (t != st_lex.top()) {
        throw "Wrong types: '='!";
    }
    st_lex.pop();
}

void Parser::check_not() {
    if (st_lex.top() != LEX_INT) {
        throw "Wrong type: 'not'!";
    }
    else {
        poliz.push_back(Lex(LEX_NOT));
    }
}

void Parser::check_id_in_read() const {
    string s;
    if (!TID[c_val].get_declare()) {
        s = "Variable is not declared: ";
        s += TID[c_val].get_name();
        throw s;
    }
}

void Parser::eq_bool() {
    if (st_lex.top() != LEX_INT) {
        throw "Expression is not int!";
    }
    st_lex.pop();
}

void Parser::check_id() {
    string s;
    if (TID[c_val].get_declare()) {
        st_lex.push(TID[c_val].get_type());
    }
    else {
        s = "Variable not declared: ";
        s += TID[c_val].get_name();
        throw s;
    }
}

int main() {
    try {
        Parser pars("test.txt");
        pars.analyze();
    }
    catch (char c) {
        cout << "unexpected symbol " << c << endl;
        return 1;
    }
    catch (Lex l) {
        cout << "unexpected lexeme" << l << endl;
        return 1;
    }
    catch (const char* source) {
        cout << source << endl;
        return 1;
    }
    catch (string source) {
        cout << source << endl;
        return 1;
    }
}
