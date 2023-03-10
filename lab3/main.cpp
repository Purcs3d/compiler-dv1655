/*
  Totte Hansen
  Marcus Kicklighter
*/
#include<iostream>
#include "parser.tab.h"
#include "semanticCheck.hpp"
#include "symbolTable.hpp"

extern Node* root;
extern FILE* yyin;
extern int yylineno;
extern int lexical_errors;

void yy::parser::error(std::string const&err)
{ 
  printf("Syntax errors found! See the logs below: \n");
  fprintf(stderr, "@error at line %d. Cannot generate a syntax for this input: %s\n", yylineno, err.c_str()); 
}

int main(int argc, char **argv)
{
  //Reads from file if a file name is passed as an argument. Otherwise, reads from stdin.
  if(argc > 1) {
    if(!(yyin = fopen(argv[1], "r"))) {
      perror(argv[1]);
      return 1;
    }
  }

  yy::parser parser;

  if(!parser.parse() && !lexical_errors) {

    // printf("\nThe compiler successfuly generated a syntax tree for the given input! \n");
    // printf("\nPrint Tree:  \n");
    
    root->generate_tree();
    // using the constructor that fills the symbol table
    SymbolTable* ST;
    ST = new SymbolTable(root);
    
    ST->print_ST();
    SemanticAnalysis* SA = new SemanticAnalysis(root, ST);
    // if(SA->contains_error()) {
    //   // exit program ?
    //   // just don't continue?
    //   // cout << "Contains Error" << endl;
    // }
    
    
    delete ST;
    delete SA;
  }

  return 0;
}