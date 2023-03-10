/*
    Marcus Kicklighter
*/

#include "semanticCheck.hpp"

SemanticAnalysis::SemanticAnalysis(Node* ast, SymbolTable* st) {
    this->AST_root = ast;
    this->ST = st;
    this->ST->reset_ST();
    semantic_check(AST_root);
    this->ST->reset_ST();
    checkDuplicates(this->AST_root);
    print_errors();

}

void SemanticAnalysis::print_errors() {
    cout << "\n\n"; // for clearer res 
    for (auto const& error : errors) {
        cout << error << "\n";
    }
}

bool SemanticAnalysis::contains_error() const {
    if (errors.size() > 0) {
        return true;
    }
    return false;
}

string SemanticAnalysis::semantic_check(Node* node) {
    string error;
    if (dynamic_cast<GoalNode*>(node) != nullptr) {
        string mainclass = semantic_check(node->children[0]);
        string classdec= semantic_check(node->children[1]);

    }
    else if(dynamic_cast<MainClassDeclaration*>(node) != nullptr) {
        ST->enter_scope();
        ST->enter_scope();
        for (auto const& child : node->children) {
            semantic_check(child);
        }
        ST->exit_scope();
        ST->exit_scope();
        return "void"; // hard coded
    }
    else if(dynamic_cast<ClassDeclaration*>(node) != nullptr) {
        ClassDeclaration* type = dynamic_cast<ClassDeclaration*>(node);
        ST->enter_scope();
        for (auto const& child : node->children) {
            semantic_check(child);
        }
        ST->exit_scope();
        return type->getIden();
    }
    else if(dynamic_cast<ClassDeclarationMult*>(node) != nullptr) {
        for (auto const& child : node->children) {
            semantic_check(child);
        }
        return "";
    }
    else if(dynamic_cast<Method*>(node) != nullptr) {
        Method* meth_node = dynamic_cast<Method*>(node);
        string meth_type = meth_node->getType();
        ST->enter_scope();
        for (auto const& child : node->children) {
            string ret = semantic_check(child);
        }
        string ret_type = semantic_check(node->children.back()); 
        ST->exit_scope();
        if (meth_type == "classType") {
            meth_type = meth_node->children[0]->children[0]->value; // get type of classType
        }
        
        if (ret_type != meth_type) {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Type mismatch: Return value type (type: "+ ret_type +") and method type (type: "+ meth_type +") is not alligning.");
        }
        return meth_type;
    }
    else if(dynamic_cast<MethodDeclarations*>(node) != nullptr) {
        for (auto const& child : node->children) {
            semantic_check(child);
        }
        return "";
    }
    else if(dynamic_cast<MethodBody*>(node) != nullptr) {
        for (auto const& child : node->children) {
            semantic_check(child);
        }
        return "";
    }
    else if(dynamic_cast<Variable*>(node) != nullptr) {
        Variable* typeNode = dynamic_cast<Variable*>(node);
        if (typeNode->getType() != "classType") {
            return typeNode->getType();
        }
        return typeNode->getClassName();
    }
    else if(dynamic_cast<VariableList*>(node) != nullptr) {
        for (auto const& child : node->children) {
            semantic_check(child);
        }
        return "";
    }
    else if(dynamic_cast<Parameter*>(node) != nullptr) {
        Parameter* typeNode = dynamic_cast<Parameter*>(node);
        return typeNode->getType();
    }
    else if(dynamic_cast<ParameterList*>(node) != nullptr) {
        for (auto const& child : node->children) {
            semantic_check(child);
        }
        return "";
    }
    else if (dynamic_cast<ArrayType*>(node) != nullptr) {
        return "int[]";
    }
    else if (dynamic_cast<BoolType*>(node) != nullptr) {
        return "bool";
    }
    else if (dynamic_cast<IntType*>(node) != nullptr) {
        return "int";
    }
    else if (dynamic_cast<IdenType*>(node) != nullptr) {
        // lookup if class, otherwise error
        return "ClassType";
    }
    else if(dynamic_cast<WhileStmt*>(node) != nullptr) {
        string expr = semantic_check(node->children[0]);
        string stmt = semantic_check(node->children[1]);
        if (expr != "bool") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: While statement has invalid expression, must be type bool");
        }
        return "";
    }
    else if(dynamic_cast<PrintStmt*>(node) != nullptr) {
        string expr = semantic_check(node->children[0]);
        return "bool";
    }
    else if(dynamic_cast<AssignStmt*>(node) != nullptr) {
        string LHS = semantic_check(node->children[0]);
        string RHS = semantic_check(node->children[1]);
        if (LHS != RHS) {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: LHS and RHS must be of same type. Currently type: " + LHS + " and type: " + RHS + ". ");
        }
        return "";
    }
    else if(dynamic_cast<ArrayIndexAssign*>(node) != nullptr) {
        string arrayType =  semantic_check(node->children[0]);
        string indexType = semantic_check(node->children[1]);
        string RHS = semantic_check(node->children[2]);
        if (indexType != "int") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: index is not of type int.");
        }
        if (arrayType != "int[]") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Array type is not of type 'int[]'.");
        }
        return "bool"; // This can cause future bugs. Follows behaviour in Python.
        // return "";
    }
    else if(dynamic_cast<Statements*>(node) != nullptr) {
        for(auto const& child : node->children) {
            semantic_check(child);
        }
        return "";
    }
    else if(node->type == "IF" || node->type == "IfElse" ){
        string expr = semantic_check(node->children[0]);
        string stmt = semantic_check(node->children[1]);
        if (expr != "bool") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: If statement has invalid expression, must be type bool");
        }
        return "";
    }
    else if (node->type == "AddExpression" || node->type == "SubExpression" || node->type == "MultExpression" || node->type == "DivExpression") { 
        string LHS = semantic_check(node->children[0]);
        string RHS = semantic_check(node->children[1]);
        if (LHS != "int" || RHS != "int") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Expression '" + node->type + "' is not supported between types: " + LHS + " and " + RHS);
        }
        return "int";
    }
    else if (node->type == "GreaterThan" || node->type == "LessThan") { 
        string LHS = semantic_check(node->children[0]);
        string RHS = semantic_check(node->children[1]);
        if (LHS != "int" || RHS != "int") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Expression '" + node->type + "' is not supported between types" + LHS + "" + RHS);
        } 
        return "bool";
    }
    else if (node->type == "Assign") { 
        string LHS = semantic_check(node->children[0]);
        string RHS = semantic_check(node->children[1]);
        if (LHS != RHS) {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Assignment between type '" + LHS + "' and type '" + RHS + " 'is not supported.");
        } 
        return "";
    }
    else if (node->type == "Equals" ) { 
        string LHS = semantic_check(node->children[0]);
        string RHS = semantic_check(node->children[1]);
        if (LHS != RHS || LHS != "int" || LHS != "bool") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Operation '"+ node->type +"' between type '" + LHS + "' and type '" + RHS + " 'is not supported.");
        } 
        return "bool";
    }
    else if (node->type == "OR" || node->type == "AND") {
        string LHS = semantic_check(node->children[0]);
        string RHS = semantic_check(node->children[1]);
        if (LHS != "bool" || RHS != "bool") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Operation '"+ node->type +"' between type '" + LHS + "' and type '" + RHS + " 'is not supported. Must be booleans.");
        } 
        return "bool";
    }
    else if (dynamic_cast<Index*>(node) != nullptr) {
        string iden = semantic_check(node->children[0]);
        string valindex = semantic_check(node->children[1]);
        if (valindex != "int") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Can't take index with non-integers.");
        }
        return "int"; // We only have int arrays in mini Java?
        // return iden; 
    }
    else if (dynamic_cast<LengthOf*>(node) != nullptr) {
        string expr = semantic_check(node->children[0]);
        if (expr != "int[]" && expr != "String[]") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Operation '" + expr + "' is not of type 'int[]'.");
        }
        return "int";
    }
    else if (dynamic_cast<MethCall*>(node) != nullptr) {
        MethCall* methnode= dynamic_cast<MethCall*>(node);
        string method_type;
        string class_name;
        string method_name;
        string exprlist;
        int amArgs;
        vector<string> arg_types;

        class_name = semantic_check(node->children[0]);
        
        Identifier* method_identifer = dynamic_cast<Identifier*>(node->children[1]);
        if (method_identifer != nullptr) {
            method_name = method_identifer->getVal();
        }

        if (node->children.size() == 3) {
            expr_check(node->children[2], arg_types);
        }
        Record* reclookup = ST->lookup_symbol(class_name, 0); // classes have their name as types
        classRecord* classrec = dynamic_cast<classRecord*>(reclookup);
        if (reclookup == nullptr) { 
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Class method calls expression must be of type class.");
        }
        else {
            methodRecord* methrec = classrec->lookupMethod(method_name);
            if (methrec == nullptr) {
                errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Undefined method: '" + method_name + "' in class: '" + class_name + "'.");
                return "";
            }
            else {
                vector<variableRecord*> parameters = methrec->getParameters();
                int args_sz = arg_types.size();
                int param_sz = parameters.size();
                if (args_sz != param_sz) {
                    errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Amount of parameters and arguments are not the same in method call.");
                }
                else {
                    for (int i = 0; i < args_sz; ++i) {
                        if (arg_types[i] != parameters[i]->type) {
                            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Type mismatch of arguments and method parameters.");
                        }
                    }
                }
                
                return methrec->type;
            }
        }
        return "";    
    }
    else if (dynamic_cast<TrueVal*>(node) != nullptr) {
        return "bool";
    }
    else if (dynamic_cast<FalseVal*>(node) != nullptr) {
        return "bool";
    }
    else if (dynamic_cast<Identifier*>(node) != nullptr) { // check for usage of non-declared variables
        Identifier* iden = dynamic_cast<Identifier*>(node);
        string var = iden->getVal();
        Record* idenRec = ST->lookup_symbol(var, 3);
        if (idenRec == nullptr) {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Undefined variable " + var);
            return "";
        }
        else {
            return idenRec->type;
        }
    }
    else if (dynamic_cast<ThisOP*>(node) != nullptr) {
        Record* classrec = ST->lookup_symbol("this", 0); // refers to class
        if (classrec == nullptr) {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Undefined variable 'this'.");
            return "";
        }
        return classrec->type;
    }
    else if (dynamic_cast<IntArray*>(node) != nullptr) {
        string expr = semantic_check(node->children[0]);
        if (expr != "int") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: index is not of type int.");
        }
        return "int[]";
    }
    else if (dynamic_cast<IdenAlloc*>(node) != nullptr) {
        // new
        Identifier* iden = dynamic_cast<Identifier*>(node->children[0]);
        string expr = iden->getVal();

        Record* rec = ST->lookup_symbol(expr, 0); // get class record
        if (rec != nullptr) {
            if (dynamic_cast<classRecord*>(rec) == nullptr) {
                errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: Invalid type. Can't use new on non-class identifers.");               
            }
            return expr;
        }
        else {
            return "";
        }
    }
    else if (dynamic_cast<Negation*>(node) != nullptr) {
        string expr = semantic_check(node->children[0]);
        if (expr != "bool") {
            errors.push_back("@error at line: " + to_string(node->lineno) + ". Semantic Error: expected negation of bool. Got: " + expr + ".");
        }
        return "bool";
    }
    else if (dynamic_cast<Expression*>(node) != nullptr) {
        string expr = semantic_check(node->children[0]);
        return expr;
    }
    else if (dynamic_cast<ExpressionList*>(node) != nullptr) {
        for (auto const& child : node->children) {
            semantic_check(child);
        }
        return "";
    }
    else if (dynamic_cast<IntVal*>(node) != nullptr) {
        return "int";
    }
    return "";
}

string SemanticAnalysis::expr_check(Node* node, vector<string> &arg_types) {
    	if (dynamic_cast<Expression*>(node) != nullptr) {
            string type = semantic_check(node->children[0]);
            arg_types.push_back(type);
            return type;
		}
		else if (dynamic_cast<ExpressionList*>(node) != nullptr) { 
            string exprlist = expr_check(node->children[0], arg_types);
            string expr = semantic_check(node->children[1]);
            arg_types.push_back(expr);
            return "";
		}
}

void SemanticAnalysis::checkDuplicates(Node* node) {
    if (node != nullptr) {
        for(auto const& child : node->children) {
            if (dynamic_cast<ClassDeclaration*>(child) != nullptr) {
                ClassDeclaration* classdec = dynamic_cast<ClassDeclaration*>(child);
                string name = classdec->getIden();
                vector<Record*> rec;
                int duplicate = ST->lookup_dup(name);
                if (duplicate > 1) {
                    errors.push_back("Duplicate declaration: Class: '" + name + "' is already declared multple times."); // can add lineno, but bison very buggy
                }
                ST->enter_scope();
                checkDuplicates(child);
                ST->exit_scope();
            }
            else if (dynamic_cast<MainClassDeclaration*>(child) != nullptr) {
                MainClassDeclaration* methclassdec = dynamic_cast<MainClassDeclaration*>(child);
                string name = methclassdec->getIdenName();
                ST->enter_scope();
                ST->enter_scope();
                int duplicate = ST->lookup_dup(name);
                if (duplicate > 1) {
                    errors.push_back("Duplicate declaration: Class: '" + name + "' is already declared multiple times."); // can add lineno, but bison very buggy
                }
                // the method in methodclassdec
                name = methclassdec->getIdenPar();
                duplicate = ST->lookup_dup(name);
                if (duplicate > 1) {
                    errors.push_back("@error at line: " + to_string(node->lineno) + ". Duplicate declaration: : '" + name + "' is already declared multiple times.");
                }
                ST->exit_scope();
				ST->exit_scope();
            }
            else if (dynamic_cast<Method*>(child) != nullptr) {
                Method* meth = dynamic_cast<Method*>(child);
                string name = meth->getIden();
                int duplicate = ST->lookup_dup(name);
                if (duplicate > 1) {
                    errors.push_back("Duplicate declaration: Method: '" + name + "' is already declared multple times."); // can add lineno, but bison very buggy
                }
                ST->enter_scope();
                checkDuplicates(child);
                ST->exit_scope();
            }
            else if (dynamic_cast<Variable*>(child) != nullptr) {
                Variable* var = dynamic_cast<Variable*>(child);
                string name = var->getIden();
                int duplicate = ST->lookup_dup(name);
                if (duplicate > 1) {
                    errors.push_back("Duplicate declaration: Variable: '" + name + "' is declared multiple times."); // can add lineno, but bison very buggy
                }
            }
            else if (dynamic_cast<Parameter*>(child) != nullptr) {
				/*Potentially check for duplicate parameters..*/
				Parameter* par = dynamic_cast<Parameter*>(child);
				string name =  par->getIden();
                int duplicate = ST->lookup_dup(name);
                if (duplicate > 1) {
                    errors.push_back("Duplicate declaration: Parameter: '" + name + "' is declared multiple times.");
                }
				checkDuplicates(child);
			}
			else if (dynamic_cast<ParameterList*>(child) != nullptr) {
				/*Potentially check for duplicate parameters..*/
				// Parameter* par = dynamic_cast<Parameter*>(child);
				// string name =  par->getIden();
				checkDuplicates(child);
			}
			else if(dynamic_cast<ClassDeclarationMult*>(child) != nullptr) {
				checkDuplicates(child);
			}
			else if(dynamic_cast<MethodDeclarations*>(child) != nullptr) {
				// can contain methods
				checkDuplicates(child); 
			}
			else if(dynamic_cast<MethodBody*>(child) != nullptr) {
				// can contain variables
				checkDuplicates(child);
			}
			else if(dynamic_cast<VariableList*>(child) != nullptr) {
				// can contain variables
				checkDuplicates(child);
			}
        }
    }

}