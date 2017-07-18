%name Parser
%define LSP_NEEDED
%define MEMBERS                 \
	Parser(std::istream* fileInput, std::ostream* fileOutput) { this->lexer = new yyFlexLexer(fileInput, fileOutput); } \
   	virtual ~Parser()   {} \
   	Tree* tree;	\
   	FlexLexer* lexer;


%define LEX_BODY { return lexer->yylex(); }
%define ERROR_BODY { cout << "Syntax error: linha "<< lexer->lineno() << ", coluna " << util->getColumnPrev() << " - last word parsed: " << lexer->YYText() << endl; tree = NULL; exit(1); }

%header{

#include <iostream>
#include <fstream>
#include <FlexLexer.h>
#include <cstdio>
#include <cstdlib>

#include "Util.h"

extern Util* util; // Sendo utilizado na análise léxica

using namespace std;

%}

%union {
       Tree* node;
}

%token MAIN
%token DOUBLE INTEGER ID IF THEN ELSE END REPEAT FLOAT VOID UNTIL READ WRITE RETURN INT EXP PLUS MINUS TIMES OVER EQUAL 
%token COMMA LT LT_EQUAL RT RT_EQUAL LPARENT RPARENT SEMICOLON ASSIGN LINE_BREAK ERROR FINISH

%type <node> inicio stmt_seq stmt_global stmt atribuicao id expressao expressao_simples termo fator declaracao declaracao_global
%type <node> lista_declaracao lista_declaracao_global condicao op_condicional repeticao declaracao_funcao retorno lista_parametros_dec_func
%type <node> declaracao_funcao_principal chamada_funcao lista_parametros_cham_func tipo lb valor lei_esc lista_lei_esc

%start inicio

%%

inicio 					   : stmt_seq		   			   							   { tree = $1; return 1;}
       					   ;

stmt_seq    			   : stmt_global stmt_seq 									   { 
																		   		         Tree* t = $2;
																                         if (t != NULL){
																                         	while (t->irmao != NULL)
																                         		t = t->irmao;
																                         	t->irmao = $1;
																                         	$$ = $2; 
																                     	 }else {
																                     	 	$$ = $1;
																                     	 }
																                 	   }
            			   | stmt_global											   { $$ = $1; }
            			   | declaracao_funcao_principal lb FINISH					   { $$ = $1; }
            			   ;


stmt_global 			   : declaracao_global lb 									   { $$ = $1; }
					       | declaracao_funcao lb									   { $$ = $1; }
					       | error 													   { $$ = NULL; }
					       ;


stmt 					   : stmt declaracao lb 								   	   { $2->irmao = $1; $$ = $2; }
					       | stmt chamada_funcao lb 								   { $2->irmao = $1; $$ = $2; }
					       | stmt atribuicao lb 									   { $2->irmao = $1; $$ = $2; }
					       | stmt lei_esc lb										   { $2->irmao = $1; $$ = $2; }
					       | stmt condicao lb										   { $2->irmao = $1; $$ = $2; }
					       | stmt repeticao lb 										   { $2->irmao = $1; $$ = $2; }
					       | stmt retorno 											   { $2->irmao = $1; $$ = $2; }
					       | declaracao lb 											   { $$ = $1; }
					       | chamada_funcao lb 										   { $$ = $1; }
					       | atribuicao lb 											   { $$ = $1; }
					       | lei_esc lb												   { $$ = $1; }
					       | condicao lb											   { $$ = $1; }
					       | repeticao lb 											   { $$ = $1; }
					       | retorno												   { $$ = $1; }
					       | error 													   { $$ = NULL; }
					       ;



atribuicao 				   : id ASSIGN expressao_simples 							   { $$ =  new Tree("ASSIGN","NULL");
																				 	     $$->sons.push_back($1);
																				 	     $$->sons.push_back($3);
																				 	     $$->setLine(lexer->lineno());
																				 	     $$->setBrother(NULL);
															      			 	  	   }
	     				   ;


id 						   : ID 													   { $$ = new Tree("ID",lexer->YYText());
																						 $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																			           }
        				   ;


expressao 				   : expressao_simples op_condicional expressao_simples 	   { $$ = new Tree("CONDICAO","NULL");
																					     $$->sons.push_back($1);
																					     $$->sons.push_back($2);
																					     $$->sons.push_back($3);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }
		 				   | expressao_simples 										   { $$ = $1;
		 					 														   }
		 				   ;


expressao_simples 		   : expressao_simples PLUS termo 							   { $$ = new Tree("+","NULL");
																					     $$->sons.push_back($1);
																					     $$->sons.push_back($3);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }
				 		   | expressao_simples MINUS termo 							   { $$ = new Tree("-","NULL");
																					     $$->sons.push_back($1);
																					     $$->sons.push_back($3);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }
				 		   | termo 													   { $$ = $1;
				 		 		 													   }
				 		   ;


termo 					   : termo TIMES fator 										   { $$ = new Tree("*","NULL");
																					     $$->sons.push_back($1);
																					     $$->sons.push_back($3);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }
	 					   | termo OVER fator  										   { $$ = new Tree("/","NULL");
																					     $$->sons.push_back($1);
																					     $$->sons.push_back($3);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
						 															   }
					   	   | fator 													   { $$ = $1;
					 		 														   }
					 	   ;


fator 					   : LPARENT expressao_simples RPARENT 						   { $$ = $2;
										 											   }
	 					   | valor 													   { $$ = $1;
	 		 																		   }
	 					   | id 	 												   { $$ = $1;
	 		 																		   }
	 		 			   | chamada_funcao											   { $$ = $1;
	 		 			   															   }
	 					   ;


declaracao 				   : tipo lista_declaracao 									   { $$ = new Tree("DECLARACAO","NULL");
																	     				 $$->sons.push_back($1);
																	     				 $$->sons.push_back($2);
																	     				 $$->setLine(lexer->lineno());
																	     				 $$->setBrother(NULL);
																	   				   }
	  	   				   ;


lista_declaracao : 		   atribuicao COMMA lista_declaracao 						   { $$ = new Tree("LISTA_DECLARACAO","NULL");
														       						     $$->sons.push_back($1);
														       						     $$->sons.push_back($3);
														       						     $$->setLine(lexer->lineno());
														       						     $$->setBrother(NULL);
														     						   }
		 				   | atribuicao 											   { $$ = $1;
		 			  																   }
		 				   | id COMMA lista_declaracao 								   { $$ = new Tree("LISTA_DECLARACAO","NULL");
														 							     $$->sons.push_back($1);
														 							     $$->sons.push_back($3); 
														 							     $$->setLine(lexer->lineno());
														 							     $$->setBrother(NULL);
														 							   }
		 				   | id 													   { $$ = $1; 
		 	  																		   }
		 				   ;


declaracao_global		   : tipo lista_declaracao_global							   { $$ = new Tree("DECLARACAO","NULL");
																	     				 $$->sons.push_back($1);
																	     				 $$->sons.push_back($2);
																	     				 $$->setLine(lexer->lineno());
																	     				 $$->setBrother(NULL);
																	   				   }
	  	   				   ;


lista_declaracao_global    : id COMMA lista_declaracao_global						   { $$ = new Tree("LISTA_DECLARACAO","NULL");
														 							     $$->sons.push_back($1);
														 							     $$->sons.push_back($3); 
														 							     $$->setLine(lexer->lineno());
														 							     $$->setBrother(NULL);
														 							   }
		 				   | id 													   { $$ = $1; 
		 	  																		   }
		 				   ;



condicao 				   : IF expressao THEN lb stmt END							   { $$ = new Tree("IF","NULL");
																					     $$->sons.push_back($2);
																					     $$->sons.push_back($5);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }

	 					   | IF expressao THEN lb stmt ELSE lb stmt END				   { $$ = new Tree("IF","NULL");
																			    		 $$->sons.push_back($2);
																			    		 $$->sons.push_back($5);
																			    		 $$->sons.push_back($8);
																			    		 $$->setLine(lexer->lineno());
																			    		 $$->setBrother(NULL);
										 											   }
	 					   ;


op_condicional 			   : EQUAL  												   { $$ = new Tree("=","NULL"); 
																						 $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }
	       				   | LT 													   { $$ = new Tree("<","NULL");
	       				   																 $$->setLine(lexer->lineno());
																	       			     $$->setBrother(NULL);
																	   				   }
						   | LT_EQUAL												   { $$ = new Tree("<=","NULL");
						   																 $$->setLine(lexer->lineno());
																	       			     $$->setBrother(NULL);
																	   				   }
                           | RT 													   { $$ = new Tree(">","NULL");
                           																 $$->setLine(lexer->lineno());
																	       			     $$->setBrother(NULL);
																	   				   }
                           | RT_EQUAL												   { $$ = new Tree(">=","NULL");
                           																 $$->setLine(lexer->lineno());
																	       			     $$->setBrother(NULL);
																	   				   }
	       				   ;


repeticao 				   : REPEAT lb stmt UNTIL expressao							   { $$ = new Tree("REPETICAO","NULL");
											  											 $$->sons.push_back($3);
																					     $$->sons.push_back($5);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }
	  					   ;


declaracao_funcao 		   : tipo id LPARENT lista_parametros_dec_func RPARENT lb stmt END { $$ = new Tree("DECL_FUNCAO","NULL");
																						 $$->sons.push_back($1);
																						 $$->sons.push_back($2);
																						 $$->sons.push_back($4); 
																						 $$->sons.push_back($7);
																						 $$->setLine(lexer->lineno());
																						 $$->setBrother(NULL);
												 								  	   }
				 		   | tipo id LPARENT lista_parametros_dec_func RPARENT lb END  { $$ = new Tree("DECL_FUNCAO","NULL");
																						 $$->sons.push_back($1);
																						 $$->sons.push_back($2);
																						 $$->sons.push_back($4);
																						 $$->setLine(lexer->lineno());
																						 $$->setBrother(NULL);
																		 			   }
				 		   | tipo id LPARENT RPARENT lb stmt END					   { $$ = new Tree("DECL_FUNCAO","NULL");
																						 $$->sons.push_back($1);
																						 $$->sons.push_back($2);
																						 $$->sons.push_back($6);
																						 $$->setLine(lexer->lineno());
																						 $$->setBrother(NULL);
														 							   }
				 		   | tipo id LPARENT RPARENT lb END							   { $$ = new Tree("DECL_FUNCAO","NULL");
																						 $$->sons.push_back($1);
																						 $$->sons.push_back($2);
																						 $$->setLine(lexer->lineno());
																						 $$->setBrother(NULL);
														 							   } 
	    	       		   ;


declaracao_funcao_principal: tipo MAIN LPARENT RPARENT lb stmt END 					   { $$ = new Tree("DECL_FUNCAO_PRINCIPAL","NULL");
																						 $$->sons.push_back($1);
																						 $$->sons.push_back($6);
																						 $$->setLine(lexer->lineno());
																						 $$->setBrother(NULL);
												 								  	   }
	    	       		   ;


retorno 				   : RETURN expressao_simples lb 							   { $$ = new Tree("RETORNA","NULL");
																					     $$->sons.push_back($2);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }
						   ;


lista_parametros_dec_func  : tipo id COMMA lista_parametros_dec_func 				   { $$ = new Tree("PARAMETROS_DEC_FUNC","NULL");
																					     $$->sons.push_back($1);
																					     $$->sons.push_back($2);
																					     $$->sons.push_back($4);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }
		 				   | tipo id 												   { $$ = new Tree("PARAMETROS_DEC_FUNC","NULL");
																					     $$->sons.push_back($1);
																					     $$->sons.push_back($2);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }
		 				   ;


chamada_funcao 			   : id LPARENT lista_parametros_cham_func RPARENT 			   { $$ = new Tree("CHAMADA_FUNC","NULL");
																					     $$->sons.push_back($1);
																					     $$->sons.push_back($3);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }
			   			   | id LPARENT RPARENT 									   { $$ = new Tree("CHAMADA_FUNC","NULL");
															     					     $$->sons.push_back($1);
															     					     $$->setLine(lexer->lineno());
															     					     $$->setBrother(NULL);
																					   }
		     			   ;


lista_parametros_cham_func : expressao_simples COMMA lista_parametros_cham_func  	   { $$ = new Tree("PARAMETROS_CHAM_FUNC","NULL");
																					     $$->sons.push_back($1);
																					     $$->sons.push_back($3);
																					     $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }

			   			   | expressao_simples  									   { $$ = $1;
			   			   		  													   }					   
			   			   ;


tipo 					   : INT 													   { $$ = new Tree("INT","NULL"); 
																						 $$->setLine(lexer->lineno());
																					     $$->setBrother(NULL);
																					   }
     					   | FLOAT 													   { $$ = new Tree("FLOAT","NULL");
     					   																 $$->setLine(lexer->lineno());
													     					   			 $$->setBrother(NULL);
													     							   }
                           | VOID 													   { $$ = new Tree("VAZIO","NULL");
                           																 $$->setLine(lexer->lineno());
													     					   			 $$->setBrother(NULL);
													     							   }
     					   ;

lb 						   : LINE_BREAK												   {  }
   						   ;

valor 					   : INTEGER 												   { $$ = new Tree("VAL_INT", lexer->YYText()); 
																						 $$->setLine(lexer->lineno());
															      					     $$->setBrother(NULL);
															    					   }
      					   | DOUBLE 												   { $$ = new Tree("VAL_DOUBLE", lexer->YYText());
      					   																 $$->setLine(lexer->lineno());
												      			 					     $$->setBrother(NULL);
												  			   						   }
      					   ;

lei_esc 				   : READ lista_lei_esc 									   { $$ = new Tree("READ","NULL");
																						 $$->sons.push_back($2);
																						 $$->setLine(lexer->lineno());
																						 $$->setBrother(NULL);
																					   }
					       | WRITE lista_lei_esc    								   { $$ = new Tree("WRITE","NULL"); 
													        						     $$->sons.push_back($2);
													        						     $$->setLine(lexer->lineno());
													        						     $$->setBrother(NULL);
													    							   }
	;

lista_lei_esc 			   : id COMMA lista_lei_esc 								   { $$ = new Tree("LISTA_LEIT_ESCRI","NULL");
																	    			     $$->sons.push_back($1);
																	    			     $$->sons.push_back($3);
																	    			     $$->setLine(lexer->lineno());
																	    			     $$->setBrother(NULL);
																	  				   }
	     				   | id 													   { $$ = $1;
	     	  					 													   }
	     				   ;


%%