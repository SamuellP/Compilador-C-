
#include "CodeGenerator.h"
#include "Parser.h"
#include "Semantic.h"

CodeGenerator::CodeGenerator(Tree* tree, boost::unordered_map<string, Simbolo*> symbolTable){

	this->module = new Module("program-T++", getGlobalContext());
	this->tree = tree;
	this->symbolTable = symbolTable;
	this->scope = "global";
	this->rf = "";
	this->counter = 0;
	this->counterIF = 0;

}


std::vector<Type*> CodeGenerator::getParametersTypes(Tree* treeNode){

	std::vector<Type*> types;
	std::vector<string> atributeTypes = this->symbolTable[treeNode->sons[1]->getValue() + "&&"]->getParametersTypes();

	for(int i = 0; i < atributeTypes.size(); i++){

		if(atributeTypes[i] == "INT")
			types.push_back(IntegerType::get(getGlobalContext(),64));
		else if(atributeTypes[i] == "FLOAT")
			types.push_back(llvm::Type::getFloatTy(getGlobalContext()));
	}

	return types;


}

Function* CodeGenerator::createMainFunction(Tree* treeNode){

	FunctionType* funcType;
	Function* function;

	if(treeNode->sons[0]->getNameNode() == "INT"){
		funcType = FunctionType::get(Type::getInt64Ty(getGlobalContext()),false);
		this->rf = "INT";
	}
	else if(treeNode->sons[0]->getNameNode() == "FLOAT"){
		funcType = FunctionType::get(Type::getFloatTy(getGlobalContext()),false);
		this->rf = "FLOAT";
	}
	else if(treeNode->sons[0]->getNameNode() == "VAZIO"){
		funcType = FunctionType::get(Type::getVoidTy(getGlobalContext()),false);
		this->rf = "VAZIO";
	}

	function = Function::Create(funcType, GlobalValue::ExternalLinkage, "main", this->module);

	function->setCallingConv(CallingConv::C);

	return function;

}


Function* CodeGenerator::createFunction(Tree* treeNode){

	FunctionType* funcType;
	Function* function;

	if(this->symbolTable[treeNode->sons[1]->getValue() + "&&"]->getParametersTypes().size() != 0){

		std::vector<Type*> types = getParametersTypes(treeNode);

		if(treeNode->sons[0]->getNameNode() == "INT"){
			funcType = FunctionType::get(Type::getInt64Ty(getGlobalContext()),types,false);
			this->rf = "INT";
		}
		else if(treeNode->sons[0]->getNameNode() == "FLOAT"){
			funcType = FunctionType::get(Type::getFloatTy(getGlobalContext()),types,false);
			this->rf = "FLOAT";
		}
		else if(treeNode->sons[0]->getNameNode() == "VAZIO"){
			funcType = FunctionType::get(Type::getVoidTy(getGlobalContext()),types,false);
			this->rf = "VAZIO";
		}


	}else{

		if(treeNode->sons[0]->getNameNode() == "INT"){
			funcType = FunctionType::get(Type::getInt64Ty(getGlobalContext()),false);
			this->rf = "INT";
		}
		else if(treeNode->sons[0]->getNameNode() == "FLOAT"){
			funcType = FunctionType::get(Type::getFloatTy(getGlobalContext()),false);
			this->rf = "FLOAT";
		}
		else if(treeNode->sons[0]->getNameNode() == "VAZIO"){
			funcType = FunctionType::get(Type::getVoidTy(getGlobalContext()),false);
			this->rf = "VAZIO";
		}

	}

	function = Function::Create(funcType, GlobalValue::ExternalLinkage, treeNode->sons[1]->getValue(), this->module);

	function->setCallingConv(CallingConv::C);

	std::vector<string> idParameters = this->symbolTable[treeNode->sons[1]->getValue() + "&&"]->getParameters();

	int i = 0;
	for (Function::arg_iterator argIt = function->arg_begin(); argIt != function->arg_end(); argIt++) {
		Value* aux = argIt;
	    aux->setName(idParameters[i]);
	    this->tabela[idParameters[i] + "&" + treeNode->sons[1]->getValue()] = aux;
	    i++;
	}

	return function;

}


void CodeGenerator::treatStmtFunction(Tree* treeNode, IRBuilder<> builder, BasicBlock* block){
	
	if(treeNode->getBrother() != NULL){

        this->treatStmtFunction(treeNode->getBrother(), builder, block);
	}

    string nameNode = treeNode->getNameNode();

    if(nameNode == "ASSIGN")
    	this->treatAssign(treeNode, builder, block);
    else if(nameNode == "DECLARACAO")
    	this->trataDeclaracao(treeNode, builder, block);
    else if(nameNode == "RETORNA")
    	this->trataRetorno(treeNode, builder, block);
    else if(nameNode == "IF")
    	this->trataIF(treeNode, builder);
    

}

void CodeGenerator::trataRetorno(Tree* treeNode, IRBuilder<> builder, BasicBlock* block){

	Value* v;

	if(treeNode->sons[0]->getNameNode() == "ID"){

		v = this->tabela[treeNode->sons[0]->getValue() + "&" + this->scope];
		builder.CreateRet(v);
	}

	if(treeNode->sons[0]->getNameNode() == "VAL_INT"){
		v = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), atoi(treeNode->sons[0]->getValue().c_str()));
		builder.CreateRet(v);
	}

	if(treeNode->sons[0]->getNameNode() == "CHAMADA_FUNC"){

		v = this->trataChamFunc(treeNode->sons[0], builder, "ret");
		builder.CreateRet(v);
	}

	if(treeNode->sons[0]->getNameNode() == "+" || treeNode->sons[0]->getNameNode() == "-" || treeNode->sons[0]->getNameNode() == "*" || treeNode->sons[0]->getNameNode() == "/"){
		v = treatMath(treeNode->sons[0], builder, block);
		builder.CreateRet(v);
	}
}

void CodeGenerator::trataDeclaracao(Tree* treeNode, IRBuilder<> builder, BasicBlock* block){

	if(treeNode->sons[1]->getNameNode() == "ASSIGN"){
		this->treatAssign(treeNode->sons[1], builder, block);
		return;
	}

	if(treeNode->sons[1]->getNameNode() == "LISTA_DECLARACAO")
		this->trataListaDeclaracao(treeNode->sons[1], builder, block);
}

void CodeGenerator::trataListaDeclaracao(Tree* treeNode, IRBuilder<> builder, BasicBlock* block){

	if(treeNode->sons[0]->getNameNode() == "ASSIGN")
		this->treatAssign(treeNode->sons[0], builder, block);

	if(treeNode->sons[1]->getNameNode() == "ASSIGN")
		this->treatAssign(treeNode->sons[1], builder, block);

	if(treeNode->sons[1]->getValue() == "LISTA_DECLARACAO")
		this->trataListaDeclaracao(treeNode->sons[1], builder, block);
}


void CodeGenerator::treatFunction(Tree* treeNode){

	this->scope = treeNode->sons[1]->getValue();

	Function* function = createFunction(treeNode);

	this->functions[treeNode->sons[1]->getValue()] = function;

	BasicBlock* block = BasicBlock::Create(getGlobalContext(), "entry", function);

	IRBuilder<> builder(block);

	if(treeNode->sons.size() == 3 && treeNode->sons[2]->getNameNode() != "PARAMETROS_DEC_FUNC")
		this->treatStmtFunction(treeNode->sons[2], builder, block);
	else if(treeNode->sons.size() == 4 && treeNode->sons[3]->getNameNode() != "PARAMETROS_DEC_FUNC")
		this->treatStmtFunction(treeNode->sons[3], builder, block);

	if(treeNode->sons[0]->getNameNode() == "VAZIO")
		builder.CreateRetVoid();

	this->scope = "global";

}


void CodeGenerator::treatMainFunction(Tree* treeNode){

	this->scope = "principal";

	Function* function = createMainFunction(treeNode);

	BasicBlock* block = BasicBlock::Create(getGlobalContext(), "entry", function);

	IRBuilder<> builder(block);

	//Value* val1 = BinaryOperator::Create(Instruction::Add,ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 10), ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0), "t", block);

	if(treeNode->sons.size() == 2)
		this->treatStmtFunction(treeNode->sons[1], builder, block);

	
	if(treeNode->sons[0]->getNameNode() == "VAZIO")
		builder.CreateRetVoid();

	this->scope = "global";

}


void CodeGenerator::treatAssign(Tree* treeNode, IRBuilder<> builder, BasicBlock* block){

	std::ostringstream oss;

	Value* val1;
	Value* val2;

	string idType1;
	string idType2;
	string scope1;
	string scope2;

	if(this->symbolTable[treeNode->sons[0]->getValue() + "&" + scope] != NULL){
		idType1 = this->symbolTable[treeNode->sons[0]->getValue() + "&" + this->scope]->getTipo();
		scope1 = this->scope;
	}else{
		idType1 = this->symbolTable[treeNode->sons[0]->getValue() + "&&"]->getTipo();
		scope1 = "global";
	}

	if(treeNode->sons[1]->getNameNode() == "ID"){

		if(this->symbolTable[treeNode->sons[1]->getValue() + "&" + scope] != NULL){
			idType2 = this->symbolTable[treeNode->sons[1]->getValue() + "&" + this->scope]->getTipo();
			scope2 = this->scope;
		}else{
			idType2 = this->symbolTable[treeNode->sons[1]->getValue() + "&&"]->getTipo();
			scope2 = "global";
		}


		if(idType1 != idType2){

			if(idType1 == "INT"){
				//oss << "tmp" << this->counter;

				val2 = this->tabela[treeNode->sons[1]->getValue() + "&" + scope2];

				oss << "tmp" << this->counter;
				this->counter++;

				val2->mutateType(Type::getInt64Ty(getGlobalContext())); 
				//val2 = BinaryOperator::Create(Instruction::Add, val2, ConstantInt::get(Type::getInt64Ty(getGlobalContext()),0),oss.str(),block);


				//oss << "tmp" <<this->counter;
				val1 = BinaryOperator::Create(Instruction::Add, val2, ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0), treeNode->sons[0]->getValue(),block);
				this->counter++;

				this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

			}else if(idType1 == "FLOAT"){
				oss << "tmp" << this->counter;
				this->counter++;

				val2 = this->tabela[treeNode->sons[1]->getValue() + "&" + scope2];

				val2->mutateType(Type::getFloatTy(getGlobalContext())); 
				//val2 = BinaryOperator::Create(Instruction::FAdd, val2, ConstantFP::get(Type::getFloatTy(getGlobalContext()),0.0),oss.str(),block);

				//oss << "tmp" << this->counter;
				//this->counter++;

				val1 = BinaryOperator::Create(Instruction::FAdd, val2, ConstantFP::get(Type::getFloatTy(getGlobalContext()), 0.0), treeNode->sons[0]->getValue(), block);
				

				this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

			}

		}else {

			if(idType1 == "INT"){
				oss << "tmp" << this->counter;

				val2 = this->tabela[treeNode->sons[1]->getValue() + "&" + scope2];

				val1 = BinaryOperator::Create(Instruction::Add, val2, ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0), treeNode->sons[0]->getValue(), block);
				this->counter++;

				this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

			}else if(idType1 == "FLOAT"){

				oss << "tmp" << this->counter;

				val2 = this->tabela[treeNode->sons[1]->getValue() + "&" + scope2];

				val1 = BinaryOperator::Create(Instruction::FAdd, val2, ConstantFP::get(Type::getFloatTy(getGlobalContext()), 0.0), treeNode->sons[0]->getValue(),block);
				this->counter++;

				this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

			}

		}

	}

	else if(treeNode->sons[1]->getNameNode() == "VAL_INT"){

		if(idType1 == "FLOAT"){
			oss << "tmp" << this->counter;
			
			float valf = atoi(treeNode->sons[1]->getValue().c_str());

			val1 = BinaryOperator::Create(Instruction::FAdd, ConstantFP::get(Type::getFloatTy(getGlobalContext()), valf), ConstantFP::get(Type::getFloatTy(getGlobalContext()), 0.0), treeNode->sons[0]->getValue(),block);
			this->counter++;

			this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

		}else if(idType1 == "INT"){

			oss << "tmp" << this->counter;

			int vali = atoi(treeNode->sons[1]->getValue().c_str());

			val1 = BinaryOperator::Create(Instruction::Add,ConstantInt::get(Type::getInt64Ty(getGlobalContext()), vali), ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0), treeNode->sons[0]->getValue(), block);
			this->counter++;

			this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

		}

	}

	else if(treeNode->sons[1]->getNameNode() == "VAL_DOUBLE"){

		if(idType1 == "FLOAT"){
			oss << "tmp" << this->counter;
			
			float valf = atof(treeNode->sons[1]->getValue().c_str());

			val1 = BinaryOperator::Create(Instruction::FAdd, ConstantFP::get(Type::getFloatTy(getGlobalContext()), valf), ConstantFP::get(Type::getFloatTy(getGlobalContext()), 0.0), treeNode->sons[0]->getValue(),block);
			this->counter++;

			this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

		}else if(idType1 == "INT"){

			oss << "tmp" << this->counter;

			int vali = atof(treeNode->sons[1]->getValue().c_str());

			val1 = BinaryOperator::Create(Instruction::Add,ConstantInt::get(Type::getInt64Ty(getGlobalContext()), vali), ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0), treeNode->sons[0]->getValue(), block);
			this->counter++;

			this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

		}

	}

	else if(treeNode->sons[1]->getNameNode() == "CHAMADA_FUNC"){
		
		val2 = this->trataChamFunc(treeNode->sons[1], builder, treeNode->sons[0]->getValue());

		if(idType1 == "FLOAT"){

			if(this->symbolTable[treeNode->sons[1]->sons[0]->getValue() + "&&"]->getTipo() == "INT")
				val2->mutateType(Type::getFloatTy(getGlobalContext()));

			val1 = BinaryOperator::Create(Instruction::FAdd, val2, ConstantFP::get(Type::getFloatTy(getGlobalContext()), 0.0), treeNode->sons[0]->getValue(),block);

			this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

		}else if(idType1 == "INT"){

			if(this->symbolTable[treeNode->sons[1]->sons[0]->getValue() + "&&"]->getTipo() == "FLOAT")
				val2->mutateType(Type::getInt64Ty(getGlobalContext()));

			val1 = BinaryOperator::Create(Instruction::FAdd, val2, ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0), treeNode->sons[0]->getValue(),block);

			this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

		}
	}

	else if(treeNode->sons[1]->getNameNode() == "+" || treeNode->sons[1]->getNameNode() == "-" || treeNode->sons[1]->getNameNode() == "*" || treeNode->sons[1]->getNameNode() == "/"){
		
		oss << "tmp" << this->counter;
		this->counter++;

		val2 = this->treatMath(treeNode->sons[1], builder, block);

		if(idType1 == "INT"){

			val2->mutateType(Type::getInt64Ty(getGlobalContext()));
			//val2 = BinaryOperator::Create(Instruction::Add, val2, ConstantInt::get(Type::getInt64Ty(getGlobalContext()),0),oss.str(),block);

			//oss << "tmp" << this->counter;
			//std::cout<<"2 - "<<oss.str()<<std::endl;
			//this->counter++;

			val1 = BinaryOperator::Create(Instruction::Add, val2, ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0), treeNode->sons[0]->getValue(), block);
			this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

		}else if(idType1 == "FLOAT"){

			val1 = BinaryOperator::Create(Instruction::FAdd, val2, ConstantFP::get(Type::getFloatTy(getGlobalContext()), 0.0), treeNode->sons[0]->getValue(), block);
			this->tabela[treeNode->sons[0]->getValue() + "&" + scope1] = val1;

		}
	}
}


/*Value* CodeGenerator::treatMath(Tree* treeNode, BasicBlock* block){

	std::cout<<"\n\n\n\nMath\n\n\n\n\n";

	string op = treeNode->getNameNode();

	std::ostringstream oss;

	Type* defaultType = Type::getFloatTy(getGlobalContext());

	Value* aux;

	if(op == "+" || op == "-" || op == "*" || op == "/"){

		Value* left = treatMath(treeNode->sons[0], block);
		Value* right = treatMath(treeNode->sons[1], block);

		//left->mutateType(defaultType);
		//right->mutateType(defaultType);

		oss << "tmp" << this->counter;
		this->counter++;

		if(treeNode->getNameNode() == "+"){
			
			aux = BinaryOperator::Create(Instruction::FAdd, left, right, oss.str(), block);
		}
		if(treeNode->getNameNode() == "-")
			aux = BinaryOperator::Create(Instruction::FSub, left, right, oss.str(), block);
		if(treeNode->getNameNode() == "*")
			aux = BinaryOperator::Create(Instruction::FMul, left, right, oss.str(), block);
		if(treeNode->getNameNode() == "/")
			aux = BinaryOperator::Create(Instruction::FDiv, left, right, oss.str(), block);

		return aux;

	}


	if(treeNode->getNameNode() == "ID"){

		if(this->symbolTable[treeNode->getValue() + "&" + this->scope]->getTipo() == "INT"){
			oss << "tmp" << this->counter;
			this->counter++;
			aux = this->tabela[treeNode->getValue() + "&" + this->scope];
			aux->mutateType(defaultType);
			aux = BinaryOperator::Create(Instruction::FAdd, aux, ConstantFP::get(Type::getFloatTy(getGlobalContext()),0.0),oss.str(),block);
			return aux;
		}
		else{
			aux = this->tabela[treeNode->getValue() + "&" + this->scope];
			return aux;
		}
	}
	if(treeNode->getNameNode() == "VAL_INT"){
		float num = atoi(treeNode->getValue().c_str());
		aux = ConstantFP::get(Type::getFloatTy(getGlobalContext()), num);
		return aux;
	}
	if(treeNode->getNameNode() == "VAL_DOUBLE"){
		float num = atof(treeNode->getValue().c_str());
		aux = ConstantFP::get(Type::getFloatTy(getGlobalContext()), num);
		return aux;
	}
}*/

Value* CodeGenerator::treatMath(Tree* treeNode, IRBuilder<> builder, BasicBlock* block){

	string op = treeNode->getNameNode();

	std::ostringstream oss;

	Type* defaultType = Type::getFloatTy(getGlobalContext());

	Value* aux;

	if(op == "+" || op == "-" || op == "*" || op == "/"){

		Value* left = treatMath(treeNode->sons[0], builder, block);
		Value* right = treatMath(treeNode->sons[1], builder, block);

		//left->mutateType(defaultType);
		//right->mutateType(defaultType);

		oss << "tmp" << this->counter;
		this->counter++;

		if(treeNode->getNameNode() == "+"){
			
			aux = BinaryOperator::Create(Instruction::Add, left, right, oss.str(), block);
		}
		if(treeNode->getNameNode() == "-")
			aux = BinaryOperator::Create(Instruction::Sub, left, right, oss.str(), block);
		if(treeNode->getNameNode() == "*")
			aux = BinaryOperator::Create(Instruction::Mul, left, right, oss.str(), block);
		if(treeNode->getNameNode() == "/")
			aux = BinaryOperator::Create(Instruction::SDiv, left, right, oss.str(), block);

		//int a = (int)aux->getValueID();
		//cout<<"\n\n\n&&&&    "<< a <<"   &&&&&&\n\n\n";
		return aux;

	}

	this->counter++;


	if(treeNode->getNameNode() == "ID"){

		if(this->symbolTable[treeNode->getValue() + "&" + this->scope]->getTipo() == "INT"){
			//oss << "tmp" << this->counter;
			//this->counter++;
			aux = this->tabela[treeNode->getValue() + "&" + this->scope];
			//aux->mutateType(defaultType);
			//aux = BinaryOperator::Create(Instruction::FAdd, aux, ConstantFP::get(Type::getFloatTy(getGlobalContext()),0.0),oss.str(),block);
			return aux;
		}
		else{
			aux = this->tabela[treeNode->getValue() + "&" + this->scope];
			return aux;
		}
	}
	if(treeNode->getNameNode() == "VAL_INT"){
		int num = atoi(treeNode->getValue().c_str());
		aux = ConstantInt::get(Type::getInt64Ty(getGlobalContext()), num);
		return aux;
	}
	if(treeNode->getNameNode() == "VAL_DOUBLE"){
		float num = atof(treeNode->getValue().c_str());
		aux = ConstantFP::get(Type::getFloatTy(getGlobalContext()), num);
		return aux;
	}

	if(treeNode->getNameNode() == "CHAMADA_FUNC"){
		oss << "tmp" << this->counter;
		this->counter++;
		return this->trataChamFunc(treeNode, builder, oss.str());

	}
}

void CodeGenerator::generateCode(Tree* treeNode){

	if(treeNode->getBrother() != NULL)
        this->generateCode(treeNode->getBrother());

    string nameNode = treeNode->getNameNode();

    if(nameNode == "DECL_FUNCAO"){
    	this->treatFunction(treeNode); 
    }else if(nameNode == "DECL_FUNCAO_PRINCIPAL"){
    	this->treatMainFunction(treeNode); 
    }
}


void CodeGenerator::runLLVM(){

	this->generateCode(this->tree);

	verifyModule(*module, PrintMessageAction);

  	module->dump();

  	//PassManager PM;
  	//PM.add(createPrintModulePass(&outs()));
  	//PM.run(*module);

  	delete module;
}


std::vector<Value*> CodeGenerator::getParametersCallFunc(Tree* treeNode, IRBuilder<> builder, string id){

	Tree* treeNodeAux = treeNode;

	bool aux = true;

	std::vector<Value*> values;

	string s;

	while(aux == true){


		if(treeNodeAux->sons[0]->getNameNode() == "ID"){
			if(this->symbolTable[treeNodeAux->sons[0]->getValue() + "&" + this->scope] != NULL)
				s = this->scope;
			else
				s = "global";

			values.push_back(this->tabela[treeNodeAux->sons[0]->getValue() + "&" + this->scope]);
		}

		else if(treeNodeAux->sons[0]->getNameNode() == "VAL_INT"){

			values.push_back(ConstantInt::get(Type::getInt64Ty(getGlobalContext()), atoi(treeNodeAux->sons[0]->getValue().c_str())));

		}

		else if(treeNodeAux->sons[0]->getNameNode() == "VAL_DOUBLE"){

			values.push_back(ConstantFP::get(Type::getFloatTy(getGlobalContext()), atof(treeNodeAux->sons[0]->getValue().c_str())));
		}

		else if(treeNodeAux->sons[0]->getNameNode() == "CHAMADA_FUNC"){

			values.push_back(this->trataChamFunc(treeNodeAux->sons[0], builder, id));
		}


		if(treeNodeAux->sons[1]->getNameNode() == "ID"){
			if(this->symbolTable[treeNodeAux->sons[1]->getValue() + "&" + this->scope] != NULL)
				s = this->scope;
			else
				s = "global";

			values.push_back(this->tabela[treeNodeAux->sons[1]->getValue() + "&" + s]);
			aux = false;
		}

		else if(treeNodeAux->sons[1]->getNameNode() == "VAL_INT"){

			values.push_back(ConstantInt::get(Type::getInt64Ty(getGlobalContext()), atoi(treeNodeAux->sons[1]->getValue().c_str())));
			aux = false;

		}

		else if(treeNodeAux->sons[1]->getNameNode() == "VAL_DOUBLE"){

			values.push_back(ConstantFP::get(Type::getFloatTy(getGlobalContext()), atof(treeNodeAux->sons[1]->getValue().c_str())));
			aux = false;
		}

		else if(treeNodeAux->sons[1]->getNameNode() == "CHAMADA_FUNC"){

			values.push_back(this->trataChamFunc(treeNodeAux->sons[1], builder, id));
			aux = false;
		}

		else if(treeNodeAux->sons[1]->getNameNode() == "PARAMETROS_CHAM_FUNC"){

			treeNodeAux = treeNodeAux->sons[1];

		}

	}

	return values;
}

Value* CodeGenerator::trataChamFunc(Tree* treeNode, IRBuilder<> builder, string id){

	Function* function = this->functions[treeNode->sons[0]->getValue()];

	std::vector<Value*> values;
	Value* value;

	string s;

	if(treeNode->sons[1]->getNameNode() == "PARAMETROS_CHAM_FUNC")
		values = this->getParametersCallFunc(treeNode->sons[1], builder, id);
	else{

		if(treeNode->sons[1]->getNameNode() == "ID"){
			if(this->symbolTable[treeNode->sons[1]->getValue() + "&" + this->scope] != NULL)
				s = this->scope;
			else
				s = "global";

			value = this->tabela[treeNode->sons[1]->getValue() + "&" + s];
			
		}

		else if(treeNode->sons[1]->getNameNode() == "VAL_INT")
			value = ConstantInt::get(Type::getInt64Ty(getGlobalContext()), atoi(treeNode->sons[1]->getValue().c_str()));

		else if(treeNode->sons[1]->getNameNode() == "VAL_DOUBLE")
			value = ConstantFP::get(Type::getFloatTy(getGlobalContext()), atof(treeNode->sons[1]->getValue().c_str()));

		else if(treeNode->sons[1]->getNameNode() == "CHAMADA_FUNC")
			values.push_back(this->trataChamFunc(treeNode->sons[1], builder, id));

		values.push_back(value);

	}

	//return builder.CreateCall(function, values.begin(), values.end(), id);
	return builder.CreateCall(function, values, id);

}


void CodeGenerator::trataIF(Tree* tree, IRBuilder<> builder){

	std::ostringstream oss;

	BasicBlock* _if;
	BasicBlock* _else;

	Value* left;
	Value* right;

	string s;

	Tree* treeNode = tree->sons[0];

	oss << "_if" << this->counterIF;
	
	_if = BasicBlock::Create(getGlobalContext(), oss.str(), this->functions[this->scope]);

	//if(treeNode->sons.size() > 2){
		oss << "_else" << this->counterIF;
		_else = BasicBlock::Create(getGlobalContext(), oss.str(), this->functions[this->scope]);
	//}


	if(treeNode->sons[0]->getNameNode() == "ID"){
		if(this->symbolTable[treeNode->sons[0]->getValue() + "&" + this->scope] != NULL)
			s = this->scope;
		else
			s = "global";

		left = this->tabela[treeNode->sons[0]->getValue() + "&" + s];
	}

		else if(treeNode->sons[0]->getNameNode() == "VAL_INT")
			left = ConstantInt::get(Type::getInt64Ty(getGlobalContext()), atoi(treeNode->sons[0]->getValue().c_str()));

		else if(treeNode->sons[0]->getNameNode() == "VAL_DOUBLE")
			left = ConstantFP::get(Type::getFloatTy(getGlobalContext()), atof(treeNode->sons[0]->getValue().c_str()));

		else if(treeNode->sons[0]->getNameNode() == "CHAMADA_FUNC"){
			oss << "tmp" << this->counter;
			this->counter++;
			left = this->trataChamFunc(treeNode->sons[0], builder, oss.str());
		}



		if(treeNode->sons[1]->getNameNode() == "ID"){
			if(this->symbolTable[treeNode->sons[1]->getValue() + "&" + this->scope] != NULL)
				s = this->scope;
			else
				s = "global";

			right = this->tabela[treeNode->sons[1]->getValue() + "&" + s];
		}

		else if(treeNode->sons[1]->getNameNode() == "VAL_INT")
			right = ConstantInt::get(Type::getInt64Ty(getGlobalContext()), atoi(treeNode->sons[1]->getValue().c_str()));

		else if(treeNode->sons[1]->getNameNode() == "VAL_DOUBLE")
			right = ConstantFP::get(Type::getFloatTy(getGlobalContext()), atof(treeNode->sons[1]->getValue().c_str()));

		else if(treeNode->sons[1]->getNameNode() == "CHAMADA_FUNC"){
			oss << "tmp" << this->counter;
			this->counter++;
			right = this->trataChamFunc(treeNode->sons[1], builder, oss.str());
		}





	if(treeNode->getNameNode() == "="){

		Value* v = builder.CreateICmpEQ(left, right, "cond");
		builder.CreateCondBr(v, _if, _else);

	}

	if(treeNode->getNameNode() == "<"){
		Value* v = builder.CreateICmpULT(left, right, "cond");
		builder.CreateCondBr(v, _if, _else);
		//BinaryOperator::Create(Instruction::Add,ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 10), ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0), "t", block)
	}

	if(treeNode->getNameNode() == "<="){

		Value* v = builder.CreateICmpULE(left, right, "cond");
		builder.CreateCondBr(v, _if, _else);
	}

	if(treeNode->getNameNode() == ">"){
		Value* v = builder.CreateICmpUGT(left, right, "cond");
		builder.CreateCondBr(v, _if, _else);
	}

	if(treeNode->getNameNode() == ">="){

		Value* v = builder.CreateICmpUGE(left, right, "cond");
		builder.CreateCondBr(v, _if, _else);
	}

	this->counterIF++;
	this->counter++;

	builder.SetInsertPoint(_if);

	this->treatStmtFunction(tree->sons[1], builder, _if);
}


int main(int argc, char** argv)
{

	if(argc != 2) {  cout << "\n\n PARÂMETROS INVÁLIDOS \n\n"; return 0; }

    std::filebuf fb_in;
	fb_in.open(argv[1],std::ios::in);			
	std::istream* in_file = new std::istream(&fb_in);

	std::filebuf fb_out;
	fb_out.open("out.txt",std::ios::out);		
	std::ostream* out_file = new std::ostream(&fb_out);     

	Parser* parser = new Parser(in_file, out_file);
       	
    parser->yyparse();

    cout << "\n\n\nANÁLISE SINTATICA REALIZADA COM SUCESSO!\n\n" << endl;

    util->printTree(parser->tree);

    cout << "\n\n\n";

    Semantic* semantic = new Semantic(parser->tree);

    semantic->runSemantic(semantic->abstractSyntaxTree);


    CodeGenerator* codeg = new CodeGenerator(parser->tree, semantic->hash->tabela);

    codeg->runLLVM();

    return 1;
}