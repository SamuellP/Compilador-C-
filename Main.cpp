
#include "Parser.h"
#include "Semantic.h"
#include "CodeGenerator.h"

#include <iostream>
#include <fstream>

extern Util* util; // Obtendo classe Util da análise léxica

using namespace std;

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