#include "TokenType.h"

using namespace std;


class Token
{
	public:

		TokenType tokenType;
		string valor;

		Token(TokenType tt, string val);
		//~Token();
		TokenType getTokenType();
		void setTokenType(TokenType tt);
		string getValor();
		void setValor(string val);
	
};