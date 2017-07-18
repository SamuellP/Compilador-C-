#include "Semantic.h"


Semantic::Semantic(Tree* abstractSyntaxTree){

	this->abstractSyntaxTree = abstractSyntaxTree;
	this->hash = new Hash();
	this->setScope("&");
}

void Semantic::runSemantic(Tree* tree){

	this->firstPass(tree);
	this->secondPass(tree);
	this->thirdPass();
	//this->thirdPass(this->hash);
	
}


void Semantic::firstPass(Tree* tree){

	Tree* aux = tree;

	while(aux != NULL){

		if(aux->getNameNode() == "DECL_FUNCAO"){

			string id = aux->sons[1]->getValue();
			string tipo = aux->sons[0]->getNameNode();

			if(this->hash->containsKey(id,"&")){
				cout << "\n\n Erro Semantico: Linha: " << tree->getLine() << ": Uma funcao/variavel ja foi declarada com o identificador " << id << "\n\n";
				exit(1);
			}

			this->hash->putFunction(id, tipo);
		}

		aux = aux->getBrother();
	}
}

void Semantic::secondPass(Tree* tree){
	if(tree->getBrother() != NULL)
        secondPass(tree->getBrother());

    
    string nameNode = tree->getNameNode();

    if(nameNode == "DECLARACAO")
    	this->trataDeclaracao(tree);
    else if(nameNode == "DECL_FUNCAO")
    	this->trataDeclaracaoFuncao(tree);
    else if(nameNode == "DECL_FUNCAO_PRINCIPAL")
    	this->trataDeclaracaoFuncaoPrincipal(tree);
    else if(nameNode == "IF")
    	this->trataIF(tree);
    else if(nameNode == "REPETICAO")
    	this->trataRepeticao(tree);
    else if(nameNode == "RETORNA")
    	this->trataRetorno(tree);
    else if(nameNode == "ASSIGN")
    	this->trataAtribuicao(tree);
    else if(nameNode == "CHAMADA_FUNC")
    	this->trataChamadaFuncao(tree);
    else if(nameNode == "READ")
    	this->trataLeitura(tree);
    else if(nameNode == "WRITE")
    	this->trataEscrita(tree);

}

void Semantic::thirdPass(){

	for ( int i = 0; i < this->hash->keys.size(); i++ ){
    	if(this->hash->tabela[this->hash->keys[i]]->isUsed == false)
    		cout << "\n\nWarning: Variável " << this->hash->tabela[this->hash->keys[i]]->getValue() << " foi declarada mas não está sendo utilizada\n\n";
	}

}

void Semantic::setScope(string scope){

	this->scope = scope;
}


double Semantic::aritmetica(Tree* treeNode){

	string nameNode = treeNode->getNameNode();
	string s;
	double value1, value2, result;

	if(nameNode == "+" || nameNode == "-" || nameNode == "*" || nameNode == "/"){

		value1 = aritmetica(treeNode->sons[0]);
		value2 = aritmetica(treeNode->sons[1]);

		if(nameNode == "+") return value1 + value2;
		if(nameNode == "-") return value1 - value2;
		if(nameNode == "*") return value1 * value2;
		if(nameNode == "/") return value1 / value2;

	}else if(nameNode == "ID"){

		if(this->scope != "&" &&  !this->hash->containsKey(treeNode->getValue(), this->scope)){
			if(!this->hash->containsKey(treeNode->getValue(), "&")){
				cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": A variavel " << treeNode->getValue() << " não foi declarada\n\n";
				exit(1);
			}else{
				s = "&";
			}
		}else if(!this->hash->containsKey(treeNode->getValue(), this->scope)){
			cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": A variavel " << treeNode->getValue() << " não foi declarada\n\n";
			exit(1);
		}else{
			s = this->scope;
		}

		if(this->hash->getAlreadyAssigned(treeNode->getValue(), s) == false){
			cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel sendo utilizada sem valor atribuído\n\n";
			exit(1);
		}

		this->hash->setIsUsed(treeNode->getValue(), s);
	}else if(nameNode == "CHAMADA_FUNC"){

		this->trataChamadaFuncao(treeNode);
	}
}

void Semantic::trataAtribuicaoNaDeclaracao(Tree* treeNode, string tipo){

		if(this->hash->containsKey(treeNode->sons[0]->getValue(), this->scope)){
			cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << treeNode->sons[0]->getValue() << " ja declarada...\n\n";
		    exit(1);
		}

		this->hash->putVariable(treeNode->sons[0]->getValue(), tipo, this->scope);
		//this->hash->setIsUsed(treeNode->sons[0]->getValue(), this->scope);

		if(treeNode->sons[1]->getNameNode() == "ID"){

			if(this->hash->containsKey(treeNode->sons[1]->getValue(), this->scope))
				this->hash->setIsUsed(treeNode->sons[1]->getValue(), this->scope);
			if(this->hash->containsKey(treeNode->sons[1]->getValue(), "&"))
				this->hash->setIsUsed(treeNode->sons[1]->getValue(), "&");

			this->trataID(treeNode, treeNode->sons[1]->getValue(), tipo);

		}

		if(treeNode->sons[1]->getNameNode() == "VAL_INT") {

			if(tipo == "FLOAT"){
				cout << "\n\n Warning: Linha: " << treeNode->getLine() << ": Atribuindo um valor do tipo inteiro a um identificador do tipo flutuante\n\n";
				//exit(1);
			}

		}

	    if(treeNode->sons[1]->getNameNode() == "VAL_DOUBLE") { 

	    	if(tipo == "INT"){
				cout << "\n\n Warning: Linha: " << treeNode->getLine() << ": Atribuindo um valor do tipo flutuante a um identificador do tipo inteiro\n\n";
				//exit(1);
			}

	    }

	    if(treeNode->sons[1]->getNameNode() == "CHAMADA_FUNC"){

	    	string t = this->trataChamadaFuncao(treeNode->sons[1]);

	    	if(tipo == "FLOAT" && t == "INT"){
				cout << "\n\n Warning: Linha: " << treeNode->getLine() << ": Atribuindo um valor do tipo inteiro a um identificador do tipo flutuante\n\n";
				//exit(1);
			}
			if(tipo == "INT" && t == "FLOAT"){
				cout << "\n\n Warning: Linha: " << treeNode->getLine() << ": Atribuindo um valor do tipo flutuante a um identificador do tipo inteiro\n\n";
				//exit(1);
			}
	    }

	    if(treeNode->sons[1]->getNameNode() == "+" || treeNode->sons[1]->getNameNode() == "-" || treeNode->sons[1]->getNameNode() == "*" || treeNode->sons[1]->getNameNode() == "/"){

			this->aritmetica(treeNode->sons[1]);
			
		}

		this->hash->setAlreadyAssigned(treeNode->sons[0]->getValue(), this->scope);

}


string Semantic::trataChamadaFuncao(Tree* tree){

	if(!this->hash->containsKey(tree->sons[0]->getValue(), "&")){
		cout << "\n\n Erro Semantico: Linha: " << tree->getLine() << ": Função " << tree->sons[0]->getValue() << " não foi declarada\n\n";
		exit(1);
	}

	if(!this->hash->isFunction(tree->sons[0]->getValue())){
		cout << "\n\n Erro Semantico: Linha: " << tree->getLine() << ": " << tree->sons[0]->getValue() << " não é uma função\n\n";
		exit(1);
	}

	std::vector<string> tipos = this->hash->getParametersTypes(tree->sons[0]->getValue());
	std::vector<string> parametros = this->hash->getParameters(tree->sons[0]->getValue());

	int amountOfParameters = parametros.size();
	int i = 0;
	Tree* aux = tree;

	if(tree->sons.size() == 1 && amountOfParameters == 0)
		return this->hash->getType(tree->sons[0]->getValue(), "&");
	if(tree->sons.size() == 1 && amountOfParameters != 0){

		cout << "\n\n Erro Semantico: Linha: " << tree->getLine() << ": Parâmetros inválidos\n\n";
		exit(1);

	}

	do{

		aux = aux->sons[1];

		if(aux->getNameNode() == "PARAMETROS_CHAM_FUNC"){
			
			if(aux->sons[0]->getNameNode() == "VAL_INT" && tipos[i] != "INT"){
				cout << "\n\n Warning: Linha: " << aux->getLine() << ": Atribuindo um valor do tipo inteiro a um identificador do tipo flutuante\n\n";
				//exit(1);
			}
			if(aux->sons[0]->getNameNode() == "VAL_DOUBLE" && tipos[i] != "FLOAT"){
				cout << "\n\n Warning: Linha: " << aux->getLine() << ": Atribuindo um valor do tipo flutuante a um identificador do tipo inteiro\n\n";
				//exit(1);	
			}
			if(aux->sons[0]->getNameNode() == "ID"){
				
				if(this->hash->containsKey(aux->sons[0]->getValue(), this->scope))
					this->hash->setIsUsed(aux->sons[0]->getValue(), this->scope);
				if(this->hash->containsKey(aux->sons[0]->getValue(), "&"))
					this->hash->setIsUsed(aux->sons[0]->getValue(), "&");

				this->trataID(aux, aux->sons[0]->getValue(), tipos[i]);

			}
			if(aux->sons[0]->getNameNode() == "CHAMADA_FUNC"){
				string t = this->trataChamadaFuncao(aux->sons[0]);

				if(t == "FLOAT" && tipos[i] == "INT"){
					cout << "\n\n Warning: Linha: " << aux->getLine() << ": Atribuindo um valor do tipo flutuante a um identificador do tipo inteiro\n\n";
					//exit(1);
				}
				if(t == "INT" && tipos[i] == "FLOAT"){
					cout << "\n\n Warning: Linha: " << aux->getLine() << ": Atribuindo um valor do tipo inteiro a um identificador do tipo flutuante\n\n";
					//exit(1);
				}
			}

		}else{

			if(aux->getNameNode() == "VAL_INT" && tipos[i] != "INT"){
				cout << "\n\n Warning: Linha: " << aux->getLine() << ": Atribuindo um valor do tipo inteiro a um identificador do tipo flutuante\n\n";
				//exit(1);
			}
			if(aux->getNameNode() == "VAL_DOUBLE" && tipos[i] != "FLOAT"){
				cout << "\n\n Warning: Linha: " << aux->getLine() << ": Atribuindo um valor do tipo flutuante a um identificador do tipo inteiro\n\n";
				//exit(1);	
			}
			if(aux->getNameNode() == "ID"){

				if(this->hash->containsKey(aux->getValue(), this->scope))
					this->hash->setIsUsed(aux->getValue(), this->scope);
				if(this->hash->containsKey(aux->getValue(), "&"))
					this->hash->setIsUsed(aux->getValue(), "&");

				this->trataID(aux, aux->getValue(), tipos[i]);
				
			}
			if(aux->getNameNode() == "CHAMADA_FUNC"){
				string t = this->trataChamadaFuncao(aux);

				if(t == "FLOAT" && tipos[i] == "INT"){
					cout << "\n\n Warning: Linha: " << aux->getLine() << ": Atribuindo um valor do tipo flutuante a um identificador do tipo inteiro\n\n";
					//exit(1);
				}
				if(t == "INT" && tipos[i] == "FLOAT"){
					cout << "\n\n Warning: Linha: " << aux->getLine() << ": Atribuindo um valor do tipo inteiro a um identificador do tipo flutuante\n\n";
					//exit(1);
				}
			}

			aux = NULL;

		}

		i++;

		if(i >= amountOfParameters && aux != NULL){
			cout << "\n\n Erro Semantico: Linha: " << tree->getLine() << ": Parâmetros inválidos\n\n";
			exit(1);
		}

	}while(aux != NULL);

	if(i < amountOfParameters){
		cout << "\n\n Erro Semantico: Linha: " << tree->getLine() << ": Parâmetros inválidos\n\n";
		exit(1);
	}

	return this->hash->getType(tree->sons[0]->getValue(), "&");
}


/* 
Trata o identificador <id> em sua utilização e não em sua atribuição. <tipoDestino> corresponde ao tipo da variável/parâmetro que está 
recebendo o valor atribuído ao identificador <id>

Verifica se o identificador já foi declarado no escopo atual ou no escopo global e se um valor já foi atribuído a ele. Caso nenhuma
dessas condições seja satisfeita e levantado um erro semântico.

Também verifica se o tipo do identificador origem corresponde ao tipo do identificador destino. Caso os tipos sejam distintos é levantado
um erro semântico.

*/
void Semantic::trataID(Tree* treeNode, string id, string tipoDestino){

	string tipoOrigem;
	string s;

	if(this->scope != "&" &&  !this->hash->containsKey(id, this->scope)){
		if(!this->hash->containsKey(id, "&")){
			cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << id << " não foi declarada";
			exit(1);
		}else{
			tipoOrigem = this->hash->getType(id,"&");
			s = "&";
		}
	}else if(!this->hash->containsKey(id, this->scope)){
		cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << id << " não foi declarada";
		exit(1);
	}else{
		tipoOrigem = this->hash->getType(id, this->scope);
		s = this->scope;
	}

	if(tipoDestino == "FLOAT" && tipoOrigem == "INT"){
		cout << "\n\n Warning: Linha: " << treeNode->getLine() << ": Atribuindo um valor do tipo inteiro a um identificador do tipo flutuante\n\n";
		//exit(1);
	}

	if(tipoDestino == "INT" && tipoOrigem == "FLOAT"){
		cout << "\n\n Warning: Linha: " << treeNode->getLine() << ": Atribuindo um valor do tipo flutuante a um identificador do tipo inteiro\n\n";
		//exit(1);
	}

	if(this->hash->getAlreadyAssigned(id, s) == false){
		cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel sendo utilizada sem valor atribuído\n\n";
		exit(1);
	}
}


void Semantic::trataListaDeclaracao(Tree* tree, string tipo){

	if(tree->sons[0]->getNameNode() == "ASSIGN"){

		this->trataAtribuicaoNaDeclaracao(tree->sons[0], tipo);
	
	}else if(tree->sons[0]->getNameNode() == "ID"){

		if(this->hash->containsKey(tree->sons[0]->getValue(), this->scope)){
			cout << "\n\n Erro Semantico: Linha: " << tree->getLine() << ": Variavel " << tree->sons[0]->getValue() << " ja declarada...\n\n";
		    exit(1);
		}else{

			this->hash->putVariable(tree->sons[0]->getValue(), tipo, this->scope);
			this->hash->keys.push_back(tree->sons[0]->getValue() + "&" + this->scope);
		}
	}

	if(tree->sons[1]->getNameNode() == "ASSIGN"){

		this->trataAtribuicaoNaDeclaracao(tree->sons[1], tipo);

	}else if(tree->sons[1]->getNameNode() == "ID"){

		if(this->hash->containsKey(tree->sons[1]->getValue(), this->scope)){

			cout << "\n\n Erro Semantico: Linha: " << tree->getLine() << ": Variavel " << tree->sons[1]->getValue() << " ja declarada...\n\n";
	   		exit(1);

		}else{

			this->hash->putVariable(tree->sons[1]->getValue(), tipo, this->scope);
			this->hash->keys.push_back(tree->sons[1]->getValue() + "&" + this->scope);
		}

	}else if(tree->sons[1]->getNameNode() == "LISTA_DECLARACAO"){

		this->trataListaDeclaracao(tree->sons[1], tipo);
	}

}


void Semantic::trataDeclaracao(Tree* treeNode){
	string tipo;

	if(treeNode->sons[0]->getNameNode() == "INT")
		tipo = "INT";
	else
		tipo = "FLOAT";


	if(treeNode->sons[1]->getNameNode() == "ASSIGN"){

		this->trataAtribuicaoNaDeclaracao(treeNode->sons[1],tipo);

			
	}else if(treeNode->sons[1]->getNameNode() == "LISTA_DECLARACAO"){

		trataListaDeclaracao(treeNode->sons[1], tipo);

	}else if(treeNode->sons[1]->getNameNode() == "ID"){
		if(this->hash->containsKey(treeNode->sons[1]->getValue(), this->scope)){
			cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << treeNode->sons[1]->getValue() << " ja declarada...\n\n";
		    exit(1);
		}else{
			this->hash->putVariable(treeNode->sons[1]->getValue(), tipo, this->scope);
			this->hash->keys.push_back(treeNode->sons[1]->getValue() + "&" + this->scope);
		}
	}
	
}


void Semantic::trataParametrosDeclaracaoDeFuncao(Tree* treeNode, string id){

	this->hash->adicionaParametro(id, treeNode->sons[0]->getNameNode());
	this->hash->adicionaId(id, treeNode->sons[1]->getValue());

	this->hash->putVariable(treeNode->sons[1]->getValue(), treeNode->sons[0]->getNameNode(), id);
	this->hash->setAlreadyAssigned(treeNode->sons[1]->getValue(), id);

	if(treeNode->sons.size() > 2)
		this->trataParametrosDeclaracaoDeFuncao(treeNode->sons[2],id);	

}


void Semantic::trataDeclaracaoFuncao(Tree* treeNode){

	string id;

	id = treeNode->sons[1]->getValue();

	this->setScope(id);


	if(treeNode->sons[2]->getNameNode() == "PARAMETROS_DEC_FUNC"){

		this->trataParametrosDeclaracaoDeFuncao(treeNode->sons[2], id);

		if(treeNode->sons.size() > 3){
			this->secondPass(treeNode->sons[3]);
		}

	}else if(treeNode->sons.size() > 2){
		this->secondPass(treeNode->sons[2]);
	}

	this->setScope("&");
}


void Semantic::trataDeclaracaoFuncaoPrincipal(Tree* treeNode){

	this->setScope("principal");

	this->hash->putFunction("principal", treeNode->sons[0]->getNameNode());

	this->secondPass(treeNode->sons[1]);

	this->setScope("&");
}


void Semantic::trataExpressao(Tree* treeNode){

	this->aritmetica(treeNode->sons[0]);

	if(treeNode->sons.size() > 2)
		this->aritmetica(treeNode->sons[2]);
}


void Semantic::trataIF(Tree* treeNode){

	this->trataExpressao(treeNode->sons[0]);

	this->secondPass(treeNode->sons[1]);

	if(treeNode->sons.size() > 2)
		this->secondPass(treeNode->sons[2]);
}


void Semantic::trataRepeticao(Tree* treeNode){

	this->secondPass(treeNode->sons[0]);

	this->trataExpressao(treeNode->sons[1]);
}


void Semantic::trataRetorno(Tree* treeNode){

	string tipoFunction;
	string tipoRetorno;
	string s;

	tipoFunction = this->hash->getType(this->scope, "&");

	if(tipoFunction == "VAZIO"){

		cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": A função não tem retorno\n\n";
		exit(1);

	}
    
	if(treeNode->sons[0]->getNameNode() == "ID"){
		if(!this->hash->containsKey(treeNode->sons[0]->getValue(),this->scope)){
			if(!this->hash->containsKey(treeNode->sons[0]->getValue(),"&")){
				cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": " + treeNode->sons[0]->getValue() << " nao foi declarado\n\n";
				exit(1);
			}else if(!this->hash->getAlreadyAssigned(treeNode->sons[0]->getValue(), "&")){
				cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Nenhum valor foi atribuído à variável " << treeNode->sons[0]->getValue() << "\n\n";
				exit(1);
			}else{
				tipoRetorno = this->hash->getType(treeNode->sons[0]->getValue(), "&");
				s = "&";
			}
		}else if(!this->hash->getAlreadyAssigned(treeNode->sons[0]->getValue(), this->scope)){
			cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Nenhum valor foi atribuído à variável " << treeNode->sons[0]->getValue() << "\n\n";
			exit(1);
		}else{
			tipoRetorno = this->hash->getType(treeNode->sons[0]->getValue(), this->scope);
			s = this->scope;
		}

		this->hash->setIsUsed(treeNode->sons[0]->getValue(), s);
	}

	else if(treeNode->sons[0]->getNameNode() == "VAL_INT")
		tipoRetorno = "INT";

	else if(treeNode->sons[0]->getNameNode() == "VAL_DOUBLE")
		tipoRetorno = "FLOAT";

	else if(treeNode->sons[0]->getNameNode() == "CHAMADA_FUNC")
		tipoRetorno = this->trataChamadaFuncao(treeNode->sons[0]);

	else if(treeNode->sons[0]->getNameNode() == "+" || treeNode->sons[0]->getNameNode() == "-" || treeNode->sons[0]->getNameNode() == "*" || treeNode->sons[0]->getNameNode() == "/"){
		this->aritmetica(treeNode->sons[0]);
		tipoRetorno = this->hash->getType(this->scope, "&"); // Arrumar aqui
	}

	if(tipoFunction != tipoRetorno)
		cout << "\n\n Warning: Linha: " << treeNode->getLine() << ": O valor retornado não corresponde ao tipo de retorno da função\n\n";
}


void Semantic::trataAtribuicao(Tree* treeNode){

	string tipoIdOrigem;
	string tipoIdDestino;
	string scope;

	if(this->scope != "&" && !this->hash->containsKey(treeNode->sons[0]->getValue(), this->scope)){
		if(!this->hash->containsKey(treeNode->sons[0]->getValue(), "&")){
			cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << treeNode->sons[0]->getValue() << " nao foi declarada...\n\n";
			exit(1);
		}else{
			scope = "&";
			tipoIdDestino = this->hash->getType(treeNode->sons[0]->getValue(), "&");
		}
	}else if(!this->hash->containsKey(treeNode->sons[0]->getValue(), this->scope)){
		cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << treeNode->sons[0]->getValue() << " nao foi declarada...\n\n";
		exit(1);
	}else{
		scope = this->scope;
		tipoIdDestino = this->hash->getType(treeNode->sons[0]->getValue(), this->scope);
	}

	if(treeNode->sons[1]->getNameNode() == "ID"){

		this->trataID(treeNode, treeNode->sons[1]->getValue(), tipoIdDestino);

		this->hash->setIsUsed(treeNode->sons[1]->getValue(), scope);

	}

	if(treeNode->sons[1]->getNameNode() == "VAL_INT") {

		if(tipoIdDestino == "FLOAT"){
			cout << "\n\n Warning: Linha: " << treeNode->getLine() << ": Atribuindo um valor do tipo inteiro a um identificador do tipo flutuante\n\n";
			//exit(1);
		}

	}

	if(treeNode->sons[1]->getNameNode() == "VAL_DOUBLE") { 

	   	if(tipoIdDestino == "INT"){
			cout << "\n\n Warning: Linha: " << treeNode->getLine() << ": Atribuindo um valor do tipo flutuante a um identificador do tipo inteiro\n\n";
			//exit(1);
		}
	}

	if(treeNode->sons[1]->getNameNode() == "CHAMADA_FUNC") {

		 string ret = this->trataChamadaFuncao(treeNode->sons[1]);

	   	if(tipoIdDestino == "INT" && ret == "FLOAT"){
			cout << "\n\n Warning: Linha: " << treeNode->getLine() << ": Atribuindo um valor do tipo flutuante a um identificador do tipo inteiro\n\n";
			//exit(1);
		}
		if(tipoIdDestino == "FLOAT" && ret == "INT"){
			cout << "\n\n Warning: Linha: " << treeNode->getLine() << ": Atribuindo um valor do tipo inteiro a um identificador do tipo flutuante\n\n";
			//exit(1);
		}
	}

	if(treeNode->sons[1]->getNameNode() == "+" || treeNode->sons[1]->getNameNode() == "-" || treeNode->sons[1]->getNameNode() == "*" || treeNode->sons[1]->getNameNode() == "/"){

		this->aritmetica(treeNode->sons[1]);
			
	}

	this->hash->setAlreadyAssigned(treeNode->sons[0]->getValue(), scope);
}


void Semantic::trataLeitura(Tree* treeNode){

	string s;

	if(treeNode->sons[0]->getNameNode() == "ID"){

		if(!this->hash->containsKey(treeNode->sons[0]->getValue(), this->scope)){
			if(!this->hash->containsKey(treeNode->sons[0]->getValue(), "&")){
				cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << treeNode->sons[0]->getValue() << " nao foi declarada...\n\n";
				exit(1);
			}else{
				s = "&";
			}
		}else{
			s = this->scope;
		}
		
		this->hash->setAlreadyAssigned(treeNode->sons[0]->getValue(), s);

	}

	else if(treeNode->sons[0]->getNameNode() == "LISTA_LEIT_ESCRI")
		this->trataListaLeitura(treeNode->sons[0]);
}


void Semantic::trataListaLeitura(Tree* treeNode){

	string s;

	if(treeNode->sons[0]->getNameNode() == "ID"){

		if(!this->hash->containsKey(treeNode->sons[0]->getValue(), this->scope)){
			if(!this->hash->containsKey(treeNode->sons[0]->getValue(), "&")){
				cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << treeNode->sons[0]->getValue() << " nao foi declarada...\n\n";
				exit(1);
			}else{
				s = "&";
			}
		}else{
			s = this->scope;
		}

		this->hash->setAlreadyAssigned(treeNode->sons[0]->getValue(), s);

	}

	if(treeNode->sons[1]->getNameNode() == "LISTA_LEIT_ESCRI")
		this->trataListaLeitura(treeNode->sons[1]);
	else if(treeNode->sons[1]->getNameNode() == "ID"){

		if(!this->hash->containsKey(treeNode->sons[1]->getValue(), this->scope)){
			if(!this->hash->containsKey(treeNode->sons[1]->getValue(), "&")){
				cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << treeNode->sons[1]->getValue() << " nao foi declarada...\n\n";
				exit(1);
			}else{
				s = "&";
			}
		}else{
			s = this->scope;
		}

		this->hash->setAlreadyAssigned(treeNode->sons[1]->getValue(), s);

	}
}


void Semantic::trataEscrita(Tree* treeNode){

	string s;
		
	if(treeNode->sons[0]->getNameNode() == "ID"){ 

		if(!this->hash->containsKey(treeNode->sons[0]->getValue(), this->scope)){
			if(!this->hash->containsKey(treeNode->sons[0]->getValue(), "&")){
				cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << treeNode->sons[0]->getValue() << " nao foi declarada...\n\n";
				exit(1);
			}else{
				s = "&";
			}
		}else{
			s = this->scope;
		}

		if(this->hash->getAlreadyAssigned(treeNode->sons[0]->getValue(), s) == false){
			cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel sendo utilizada sem valor atribuído\n\n";
			exit(1);
		}

		this->hash->setIsUsed(treeNode->sons[0]->getValue(), s);

	}

	else if(treeNode->sons[0]->getNameNode() == "CHAMADA_FUNC")
		this->trataChamadaFuncao(treeNode->sons[0]);

	else if(treeNode->sons[0]->getNameNode() == "LISTA_LEIT_ESCRI")
		this->trataListaEscrita(treeNode->sons[0]);
}


void Semantic::trataListaEscrita(Tree* treeNode){

	string s;

	if(treeNode->sons[0]->getNameNode() == "ID"){

		if(!this->hash->containsKey(treeNode->sons[0]->getValue(), this->scope)){
			if(!this->hash->containsKey(treeNode->sons[0]->getValue(), "&")){
				cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << treeNode->sons[0]->getValue() << " nao foi declarada...\n\n";
				exit(1);
			}else{
				s = "&";
			}
		}else{
			s = this->scope;
		}

		if(this->hash->getAlreadyAssigned(treeNode->sons[0]->getValue(), s) == false){
			cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel sendo utilizada sem valor atribuído\n\n";
			exit(1);
		}

		this->hash->setIsUsed(treeNode->sons[0]->getValue(), s);

	}else if(treeNode->sons[0]->getNameNode() == "CHAMADA_FUNC")
		this->trataChamadaFuncao(treeNode->sons[0]);


	if(treeNode->sons[1]->getNameNode() == "LISTA_LEIT_ESCRI")
		this->trataListaEscrita(treeNode->sons[1]);
	else if(treeNode->sons[1]->getNameNode() == "ID"){

		if(!this->hash->containsKey(treeNode->sons[1]->getValue(), this->scope)){
			if(!this->hash->containsKey(treeNode->sons[1]->getValue(), "&")){
				cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel " << treeNode->sons[1]->getValue() << " nao foi declarada...\n\n";
				exit(1);
			}else{
				s = "&";
			}
		}else{
			s = this->scope;
		}

		if(this->hash->getAlreadyAssigned(treeNode->sons[1]->getValue(), s) == false){
			cout << "\n\n Erro Semantico: Linha: " << treeNode->getLine() << ": Variavel sendo utilizada sem valor atribuído\n\n";
			exit(1);
		}

		this->hash->setIsUsed(treeNode->sons[1]->getValue(), s);

	}else if(treeNode->sons[1]->getNameNode() == "CHAMADA_FUNC")
		this->trataChamadaFuncao(treeNode->sons[1]);


}