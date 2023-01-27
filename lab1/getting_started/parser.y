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
%token <std::string> CLASS STATIC VOID MAIN PUBLIC 
%token <std::string> PERIOD COMMA EXCLAMATION SEMICOLON INTTYPE BOOLTYPE STRING
%token <std::string> IF ELSE WHILE NEW LENGTH PRINT IDENTIFIER TRUE FALSE THIS
%token END 0 "end of file"

//defition of operator precedence. See https://www.gnu.org/software/bison/manual/bison.html#Precedence-Decl
%right ASSIGN

%left AND OR 
%left EQ  
%left LT GT
%left PLUSOP MINUSOP
%left MULTOP DIVOP 
%left LHB
%left EXCLAMATION
%left PERIOD


%nonassoc NO_ELSE
%nonassoc ELSE

// definition of the production rules. All production rules are of type Node
%type <Node *> root expression factor identifier type vardeclaration mainclass statement statements
%type <Node *> exprlist experiment stmt_if

%%
// Change this later to be the class 
root: statement {root = $1;};

mainclass: PUBLIC CLASS identifier LCB PUBLIC STATIC VOID MAIN LP STRING LHB RHB identifier LP LCB statement RCB RCB {
                    // TODO
              }

vardeclaration: type identifier SEMICOLON {
                      $$ = new Node("Variable", "", yylineno);
                      $$->children.push_back($1); // type
                      $$->children.push_back($2); // identifier
              }


type: INTTYPE LHB RHB {
            $$ = new Node("ArrayType", "", yylineno);
              }
      | BOOLTYPE {
            $$ =  new Node("BoolType", "", yylineno);
              }
      | INTTYPE {
            $$ = new Node("IntType", "", yylineno);
              }
      | identifier {
            $$ = new Node("IdentifierType", "", yylineno);
              }
        ;

statement: 
            /* empty */
            | LCB statements RCB  { 
              $$ = new Node("Statements", "", yylineno);
            }
            | stmt_if
            | WHILE LP expression RP statement {
              $$ = new Node("While", "", yylineno);
              $$->children.push_back($3);
              $$->children.push_back($5);
            }
            | PRINT LP expression RP SEMICOLON {
              $$ = new Node("Print", "", yylineno);
              $$->children.push_back($3);
            }
            | identifier ASSIGN expression SEMICOLON {
              $$ = new Node("Assign", "", yylineno);
              $$->children.push_back($1);
              $$->children.push_back($3);
            } 
            | identifier LHB expression RHB ASSIGN expression SEMICOLON {
              $$ = new Node("indexAssign", "", yylineno);
              $$->children.push_back($1);
              $$->children.push_back($3);
              $$->children.push_back($6);
            }
            ;

statements:
            /* empty */  
            | statements statement { 
              $$ = new Node("StatementsList", "", yylineno);
              $$->children.push_back($1);
              $$->children.push_back($2);
                }
            ;

stmt_if: IF LP expression RP statement %prec NO_ELSE { // To solve dangling else ambiguity
              $$ = new Node("IF", "", yylineno);
              $$->children.push_back($3);
              $$->children.push_back($5);
            }
            | IF LP expression RP statement ELSE statement {
              $$ = new Node("IfElse", "", yylineno);
              $$->children.push_back($3);
              $$->children.push_back($5);
              $$->children.push_back($7);
            };


expression: expression PLUSOP expression {      
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
            
            | experiment
                
            | expression PERIOD LENGTH {
                            $$ = new Node("LenghtOfExpression", "", yylineno);
                            $$->children.push_back($1);
                          }
            | expression PERIOD identifier LP exprlist RP {  // recursive grammar, follows recursive rules
                              $$ = new Node("Method call", "", yylineno);
                          }
            | expression PERIOD identifier LP RP {  // for the empty case
                              $$ = new Node("Method call", "", yylineno);
                          }
            | TRUE {
                  $$ = new Node("True", "", yylineno);
                }
            | FALSE {
                  $$ = new Node("False", "", yylineno);
                }
            | identifier {
                  $$ = $1;
                }
            | THIS  {
                  $$ = new Node("This", "", yylineno);
                }
            | NEW INTTYPE LHB expression RHB {
                      $$ = new Node("AllocateIntArray", "", yylineno);
                      $$->children.push_back($4);  // Size of int array
                  }
            | NEW identifier LP RP {
                      $$ = new Node("AllocateIdentifier", "", yylineno);
                      $$->children.push_back($2);  
                  }
            | EXCLAMATION expression {
                      $$ = new Node("Negation", "", yylineno);
                      $$->children.push_back($2); 
                  }
            | factor      {$$ = $1; /* printf("r4 ");*/}
            ;

experiment: expression LHB expression RHB {
        $$ = new Node("Index", "", yylineno);
        $$->children.push_back($1);  // what to take index of
        $$->children.push_back($3);  // index value
      }

exprlist: 
    /*empty*/
    | expression {
              $$ = new Node("ExpressionList", "", yylineno);
              $$->children.push_back($1);
                }
    | exprlist COMMA expression {
              $$ = new Node("ExpressionList", "", yylineno);
              $$->children.push_back($1);
              $$->children.push_back($3);
                }
          ;

// Factor like an integer
factor:     INT           {  $$ = new Node("Int", $1, yylineno); /* printf("r5 ");  Here we create a leaf node Int. The value of the leaf node is $1 */}
            | LP expression RP { $$ = $2; /* printf("r6 ");  simply return the expression */}
    ;

identifier: IDENTIFIER {
                    $$ = new Node("Identifier", $1, yylineno); // Here we create a leaf node Int. The value of the leaf node is $1 
                          }
            ;
