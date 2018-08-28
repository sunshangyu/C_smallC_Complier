/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_FI_TAB_H_INCLUDED
# define YY_YY_FI_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TYPE = 258,
    WRITE = 259,
    READ = 260,
    STRUCT = 261,
    RETURN = 262,
    IF = 263,
    ELSE = 264,
    WHILE = 265,
    EIGHTINT = 266,
    SIXTEENINT = 267,
    INT = 268,
    WRONG_EIGHTINT = 269,
    WRONG_SIXTEENINT = 270,
    EIGHTFLOAT = 271,
    SIXTEENFLOAT = 272,
    FLOAT = 273,
    EFLOAT = 274,
    WRONG_EFLOAT = 275,
    ID = 276,
    SEMI = 277,
    COMMA = 278,
    ASSIGNOP = 279,
    PLUS = 280,
    MINUS = 281,
    STAR = 282,
    DIV = 283,
    AND = 284,
    OR = 285,
    DOT = 286,
    NOT = 287,
    LP = 288,
    RP = 289,
    LB = 290,
    RB = 291,
    LC = 292,
    RC = 293,
    RELOP = 294,
    COMMENT = 295,
    SPACE = 296,
    EOL = 297,
    ERROR_TYPE = 298,
    LOWER_THAN_ELSE = 299
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 10 "fi.y" /* yacc.c:1909  */

	struct NODE* token_node;

#line 103 "fi.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_FI_TAB_H_INCLUDED  */
