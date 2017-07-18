#include "Simbolo.h"

Simbolo::Simbolo(string e, string t) {
	this->escopo = e;
	this->tipo = t;
	this->alreadyAssigned = false;
	this->isUsed = false;
	//tipo_char = (char*)calloc(strlen(t)+1,sizeof(char));
    //strcpy(tipo_char,t);
}

void Simbolo::addParametro(string parametro){

	this->tipoParametros.push_back(parametro);

}

void Simbolo::addID(string id){

	idsParametros.push_back(id);

}

string Simbolo::getEscopo() { 

	return escopo; 

}

string Simbolo::getTipo() { 

	return this->tipo; 

}

string Simbolo::getValue() { 

	return this->value; 

}

void Simbolo::setValue(string value){

	this->value = value;
}

bool Simbolo::getAlreadyAssigned(){

	return this->alreadyAssigned;
}

void Simbolo::setAlreadyAssigned(bool status){

	this->alreadyAssigned = status;
}

vector<string> Simbolo::getParameters(){

	return this->idsParametros;
}


vector<string> Simbolo::getParametersTypes(){

	return this->tipoParametros;
}

void Simbolo::setIsUsed(string id, string scope){

	this->isUsed = true;
}