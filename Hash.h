#ifndef Included_Hash_H
#define Included_Hash_H

#include <boost/unordered_map.hpp>
#include <iostream>
#include "Simbolo.h"


using namespace std;

class Hash {

	public:

		boost::unordered_map<string, Simbolo*> tabela;  

		std::vector<string> keys;

		int containsKey(string key, string scope);
		void putVariable(string variable, string type, string scope);
		void adicionaParametro(string idFuncao, string idParametro);
		void adicionaId(string identificadorFuncao, string id);
		string getType(string variable, string scope);
		void putFunction(string variable, string type);
		bool isFunction(string id);
		string getIdValue(string id, string scope);
		void setIdValue(string id, string scope, string value);
		bool getAlreadyAssigned(string id, string scope);
		void setAlreadyAssigned(string id, string scope);
		vector<string> getParameters(string id);
		vector<string> getParametersTypes(string id);
		void setIsUsed(string id, string scope);
};

#endif