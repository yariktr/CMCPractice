#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <stack>
#include <set>

using namespace std;

typedef set<int,less<int> > IntSet;
typedef set<float,less<float> > RealSet;
typedef set<string,less<string> > StringSet;

class LineException{
protected:
	int line;
public:
	int getLine() { return line; }
};

class US: public LineException{
	string symb;
public:
	US (string str, int l)  { symb = str; line = l;}
	string getSymb() { return symb; }  
};

class USAfter: public LineException{
	string last;
public:
	USAfter (string str, int l) { last = str; line = l;}
	string getLast() { return last; }  
};

class UEOF{
public:
	UEOF () {}
};

class SyntaxError: public LineException{
	string seText;
public:
	SyntaxError (string str, int l) { seText = str; line = l;}
	string getText() { return seText; }  
};

class SemanticError: public LineException{
	string seText;
public:
	SemanticError (string str, int l) { seText = str; line = l;}
	string getText() { return seText; }  
};

class RuntimeError{
	string reText;
public:
	RuntimeError (string str) { reText = str; }
	string getText() { return reText; }  
};

enum KindOfLex {IDENT = 0, NUMBER, REAL, STRINGC, KEYWORD, DELIM, LABEL}; 

class Buffer{
	string buf;
public:
	Buffer () { clear(); }
	Buffer (string str) { buf = str; }
	
	void add (char c) { buf += c; }
	void clear () { buf = ""; }
	string getString () const { return buf; } 
};
	
class Lex{
protected: 
	KindOfLex lexType;
	string type; 
public:
	virtual bool isEqual (Buffer& buf) = 0;
	bool isEqual (string str){
		Buffer buf(str);
		return isEqual(buf);
	}
	virtual string getString () const = 0;
	virtual string getStringVal () { return getString (); }
	bool isType (string str) { return type == str; }
	KindOfLex getKind () { return lexType; }
	virtual ~Lex() {}
};

class SmthNumber{  
public:
	virtual float getRealVal() = 0;
	virtual int getIntVal() = 0;
};

class SmthString{
public:
	virtual string getStringVal() = 0;
};

class Ident: public Lex, public SmthNumber, public SmthString { 
	string name;
	bool declared, assigned;
	bool lvalue;
	int valueInt;		
	float valueReal;
	string valueString;
	
public:
	Ident (Buffer &buf){
		name = buf.getString();
		declared = assigned = false;
		lexType = IDENT;
		lvalue = true;
	}	
	Ident (int val){
		name = "";
		declared = assigned = true;
		lexType = IDENT;
		lvalue = false;
		valueInt = val;
	}
	Ident (float val){
		name = "";
		declared = assigned = true;
		lexType = IDENT;
		lvalue = false;
		valueReal = val;
	}
	Ident (string val){
		name = "";
		declared = assigned = true;
		lexType = IDENT;
		lvalue = false;
		valueString = val;
	}
	
	bool isEqual (Buffer &buf) { return name == buf.getString(); }
	string getString () const { return name; }
	string getType (){ return type; }
	
	int getIntVal () { 
		if (!assigned) throw RuntimeError ("Using value of '"+name+"' was not assigned!");
		if (type == "int") return valueInt;  return (int)valueReal; }
	float getRealVal () { 
		if (!assigned) throw RuntimeError ("Using value of '"+name+"' was not assigned!");
		if (type == "int") return valueInt;  return valueReal; }
	string getStringVal (){ 
		if (!assigned) throw RuntimeError ("Using value of '"+name+"' was not assigned!");
		if (type == "string") return valueString; 
		if (type == "real") {stringstream ss;  string str;  ss<<valueReal;  ss>>str;  return str;} 
		if (type == "int") {stringstream ss;  string str;  ss<<valueInt;  ss>>str;  return str;}
		return ""; }
	bool isDeclared() { return declared; }
	bool isAssigned() { return assigned; }
	bool isLvalue() { return lvalue; }
	
	void declare (string t) { declared = true; type = t; }
	void assign (int ass) { valueReal = valueInt = ass; assigned = true; }
	void assign (float ass) { valueReal = ass; valueInt = (int)ass; assigned = true; }
	void assign (string ass) {valueString = ass; assigned = true; }
};

class Number: public Lex, public SmthNumber {
	int value;
public:
	Number (Buffer &buf){
		stringstream ss;
		ss<<buf.getString();
		ss>>value;
		type = "int";
		lexType = NUMBER;
	}
	Number (int num){
		value = num;
		type = "int";
		lexType = NUMBER;
	}
	bool isEqual (Buffer &buf) { 
		stringstream ss;
		int bufVal;
		ss<<buf.getString();
		ss>>bufVal;
		return value == bufVal; }
	string getString () const { 
		stringstream ss;  string str;
		ss<<value;        ss>>str;
		return str; }
	int getIntVal() { return value; }
	float getRealVal() { return value; }
	void neg() { value = -value; } 
};

class Real: public Lex, public SmthNumber {
	float value;
public:
	Real (Buffer &buf){
		stringstream ss;
		ss<<buf.getString();
		ss>>value;
		type = "real";
		lexType = REAL;
	}
	Real (float val){
		value = val;
		type = "real";
		lexType = REAL;
	}
	bool isEqual (Buffer &buf) { 
		stringstream ss;
		float bufVal;
		ss<<buf.getString();
		ss>>bufVal;
		return value == bufVal; }
	string getString () const { 
		stringstream ss;  string str;
		ss<<value;        ss>>str;
		return str; }
	float getRealVal() { return value; }
	int getIntVal() { return (int)value; }
	void neg() { value = -value; } 
};

class StringC: public Lex, public SmthString{  
	string word;
public:
	StringC (string str) { word = str; lexType = STRINGC; type = "string";}
	bool isEqual(Buffer &buf) { 
		return word == buf.getString();
	}
	string getString () const { return word; }
	string getStringVal (){ return word; }
};

class Keyword: public Lex{ 
	string word;
public:
	Keyword (string str) { word = str; lexType = KEYWORD; }
	bool isEqual(Buffer &buf) { 
		return word == buf.getString();
	}
	string getString () const { return word; }
};

class Delim: public Lex{ 
	string word;
public:
	Delim (string str) { word = str; lexType = DELIM;}
	bool isEqual(Buffer &buf) { 
		return word == buf.getString();
	}
	string getString () const { return word; }
};

class Label: public Lex { 
	int value;
public:
	Label (Buffer &buf){
		stringstream ss;
		ss<<buf.getString();
		ss>>value;
		lexType = LABEL;
	}
	Label (int addr){
		value = addr;
		lexType = LABEL;
	}
	bool isEqual (Buffer &buf) { 
		stringstream ss;
		int bufVal;
		ss<<buf.getString();
		ss>>bufVal;
		return value == bufVal; }
	string getString () const { 
		stringstream ss;  string str;
		ss<<value;        ss>>str;
		return str; }
	int getAddress() { return value; }
};

class HashMapLex {  
	static int full; 
	static int spreadRate;
	int size;           
	int count;     
	vector<Lex*> data; 
	
	int hash (string str){
		int res = 97*str[0];
		for (unsigned i = 1; i < str.size(); i++) res += str[i];
		return res % size;
	}
	
	void rebuild ();
	
public:
	HashMapLex (int s){
		size = s;
		data.resize(size);
		count = 0;
		for (vector<Lex*>::iterator i = data.begin(); i != data.end(); i++) *i = NULL; 
	}
	~HashMapLex (){
		for (int i = 0; i < size; i++) { delete data[i]; }
	}
	
	Lex* operator[] (Buffer buf);  
	Lex* tryToAdd (Lex* elem); 
	
	void print(){  // cerr<<table
		for (int i = 0; i < size; i++) {
			if (data[i] == NULL) cerr<<i<<" NULL"<<endl;
			else
				cerr<<i<<" "<<hash(data[i] -> getString())<<" "<<data[i] -> getString()<<endl;
		}
	}
	
	void save(ofstream &out){
		out<<count*100/full + 1<<" "<<count<<endl;
		for (int i = 0; i < size; i++) if (data[i] != NULL)  out<<data[i] -> getString()<<endl;
	}
};

class Scanner{
	
	ifstream proga;
	char curr;   
	int currLine;
	
	HashMapLex identTab;
	HashMapLex numberTab;
	HashMapLex realTab;
	HashMapLex stringTab;
	HashMapLex keywordTab;
	HashMapLex delimTab;
	
	void pop(){ if (proga.eof()) throw UEOF(); proga.get(curr); }
	
	bool isJunk() { return curr == ' ' || curr == '\t' || curr == '\n';}	
	bool isEnter() { return curr == '\n';}	
	bool isAlpha() { return curr >= 'a' && curr <= 'z' || curr >= 'A' && curr <= 'Z';}
	bool isNum() { return curr >= '0' && curr <= '9'; }
	bool isDot() { return curr == '.'; }
	bool isAlphaOrNum() { return isAlpha() || isNum(); }
	bool isDelimFst() { return curr == '<' || curr == '>' || curr == '=';}
	bool isEq() { return curr == '=';}
	bool isExclam() { return curr == '!';}
	bool isSlash() { return curr == '/';}
	bool isAst() { return curr == '*';}
	bool isQuote() { return curr == '"';}
	bool isOpenBracket() { return curr == '(';}
	bool isCloseBracket() { return curr == ')';}
	bool isOpenBrace() { return curr == '{';}
	bool isCloseBrace() { return curr == '}';}
	
public:
	Scanner (const char* file);
	Lex* getLex(); 
	int getLine() { return currLine; }
	Lex* findDelim (string str) { return delimTab[str]; }
	Lex* findKeyword (string str) { return keywordTab[str]; }
	void showTablesToLarisaNikolaevna() {
		cerr<<"Identifier table:"<<endl;    identTab.print();   
		cerr<<"Real table:"<<endl;    realTab.print();  
		cerr<<"String table:"<<endl;    stringTab.print();  
	}
};

class Poliz {  
	vector<Lex*> code;
public:
	Lex* &operator[] (int i) { return code[i]; }
	
	int push(Lex* lex) { code.push_back(lex);   return code.size() - 1; }
	void go();
	
	void print() { for (unsigned i = 0; i < code.size(); i++) cout<<code[i]->getString()<<" "; }
};

class Parser {
	Scanner s;
	Lex* curr;     
	string currType;  
	Poliz *poliz;
	stack<int> cycbeg;
	bool lval; 
	
	void pop() { curr = s.getLex(); }
	
	void parseProgram();
	void parseDeclarations();
	void parseDeclaration();
	void parseType();
	void parseVariable();
	void parseIdent();
	Lex* parseConstant();

	void parseOperators();
	void parseOperator();
	void parseVariantList(Ident* expr);
	int parseVariant(IntSet &prev, Ident* expr, int addr); 
	int parseVariant(RealSet &prev, Ident* expr, int addr);
	int parseVariant(StringSet &prev, Ident* expr, int addr);
	void parseComplexOperator();
	Ident* parseExpression();
	string parseDisjunction(); 
	string parseConjunction();
	string parseComparison();
	string parseSum();
	string parseProduct();
	string parseUnary();
	
public:
	Parser (const char *file, Poliz &p): s(file) { 
		curr = s.getLex(); 
		poliz = &p; }
	bool go ();
};

int HashMapLex::full = 70;
int HashMapLex::spreadRate = 2;

Lex* HashMapLex::operator[] (Buffer buf){
			int i, h;
			for (i = h = hash(buf.getString()); i < size; i++){
				if (data[i] == NULL) return NULL;
				if (data[i]->isEqual(buf)) return data[i];
			}
			for (i = 0; i < h; i++){
				if (data[i] == NULL) return NULL;
				if (data[i]->isEqual(buf)) return data[i];
			}
			return NULL;
}
Lex* HashMapLex::tryToAdd (Lex* elem){
	if (this -> operator[] (Buffer(elem->getString())) == NULL){
		int i;
		for (i = hash(elem->getString()); i < size && data[i]; i++);
		if (data[i]) for (i = 0; data[i]; i++);
		data[i] = elem;
		if (100 * ++count >= full * size) rebuild();		
	}
	return elem;
}

void HashMapLex::rebuild (){
	try{  data.resize (size *= spreadRate); } 
	catch ( ... ) { cerr<<"Out of memory! Improve your machine!";}
	Lex* lex;
	for (int i = 0; i < size / spreadRate; i++) if (data[i] != NULL){
		lex = data[i];   data[i] = NULL;  --count;
		tryToAdd (lex);
	}
}

Scanner::Scanner (const char* file): identTab(64), numberTab(64), realTab(64), stringTab(32), keywordTab(64), delimTab(64){
	proga.open(file);
	currLine = 1;
	
	delimTab.tryToAdd(new Delim("{"));
	delimTab.tryToAdd(new Delim("}"));
	delimTab.tryToAdd(new Delim(","));
	delimTab.tryToAdd(new Delim(";"));
	delimTab.tryToAdd(new Delim(":"));
	delimTab.tryToAdd(new Delim("("));
	delimTab.tryToAdd(new Delim(")"));
	delimTab.tryToAdd(new Delim("="));
	delimTab.tryToAdd(new Delim("+"));
	delimTab.tryToAdd(new Delim("-"));
	delimTab.tryToAdd(new Delim("*"));
	delimTab.tryToAdd(new Delim("/"));
	delimTab.tryToAdd(new Delim("<"));
	delimTab.tryToAdd(new Delim(">"));
	delimTab.tryToAdd(new Delim("<="));
	delimTab.tryToAdd(new Delim(">="));
	delimTab.tryToAdd(new Delim("=="));
	delimTab.tryToAdd(new Delim("!="));
	
	delimTab.tryToAdd(new Delim("backspace")); 
	delimTab.tryToAdd(new Delim("jump"));     
	delimTab.tryToAdd(new Delim("jumpfalse"));
	delimTab.tryToAdd(new Delim("empty"));    
	delimTab.tryToAdd(new Delim("dup"));
	delimTab.tryToAdd(new Delim("~"));

	keywordTab.tryToAdd(new Keyword("program"));
	keywordTab.tryToAdd(new Keyword("int"));
	keywordTab.tryToAdd(new Keyword("real"));
	keywordTab.tryToAdd(new Keyword("string"));
	keywordTab.tryToAdd(new Keyword("not"));
	keywordTab.tryToAdd(new Keyword("and"));
	keywordTab.tryToAdd(new Keyword("or"));
	keywordTab.tryToAdd(new Keyword("if"));
	keywordTab.tryToAdd(new Keyword("else"));
	keywordTab.tryToAdd(new Keyword("case"));
	keywordTab.tryToAdd(new Keyword("of"));
	keywordTab.tryToAdd(new Keyword("while"));
	keywordTab.tryToAdd(new Keyword("for"));
	keywordTab.tryToAdd(new Keyword("continue"));
	keywordTab.tryToAdd(new Keyword("read"));
	keywordTab.tryToAdd(new Keyword("write"));
	keywordTab.tryToAdd(new Keyword("end"));

	pop();
}

Lex* Scanner::getLex(){
	Buffer buf;
	Lex *res = NULL;
	while (isJunk()) { if (isEnter()) ++currLine; pop();}
	if (isAlpha()) {  
		buf.add(curr);   pop();
		while (isAlphaOrNum()) { buf.add(curr);   pop(); };
		if ((res = keywordTab[buf.getString()]) != NULL) return res;
		if ((res = identTab[buf.getString()]) != NULL) return res;
		return identTab.tryToAdd(new Ident(buf));
	} 
	if (isNum()) {
		buf.add(curr);   pop();
		bool real = false;
		while (isNum()||isDot()) { 
			if (isDot()) if (real) break;
			    else real = true;
			buf.add(curr);   pop(); };
		if (real) if ((res = realTab[buf.getString()]) != NULL) return res;
				else return realTab.tryToAdd(new Real(buf));
		   else {
			   if ((res = numberTab[buf.getString()]) != NULL) return res;
		   }
		return numberTab.tryToAdd(new Number(buf));
	}
	if (isDelimFst()){  
		buf.add(curr);   pop();
		if (isEq()) {
			buf.add(curr);   pop();
			return delimTab[buf.getString()];
		}
		return delimTab[buf.getString()];
	}
	if (isExclam()){   
		buf.add(curr);   pop();
		if (isEq()) {
			buf.add(curr);   pop();
			return delimTab[buf.getString()];
		}
		throw USAfter("!",currLine);
	}
	if (isSlash()){ 
		buf.add(curr);   pop();
		if (isAst()){ 
			while (!isAst()||(pop(),!isSlash())) pop(); 
			pop();
			return getLex();
		}
		return delimTab[buf.getString()];
	}
	if (isQuote()){ 
		pop();   
		while (!isQuote()){ buf.add(curr);   pop(); } 
		pop();
		if ((res = stringTab[buf.getString()]) != NULL) return res;
		return stringTab.tryToAdd(new StringC(buf.getString()));
	}

	buf.add(curr); 
	if ((res = delimTab[buf.getString()]) != NULL) 
		{ pop(); return res;}
	throw US(buf.getString(),currLine);
}

#define BIN_NUM_OP( sgn ) \
if (code[i]->isEqual(#sgn)) { \
	op1 = actoynick.top();   actoynick.pop(); \
	op2 = actoynick.top();   actoynick.pop(); \
	if (op1->isType("int") && op2->isType("int")) \
		actoynick.push (new Number(dynamic_cast<SmthNumber*>(op2)->getIntVal()sgn \
			dynamic_cast<SmthNumber*>(op1)->getIntVal())); \
	if (op1->isType("real") || op2->isType("real")) \
		actoynick.push (new Real(dynamic_cast<SmthNumber*>(op2)->getRealVal()sgn \
			dynamic_cast<SmthNumber*>(op1)->getRealVal())); \
	break;}
	
#define BIN_INT_NUM_OP(sgn) \
if (code[i]->isEqual(#sgn)) { \
	op1 = actoynick.top();   actoynick.pop(); \
	op2 = actoynick.top();   actoynick.pop(); \
	actoynick.push (new Number(dynamic_cast<SmthNumber*>(op2)->getIntVal()sgn \
		dynamic_cast<SmthNumber*>(op1)->getIntVal())); \
	break;}
	
#define BIN_NUM_STR_OP(sgn) \
if (code[i]->isEqual(#sgn)) { \
	op1 = actoynick.top();   actoynick.pop(); \
	op2 = actoynick.top();   actoynick.pop(); \
	if (op1->isType("int") && op2->isType("int")) \
		actoynick.push (new Number(dynamic_cast<SmthNumber*>(op2)->getIntVal()sgn \
			dynamic_cast<SmthNumber*>(op1)->getIntVal())); \
	if (op1->isType("real") || op2->isType("real")) \
		actoynick.push (new Real(dynamic_cast<SmthNumber*>(op2)->getRealVal()sgn \
			dynamic_cast<SmthNumber*>(op1)->getRealVal())); \
	if (op1->isType("string")) \
		actoynick.push (new StringC(dynamic_cast<SmthString*>(op2)->getStringVal()sgn\
			dynamic_cast<SmthString*>(op1)->getStringVal())); \
	break;}
	
#define BIN_COMP_OP(sgn) \
if (code[i]->isEqual(#sgn)) { \
	op1 = actoynick.top();   actoynick.pop(); \
	op2 = actoynick.top();   actoynick.pop(); \
	if (op1->isType("int") && op2->isType("int")) \
		actoynick.push (new Number(dynamic_cast<SmthNumber*>(op2)->getIntVal() sgn \
			dynamic_cast<SmthNumber*>(op1)->getIntVal())); \
	if (op1->isType("real") || op2->isType("real")) \
		actoynick.push (new Number(dynamic_cast<SmthNumber*>(op2)->getRealVal() sgn \
			dynamic_cast<SmthNumber*>(op1)->getRealVal())); \
	if (op1->isType("string")) \
		actoynick.push (new Number(dynamic_cast<SmthString*>(op2)->getStringVal() sgn \
			dynamic_cast<SmthString*>(op1)->getStringVal())); \
	break;}

void Poliz::go(){
	stack<Lex*> actoynick;
	Lex *op1, *op2;
	for (unsigned i = 0; i < code.size(); i++){
		switch (code[i]->getKind()){
			case IDENT:
			case NUMBER:
			case REAL:
			case STRINGC:
			case LABEL:
				actoynick.push(code[i]);
			break;
			case KEYWORD:
			case DELIM:
				BIN_NUM_OP(*)
				if (code[i]->isEqual("/")) { 
					op1 = actoynick.top();   actoynick.pop(); 
					op2 = actoynick.top();   actoynick.pop(); 
					if (dynamic_cast<SmthNumber*>(op1)->getRealVal() == 0)
						throw RuntimeError ("Division by zero!");
					if (op1->isType("int") && op2->isType("int")) 
						actoynick.push (new Number(dynamic_cast<SmthNumber*>(op2)->getIntVal()/
							dynamic_cast<SmthNumber*>(op1)->getIntVal())); 
					if (op1->isType("real") || op2->isType("real")) 
						actoynick.push (new Real(dynamic_cast<SmthNumber*>(op2)->getRealVal()/ 
							dynamic_cast<SmthNumber*>(op1)->getRealVal())); 
					break;}
				BIN_NUM_OP(-)
				BIN_NUM_STR_OP(+)
				if (code[i]->isEqual("=")) { 
					op1 = actoynick.top();   actoynick.pop(); 
					op2 = actoynick.top();   actoynick.pop(); 
					if (op1->isType("int") && op2->isType("int")){ 
						((Ident*)op2)->assign(dynamic_cast<SmthNumber*>(op1)->getIntVal());
						actoynick.push (op2); }
					if (op1->isType("real") || op2->isType("real")) {
						((Ident*)op2)->assign(dynamic_cast<SmthNumber*>(op1)->getRealVal());
						actoynick.push (op2); }
					if (op1->isType("string")) {
						((Ident*)op2)->assign(dynamic_cast<SmthString*>(op1)->getStringVal());
						actoynick.push (op2); } 
					break;}
				BIN_COMP_OP(<)
				BIN_COMP_OP(>)
				BIN_COMP_OP(<=)
				BIN_COMP_OP(>=)
				BIN_COMP_OP(==)
				BIN_COMP_OP(!=)
				BIN_INT_NUM_OP(and)
				BIN_INT_NUM_OP(or)
				if (code[i]->isEqual("not")){
					op1 = actoynick.top();   actoynick.pop(); 
					actoynick.push (new Number(!(dynamic_cast<SmthNumber*>(op1)->getIntVal())));
					break;
				}
				if (code[i]->isEqual("read")) { 
					op1 = actoynick.top();   actoynick.pop(); 
					if (op1->isType("int")){
						int tmp;  cin>>tmp;
						((Ident*)op1)->assign(tmp);  break;
					}
					if (op1->isType("real")){
						float tmp;  cin>>tmp;
						((Ident*)op1)->assign(tmp);  break;
					}
					if (op1->isType("string")){
						string tmp = "";  while (tmp == "") getline(cin,tmp);
						((Ident*)op1)->assign(tmp);  break;
					}
				}
				if (code[i]->isEqual("write")) { 
					op1 = actoynick.top();   actoynick.pop(); 
					int num = ((Number*)op1)->getIntVal();
					vector<Lex*> superActoynick(num);
					for (int i = 0; i < num; i++){
						superActoynick[i] = actoynick.top();   actoynick.pop(); 
					}
					for (int i = num - 1; i >= 0; i--)	cout<<superActoynick[i]->getStringVal();
					break;
				}
				if (code[i]->isEqual("backspace")) { 
					actoynick.pop();   break;
				}
				if (code[i]->isEqual("jump")) { 
					op1 = actoynick.top();   actoynick.pop(); 
					i = ((Label*)op1)->getAddress();
					break;
				}
				if (code[i]->isEqual("jumpfalse")) { 
					op1 = actoynick.top();   actoynick.pop(); 
					op2 = actoynick.top();   actoynick.pop();
					if (!((Number*)op2)->getIntVal()) i = ((Label*)op1)->getAddress();
					break;
				}
				if (code[i]->isEqual("dup")) { 
					actoynick.push (actoynick.top());
					break;
				}
				if (code[i]->isEqual("~")) { 
					op1 = actoynick.top();   actoynick.pop(); 
					if (op1->isType("int")) 
						actoynick.push (new Number(- dynamic_cast<SmthNumber*>(op1)->getIntVal())); 
					break;
					if (op1->isType("real")) 
						actoynick.push (new Real(- dynamic_cast<SmthNumber*>(op1)->getRealVal())); 
					break;
				}
		}
	}
}

bool Parser::go(){
	parseProgram();
	return true;
}

void Parser::parseProgram(){
	if (!curr->isEqual("program")) throw SyntaxError ("Syntax error after 'program'!",s.getLine());
	    else pop();
	if (!curr->isEqual("{")) throw SyntaxError ("Syntax error after 'program'!",s.getLine());
		else pop();
	parseDeclarations();
	parseOperators();
	if (!curr->isEqual("}")) throw SyntaxError ("Syntax error in the end",s.getLine());
}

void Parser::parseDeclarations(){
		while (curr->isEqual("int") || curr->isEqual("real") || curr->isEqual("string")) 
			{ parseDeclaration();
			if (!curr->isEqual(";")) 
				throw SyntaxError ("semicolon is expexted but '" + curr->getString() +"' found!",s.getLine());
			else pop(); }
}

void Parser::parseDeclaration(){
	parseType();
	parseVariable();
	while (curr->isEqual(",")) { pop(); parseVariable(); 	}
}

void Parser::parseType(){
	currType = curr->getString();
	pop();
}

void Parser::parseVariable(){
	if (!curr->getKind() == IDENT) 
		throw SyntaxError("Identifier is expected but " + curr->getString() + "' found!",s.getLine());
	Ident* id = (Ident*)curr;
	if (id->isDeclared()) throw SemanticError("Redeclaration of '"+ curr->getString() + "'!",s.getLine());
	id->declare(currType);   pop();
	if (curr->isEqual("=")){
		pop(); 
		curr->getString();
		if (curr->getKind() == STRINGC && id->getType() == "string") 
			id->assign(curr->getString());   
		else {
			int sign = 1;
			if (curr->isEqual("+")) { pop(); }
			else if (curr->isEqual("-")) { sign = -1;  pop(); }
			if (curr->getKind() == NUMBER && (id->getType() == "real" || id->getType() == "int")) {
				id->assign(dynamic_cast<SmthNumber*>(curr)->getIntVal() * sign);			
			}
			else if (curr->getKind() == REAL && id->getType() == "real") 
				id->assign(dynamic_cast<SmthNumber*>(curr)->getRealVal() * sign);
			else throw SyntaxError ("Constant is expexted but '" + curr->getString() +"' found!",s.getLine());
		}
		pop();
	}
}

void Parser::parseIdent(){
	if (curr->getKind() != IDENT) 
		throw SyntaxError ("Identifier expected but '" + curr->getString() +"'found!",s.getLine());
	if (!((Ident*)curr)->isDeclared()) 
		throw SemanticError ("Undeclared identifier: '" + curr->getString() +"'!",s.getLine());
	poliz->push(curr);
	pop();
}

Lex* Parser::parseConstant(){
	Lex* res;
	bool nega = false;
	if (curr->getKind() == STRINGC) 
		res = curr;
	else {
		curr->getString();
		if (curr->isEqual("+")) { pop(); }
		else if (curr->isEqual("-")) {  nega = true; pop(); }
		if (curr->getKind() == NUMBER) 
			{res = curr;  if (nega)  ((Number*)curr)->neg();}
		else if (curr->getKind() == REAL)  {res = curr;  if (nega)  ((Number*)curr)->neg();}
		else throw SyntaxError ("Constant is expexted but '" + curr->getString() +"' found!",s.getLine());
	}
	poliz->push(curr);
	pop();  return res;
}

void Parser::parseOperators(){
	while (!curr->isEqual("}")) { 
		parseOperator(); }
}

void Parser::parseOperator(){
curr->getString();
	Ident* res;
	if (curr->isEqual("}")) return;
	if (curr->isEqual("if")) { pop(); 
		if (!curr->isEqual("(")) 
			throw SyntaxError ("'(' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		res = parseExpression();
		if (!res->isType("int")) throw SemanticError ("Expression in 'if' should be ingeger!",s.getLine());
		if (!curr->isEqual(")")) 
			throw SyntaxError ("')' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		int pElse = poliz->push(s.findDelim("empty")); 
		poliz->push(s.findDelim("jumpfalse"));
		parseOperator();
		int pEnd = poliz->push(s.findDelim("empty"));  
		poliz->push(s.findDelim("jump"));
		int tmp = poliz->push(s.findDelim("empty"));
		(*poliz)[pElse] = new Label(tmp);
		if (!curr->isEqual("else")) 
			throw SyntaxError ("'else' is expected but '" + curr->getString() +"' found!",s.getLine());  pop();
		parseOperator();
		tmp = poliz->push(s.findDelim("empty"));
		(*poliz)[pEnd] = new Label(tmp);
		return; }
	if (curr->isEqual("while")) { pop();
		if (!curr->isEqual("(")) 
			throw SyntaxError ("'(' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		int lBeg = poliz->push(s.findDelim("empty"));  
		res = parseExpression();
		if (!res->isType("int")) throw SemanticError ("Expression in 'while' should be ingeger!",s.getLine());
		if (!curr->isEqual(")")) 
			throw SyntaxError ("')' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		int pEnd = poliz->push(s.findDelim("empty")); 
		poliz->push(s.findDelim("jumpfalse"));
		cycbeg.push(lBeg);
		parseOperator();
		cycbeg.pop();
		poliz->push(new Label(lBeg)); 
		poliz->push(s.findDelim("jump"));
		int tmp = poliz->push(s.findDelim("empty"));
		(*poliz)[pEnd] = new Label(tmp);
		return; }
	if (curr->isEqual("for")) { pop();
		if (!curr->isEqual("(")) 
			throw SyntaxError ("'(' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		if (!curr->isEqual(";")) parseExpression();
		if (!curr->isEqual(";"))
			throw SyntaxError ("semicolon is expexted but '" + curr->getString() +"' found!",s.getLine());  pop(); 
		int lBeg = poliz->push(s.findDelim("empty"));
		if (!curr->isEqual(";")) res = parseExpression();   
		   else { res = NULL; poliz->push (new Number(1)); }
		if (!curr->isEqual(";")) 
			throw SyntaxError ("semicolon is expexted but '" + curr->getString() +"' found!",s.getLine());  pop(); 
		if (res && !res->isType("int")) throw SemanticError ("The second expression in 'for' should be ingeger!",s.getLine());
		int pEnd = poliz->push(s.findDelim("empty"));
		poliz->push(s.findDelim("jumpfalse"));
		int pOp = poliz->push(s.findDelim("empty"));  
		poliz->push(s.findDelim("jump"));
		int lInc = poliz->push(s.findDelim("empty")); 
		if (!curr->isEqual(")")) parseExpression();
		if (!curr->isEqual(")")) 
			throw SyntaxError ("')' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		poliz->push(new Label(lBeg)); 
		poliz->push(s.findDelim("jump"));
		int tmp = poliz->push(s.findDelim("empty"));
		(*poliz)[pOp] = new Label(tmp);
		cycbeg.push(lInc);
		parseOperator();
		cycbeg.pop();
		poliz->push(new Label(lInc));
		poliz->push(s.findDelim("jump"));
		tmp = poliz->push(s.findDelim("empty"));
		(*poliz)[pEnd] = new Label(tmp);
		return; }
	if (curr->isEqual("case")) { pop();
		if (!curr->isEqual("(")) 
			throw SyntaxError ("'(' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		res = parseExpression();
		if (!curr->isEqual(")")) 
			throw SyntaxError ("')' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		if (!curr->isEqual("of")) 
			throw SyntaxError ("'of' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		parseVariantList(res);
		if (!curr->isEqual("end")) 
			throw SyntaxError ("'end' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		if (!curr->isEqual(";")) 
			throw SyntaxError ("semicolon is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();  
		return; }
	if (curr->isEqual("read")) { pop();
		if (!curr->isEqual("(")) 
			throw SyntaxError ("'(' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		parseIdent();
		if (!curr->isEqual(")")) 
			throw SyntaxError ("')' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		if (!curr->isEqual(";")) 
			throw SyntaxError ("semicolon is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();  
		poliz->push(s.findKeyword("read"));
		return; }
	if (curr->isEqual("write")) { pop();
		if (!curr->isEqual("(")) 
			throw SyntaxError ("'(' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		parseExpression();
		int num = 1;
		while (curr->isEqual(",")) { pop(); parseExpression(); ++num; }
		if (!curr->isEqual(")")) 
			throw SyntaxError ("')' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
		if (!curr->isEqual(";")) 
			throw SyntaxError ("semicolon is expexted but '" + curr->getString() +"' found!",s.getLine());    pop();
		poliz->push(new Number(num));
		poliz->push(s.findKeyword("write"));
		return; }
	if (curr->isEqual("continue")) { pop();
			if (cycbeg.empty()) throw SemanticError ("Not inner-cycle 'continue'!",s.getLine());
			poliz->push(new Label(cycbeg.top()));
			poliz->push(new Delim("jump"));
			if (!curr->isEqual(";")) 
				throw SyntaxError ("semicolon is expexted but '" + curr->getString() +"' found!",s.getLine());    pop();
		return;
	}
	if (curr->isEqual("{")) parseComplexOperator();
	else { parseExpression();  
	    if (!curr->isEqual(";")) 
			throw SyntaxError ("semicolon is expexted but '" + curr->getString() +"' found!",s.getLine()); pop();}
}

void Parser::parseVariantList(Ident* expr){
	if (expr->isType("int")){
		IntSet is;  int addr = parseVariant(is,expr,0);
		while (!curr->isEqual("end")) addr = parseVariant(is,expr,addr); 
		int tmp = poliz->push(s.findDelim("empty"));
		if (addr) (*poliz)[addr] = new Label(tmp);}
	if (expr->isType("real")){
		RealSet rs;  int addr = parseVariant(rs,expr,0);
		while (!curr->isEqual("end")) addr = parseVariant(rs,expr,addr); 
		int tmp = poliz->push(s.findDelim("empty"));
		if (addr) (*poliz)[addr] = new Label(tmp);}
	if (expr->isType("string")){
		StringSet ss;  int addr = parseVariant(ss,expr,0);
		while (!curr->isEqual("end")) addr = parseVariant(ss,expr,addr); 
		int tmp = poliz->push(s.findDelim("empty"));
		if (addr) (*poliz)[addr] = new Label(tmp); }
	if (!curr->isEqual("end")) 
		throw SyntaxError ("'end' is expexted but '" + curr->getString() +"' found!",s.getLine()); 
}

int Parser::parseVariant(IntSet &prev, Ident* expr, int addr){
	int nextAddr;
	stack<int> begOp;
	int tmp = poliz->push(s.findDelim("empty"));
	if (addr) (*poliz)[addr] = new Label(tmp);
	Lex* res = parseConstant();
	if (!res->isType("int")) throw SemanticError ("Type mismatch in 'case'!",s.getLine());
	pair<IntSet::const_iterator,bool> p;
	p = prev.insert(((Number*)res)->getIntVal());
	if (!p.second) throw SemanticError("Double constant declaring!",s.getLine());
	poliz->push(expr);  poliz->push(new Delim("!="));
	begOp.push(poliz->push(s.findDelim("empty")));
	poliz->push(s.findDelim("jumpfalse"));
	while (!curr->isEqual(":")){
		if (!curr->isEqual(",")) 
			throw SyntaxError ("',' or ':' is expexted but '" + curr->getString() +"' found!",s.getLine()); pop();
		res = parseConstant();
		if (!res->isType("int")) throw SemanticError ("Type mismatch in 'case'!",s.getLine());
		p = prev.insert(((Number*)res)->getIntVal());
		if (!p.second) throw SemanticError("Double constant declaring!",s.getLine());
		poliz->push(expr);  poliz->push(new Delim("!="));
		begOp.push(poliz->push(s.findDelim("empty")));
		poliz->push(s.findDelim("jumpfalse"));
	}
	pop();
	nextAddr = poliz->push(s.findDelim("empty"));
	poliz->push(s.findDelim("jump"));
	tmp = poliz->push(s.findDelim("empty"));
	while (!begOp.empty()){ (*poliz)[begOp.top()] = new Label(tmp);  begOp.pop(); }
	parseOperator();
	return nextAddr;
}

int Parser::parseVariant(RealSet &prev, Ident* expr, int addr){
	int nextAddr;
	stack<int> begOp;
	int tmp = poliz->push(s.findDelim("empty"));
	if (addr) (*poliz)[addr] = new Label(tmp);
	Lex* res = parseConstant();
	if (!res->isType("real")) throw SemanticError ("Type mismatch in 'case'!",s.getLine());
	pair<RealSet::const_iterator,bool> p;
	p = prev.insert(((Real*)res)->getRealVal());
	if (!p.second) throw SemanticError("Double constant declaring!",s.getLine());
	poliz->push(expr);  poliz->push(new Delim("!="));
	begOp.push(poliz->push(s.findDelim("empty")));
	poliz->push(s.findDelim("jumpfalse"));
	while (!curr->isEqual(":")){
		if (!curr->isEqual(",")) 
			throw SyntaxError ("',' or ':' is expexted but '" + curr->getString() +"' found!",s.getLine()); pop();
		res = parseConstant();
		if (!res->isType("real")) throw SemanticError ("Type mismatch in 'case'!",s.getLine());
		p = prev.insert(((Real*)res)->getRealVal());
		if (!p.second) throw SemanticError("Double constant declaring!",s.getLine());
		poliz->push(expr);  poliz->push(new Delim("!="));
		begOp.push(poliz->push(s.findDelim("empty")));
		poliz->push(s.findDelim("jumpfalse"));
	}
	pop();
	nextAddr = poliz->push(s.findDelim("empty"));
	poliz->push(s.findDelim("jump"));
	tmp = poliz->push(s.findDelim("empty"));
	while (!begOp.empty()){ (*poliz)[begOp.top()] = new Label(tmp);  begOp.pop(); }
	parseOperator();
	return nextAddr;
}

int Parser::parseVariant(StringSet &prev, Ident* expr, int addr){
	int nextAddr;
	stack<int> begOp;
	int tmp = poliz->push(s.findDelim("empty"));
	if (addr) (*poliz)[addr] = new Label(tmp);
	Lex* res = parseConstant();
	if (!res->isType("string")) throw SemanticError ("Type mismatch in 'case'!",s.getLine());
	pair<StringSet::const_iterator,bool> p;
	p = prev.insert(((StringC*)res)->getStringVal());
	if (!p.second) throw SemanticError("Double constant declaring!",s.getLine());
	poliz->push(expr);  poliz->push(new Delim("!="));
	begOp.push(poliz->push(s.findDelim("empty")));
	poliz->push(s.findDelim("jumpfalse"));
	while (!curr->isEqual(":")){
		if (!curr->isEqual(",")) 
			throw SyntaxError ("',' or ':' is expexted but '" + curr->getString() +"' found!",s.getLine()); pop();
		res = parseConstant();
		if (!res->isType("string")) throw SemanticError ("Type mismatch in 'case'!",s.getLine());
		p = prev.insert(((StringC*)res)->getStringVal());
		if (!p.second) throw SemanticError("Double constant declaring!",s.getLine());
		poliz->push(expr);  poliz->push(new Delim("!="));
		begOp.push(poliz->push(s.findDelim("empty")));
		poliz->push(s.findDelim("jumpfalse"));
	}
	pop();
	nextAddr = poliz->push(s.findDelim("empty"));
	poliz->push(s.findDelim("jump"));
	tmp = poliz->push(s.findDelim("empty"));
	while (!begOp.empty()){ (*poliz)[begOp.top()] = new Label(tmp);  begOp.pop(); }
	parseOperator();
	return nextAddr;
}

void Parser::parseComplexOperator(){
	if (!curr->isEqual("{")) return;  pop();
	parseOperators();
	if (!curr->isEqual("}")) 
		throw SyntaxError ("'}' is expexted but '" + curr->getString() +"' found!",s.getLine());  pop();
}

Ident* Parser::parseExpression(){
	Ident* tmp;
	poliz->push(tmp = new Ident(""));
	lval = true;
	string res = parseDisjunction();
	if (!curr->isEqual("=")) {
		tmp->declare(res);
		poliz->push(s.findDelim("="));   return tmp;
	}
	while (curr->isEqual("=")) { 
		if (!lval) throw SemanticError ("Assigning to rvalue!",s.getLine());
		Lex *sgn = curr;  pop(); lval = true;
		if ((parseDisjunction() == "string") ^ (res == "string")) 
			throw SemanticError ("String cannot be assigned to a number!",s.getLine()); 
		poliz->push(sgn); }
	tmp->declare(res);
	poliz->push(s.findDelim("="));   return tmp;
}

string Parser::parseDisjunction(){
	string res = parseConjunction();
	if (!curr->isEqual("or")) return res;
	lval = false;
	stack<int> ends;
	while (curr->isEqual("or")) { Lex *sgn = curr;   pop(); 
		poliz->push(s.findDelim("dup"));
		poliz->push(s.findKeyword("not"));
		ends.push (poliz->push(s.findDelim("empty")));
		poliz->push(s.findDelim("jumpfalse"));
		if (res != "int") throw SemanticError ("Non-integer operand in disjunction!",s.getLine()); 
		res = parseConjunction(); 
		poliz->push(sgn); }
	if (res != "int") throw SemanticError ("Non-integer operand in disjunction!",s.getLine()); 
	int tmp = poliz->push(s.findDelim("empty"));
	while (ends.size() > 0){
		(*poliz)[ends.top()] = new Label(tmp);   ends.pop();
	}
	return res;
}

string Parser::parseConjunction(){
	string res = parseComparison();
	if (!curr->isEqual("and")) return res;
	lval = false;
	stack<int> ends;
	while (curr->isEqual("and")) { Lex *sgn = curr;   pop(); 
		poliz->push(s.findDelim("dup"));
		ends.push (poliz->push(s.findDelim("empty")));
		poliz->push(s.findDelim("jumpfalse"));
		if (res != "int") throw SemanticError ("Non-integer operand in conjunction!",s.getLine()); 
		res = parseComparison(); 
		poliz->push(sgn); }
	if (res != "int") throw SemanticError ("Non-integer operand in conjunction!",s.getLine()); 
	int tmp = poliz->push(s.findDelim("empty"));
	while (ends.size() > 0){
		(*poliz)[ends.top()] = new Label(tmp);   ends.pop();
	}
	return res;
}

string Parser::parseComparison(){
	string s1 = parseSum(), s2;
	if (!curr->isEqual(">") && !curr->isEqual("<") &&
		!curr->isEqual(">=") && !curr->isEqual("<=") &&
		!curr->isEqual("==") && !curr->isEqual("!=")) return s1;  
	lval = false;
	Lex *sgn = curr;   pop(); 
	s2 = parseSum();
	poliz->push(sgn);
	if ((s1 == "string") ^ (s2 == "string"))
		throw SemanticError ("Strings are not comparable to numbers!",s.getLine()); 
	return "int";
}

string Parser::parseSum(){
	string res = parseProduct();
	if (!curr->isEqual("+") && !curr->isEqual("-")) return res;
	string unares = res;  lval = false;
	bool plus, pplus;
	while ((plus = curr->isEqual("+")) || curr->isEqual("-")) { 
		Lex *sgn = curr;   pop(); 
		if (unares == "string" && (res != "string" || !plus)) 
			throw SemanticError ("Strings are not allowed in sum!",s.getLine()); 
		if ((unares == "real" || unares == "int") && res == "string") 
			throw SemanticError ("Sorry, I cannot concatenate numbers!",s.getLine()); 
		if (unares == "real") res = "real";
		unares = parseProduct(); 
		poliz->push(sgn);  pplus = plus; }
	if (unares == "string" && (res != "string" || !pplus)) 
		throw SemanticError ("Strings are not allowed in sum!",s.getLine()); 
	if ((unares == "real" || unares == "int") && res == "string") 
		throw SemanticError ("Sorry, I cannot concatenate numbers!",s.getLine()); 
	if (unares == "real") res = "real";
	return res;
}

string Parser::parseProduct(){
	string res = parseUnary();
	if (!curr->isEqual("*") && !curr->isEqual("/")) return res;
	lval = false;
	string unares = res;
	while (curr->isEqual("*") || curr->isEqual("/")) { 
		Lex *sgn = curr;   pop(); 
		if (unares == "string") throw SemanticError ("Strings are not allowed in product!",s.getLine()); 
		if (unares == "real") res = "real";
		unares = parseUnary(); 
		poliz->push(sgn);}
	if (unares == "string") throw SemanticError ("Strings are not allowed in product!",s.getLine()); 
	if (unares == "real") res = "real";
	return res;
}

string Parser::parseUnary(){
curr->getString();
	if (curr->isEqual("not")) { 
		Lex *sgn = curr; lval = false;  pop(); 
		if (parseUnary() != "int") 
			throw SemanticError ("Integer expression is expected after 'not'!",s.getLine()); 
		poliz->push(sgn);
		return "int"; }
	if (curr->isEqual("+")) { 
		pop(); lval = false;
		string res = parseUnary();
		if (res != "int" && res !="real") 
			throw SemanticError ("Integer or real expression is expected after unary '+'!",s.getLine()); 
		return res; } 
	if (curr->isEqual("-")) { 
		pop(); lval = false;
		string res = parseUnary();
		if (res != "int" && res !="real") 
			throw SemanticError ("Integer or real expression is expected after unary '-'!",s.getLine()); 
		poliz->push(new Delim("~"));
		return res; }
	if (curr->isEqual("(")) { 
		pop(); lval = false;
		Ident* res = parseExpression(); 
		if (!curr->isEqual(")")) 
			throw SyntaxError ("')' is expected but '" + curr->getString() +"' found!",s.getLine());
		pop();  return res->getType(); }
	if (curr->getKind() == IDENT) { 
		if (!((Ident*)curr)->isDeclared()) 
			throw SemanticError ("Undeclared identifier: '" + curr->getString() +"'!",s.getLine());
		string res = ((Ident*)curr)->getType();
		poliz->push(curr);
		pop(); return res; }
	lval = false;
	Lex* q = parseConstant();
	if (q->isType("int")) return "int";
	if (q->isType("real")) return "real";
	return "string";
}

int main(int argc, char* argv[]){
	Poliz poliz;
	char fname[256];
	if (argc == 1) strcpy (fname,"test.txt");
	else strcpy (fname,argv[1]);
	Parser p(fname, poliz);
	try{
		p.go();
		cout<<endl<<"Total congratulations! Your program has compiled successfully!"<<endl;
		poliz.go();
	} catch (US us){ cerr<<"Error (line "<<us.getLine()<<"): unresolved symbol "<<us.getSymb()<<endl; return 1;}
	catch (USAfter us){ cerr<<"Error (line "<<us.getLine()<<"): unresolved symbol after "<<us.getLast()<<endl;return 1;}
	catch (UEOF){ cerr<<"Unexpected end of file!"<<endl; return 1;}
	catch (SyntaxError se){ cerr<<"Syntax error (line "<<se.getLine()<<"): "<<se.getText()<<endl; return 2;}
	catch (SemanticError se){ cerr<<"Semantic error (line "<<se.getLine()<<"): "<<se.getText()<<endl; return 3;}
	catch (RuntimeError re){ cerr<<"Runtime error: "<<re.getText()<<endl; return 4;}
	cout<<endl<<"More total congratulations! Your program has runned and worked successfully!"<<endl;
	return 0;}
