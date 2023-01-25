%skeleton "lalr1.cc" 
%defines
%define parse.error verbose
%define api.value.type variant
%define api.token.constructor

%code requires{
  #include <string>
  #include "Node.h"
}
%code{
  #define YY_DECL yy::parser::symbol_type yylex()

  YY_DECL;
  
  Node* root;
  extern int yylineno;
  
}
// definition of set of tokens. All tokens are of type string
%token <std::string> PLUSOP MINUSOP MULTOP DIVOP INT LP RP LHB RHB LCB RCB ASSIGN EQ AND OR GT LT
%token <std::string> CLASS STATIC VOID MAIN PUBLIC COMMENT
%token <std::string> PERIOD COMMA EXCLAMATION SEMICOLON INTTYPE BOOLTYPE STRING
%token <std::string> IF ELSE WHILE NEW LENGTH PRINT IDENTIFIER TRUE FALSE THIS
%token END 0 "end of file"

//defition of operator precedence. See https://www.gnu.org/software/bison/manual/bison.html#Precedence-Decl
%left ASSIGN
%left AND OR 
%left EQ  
%left LT GT
%left PLUSOP MINUSOP
%left MULTOP DIVOP

// definition of the production rules. All production rules are of type Node
%type <Node *> root expression factor identifier

%%
root:       expression {root = $1;};

expression: expression PLUSOP expression {      /*
                                                  Create a subtree that corresponds to the AddExpression
                                                  The root of the subtree is AddExpression
                                                  The childs of the AddExpression subtree are the left hand side (expression accessed through $1) and right hand side of the expression (expression accessed through $3)
                                                */
                            $$ = new Node("AddExpression", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);
                            /* printf("r1 "); */
                          }
            | expression MINUSOP expression {
                            $$ = new Node("SubExpression", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);
                            /* printf("r2 "); */
                          }
            | expression MULTOP expression {
                            $$ = new Node("MultExpression", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);
                            /* printf("r3 "); */
                          }
            | expression DIVOP expression {
                            $$ = new Node("DivExpression", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);
                            /* printf("r3 "); */
                          }
            | expression ASSIGN expression {
                            $$ = new Node("Assigning", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);                            
                          }
            | expression GT expression {
                            $$ = new Node("GreaterThan", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);                            
                          }
            | expression LT expression {
                            $$ = new Node("LessThan", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);                            
                          }
            | expression EQ expression {
                            $$ = new Node("Equals", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);                            
                          }
            | expression OR expression {
                            $$ = new Node("OR", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);                            
                          }
            | expression AND expression {
                            $$ = new Node("AND", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);                            
                          }
            
            | expression LHB expression RHB {
                            // Do something;
                          } 
            | expression PERIOD LENGTH {
                            // Do something
                          }
            | expression PERIOD identifier LP expression COMMA expression RP {
                              // Do something
                          }
            
            | TRUE {
                  // Do something
                }
            | FALSE {
                  // Do something 
                }
            | identifier {
                  // Do something
                }
            | THIS  {
                  // Do something
                }
            | NEW INTTYPE LHB expression RHB {
                      // Do something
                  }
            | NEW identifier LP RP {
                      // DO something
                  }
            | EXCLAMATION expression {
                      // Do something
                  }
            | factor      {$$ = $1; /* printf("r4 ");*/}
            ;


factor:     INT           {  $$ = new Node("Int", $1, yylineno); /* printf("r5 ");  Here we create a leaf node Int. The value of the leaf node is $1 */}
            | LP expression RP { $$ = $2; /* printf("r6 ");  simply return the expression */}
    ;

identifier: IDENTIFIER {
                    $$ = new Node("Identifier", $1, yylineno); // Here we create a leaf node Int. The value of the leaf node is $1 
                          }
            ;
