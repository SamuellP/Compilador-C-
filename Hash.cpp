#include "Hash.h"


int Hash::containsKey(string key, string scope){

    if(this->tabela[key + "&" + scope] == NULL)
        return 0;

    return 1;
}

void Hash::putVariable(string variable, string type, string scope){

    this->tabela[variable + "&" + scope] = new Simbolo(scope,type);

    this->tabela[variable + "&" + scope]->isFunction = false;
}

void Hash::putFunction(string variable, string type){

    this->tabela[variable + "&&"] = new Simbolo("&",type);

    this->tabela[variable + "&&"]->isFunction = true;
}

bool Hash::isFunction(string id){

    if(this->tabela[id + "&&"]->isFunction)
        return 1;
    return 0;
}

void Hash::adicionaParametro(string idFuncao, string idParametro){ // Grava o tipo do parâmetro

    this->tabela[idFuncao + "&&"]->addParametro(idParametro);
}

void Hash::adicionaId(string identificadorFuncao, string id){ // Grava o id do parâmetro

    this->tabela[identificadorFuncao + "&&"]->addID(id);
}

string Hash::getType(string variable, string scope){

    return this->tabela[variable + "&" + scope]->getTipo();
}

vector<string> Hash::getParameters(string id){

	return this->tabela[id + "&&"]->getParameters();
}


vector<string> Hash::getParametersTypes(string id){

	return this->tabela[id + "&&"]->getParametersTypes();
}

string Hash::getIdValue(string id, string scope){

	return this->tabela[id + "&" + scope]->getValue();
}

void Hash::setIdValue(string id, string scope, string value){

	this->tabela[id + "&" + scope]->setValue(value);
}

bool Hash::getAlreadyAssigned(string id, string scope){

	return this->tabela[id + "&" + scope]->getAlreadyAssigned();
}

void Hash::setAlreadyAssigned(string id, string scope){

	this->tabela[id + "&" + scope]->setAlreadyAssigned(true);

}

void Hash::setIsUsed(string id, string scope){

	this->tabela[id + "&" + scope]->setIsUsed(id, scope);

}