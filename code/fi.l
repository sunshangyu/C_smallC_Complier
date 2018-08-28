%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
    #include "fi.tab.h"
	#include "tree.c"
	char str[40];
	char str1[40];
	char str2[40];
	int ERROR_A = 0;
	struct NODE* nodee = NULL;
%}
%option yylineno
TYPE int|float
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
sixteen [0-9]|[A-F]
sixteennoo [1-9]|[A-F]
eight [0-7]
eightnoo [1-7]
ten [0-9]
tennoo [1-9]
EIGHTINT [-+]?(0{eightnoo}{eight}*)
SIXTEENINT [-+]?(0x({sixteennoo}{sixteen}*))
INT (({tennoo}{ten}*)|0+)
SIXTEENFLOAT [-+]?(0x(({sixteennoo}{sixteen}*)|0)"."{sixteen}+)
EIGHTFLOAT [-+]?(0({eightnoo}{eight}*|0)"."{eight}+)
FLOAT [-+]?({ten}+"."{ten}+)
EFLOAT [-+]?(({ten}*"."{ten}*|{ten}*)("e"|"E")[-+]?{ten}+)
WRONG_EIGHTINT [-+]?(0{eight}*[9A-Za-z]+{eight}*)
WRONG_SIXTEENINT [-+]?(0x{sixteen}*[G-Zg-z]+{sixteen}*)
WRONG_EFLOAT [-+]?(({ten}*"."{ten}*|{ten}+)("e"|"E")|("e"|"E")[-+]?({ten}+|{ten}*"."{ten}*))
ID [A-Za-z_][A-Za-z_0-9]*
SEMI ;
COMMA ,
ASSIGNOP =
PLUS \+
MINUS \-
AND &&
OR \|\|
DOT \.
NOT \!
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
RELOP >|<|==|<=|>=|!=
COMMENT (\/\/.*)|(\/\*.*\*\/)
STAR \*
DIV \/
SPACE [ \t]
EOL \n
ERROR_TYPE .
%%
{TYPE} {strcpy(str,"TYPE");strcpy(str2,yytext);nodee = Create_newnode(str,yylineno);strcpy(nodee->var_fun_str_name,str2);yylval.token_node = nodee;return TYPE;}

{STRUCT} {strcpy(str,"STRUCT");yylval.token_node = Create_newnode(str,yylineno);return STRUCT;}
{RETURN} {strcpy(str,"RETURN");yylval.token_node = Create_newnode(str,yylineno);return RETURN;}
{IF} {strcpy(str,"IF");yylval.token_node = Create_newnode(str,yylineno);return IF;}
{ELSE} {strcpy(str,"ELSE");yylval.token_node = Create_newnode(str,yylineno);return ELSE;}
{WHILE} {strcpy(str,"WHILE");yylval.token_node = Create_newnode(str,yylineno);return WHILE;}


{EIGHTINT} {strcpy(str,"OINT->INT: ");strcpy(str1,yytext);strcat(str,eightint_to_int(str1));yylval.token_node = Create_newnode(str,yylineno);return EIGHTINT;}
{SIXTEENINT} {strcpy(str,"OXINT->INT: ");strcpy(str1,yytext);strcat(str,sixteenint_to_int(str1));yylval.token_node = Create_newnode(str,yylineno);return SIXTEENINT;}


{INT} {strcpy(str,"INT");strcpy(str2,yytext);nodee = Create_newnode(str,yylineno);strcpy(nodee->var_fun_str_name,str2);yylval.token_node = nodee;return INT;}
{FLOAT} {strcpy(str,"FLOAT");strcpy(str2,yytext);nodee = Create_newnode(str,yylineno);strcpy(nodee->var_fun_str_name,str2);yylval.token_node = nodee;return FLOAT;}


{EIGHTFLOAT} {strcpy(str,"OFLOAT: ");strcat(str,yytext);yylval.token_node = Create_newnode(str,yylineno);return EIGHTFLOAT;}
{SIXTEENFLOAT} {strcpy(str,"OXFLOAT: ");strcat(str,yytext);yylval.token_node = Create_newnode(str,yylineno);return SIXTEENFLOAT;}
{EFLOAT} {strcpy(str,"EFLOAT: ");strcat(str,yytext);yylval.token_node = Create_newnode(str,yylineno);return EFLOAT;}
{WRONG_SIXTEENINT} {ERROR_A = 1;printf("Error type A at line %d: Illegal hexadecimal number \"%s\";\n",yylineno,yytext);strcpy(str,"WRONG_SIXTEENINT: ");strcat(str,yytext);yylval.token_node = Create_newnode(str,yylineno);return WRONG_SIXTEENINT;}
{WRONG_EIGHTINT} {ERROR_A = 1;printf("Error type A at line %d: Illegal octal number \"%s\";\n",yylineno,yytext);strcpy(str,"WRONG_EIGHTINT: ");strcat(str,yytext);yylval.token_node = Create_newnode(str,yylineno);return WRONG_EIGHTINT;}
{WRONG_EFLOAT} {ERROR_A = 1;printf("Error type A at line %d: Illegal floating point number \"%s\";\n",yylineno,yytext);strcpy(str,"WRONG_EFLOAT: ");strcat(str,yytext);yylval.token_node = Create_newnode(str,yylineno);return WRONG_EFLOAT;}


{ID} {strcpy(str,"ID");strcpy(str2,yytext);nodee = Create_newnode(str,yylineno);strcpy(nodee->var_fun_str_name,str2);yylval.token_node = nodee;return ID;}

{SEMI} {strcpy(str,"SEMI");yylval.token_node = Create_newnode(str,yylineno);return SEMI;}
{COMMA} {strcpy(str,"COMMA");yylval.token_node = Create_newnode(str,yylineno);return COMMA;}
{ASSIGNOP} {strcpy(str,"ASSIGNOP");yylval.token_node = Create_newnode(str,yylineno);return ASSIGNOP;}
{PLUS} {strcpy(str,"PLUS");yylval.token_node = Create_newnode(str,yylineno);return PLUS;}
{MINUS} {strcpy(str,"MINUS");yylval.token_node = Create_newnode(str,yylineno);return MINUS;}
{AND} {strcpy(str,"AND");yylval.token_node = Create_newnode(str,yylineno);return AND;}
{OR} {strcpy(str,"OR");yylval.token_node = Create_newnode(str,yylineno);return OR;}
{DOT} {strcpy(str,"DOT");yylval.token_node = Create_newnode(str,yylineno);return DOT;}
{NOT} {strcpy(str,"NOT");yylval.token_node = Create_newnode(str,yylineno);return NOT;}
{LP} {strcpy(str,"LP");yylval.token_node = Create_newnode(str,yylineno);return LP;}
{RP} {strcpy(str,"RP");yylval.token_node = Create_newnode(str,yylineno);return RP;}
{LB} {strcpy(str,"LB");yylval.token_node = Create_newnode(str,yylineno);return LB;}
{RB} {strcpy(str,"RB");yylval.token_node = Create_newnode(str,yylineno);return RB;}
{LC} {strcpy(str,"LC");yylval.token_node = Create_newnode(str,yylineno);return LC;}
{RC} {strcpy(str,"RC");yylval.token_node = Create_newnode(str,yylineno);return RC;}

{RELOP} {strcpy(str,"RELOP");strcpy(str2,yytext);nodee = Create_newnode(str,yylineno);strcpy(nodee->var_fun_str_name,str2);yylval.token_node = nodee;return RELOP;}

{COMMENT} {strcpy(str,"COMMENT");yylval.token_node = Create_newnode(str,yylineno);return COMMENT;}
{STAR} {strcpy(str,"STAR");yylval.token_node = Create_newnode(str,yylineno);return STAR;}
{DIV} {strcpy(str,"DIV");yylval.token_node = Create_newnode(str,yylineno);return DIV;}
{SPACE} {}
{EOL} {}
{ERROR_TYPE} {ERROR_A = 1;printf("Error type A at line %d: Mystirious charachter \"%s\";\n",yylineno,yytext);}
%%
int yywrap(){
    return 1;
}
