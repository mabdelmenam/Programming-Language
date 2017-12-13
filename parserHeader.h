/*Skeleton done by professor, all the filling in done by me*/
#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>
using std::istream;

#include <string>
#include <map>
using std::string;
using std::stoi;
using std::map;

#include "lexer.h"

extern void error(int linenum, const string& message);
extern void semanticError(int linenum, const string& message);
extern bool errorFlag;

enum TypeForNode { INT_TYPE, STRING_TYPE, ERROR_TYPE, PRINT_TYPE };

class Value {
	TypeForNode	type;
	int			ival;
	string		sval;

public:
	Value(TypeForNode t = ERROR_TYPE) : type(t), ival(0) {}

	Value(int i) : ival(i) { type = INT_TYPE; }
	Value(string s) : sval(s) { type = STRING_TYPE; }

	TypeForNode GetType() const { return type; }

	int GetIntValue() const {
		if (type == INT_TYPE) return ival;
		throw "This variable is not an int";
	}
	void SetIntValue(int v) {
		if (type == INT_TYPE) ival = v;
		throw "This variable is not an int";
	}

	string GetStringValue() const {
		if (type == STRING_TYPE) return sval;
		throw "This variable is not a string";
	}
	void SetStringValue(string v) {
		if (type == STRING_TYPE) sval = v;
		throw "This variable is not a string";
	}
};

extern map<string, Value> SymbolTable;
//extern map<string,Value>::iterator it;
//extern it = SymbolTable.find(id);

class ParseTree {
	int			linenumber;
	ParseTree	*left;
	ParseTree	*right;

public:
	ParseTree(int n, ParseTree *l = 0, ParseTree *r = 0) : linenumber(n), left(l), right(r) {}
	virtual ~ParseTree() {}

	ParseTree* getLeft() const { return left; }
	ParseTree* getRight() const { return right; }
	int getLineNumber() const { return linenumber; }

	virtual TypeForNode GetType() const { return ERROR_TYPE; } // TYPE FOR NODE
	virtual int GetIntValue() const { throw "no integer value"; }
	virtual string GetStringValue() const { throw "no string value"; }

	virtual int countSet() const { return 0; }
	virtual int countPlus() const { return 0; }
	virtual int countStar() const { return 0; }

	virtual int findSemanticErrors() const { return 0; }
	virtual Value eval() { return Value(); }
	/* EVAL FUNCTION */
};

class StatementList : public ParseTree {
public:
	StatementList(ParseTree *first, ParseTree *rest) : ParseTree(0, first, rest) {}

};

class DeclStatement : public ParseTree {
	TypeForNode	type;
	string 		id;

public:
	DeclStatement(int line, TypeForNode ty, string id) : ParseTree(line), type(ty), id(id) {}

	int findSemanticErrors() const {
		if (SymbolTable.find(id) != SymbolTable.end()) {
			semanticError(this->getLineNumber(), "variable " + id + " was already declared");
			return 1;
		}
		SymbolTable[id] = Value(type);
		return 0;
	}

};

class SetStatement : public ParseTree {
	string id;

public:
	SetStatement(int line, string id, ParseTree *ex) : ParseTree(line, ex), id(id) {}
	int countSet() const { return 1; }

	Value eval()
	{

		// if(it != SymbolTable.end())
		// {
		if (getLeft()->eval().GetType() == SymbolTable[this->id].GetType())
		{
			SymbolTable[id] = Value(getLeft()->eval());
		}
		//   }
		return Value();
	}

	int findSemanticErrors() const {
		if (SymbolTable.find(id) == SymbolTable.end()) {
			semanticError(this->getLineNumber(), "variable " + id + " is used before being declared");
			return 1;
		}
		if (getLeft()->eval().GetType() != SymbolTable[this->id].GetType())
			semanticError(this->getLineNumber(), "type error");
		return 0;
	}
};
/*PRINT CLASS*/
class PrintStatement : public ParseTree {
	bool addNL;

public:
	PrintStatement(int line, bool isPrintln, ParseTree *ex) : ParseTree(line, ex), addNL(isPrintln) {}
	TypeForNode GetType() const { return PRINT_TYPE; }

	void doprinting() const {
		if (addNL) std::cout << std::endl;
	}

	Value eval()
	{
		if (getLeft()->eval().GetType() == INT_TYPE)
			std::cout << getLeft()->eval().GetIntValue();
		else if (getLeft()->eval().GetType() == STRING_TYPE)
			std::cout << getLeft()->eval().GetStringValue();

		doprinting();
		return Value();
	}
};

class Addition : public ParseTree {
public:
	Addition(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2) {}

	int findSemanticErrors() const {
		if ((getLeft()->eval().GetType() != getRight()->eval().GetType()))
		{
			semanticError(this->getLineNumber(), "type error");
			errorFlag = true;
		}
		return 1;
	}
	Value eval() {
		if (getLeft()->eval().GetType() == INT_TYPE && getRight()->eval().GetType() == INT_TYPE)
			return Value(getLeft()->eval().GetIntValue() + getRight()->eval().GetIntValue());
		else if ((getLeft()->eval().GetType() == STRING_TYPE && getRight()->eval().GetType() == STRING_TYPE))
			return Value(getLeft()->eval().GetStringValue() + getRight()->eval().GetStringValue());


		return Value();
	}
	int countPlus() const { return 1; }
};

class Subtraction : public ParseTree {
public:
	Subtraction(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2) {}

	int findSemanticErrors() const {
		if (getLeft()->eval().GetType() == STRING_TYPE && getRight()->eval().GetType() == INT_TYPE)
		{
			errorFlag = true;
			semanticError(this->getLineNumber(), "type error");
			return 1;
		}
		return 0;
	}

	Value eval() {
		if (getLeft()->eval().GetType() == INT_TYPE && getRight()->eval().GetType() == INT_TYPE)
		{
			return Value(getLeft()->eval().GetIntValue() - getRight()->eval().GetIntValue());
		}

		return Value();
	}
};
class Multiplication : public ParseTree {
public:
	Multiplication(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2) {}

	int findSemanticErrors() const {
		if ((getLeft()->eval().GetType() == STRING_TYPE && getRight()->eval().GetType() == STRING_TYPE))
		{
			semanticError(this->getLineNumber(), "type error");
			errorFlag = true;
		}
		return 1;
	}
	Value eval() {
		string currentStrings;

		if (getLeft()->eval().GetType() == INT_TYPE && getRight()->eval().GetType() == INT_TYPE) // correct
		{
			//std::cout << "Multiplication Int: " << getLeft()->eval().GetIntValue() << std::endl;
			return Value(getLeft()->eval().GetIntValue() * getRight()->eval().GetIntValue());
		}

		else if ((getLeft()->eval().GetType() == INT_TYPE && getRight()->eval().GetType() == STRING_TYPE))
		{
			int x = 0;
			do {
				x++;
				currentStrings += getRight()->eval().GetStringValue();

			} while (x < getLeft()->eval().GetIntValue());
			return Value(currentStrings);
		}
		else if ((getLeft()->eval().GetType() == STRING_TYPE && getRight()->eval().GetType() == INT_TYPE))
		{
			int x = 0;
			do {
				x++;
				currentStrings += getLeft()->eval().GetStringValue();

			} while (x < getRight()->eval().GetIntValue());
			return Value(currentStrings);
		}
		return Value();
	}
	int countStar() const { return 1; }
};

class Division : public ParseTree {
public:
	Division(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2) {}

	int findSemanticErrors() const {
		if ((getLeft()->eval().GetType() == INT_TYPE && getRight()->eval().GetType() == STRING_TYPE))
		{
			semanticError(this->getLineNumber(), "type error");
			errorFlag = true;
		}
		else if ((getLeft()->eval().GetType() == STRING_TYPE && getRight()->eval().GetType() == INT_TYPE))
		{
			semanticError(this->getLineNumber(), "type error");
			errorFlag = true;
		}
		return 1;
	}

	Value eval()
	{
		string one, two;
		if (getLeft()->eval().GetType() == INT_TYPE && getRight()->eval().GetType() == INT_TYPE) // 1
		{
			//std::cout << "Division Int: " << getRight()->eval().GetIntValue() << std::endl;
			//std::cout << getLeft()->eval().GetIntValue() / getRight()->eval().GetIntValue() << std::endl;
			return Value(getLeft()->eval().GetIntValue() / getRight()->eval().GetIntValue());
		}
		if ((getLeft()->eval().GetType() == STRING_TYPE && getRight()->eval().GetType() == STRING_TYPE))
		{
			one = getLeft()->eval().GetStringValue();
			two = getRight()->eval().GetStringValue();

			/*use this example:
			string base="this is a test string.";
			string str2="n example";
			string str=base;                // "this is a test string."
			str.replace(9,5,str2);          // "this is an example string." */


			std::size_t w = one.find(two);
			if (w != std::string::npos)
			{
				one.replace(one.find(two), two.length(), "");
			}
			return Value(one); //correct
		}
		return Value();
	}

	// will need to fill in type and value;
	// remember type is a function of
};

class IntegerConstant : public ParseTree {
	int	value;

public:
	IntegerConstant(const Token& tok) : ParseTree(tok.GetLinenum()) {
		value = stoi(tok.GetLexeme());
	}

	Value eval() {
		return Value(value);
	}

	TypeForNode GetType() const { return INT_TYPE; }
	int GetIntValue() const { return value; }
};

class StringConstant : public ParseTree {
	string	value;

public:
	StringConstant(const Token& tok) : ParseTree(tok.GetLinenum()) {
		value = tok.GetLexeme();
		value = value.substr(1, value.length() - 2);
	}

	Value eval() {
		return Value(value);
	}

	TypeForNode GetType() const { return STRING_TYPE; }
	string GetStringValue() const { return value; }
};

class Identifier : public ParseTree {
	string	value;

public:
	Identifier(const Token& tok) : ParseTree(tok.GetLinenum()) {
		value = tok.GetLexeme();
	}
	int findSemanticErrors() const {
		if (SymbolTable.find(value) == SymbolTable.end()) {
			semanticError(this->getLineNumber(), "variable " + value + " is used before being declared");
			errorFlag = true;
			return 1;
		}
		return 0;
	}
	Value eval() {
		//std::cout << "Value: " << value << std::endl;
		return Value(SymbolTable[value]);
		//Value(map[lexeme])
	}

	TypeForNode GetType() const { return STRING_TYPE; }
	int GetIntValue() const { return 0; }
	string GetStringValue() const { return "foo"; }
};

extern ParseTree *	Prog(istream* in);
extern ParseTree *	StmtList(istream* in);
extern ParseTree *	Stmt(istream* in);
extern ParseTree *	Decl(istream* in);
extern ParseTree *	Set(istream* in);
extern ParseTree *	Print(istream* in);
extern ParseTree *	Expr(istream* in);
extern ParseTree *	Term(istream* in);
extern ParseTree *	Primary(istream* in);


#endif /* PARSER_H_ */
