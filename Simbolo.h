#ifndef Included_Simbolo_H
#define Included_Simbolo_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;


class Simbolo {
    public:
        bool isFunction;
        bool alreadyAssigned;
        bool isUsed;
        string escopo;
        string tipo;
        string value;
        //char* tipo_char;
        vector<string> tipoParametros;
        vector<string> idsParametros;
    
        Simbolo(string e, string t);

        void addParametro(string parametro);

        void addID(string id);

        string getEscopo();
        
        string getTipo();

        string getValue();

        void setValue(string value);

        bool getAlreadyAssigned();

        void setAlreadyAssigned(bool status);

        vector<string> getParameters();

		vector<string> getParametersTypes(); 

		void setIsUsed(string id, string scope);       
};

#endif