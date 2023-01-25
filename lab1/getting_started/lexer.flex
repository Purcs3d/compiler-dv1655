%top{
#include "parser.tab.hh"
#define YY_DECL yy::parser::symbol_type yylex()
#include "Node.h"
int lexical_errors = 0;
}
%option yylineno noyywrap nounput batch noinput stack 
%%

"class"                     {printf("CLASS");}
"static"                    {printf("STATIC");}
"void"                      {printf("VOID");}
"main"                      {printf("MAIN");}
"class"                     {printf("CLASS");}
"public"                    {printf("PUBLIC"); }


"+"                         {printf("PLUSOP ");}// return yy::parser::make_PLUSOP(yytext);}
"-"                         {printf("SUBOP ");}//return yy::parser::make_MINUSOP(yytext);}
"*"                         {printf("MULTOP ");}//return yy::parser::make_MULTOP(yytext);}
"*"                         {printf("DIVOP ");}
"("                         {printf("LP ");} //return yy::parser::make_LP(yytext);}
")"                         {printf("RP ");} //return yy::parser::make_RP(yytext);}
0|[1-9][0-9]*               {printf("INT "); }//return yy::parser::make_INT(yytext);}
"["                         {printf("LHB"); }
"]"                         {printf("RHB"); }
"{"                         {printf("LCB"); }
"}"                         {printf("RCB"); }
"="                         {printf("SET-TO"); }
"=="                        {printf("EQ"); }
"&&"                        {printf("AND");}
"||"                        {printf("OR");}
">"                         {printf("GT");}
"<"                         {printf("LT");}

"."                         {printf("PERIOD");}
","                         {printf("COMMA");}
"!"                         {printf("EXCLAMATION");}
";"                         {printf("DEC_END"); }

[ \t\n\r]+                  {} // Whitespaces
"int"                       {printf("INT_TYPE"); }
"boolean"                   {printf("BOOL_TYPE"); }
"String"                    {printf("STRING");}

"if"                        {printf("IF");} 
"else"                      {printf("ELSE");}
"while"                     {printf("WHILE");}

"new"                       {printf("NEW");}

"length"                    {printf("LENGTH");}
"System.out.println"        {printf("PRINT");}

[a-zA-Z]+[a-zA-Z0-9\-_]*    {printf("IDENTIFIER");}
.                           { if(!lexical_errors) fprintf(stderr, "Lexical errors found! See the logs below: \n"); printf("Character %s is not recognized\n", yytext); lexical_errors = 1;}
<<EOF>>                     return yy::parser::make_END();
%%