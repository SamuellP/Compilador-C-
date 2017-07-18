#ifndef Included_Util_H
#define Included_Util_H

#include "TokenType.h"
#include "Tree.h"
#include <string>
#include <iostream>


using namespace std;

class Util {

    public:
	int column;
	int line;
	int columnPrev;

	Util();
	string getTokenString(TokenType tokenType);
    void refreshColumn(/*char* token,*/ int length);
	void refreshLine();
	void setLine(int newValue);
	void setColumn(int newValue);
	void setColumnPrev(int newValue);
	int getLine();
	int getColumn();
	int getColumnPrev();
	void printOutput(string token, string value);
	void printOutputCh(string token, char* value);
	void printTree(Tree* node);
};

#endif
