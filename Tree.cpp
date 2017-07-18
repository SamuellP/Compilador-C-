#include "Tree.h"


Tree::Tree(string nameNode, const char* value){

	string s = value;
	setNameNode(nameNode);
	setValue(s);
	setBrother(NULL);

}

Tree* Tree::getBrother(){

	return this->irmao;
}

void Tree::setBrother(Tree* irmao){

	this->irmao = irmao;
}

string Tree::getNameNode(){

	return this->nameNode;
}

void Tree::setNameNode(string nameNode){

	this->nameNode = nameNode;
}

string Tree::getValue(){

	return this->value;
}

void Tree::setValue(string value){

	this->value = value;
	
}

int Tree::getLine(){

	return this->line;
}

void Tree::setLine(int line){

	this->line = line;
	
}

int Tree::getAmountOfChildren(){

	return this->sons.size();
}