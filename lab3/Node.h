
#ifndef NODE_H
#define	NODE_H

#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <map>
#include "BB.hpp"
using namespace std;

class Node {
public:
	int id, lineno;
	string type, value;
	vector<Node*> children;
	Node(string t, string v, int l) : type(t), value(v), lineno(l){}
	Node()
	{
		type = "uninitialised";
		value = "uninitialised"; 
	}   // Bison needs this.
	virtual ~Node() = default;

	virtual void print_tree(int depth=0) {
		for(int i=0; i<depth; i++){
			cout << "  ";
		}
		cout << type << ":" << value << endl; //<< " @line: "<< lineno << endl;
		for(auto i=children.begin(); i!=children.end(); i++) {
			(*i)->print_tree(depth+1);
		}
	}
  
	virtual void generate_tree() {
		std::ofstream outStream;
		char* filename = "tree.dot";
	  	outStream.open(filename);

		int count = 0;
		outStream << "digraph {" << std::endl;
		generate_tree_content(count, &outStream);
		outStream << "}" << std::endl;
		outStream.close();

		printf("\nBuilt a parse-tree at %s. Use 'make tree' to generate the pdf version.\n", filename);
  	}

  	virtual void generate_tree_content(int &count, ofstream *outStream) {
	  id = count++;
	  *outStream << "n" << id << " [label=\"" << type << ":" << value << "\"];" << endl;

	  for (auto i = children.begin(); i != children.end(); i++)
	  {
		  (*i)->generate_tree_content(count, outStream);
		  *outStream << "n" << id << " -> n" << (*i)->id << endl;
	  }
  	}
		
	virtual string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) {
		/*
			currentBlock : give a pointer to which block is currently being worked on
			methods : vector that contains all the methods in the program
			BBnames : map to later use to add all temporary variables in the ST
			id : used to give unique tempNmes
		*/
		
		for(auto const& child : children) {
			child->genIR(currentBlock, methods, BBnames, id);
		}
		return value;
	} 
	
};

/*
	Expressions
*/
class IntVal : public Node {
public:
	IntVal(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~IntVal() = default;
	string getVal() {
		return value;
	}
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		return getVal();
	}
};

class Identifier : public Node {
public:
	Identifier(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~Identifier() = default;
	string getVal() {
		return value;
	}
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		return getVal();
	}
};

class PlusOP : public Node {
public:
	PlusOP(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~PlusOP() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id);  // generate a unique name
		BBnames.insert(pair<string, string>(name, "int")); // for the 
		string lhs_name = children[0]->genIR(currentBlock, methods, BBnames, id); // children 0 visited, currentBlock is passed on, expressions do not create new blocks
		string rhs_name = children[1]->genIR(currentBlock, methods, BBnames, id); // // children 2 visited, currentBlock is passed
		ExprTac* in = new ExprTac("+", lhs_name, rhs_name, name); // create the new tac that will be added to the CFG, contains a temporary variable
		(*currentBlock)->add_Tac(in); // add to tac_instructions of the block
		return name;
	}
};

class MinusOP : public Node {
public:
	MinusOP(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~MinusOP() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id); 
		BBnames.insert(pair<string, string>(name, "int"));
		string lhs_name = children[0]->genIR(currentBlock, methods, BBnames, id); // still in same block, only statements create news
		string rhs_name = children[1]->genIR(currentBlock, methods, BBnames, id);
		ExprTac* in = new ExprTac("-", lhs_name, rhs_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

class MultOP : public Node {
public:
	MultOP(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~MultOP() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id); 
		BBnames.insert(pair<string, string>(name, "int"));
		string lhs_name = children[0]->genIR(currentBlock, methods, BBnames, id); // still in same block, only statements create news
		string rhs_name = children[1]->genIR(currentBlock, methods, BBnames, id);
		ExprTac* in = new ExprTac("*", lhs_name, rhs_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

class DivOP : public Node {
public:
	DivOP(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~DivOP() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id); 
		BBnames.insert(pair<string, string>(name, "int"));
		string lhs_name = children[0]->genIR(currentBlock, methods, BBnames, id); // still in same block, only statements create news
		string rhs_name = children[1]->genIR(currentBlock, methods, BBnames, id);
		ExprTac* in = new ExprTac("/", lhs_name, rhs_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

class AssignExpr : public Node {
public:
	AssignExpr(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~AssignExpr() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = children[0]->genIR(currentBlock, methods, BBnames, id);
		string lhs_name = children[1]->genIR(currentBlock, methods, BBnames, id); // still in same block, only statements create news
		CopyTac* in = new CopyTac(lhs_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

class GreaterThan : public Node {
public:
	GreaterThan(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~GreaterThan() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id); 
		BBnames.insert(pair<string, string>(name, "bool"));
		string lhs_name = children[0]->genIR(currentBlock, methods, BBnames, id); // still in same block, only statements create news
		string rhs_name = children[1]->genIR(currentBlock, methods, BBnames, id);
		ExprTac* in = new ExprTac(">", lhs_name, rhs_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}

};

class LessThan : public Node {
public:
	LessThan(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~LessThan() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id);
		BBnames.insert(pair<string, string>(name, "bool"));
		string lhs_name = children[0]->genIR(currentBlock, methods, BBnames, id); // still in same block, only statements create news
		string rhs_name = children[1]->genIR(currentBlock, methods, BBnames, id);
		ExprTac* in = new ExprTac("<", lhs_name, rhs_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

class Equals : public Node {
public:
	Equals(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~Equals() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id); 
		BBnames.insert(pair<string, string>(name, "int"));
		string lhs_name = children[0]->genIR(currentBlock, methods, BBnames, id); // still in same block, only statements create news
		string rhs_name = children[1]->genIR(currentBlock, methods, BBnames, id);
		ExprTac* in = new ExprTac("==", lhs_name, rhs_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

class Or : public Node {
public:
	Or(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~Or() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id); 
		BBnames.insert(pair<string, string>(name, "bool"));
		string lhs_name = children[0]->genIR(currentBlock, methods, BBnames, id); // still in same block, only statements create news
		string rhs_name = children[1]->genIR(currentBlock, methods, BBnames, id);
		ExprTac* in = new ExprTac("||", lhs_name, rhs_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

class And : public Node {
public:
	And(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~And() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id); 
		BBnames.insert(pair<string, string>(name, "int"));
		string lhs_name = children[0]->genIR(currentBlock, methods, BBnames, id); // still in same block, only statements create news
		string rhs_name = children[1]->genIR(currentBlock, methods, BBnames, id);
		ExprTac* in = new ExprTac("&&", lhs_name, rhs_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

class LengthOf : public Node {
public:
	LengthOf(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~LengthOf() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id); 
		BBnames.insert(pair<string, string>(name, "int"));
		string y_name = children[0]->genIR(currentBlock, methods, BBnames, id); // what to take length of
		LengthTac* in = new LengthTac(y_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

class Expression : public Node {
public:
	Expression(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~Expression() = default;
	string getIden() {
		return children[0]->value;
	}
};

class ExpressionList : public Node {
public:
	ExpressionList(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~ExpressionList() = default;

	string getIden() {
		return children[1]->value;
	}
};

class MethCall : public Node {
public:
	MethCall(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~MethCall() = default;
	
	string getIden() {
		return children[1]->value;
	}
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id); // BB->getRandomName
		BBnames.insert(pair<string, string>(name, ""));
		string expr = children[0]->genIR(currentBlock, methods, BBnames, id); // still in same block, only statements create news
		string func = children[1]->genIR(currentBlock, methods, BBnames, id);
		int param_num = 0; 
		if (children.size() > 2) {
			for (auto const& child : children) {
				++param_num; // one more parameter
				child->genIR(currentBlock, methods, BBnames, id);
			}
		}
		MethCallTac* in = new MethCallTac(func, to_string(param_num), name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

class TrueVal : public Node {
public:
	TrueVal(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~TrueVal() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		return "true";
	}
};

class FalseVal : public Node {
public:
	FalseVal(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~FalseVal() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		return "false";
	}
};

class ThisOP : public Node {
public:
	ThisOP(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~ThisOP() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		return "this";
	}
};

class IntArray : public Node {
public:
	IntArray(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~IntArray() = default;
};

class IdenAlloc : public Node {
public:
	IdenAlloc(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~IdenAlloc() = default;

};

class Negation : public Node {
public:
	Negation(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~Negation() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = (*currentBlock)->generate_name(++id); 
		BBnames.insert(pair<string, string>(name, "bool"));
		string rhs_name = children[0]->genIR(currentBlock, methods, BBnames, id);
		UnaryTac* in = new UnaryTac("!", rhs_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

/*
	Statements
*/

class NonStmt : public Node {
public:
	NonStmt(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~NonStmt() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {	
		return "";
	}
};

class Statements : public Node {
public:
	Statements(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~Statements() = default;
};

class IfStmt : public Node {
public:
	IfStmt(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~IfStmt() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {	
		
		int idB = (*currentBlock)->id + 30;  // !!!! 
		BB* tBlock = new BB(idB);
		idB += 1;
		BB* jBlock = new BB(idB);
		
		/* set the the block after the if-else branching */
		tBlock->setTrue(jBlock);
		
		string conName = children[0]->genIR(currentBlock, methods, BBnames, id); // boolean condition

		/*If case too see if nested*/		
		if ((*currentBlock)->getTrue() != nullptr) {
			BB* jumpPath = (*currentBlock)->getTrue(); // hBlock in whileStmt
			jBlock->setTrue(jumpPath);
		}
		// set true and false path to be directly to jump block
		(*currentBlock)->setTrue(tBlock);
		(*currentBlock)->setFalse(jBlock);  

		/*Fill tBlock by calling genIR on various children. Called by reference*/
		string tName = children[1]->genIR(&tBlock, methods, BBnames, id);
		
		// continue to write to the block after the if branching
		*currentBlock = jBlock;

		return "";
	}
	
};

class IfElseStmt : public Node {
public:
	IfElseStmt(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~IfElseStmt() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {

		int idB = (*currentBlock)->id + 20; // this is "silvertejp" --> so no ID clash in nested if while cases
		BB* tBlock = new BB(idB);
		idB += 1;
		BB* fBlock = new BB(idB);
		idB += 1;
		BB* jBlock = new BB(idB);

		/* set the the block after the if-else branching */
		tBlock->setTrue(jBlock);  
		fBlock->setTrue(jBlock); 

		string conName = children[0]->genIR(currentBlock, methods, BBnames, id); // boolean condition
		
		/* If-case to see if nested*/
		if ((*currentBlock)->getTrue() != nullptr) {
			BB* jumpPath = (*currentBlock)->getTrue(); // hBlock in whileStmt
			jBlock->setTrue(jumpPath);
		}

		/* Assign the true and false path for the current block */
		(*currentBlock)->setTrue(tBlock); 
		(*currentBlock)->setFalse(fBlock);

		/*Fill fBlock and tBlock by calling genIR on various children. Called by reference*/
		string tName = children[1]->genIR(&tBlock, methods, BBnames, id); // generate true block
		string fName = children[2]->genIR(&fBlock, methods, BBnames, id); // generate false block
		
		// continue to write to the block after the while branch
		*currentBlock = jBlock;

		return "";
	}
};

class WhileStmt : public Node {
public:
	WhileStmt(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~WhileStmt() = default;

	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		
		int idB = (*currentBlock)->id + 50;  // To avoid ID clashes
		BB* hBlock = new BB(idB); // header block
		idB += 1;
		BB* bBlock = new BB(idB); // body block
		idB += 1;
		BB* jBlock = new BB(idB); // jump block	

		hBlock->setTrue(bBlock); // if true, re-enter the while-loop
		hBlock->setFalse(jBlock); // if false, exit while loop, by going to next block
		bBlock->setTrue(hBlock); // in the true scenario, loop the while loop

		/*If case too see if nested*/		
		if ((*currentBlock)->getTrue() != nullptr) {
			BB* jumpPath = (*currentBlock)->getTrue(); // hBlock in whileStmt
			jBlock->setTrue(jumpPath);
		}
		(*currentBlock)->setTrue(hBlock); // set currentBlock to point to hblock

		/*Fill hBlock and bBlock by calling genIR on various children. Called by reference*/
		string hName = children[0]->genIR(&hBlock, methods, BBnames, id); // while condition
		string bName = children[1]->genIR(&bBlock, methods, BBnames, id); // the body

		// continue to write to the block after the while branch
		*currentBlock = jBlock;
		return "";
	}
};

class PrintStmt : public Node {
public:
	PrintStmt(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~PrintStmt() = default;
	
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string expr = children[0]->genIR(currentBlock, methods, BBnames, id);
		PrintTac* in = new PrintTac(expr);
		(*currentBlock)->add_Tac(in);
		return "";
	}

};

class AssignStmt : public Node {
public:
	AssignStmt(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~AssignStmt() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name = children[0]->genIR(currentBlock, methods, BBnames, id);
		string lhs_name = children[1]->genIR(currentBlock, methods, BBnames, id); // still in same block, only statements create news
		CopyTac* in = new CopyTac(lhs_name, name);
		(*currentBlock)->add_Tac(in);
		return name;
	}
};

class ArrayIndexAssign : public Node {
public:
	ArrayIndexAssign(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~ArrayIndexAssign() = default;
};

class Index : public Node {
  public:
	Index(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~Index() = default;
};

/*
	Types
*/

class ArrayType : public Node {
public:
	ArrayType(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~ArrayType() = default;
};

class BoolType : public Node {
public:
	BoolType(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~BoolType() = default;
};

class IntType : public Node {
public:
	IntType(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~IntType() = default;
};

class IdenType : public Node {
public:
	IdenType(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~IdenType() = default;
};


/*
	Method Body
*/

class Variable : public Node {
public:
	Variable(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~Variable() = default;

	string getType() {
		return children[0]->type;
	}
	string getIden() {
		Identifier* identifier = dynamic_cast<Identifier*>(children[1]);
		return identifier->getVal();
	}
	string getClassName() {
		return children[0]->children[0]->value;
	}
};

class VariableList : public Node {
public:
	VariableList(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~VariableList() = default;
};

class MethodBody : public Node {
public:
	MethodBody(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~MethodBody() = default;
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name;
		for (auto const& child: children) {
			name = child->genIR(currentBlock, methods, BBnames, id);
		}
		return name;
	}
};

/*
	Method Declarations

	May want to divide into multple classes of method declarations later
*/

class Parameter : public Node {
public:
	Parameter(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~Parameter() = default;
	
	string getType() {
		string ret = children[0]->type;
		if (ret == "classType") {
			ret = children[0]->children[0]->value; // get class type that classType corralates to
		}
		return ret;
	}
	string getIden() {
		Identifier* identifier = dynamic_cast<Identifier*>(children[1]);
		return identifier->getVal();
	}

	// string genIR(BB* &currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
	// 	string paramType = children[0]->genIR(currentBlock, methods, BBnames, id);
	// 	string paramName = children[1]->genIR(currentBlock, methods, BBnames, id);
	// 	// ParTac* parTac = new ParTac(paramName);
	// }
	
};

class ParameterList : public Node {
public:
	ParameterList(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~ParameterList() = default;
	
	string getType() {
		string ret = children[0]->type;
		if (ret == "classType") {
			ret = children[0]->children[0]->value; // get class type that classType corralates to
		}
		return ret;
	}
	string getIden() {
		Identifier* identifier = dynamic_cast<Identifier*>(children[1]);
		return identifier->getVal();
	}

};

class Method : public Node {
public:
	Method(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~Method() = default;
	string getType() {
		return children[0]->type;
	}
	string getIden() {
		Identifier* identifier = dynamic_cast<Identifier*>(children[1]);
		return identifier->getVal();
	}
	void getParameterList(vector<std::string> &vec) { // take empty vec as argument
		getParam(children[2], vec);
	}
	void getParam(Node* node, vector<string> &params) { // can be void because vector is passed as reference
		if (dynamic_cast<Parameter*>(node) != nullptr) {
			Parameter* par = dynamic_cast<Parameter*>(node);
			params.push_back(par->getIden());
			params.push_back(par->getType());
		}
		else if (dynamic_cast<ParameterList*>(node) != nullptr) {
			ParameterList* par = dynamic_cast<ParameterList*>(node);
			params.push_back(par->getIden());
			params.push_back(par->getType());
			getParam(node->children[2], params);
		}
	}
	
	string genIR(BB** currentBlock, vector<BB*> &methods, std::map<string, string> &BBnames, int &id) override {
		string name;
		int idB = (*currentBlock)->id + 1;
		BB* newBlock = new BB(idB);
		methods.push_back(newBlock);
		*currentBlock = newBlock;
		for (auto const& child : children) {
			name = child->genIR(currentBlock, methods, BBnames, id);
		}
		
		// Add the return value to current block
		ReturnTac* in = new ReturnTac(name); // the last iterated will be return name --> 
		(*currentBlock)->add_Tac(in);

		return "";
	}
	
};

class MethodDeclarations : public Node {
public:
	MethodDeclarations(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~MethodDeclarations() = default;
};

/*
	Class Declaration
*/

class ClassDeclaration : public Node {

public:
	ClassDeclaration(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~ClassDeclaration() = default;
	string getIden() {
		Identifier* identifier = dynamic_cast<Identifier*>(children[0]);
		return identifier->getVal();
	}
};

class ClassDeclarationMult : public Node {
public:
	ClassDeclarationMult(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~ClassDeclarationMult() = default;
};

/*
	Main Class Declaration
*/

class MainClassDeclaration : public Node {
public:
	MainClassDeclaration(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~MainClassDeclaration() = default;
	string getIdenName() {
		Identifier* identifier = dynamic_cast<Identifier*>(children[0]);
		return identifier->getVal();
	}
	string getIdenPar() {
		Identifier* identifier = dynamic_cast<Identifier*>(children[1]);
		return identifier->getVal();
	}
};

class GoalNode : public Node {
public:
	GoalNode(string t, string v, int l) { type = t; value = v; lineno = l;}
	virtual ~GoalNode() = default;
};



#endif