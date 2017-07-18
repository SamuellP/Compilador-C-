%option c++
%option noyywrap
%option yylineno

%{

#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "Util.h"

Util* util = new Util();

int aux = 1; /*  utilizado para saber se o último token retornado foi um '\n', evitando que o mesmo seja retornado mais de uma vez 
				na mesma sequência */


%}

%%

"se"						          {util->refreshColumn(YYLeng()); util->printOutput("IF","se"); aux=0; 				return IF;}
"então"					              {util->refreshColumn(YYLeng()); util->printOutput("THEN","então"); aux=0;			return THEN;}
"senão"					              {util->refreshColumn(YYLeng()); util->printOutput("ELSE","senão"); aux=0;			return ELSE;}
"fim"					          	  {util->refreshColumn(YYLeng()); util->printOutput("END","fim"); aux=0;  			return END;}
"repita"				      		  {util->refreshColumn(YYLeng()); util->printOutput("REPEAT","repita"); aux=0;		return REPEAT;}
"flutuante"			  				  {util->refreshColumn(YYLeng()); util->printOutput("FLOAT","flutuante"); aux=0;	return FLOAT;}
"vazio"						  		  {util->refreshColumn(YYLeng()); util->printOutput("VOID","vazio"); aux=0;			return VOID;}
"até"					              {util->refreshColumn(YYLeng()); util->printOutput("UNTIL","até"); aux=0;			return UNTIL;}
"leia"					      		  {util->refreshColumn(YYLeng()); util->printOutput("READ","leia"); aux=0;			return READ;}
"escreve"				  			  {util->refreshColumn(YYLeng()); util->printOutput("WRITE","escreve"); aux=0;		return WRITE;}
"retorna"                 			  {util->refreshColumn(YYLeng()); util->printOutput("RETURN","retorna"); aux=0;		return RETURN;}
"inteiro"				  			  {util->refreshColumn(YYLeng()); util->printOutput("INT","inteiro"); aux=0;		return INT;}
"principal"							  {util->refreshColumn(YYLeng()); util->printOutput("MAIN","principal"); aux=0;		return MAIN;}
[E]						              {util->refreshColumn(YYLeng()); util->printOutput("EXP","E"); aux=0;				return EXP;}
[+]						              {util->refreshColumn(YYLeng()); util->printOutput("PLUS","+"); aux=0;				return PLUS;}
[-]						              {util->refreshColumn(YYLeng()); util->printOutput("MINUS","-"); aux=0;			return MINUS;}
[*]						              {util->refreshColumn(YYLeng()); util->printOutput("TIMES","*"); aux=0;			return TIMES;}
[/]						              {util->refreshColumn(YYLeng()); util->printOutput("OVER","/"); aux=0;				return OVER;}
[=]						              {util->refreshColumn(YYLeng()); util->printOutput("EQUAL","="); aux=0;			return EQUAL;}
[,]						              {util->refreshColumn(YYLeng()); util->printOutput("COMMA",","); aux=0;			return COMMA;}
[<][=]								  {util->refreshColumn(YYLeng()); util->printOutput("LT_EQUAL","<="); aux=0;		return LT_EQUAL;}
[<]						              {util->refreshColumn(YYLeng()); util->printOutput("LT","<"); aux=0;				return LT;}
[>]									  {util->refreshColumn(YYLeng()); util->printOutput("RT",">"); aux=0;				return RT;}
[>][=]								  {util->refreshColumn(YYLeng()); util->printOutput("RT_EQUAL",">="); aux=0; 		return RT_EQUAL;}
[(]						              {util->refreshColumn(YYLeng()); util->printOutput("LPARENT","("); aux=0;			return LPARENT;}
[)]						              {util->refreshColumn(YYLeng()); util->printOutput("RPARENT",")"); aux=0;			return RPARENT;}
[;]						              {util->refreshColumn(YYLeng()); util->printOutput("SEMICOLON",";"); aux=0;		return SEMICOLON;}
[:][=]						          {util->refreshColumn(YYLeng()); util->printOutput("ASSIGN",":="); aux=0;			return ASSIGN;}
[0-9]+\.[0-9]*  				      {util->refreshColumn(YYLeng()); util->printOutputCh("DOUBLE",yytext); aux=0;		return DOUBLE;}
[0-9]+						          {util->refreshColumn(YYLeng()); util->printOutputCh("INTEGER",yytext); aux=0;		return INTEGER;}
[a-zA-Zá-ñÁ-Ñ][a-zA-Zá-ñÁ-Ñ0-9]*	  {util->refreshColumn(YYLeng()); util->printOutputCh("ID",yytext); aux=0;			return ID;}
[\n]+						          {util->setLine(lineno()); util->setColumn(1); if(aux==0){ aux=1; return LINE_BREAK;}}
[ ]+						          {util->refreshColumn(YYLeng()); }
[\t]						          {util->refreshColumn(8); }
"{"			                          { char c;
			                             do{
	           	                           c = char(yyin->get());
	                                       if (c == EOF) break;
                    	                   if (c == '\n') { util->setLine(lineno()); util->setColumn(1); }
                                           else { util->refreshColumn(1); }
                                         }while (c != '}');
                 	                  }

<<EOF>>			                      { util->printOutput("FINISH","FIM DE ARQUIVO"); return FINISH; }

[.]               	                  {return ERROR;}


%%

/*int main() { 

	std::filebuf fb;
	fb.open("in.tpp",std::ios::in);
	std::istream* in_file = new std::istream(&fb);

	std::filebuf fb_out;
	fb_out.open("out.txt",std::ios::out);		
	std::ostream* out_file = new std::ostream(&fb_out);

	FlexLexer* lexer = new yyFlexLexer(in_file, out_file);

	while(lexer->yylex() != FINISH);

	return 0;

	//([\n]+[\t]*[\n]*)+			          {util->setLine(lineno()); util->setColumn(1); tokenString = NULL; return LINE_BREAK;}

}*/