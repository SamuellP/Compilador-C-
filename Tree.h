#ifndef Included_Tree_H
#define Included_Tree_H

#include <string>
#include <vector>

using namespace std;

class Tree
{
	public:

		Tree* irmao;
		vector<Tree*> sons;
		string nameNode;
		string value;
		int line;

		Tree(string nameNode, const char* value);
		//~Tree();
		Tree* getBrother();
		void setBrother(Tree* irmao);
		string getNameNode();
		void setNameNode(string nameNode);
		string getValue();
		void setValue(string value);
		int getLine();
		void setLine(int line);
		int getAmountOfChildren();
	
};

#endif