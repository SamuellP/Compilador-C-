#ifndef Included_CodeGenerator_H
#define Included_CodeGenerator_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <boost/unordered_map.hpp>
#include "Tree.h"
#include "Simbolo.h"
#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/PassManager.h"
#include "llvm/CallingConv.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;


class CodeGenerator {

public:

	boost::unordered_map<string, Value*> tabela;
	boost::unordered_map<string, Function*> functions;
	boost::unordered_map<string, Simbolo*> symbolTable;
	string scope;
	string rf;
	Tree* tree;
	Module* module;
	int counter;
	int counterIF;

	CodeGenerator(Tree* tree, boost::unordered_map<string, Simbolo*> symbolTable);
	std::vector<Type*> getParametersTypes(Tree* treeNode);
	Function* createMainFunction(Tree* treeNode);
	Function* createFunction(Tree* treeNode);
	void treatStmtFunction(Tree* treeNode, IRBuilder<> builder, BasicBlock* block);
	void treatFunction(Tree* treeNode);
	void treatMainFunction(Tree* treeNode);
	void treatAssign(Tree* treeNode, IRBuilder<> builder, BasicBlock* block);
	Value* treatMath(Tree* treeNode, IRBuilder<> builder, BasicBlock* block);
	void generateCode(Tree* treeNode);
	void trataDeclaracao(Tree* treeNode, IRBuilder<> builder, BasicBlock* block);
	void trataListaDeclaracao(Tree* treeNode, IRBuilder<> builder, BasicBlock* block);
	void trataRetorno(Tree* treeNode, IRBuilder<> builder, BasicBlock* block);
	Value* trataChamFunc(Tree* treeNode, IRBuilder<> builder, string id);
	std::vector<Value*> getParametersCallFunc(Tree* treeNode, IRBuilder<> builder, string id);
	void trataIF(Tree* treeNode, IRBuilder<> builder);
	void runLLVM();

};




#endif