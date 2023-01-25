---
author: Totte Hansen
title: Notes from GNU
---

## General Glossary ##

### Bison ###

``YY_DECL`` (``%code``): 
Defines *scanner prototype* of ``yylex()``.
Ex: 
```c
// Give Flex the prototype of yylex we want
#define YY_DECL yy::parser::symbol_type yylex (driver& drv);
// declare it for the parser's sake
YY_DECL;
```

### Flex ###

***

## ``make_*`` in Flex ##

``make_[operator]`` functions are generated from bison files from defined *tokens*. Defined *tokens* are then appended with the ``make_`` in *file*.tab.c, to then be imported and used with Flex/Lex.
