%{
	#include "lex.yy.c"
	#include "semantic_analysis.c"
	#include "translate.c"
	#include "aimcode.c"
	struct NODE* node = NULL;
	int ERROR_B = 0;
	#define YYERROR_VERBOSE 1
%}
%union{
	struct NODE* token_node;
};
%type<token_node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args
%token<token_node> TYPE WRITE READ
%token<token_node> STRUCT RETURN IF ELSE WHILE
%token<token_node> EIGHTINT SIXTEENINT INT WRONG_EIGHTINT WRONG_SIXTEENINT
%token<token_node> EIGHTFLOAT SIXTEENFLOAT FLOAT EFLOAT WRONG_EFLOAT
%token<token_node> ID
%token<token_node> SEMI
%token<token_node> COMMA
%token<token_node> ASSIGNOP
%token<token_node> PLUS MINUS STAR DIV
%token<token_node> AND OR
%token<token_node> DOT
%token<token_node> NOT
%token<token_node> LP RP LB RB LC RC
%token<token_node> RELOP
%token<token_node> COMMENT
%token<token_node> SPACE EOL
%token<token_node> ERROR_TYPE

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left MINUS PLUS
%left DIV STAR
%right NOT
%left DOT LP RP LB RB


%%
Program : ExtDefList {$$ = insert(1,$1);strcpy($$->name,"Program");node = $$;}
	;
ExtDefList : {$$ = insert(0);}
	|ExtDef ExtDefList {$$ = insert(2,$1,$2);strcpy($$->name,"ExtDefList");}
	;
ExtDef : Specifier ExtDecList SEMI {//全局变量的定义
		$$ = insert(3,$1,$2,$3);strcpy($$->name,"ExtDef");}
	|Specifier SEMI {$$ = insert(2,$1,$2);strcpy($$->name,"ExtDef");}
	|Specifier FunDec CompSt {$$ = insert(3,$1,$2,$3);strcpy($$->name,"ExtDef");}
	|Specifier FunDec SEMI{//函数的声明
		$$ = insert(3,$1,$2,$3);strcpy($$->name,"ExtDef");}
	;
ExtDecList : VarDec {$$ = insert(1,$1);strcpy($$->name,"ExtDecList");}
	|VarDec COMMA ExtDecList {$$ = insert(3,$1,$2,$3);strcpy($$->name,"ExtDecList");}
	;
Specifier : TYPE {//普通类型,int和float
		$$ = insert(1,$1);strcpy($$->name,"Specifier");}
	|StructSpecifier {//结构体类型
		$$ = insert(1,$1);strcpy($$->name,"Specifier");}
	;
StructSpecifier : STRUCT OptTag LC DefList RC {//定义结构体的产生式
		$$ = insert(5,$1,$2,$3,$4,$5);strcpy($$->name,"StructSpecifier");}
	|STRUCT Tag {//使用已经定义的结构体
		$$ = insert(2,$1,$2);strcpy($$->name,"StructSpecifier");}
	;
OptTag : {$$ = insert(0);}
	|ID {$$ = insert(1,$1);strcpy($$->name,"OptTag");}
	;
Tag : ID {$$ = insert(1,$1);strcpy($$->name,"Tag");}
	;
VarDec : ID {$$ = insert(1,$1);strcpy($$->name,"VarDec");}
	|VarDec LB INT RB {$$ = insert(4,$1,$2,$3,$4);strcpy($$->name,"VarDec");}
	;
FunDec : ID LP VarList RP {$$ = insert(4,$1,$2,$3,$4);strcpy($$->name,"FunDec");}
	|ID LP RP {$$ = insert(3,$1,$2,$3);strcpy($$->name,"FunDec");}
	;
VarList : ParamDec COMMA VarList {//作为形参
		$$ = insert(3,$1,$2,$3);strcpy($$->name,"VarList");}
	|ParamDec {$$ = insert(1,$1);strcpy($$->name,"VarList");}
	; 
ParamDec : Specifier VarDec {$$ = insert(2,$1,$2);strcpy($$->name,"ParamDec");}
	;
CompSt : LC DefList StmtList RC {$$ = insert(4,$1,$2,$3,$4);strcpy($$->name,"CompSt");}
	;
StmtList : {$$ = insert(0);}
	|Stmt StmtList {$$ = insert(2,$1,$2);strcpy($$->name,"StmtList");}
	;
Stmt : Exp SEMI {$$ = insert(2,$1,$2);strcpy($$->name,"Stmt");}
	|CompSt {$$ = insert(1,$1);strcpy($$->name,"Stmt");}
	|RETURN Exp SEMI {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Stmt");}
	|IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = insert(5,$1,$2,$3,$4,$5);strcpy($$->name,"Stmt");}
	|IF LP Exp RP Stmt ELSE Stmt {$$ = insert(7,$1,$2,$3,$4,$5,$6,$7);strcpy($$->name,"Stmt");}
	|WHILE LP Exp RP Stmt {$$ = insert(5,$1,$2,$3,$4,$5);strcpy($$->name,"Stmt");}
	|IF LP Exp RP Exp ELSE Stmt {ERROR_B = 1;//yyerror("Missing \";\"");
		}
	;
DefList : {$$ = insert(0);}
	|Def DefList {$$ = insert(2,$1,$2);strcpy($$->name,"DefList");}
	;
Def : Specifier DecList SEMI {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Def");}
	;
DecList : Dec {$$ = insert(1,$1);strcpy($$->name,"DecList");}
	|Dec COMMA DecList {$$ = insert(3,$1,$2,$3);strcpy($$->name,"DecList");}
	;
Dec : VarDec {$$ = insert(1,$1);strcpy($$->name,"Dec");}
	|VarDec ASSIGNOP Exp {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Dec");}
	;
Exp : Exp ASSIGNOP Exp {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Exp");}
//在测试文件中，没有AND，OR，RELOP运算，偷懒，不写了
	|Exp AND Exp {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Exp");}
	|Exp OR Exp {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Exp");}
	|Exp RELOP Exp {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Exp");}
	

	|Exp PLUS Exp {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Exp");}

//在测试文件中，没有MINUS，STAR，DIV运算，偷懒，不写了
	|Exp MINUS Exp {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Exp");}
	|Exp STAR Exp {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Exp");}
	|Exp DIV Exp {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Exp");}


	|LP Exp RP {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Exp");}

//在测试文件中，没有取负运算和NOT运算，不写了
	|MINUS Exp {$$ = insert(2,$1,$2);strcpy($$->name,"Exp");}	
	|NOT Exp {$$ = insert(2,$1,$2);strcpy($$->name,"Exp");}
	
	|ID LP Args RP {$$ = insert(4,$1,$2,$3,$4);strcpy($$->name,"Exp");}
	|ID LP RP {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Exp");}
	
	|Exp LB Exp RB {$$ = insert(4,$1,$2,$3,$4);strcpy($$->name,"Exp");}
	|Exp DOT ID {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Exp");}
	|ID {$$ = insert(1,$1);strcpy($$->name,"Exp");}
	|INT {$$ = insert(1,$1);strcpy($$->name,"Exp");}
	|FLOAT {$$ = insert(1,$1);strcpy($$->name,"Exp");}
	

	|EIGHTINT {$$ = insert(1,$1);strcpy($$->name,"Exp");}
	|SIXTEENINT {$$ = insert(1,$1);strcpy($$->name,"Exp");}
	|EIGHTFLOAT {$$ = insert(1,$1);strcpy($$->name,"Exp");}
	|SIXTEENFLOAT {$$ = insert(1,$1);strcpy($$->name,"Exp");}
	|EFLOAT {$$ = insert(1,$1);strcpy($$->name,"Exp");}
	|Exp LB Exp COMMA Exp RB {//yyerror("Missing \"]\";");
			ERROR_B = 1;}
	|WRONG_EIGHTINT {ERROR_B = 1;}
	|WRONG_SIXTEENINT {ERROR_B = 1;}
	|WRONG_EFLOAT {ERROR_B = 1;}
	;
Args : Exp COMMA Args {$$ = insert(3,$1,$2,$3);strcpy($$->name,"Args");}
	|Exp {$$ = insert(1,$1);strcpy($$->name,"Args");}
	;
%%
int main(int argc,char** argv)
{
	if(argc!=3)
	{
		printf("请输入一个源文件，一个目标文件\n");
		return 1;
	}
	FILE* f=fopen(argv[1],"r");
	if (f==NULL)
	{
		printf("无法打开文件 %s\n",argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	if(!ERROR_A&&!ERROR_B)
	{
		Type read = (Type)malloc(sizeof(struct Type_));
		read->kind = FUNCTIONDEF;
		read->name = (char *)malloc(30);
		strcpy(read->name,"read");
		read->function = (Function)malloc(sizeof(struct Function_));
		read->function->kind = INTEGER;
		Type write = (Type)malloc(sizeof(struct Type_));
		write->kind = FUNCTIONDEF;
		write->name = (char *)malloc(30);
		strcpy(write->name,"write");
		write->function = (Function)malloc(sizeof(struct Function_));
		write->function->kind = INTEGER;
		write->function->param = (FieldList)malloc(sizeof(struct FieldList_));
		write->function->param->kind = INTEGER;
		write->function->param->name = (char *)malloc(30);
		strcpy(write->function->param->name,"x");
		read->next = symbol_head;
		write->next = read;
		symbol_head = write;
		semantic_analysis(node);
		//check_DEC_but_DEF();
		translate(node);
		//print_code(argv[2]);
		all_tra(argv[2]);
	}
	fclose(f);
	return 0;
}
int yyerror(char *msg) {
	fprintf(stderr, "Error type B at line %d: %s\n",yylineno,msg);
}
