#include "Util.h"

Util::Util(){

	this->line = 1;
	this->column = 1;
	this->columnPrev = 1;
}

string Util::getTokenString(TokenType tokenType){

	

	if(tokenType == DOUBLE)
		return "DOUBLE";
    else if(tokenType == INTEGER)
		return "INTEGER";
    else if(tokenType == ID)
		return "ID";
    else if(tokenType == IF)
		return "IF";
    else if(tokenType == THEN)
		return "THEN";
    else if(tokenType == ELSE)
		return "ELSE";
    else if(tokenType == END)
		return "END";
    else if(tokenType == REPEAT)
		return "REPEAT";
    else if(tokenType == FLOAT)
		return "FLOAT";
    else if(tokenType == UNTIL)
		return "UNTIL";
    else if(tokenType == READ)
		return "READ";
    else if(tokenType == WRITE)
		return "WRITE";
    else if(tokenType == INT)
		return "INT";
    else if(tokenType == EXP)
		return "EXP";
    else if(tokenType == PLUS)
		return "PLUS";
    else if(tokenType == MINUS)
		return "MINUS";
    else if(tokenType == TIMES)
		return "TIMES";
    else if(tokenType == OVER)
		return "OVER";
    else if(tokenType == EQUAL)
		return "EQUAL";
    else if(tokenType == COMMA)
		return "COMMA";
    else if(tokenType == LT)
		return "LT";
    else if(tokenType == LPARENT)
		return "LPARENT";
    else if(tokenType == RPARENT)
		return "RPARENT";
    else if(tokenType == SEMICOLON)
		return "SEMICOLON";
    else if(tokenType == ASSIGN)
		return "ASSIGN";
    else if(tokenType == ERROR)
		return "ERROR";

    return NULL;
}

void Util::refreshColumn(/*char* token,*/ int length){
    //if(token == NULL) return;

    this->setColumnPrev(this->getColumn());

    setColumn(this->getColumn() + length);

}

void Util::refreshLine(){

	setLine(this->line + 1);

}

void Util::setLine(int newValue){
	this->line = newValue;
}

void Util::setColumn(int newValue){
	this->column = newValue;
}

void Util::setColumnPrev(int newValue){
	this->columnPrev = newValue;
}

int Util::getLine(){
	return this->line;
	
}

int Util::getColumn(){
	return this->column;
}

int Util::getColumnPrev(){
	return this->columnPrev;
}

void Util::printOutput(string token, string value){

	cout << "[ " << token << ", " << value << ", " << this->getLine() << ", " << this->getColumnPrev() << "]" << endl;
}

void Util::printOutputCh(string token, char* value){

	cout << "[ " << token << ", " << value << ", " << this->getLine() << ", " << this->getColumnPrev() << "]" << endl;
}

void Util::printTree(Tree* node){

	Tree* irmao = node->getBrother();
	int i;

	if(irmao != NULL)
		printTree(irmao);

	cout << "( " << node->getNameNode();

	for(i = 0; i < node->getAmountOfChildren(); i++){

		printTree(node->sons[i]);
	}

	cout << " )";
}
