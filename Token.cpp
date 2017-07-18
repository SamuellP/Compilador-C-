#include "Token.h"

Token::Token(TokenType tt, string val){

	setTokenType(tt);
	setValor(val);
}

TokenType Token::getTokenType(){

	return this.tokenType;
}

void Token::setTokenType(TokenType tt){

	this.tokenType = tt;
}

string Token::getValor(){

	return this.valor;
}

void Token::setValor(string val){

	this.valor = val;
}
