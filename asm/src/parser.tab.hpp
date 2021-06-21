/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_HPP_INCLUDED
# define YY_YY_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    T_EOF = 0,                     /* T_EOF  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    T_ENDL = 258,                  /* T_ENDL  */
    T_INT = 259,                   /* T_INT  */
    T_REGISTER = 260,              /* T_REGISTER  */
    T_IDENTIFIER = 261,            /* T_IDENTIFIER  */
    T_STRING = 262,                /* T_STRING  */
    T_PLUS = 263,                  /* T_PLUS  */
    T_MINUS = 264,                 /* T_MINUS  */
    T_TIMES = 265,                 /* T_TIMES  */
    T_DIVIDE = 266,                /* T_DIVIDE  */
    T_LSHIFT = 267,                /* T_LSHIFT  */
    T_RSHIFT = 268,                /* T_RSHIFT  */
    T_COMMA = 269,                 /* T_COMMA  */
    T_COLON = 270,                 /* T_COLON  */
    T_LPAREN = 271,                /* T_LPAREN  */
    T_RPAREN = 272,                /* T_RPAREN  */
    T_LBRACE = 273,                /* T_LBRACE  */
    T_RBRACE = 274,                /* T_RBRACE  */
    T_EQUAL = 275,                 /* T_EQUAL  */
    T_HASH = 276,                  /* T_HASH  */
    T_PERCENT = 277,               /* T_PERCENT  */
    T_ALIAS = 278,                 /* T_ALIAS  */
    T_ORIGIN = 279,                /* T_ORIGIN  */
    T_MACRO = 280,                 /* T_MACRO  */
    T_VAR = 281,                   /* T_VAR  */
    T_ASCII = 282,                 /* T_ASCII  */
    T_CARET = 283                  /* T_CARET  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 16 "parser.y"

	int ival;
	char* sval;

#line 97 "parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_TAB_HPP_INCLUDED  */
