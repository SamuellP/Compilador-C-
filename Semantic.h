#ifndef Included_Semantic_H
#define Included_Semantic_H

#include <string>
#include "Tree.h"
#include "Hash.h"
#include <boost/unordered_map.hpp>
#include <iostream>
#include <stdlib.h>

using namespace std;

class Semantic {

	public:

		Tree* abstractSyntaxTree;

		string scope;

		Hash* hash;

		Semantic(Tree* abstractSyntaxTree);

		void runSemantic(Tree* tree);

	private:
		void firstPass(Tree* tree);

		void secondPass(Tree* tree);

		void thirdPass();

		void setScope(string scope);

		double aritmetica(Tree* treeNode);

		void trataAtribuicaoNaDeclaracao(Tree* treeNode, string tipo);

		string trataChamadaFuncao(Tree* tree);

		void trataID(Tree* treeNode, string id, string tipoDestino);

		void trataListaDeclaracao(Tree* tree, string tipo);

		void trataDeclaracao(Tree* treeNode);

		void trataParametrosDeclaracaoDeFuncao(Tree* treeNode, string id);

		void trataDeclaracaoFuncao(Tree* treeNode);

		void trataDeclaracaoFuncaoPrincipal(Tree* treeNode);

		void trataExpressao(Tree* treeNode);

		void trataIF(Tree* treeNode);

		void trataRepeticao(Tree* treeNode);

		void trataRetorno(Tree* treeNode);

		void trataAtribuicao(Tree* treeNode);

		void trataLeitura(Tree* treeNode);

		void trataListaLeitura(Tree* treeNode);

		void trataEscrita(Tree* treeNode);

		void trataListaEscrita(Tree* treeNode);

};

#endif

