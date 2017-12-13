#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "parserHeader.h"

/*
* main.cpp
*/

int lineNumber;
string fileMessage;
int errorcount = 0;
bool errorFlag = false;

void messageStart(int linenum) {
	cout << fileMessage << linenum + 1 << ":";
}

void error(int linenum, const string& message) {
	++errorcount;
	messageStart(linenum);
	cout << "Syntax error " << message << endl;
}

void semanticError(int linenum, const string& message) {
	messageStart(linenum);
	cout << message << endl;
}

/* TRAVERSING */
void trace(ParseTree *f) {
	if (f == 0)
		return;
	if (f->getLeft() != 0)
	{
		trace(f->getLeft());
	}
	if (f->getRight() != 0)
	{
		trace(f->getRight());
	}
	f->eval();
}

void trace2(ParseTree *f) {
	if (f == 0)
		return;
	if (f->getLeft() != 0)
	{
		trace2(f->getLeft());
	}
	if (f->getRight() != 0)
	{
		trace2(f->getRight());
	}

	f->findSemanticErrors();
}
map<string, Value> SymbolTable;

int
main(int argc, char *argv[])
{
	ifstream file;
	istream *in = &cin;
	//bool doTrace = false;

	for (int i = 1; i < argc; i++) {
		string arg(argv[i]);

		if (arg[0] == '-') {
			if (arg == "-t") {
				//doTrace = true;
				continue;
			}

			cout << arg << " UNRECOGNIZED FLAG" << endl;
			return 0;
		}

		if (in != &cin) {
			cout << "TOO MANY FILES" << endl;
			return 0;
		}

		fileMessage = arg + ":";
		file.open(arg);
		if (file.is_open() == false) {
			cout << arg << " FILE NOT FOUND" << endl;
			return 0;
		}

		in = &file;
	}

	ParseTree *tree = Prog(in);

	if (tree == 0 || errorcount > 0) {
		// there was some kind of parse error
		return 1;
	}

	trace2(tree);

	if (errorFlag == false)
		trace(tree);

	extern map<string, Value>	SymbolTable;

	return 0;
}


