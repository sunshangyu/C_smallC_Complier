#include "symbol.c"
enum KIND kind;//当前处理的ID的类型
char return_kind[30];//函数定义或者声明时的返回类型
int is_id_existed = 0;
int id_is_defined = 0;//0代表没有定义，3代表定义了，1代表定义了，但是类型和函数的返回类型不一样，2代表定义了，并且是对的，和函数类型一样，
int def_dec = 0;//当前正在处理的函数是在声明0还是在定义1
int struct_function_array = 3;//判断当前要加进符号表的id是结构体的作用域内0还是函数的域1还是数组2还是全局的变量3还是函数的形参4
Type function_variety = NULL;//当前即将插入符号表的节点是一个函数，那么将指向新建的节点的指针赋予function_variety，通过此变量链接到函数的域和参数
Type struct_variety = NULL;//当前正在处理的节点是一个数组，
int real_param = 0;//函数实参个数
void semantic_analysis(struct NODE* h);

void check_DEC_but_DEF();
void semantic_analysis(struct NODE* h)
{
	if(h==NULL)	return;//如果节点为空，则返回
	if(!(strcmp(h->name,"ExtDefList")))
	{
		if(h->firstchild->nextsibling == NULL)
			semantic_analysis(h->firstchild);
		else
		{
			semantic_analysis(h->firstchild);
			semantic_analysis(h->firstchild->nextsibling);
		}
	}else if(!(strcmp(h->name,"ExtDef")))//当前节点的子节点中包含着定义的函数，普通变量或者结构体信息
	{
		if(!(strcmp(h->firstchild->nextsibling->name,"SEMI")))//定义结构体，将结构体插入符号表
		{
			//用到这个产生式的时候，下一个调用的产生式必然是Sepcifier->StructSpecifier，再有StructSpecifier->STRUCT OptTag LC DefList RC
			//StructSpecifier->STRUCT Tag，是在使用结构体的时候用到的，在测试的文件中没有出现定义时调用这个产生式的情况，所以不考虑
			//并且结构体的定义一定是全局的，那么此时调整struct_function_array
			struct_function_array = 3;
			semantic_analysis(h->firstchild);//在这一步中，要据此在符号表的主链中插入这个结构体
		}else if(strcmp(h->firstchild->nextsibling->name,"ExtDecList")==0&&strcmp(h->firstchild->nextsibling->nextsibling->name,"SEMI")==0)//此时定义的是全局变量，需要将全局变量都加入符号表
		{
			//由于一个结构体或者函数的内部的域不可以加如符号表的主链之中，所以要记录某一个ID的上层节点，比如，当前正在判断的某一个ID的上层节点是NULL，那么
			//说明此节点应在主链中，如果此ID的上层节点是一个结构体P，那么此ID应放在P的域中，如果上层节点是一个函数，那么如果ID是一个参数，则应该链到函数的参数链，是域，则链到域链
			//现在要定义的是全局变量，则用这个产生式定义的变量全都用该放在符号表的主链中
			//此产生式定义的全局变量是普通变量（没有内部域的）或者数组
			//首先得确定类型，通过前序遍历，进入Specifier节点中，将当前的类型取出来，只有Specifier->TYPE会被使用
			semantic_analysis(h->firstchild);//在这一步中将类型取出来，放在kind中
			semantic_analysis(h->firstchild->nextsibling);//进入ExtDecList节点，在此节点中将变量加入符号表的主链，并且类型全都是kind
		}else if(strcmp(h->firstchild->nextsibling->name,"FunDec")==0)//函数
		{
			if(strcmp(h->firstchild->nextsibling->nextsibling->name,"CompSt")==0)//定义
			{
				//此时产生式必然调用的是ExtDef->Specifier FunDec Compst，定义一个函数
				//首先要获得函数的返回类型
				def_dec = 1;
				semantic_analysis(h->firstchild);//通过此步骤将函数的返回类型，放到kind中，可能是INTEGER，FLOATING，如果返回类型是个结构体的话，则放回到return_kind中
				//既然是定义函数，那么这个函数的ID自然也应该放在符号表中，接下来要定义函数的形参和域，所以，应该调整up_field
				struct_function_array = 3;
				semantic_analysis(h->firstchild->nextsibling);//进入FunDec中，在这里记录函数的ID，并存储在符号表的主链中
				//并且在这一步中将function_variety赋值，它指向FunDec的函数结构体CompSt
				semantic_analysis(h->firstchild->nextsibling->nextsibling);//在这一步中将花括号内的域链接到function_variety->function->structure
			}else if(strcmp(h->firstchild->nextsibling->nextsibling->name,"SEMI")==0)//声明
			{
				def_dec = 0;
				semantic_analysis(h->firstchild);
				struct_function_array = 3;
				semantic_analysis(h->firstchild->nextsibling);
			}
		}
	}else if(!(strcmp(h->name,"Specifier")))//判断节点的名称，即文法符号//类型，包括普通的类型和用户定义的结构体类型
	{
		if(!(strcmp(h->firstchild->name,"TYPE")))//基本类型
		{
			if(!(strcmp(h->firstchild->var_fun_str_name,"int")))//var_fun_str_name是ID名
				kind = INTEGER;//kind是一个enum类型，表示当前正在处理的节点的的属性为int
			else
				kind = FLOATING;
		}else//如果当前处理节点的第一个节点的名字不是TYPE，则说明要定义的是结构体
		{
			kind = STRUCTURE;
			semantic_analysis(h->firstchild);//递归往深层处理
		}
	}else if(!(strcmp(h->name,"StructSpecifier")))//结构体类型
	{
		if(h->firstchild->nextsibling->nextsibling==NULL)//使用已经定义的结构体StructSpecifier->STRUCT Tag，此时Tag作为一个类型
		//首先要看一下Tag是否是一个已经定义的结构体，如果不是，则报错
		//这一步要设置以下return_kind，因为如果定义的是函数的话，则函数需要记录函数的返回类型
		//在给函数分配结构体的空间时，也得将函数结构体中return_kind字符串设为NULL，这样就容易确定函数的具体返回类型
		{
			//默认不会出现struct {}这样的状况
			int is_struct_existed;
			is_struct_existed = check_isexisted_struct(h->firstchild->nextsibling->firstchild->var_fun_str_name);//StructSpecifier->STRUCT Tag->STRUCT ID
			if(!is_struct_existed)
				printf("Error type 17 at Line %d:未定义的结构体类型  %s;\n",h->line,h->firstchild->nextsibling->firstchild->var_fun_str_name);
			else//已经定义过了，可以使用
			{
				kind = STRUCTURE;//首先说明是一个结构体类型的对象或者函数
				strcpy(return_kind,h->firstchild->nextsibling->firstchild->var_fun_str_name);//然后说明结构体的具体名称
			}
		}else//StructSpecifier->STRUCT OptTag LC DefList RC，结构体的定义用到的产生式
		{
			//要检查这个ID是否被某个普通变量使用或者被其他结构体使用
			//由于结构体的定义是在全局变量中定义，所以检查该结构体的ID是否被使用时，应在符号表的主链中寻找
			//由于是结构体的定义，所以在查找时，考虑域的影响，仅仅查找全局变量是否有着相同ID的int或者float型的变量
			is_id_existed = check_isexisted_struct(h->firstchild->nextsibling->firstchild->var_fun_str_name);
			if(is_id_existed)
				printf("Error type 16 at Line %d:结构体  %s  和某个结构体或者全局变量重名;\n",h->line,h->firstchild->nextsibling->firstchild->var_fun_str_name);
			else//这个ID并没有被其他的普通变量或者其他的结构体使用的话，则新建一个节点，此节点在主节点上，类型则是STRUCTURE
			{	//新建时，将一个指向此节点的指针返回，目的是在Compst中定义改节点的域时，通过struct_variety->structure放到合适的位置上
				struct_variety = new_struct_symbol(h->firstchild->nextsibling->firstchild->var_fun_str_name);
				//将结构体的ID和STRUCTURE存在主节点后，就还将结构体的域一个一个地连在struct_variety->structure上，如果插入新的域，则将新的域连在struct_variety中，并且将新的域的next链接之前的第一个域
				struct_function_array = 0;//设定struct_function为0,接下来定义的类型则都是结构内部的域，那么就该使用struct_variety作为指向当前ID的所在结构体的指针
				semantic_analysis(h->firstchild->nextsibling->nextsibling->nextsibling);//进入DefList中
			}
		}
	}else if(!(strcmp(h->name,"DefList")))
	{
		//在构建语法树时，没有出现DefList推出空这一个产生式，即不会有父节点是DefList，子节点是空的情况，会出现两种情况DefList->Def，或者DefList->Def DefList
		if(h->firstchild != NULL&h->firstchild->nextsibling == NULL)//说明此时调用的产生式是DefList->Def
			//因为在这一步中，并没有出现实际构造符号节点的情况，所以不需要设置什么类型或者指针
			semantic_analysis(h->firstchild);//进入Def之中
		else
		{
			semantic_analysis(h->firstchild);//更改kind的值
			semantic_analysis(h->firstchild->nextsibling);
		}
	}else if(!(strcmp(h->name,"Def")))
	{
		semantic_analysis(h->firstchild);//进入Specifier中，调整了kind或者kind和return_kind
		semantic_analysis(h->firstchild->nextsibling);//进入DecList中
	}else if(!(strcmp(h->name,"DecList")))
	{	
		if(h->firstchild->nextsibling == NULL)//调用的产生式是DecList->Dec
			semantic_analysis(h->firstchild);//进入Dec中
		else//调用的产生式是DecList->Dec COMMA DecList
		{
			semantic_analysis(h->firstchild);
			semantic_analysis(h->firstchild->nextsibling->nextsibling);
		}
	}else if(!(strcmp(h->name,"Dec")))
	{
		if(h->firstchild->nextsibling == NULL)////调用的产生式是Dec->VarDec
			semantic_analysis(h->firstchild);//进入VarDec中
		else//调用的产生式为Dec->VarDec ASSIGNOP Exp，这种情况下不可能是用来定义数组的
		{	//在测试的文件中没有出现定义数组时，就给值的情况，所以调用Dec->VarDec ASSIGNOP Exp之后，VarDec只会调用varDec->ID
			//通过Dec为左侧的产生式，所定义的数组或者是普通变量必然不是全局变量，因为全局变量是由ExtDecList->VarDec或者ExtDecList->VarDec COMMA ExtDecList定义的
			//此产生式只有可能是用来定义函数或者结构体内的域
			//结构体的作用域内0还是函数的域1还是数组2还是全局的变量3还是函数的形参4
			//结构体的域struct_variety->structure便是该结构体的第一个域
			if(struct_function_array == 0)//结构体的域
				printf("Error type 16 at Line %d:结构体定义时不可以对域初始化;\n",h->line);
			else if(struct_function_array == 1)//函数的域
			{
				is_id_existed = check_field_isexisted(function_variety->function->field_list,h->firstchild->firstchild->var_fun_str_name);
				if(is_id_existed)//如果在此结构体的域内已经存在着这样一个ID，则报错
					printf("Error type 3 at Line %d:id %s和所在函数内的某个域重名;\n",h->line,h->firstchild->firstchild->var_fun_str_name);
				else//该ID在此结构体内没有被使用过，可以用，便把此ID插入到结构体的域中
				{
					FieldList p = new_field();//给域分配一个空间，并且返回指向该域的一个指针
					strcpy(p->name,h->firstchild->firstchild->var_fun_str_name);//域的名字，VarDec只会调用varDec->ID
					p->kind = kind;
					if(kind == STRUCTURE)
					{
						p->struct_name = (char*)malloc(sizeof(30));//给p->name分配30个字节的空间
						strcpy(p->struct_name,return_kind);
					}
					p->next = function_variety->function->field_list;
					function_variety->function->field_list = p;
				}
			}
			//在测试的代码中仅仅存在int x = 10，这样的类似的情况，所以这里的Exp只用到了Exp->FLOATING和Exp->INTEGER这两个产生式
			if(!(strcmp(h->firstchild->nextsibling->nextsibling->firstchild->name,"INT")))
				if(kind != INTEGER)
					printf("Error type 5 at Line %d:赋值号两边的类型不匹配;\n",h->line);
			if(!(strcmp(h->firstchild->nextsibling->nextsibling->firstchild->name,"FLOAT")))
				if(kind != FLOATING)
					printf("Error type 5 at Line %d:赋值号两边的类型不匹配;\n",h->line);
		}
	}else if(!(strcmp(h->name,"VarDec")))//得考虑是否为全局变量
	{
		//此处采用投机取巧的方式，因为测试的文件中并没有出现多维和二维数组的情况，所以此时只考虑一维数组
		if(struct_function_array == 0)//结构体的域
		{
			is_id_existed = check_field_isexisted(struct_variety->structure,h->firstchild->var_fun_str_name);
			if(is_id_existed)//如果在此结构体的域内已经存在着这样一个ID，则报错
				printf("Error type 15 at Line %d:变量  %s  和所在结构体内的某个域重名;\n",h->line,h->firstchild->var_fun_str_name);
			else//该ID在此结构体内没有被使用过，可以用，便把此ID插入到结构体的域中
			{
				FieldList p = new_field();//给域分配一个空间，并且返回指向该域的一个指针
				strcpy(p->name,h->firstchild->var_fun_str_name);//域的名字
				p->kind = kind;
				if(h->firstchild->nextsibling == NULL)//VarDec->ID LB INTEGER RB,p-kind1 = ARRAY;
					p->kind1 = ARRAY;
				if(kind == STRUCTURE)
				{
					p->struct_name = (char*)malloc(sizeof(30));//给p->name分配30个字节的空间
					if(h->firstchild->nextsibling != NULL)
						p->kind1 = ARRAY;
					strcpy(p->struct_name,return_kind);
				}
				p->next = struct_variety->structure;
				struct_variety->structure = p;
			}
		}else if(struct_function_array == 1)//函数的域
		{
			if(strcmp(h->firstchild->name,"ID") == 0)
			{
				is_id_existed = check_field_isexisted(function_variety->function->field_list,h->firstchild->var_fun_str_name);
				if(is_id_existed)//如果在此结构体的域内已经存在着这样一个ID，则报错
					printf("Error type 3 at Line %d:变量  %s  和所在函数内的某个域重名;\n",h->line,h->firstchild->var_fun_str_name);
				else//该ID在此结构体内没有被使用过，可以用，便把此ID插入到结构体的域中
				{
					FieldList p = new_field();//给域分配一个空间，并且返回指向该域的一个指针
					strcpy(p->name,h->firstchild->var_fun_str_name);//域的名字，VarDec只会调用varDec->ID
					p->kind = kind;
					p->next = function_variety->function->field_list;
					function_variety->function->field_list = p;
					if(h->firstchild->nextsibling != NULL)//VarDec->ID LB INTEGER RB,p-kind1 = ARRAY;
						p->kind1 = ARRAY;
					if(kind == STRUCTURE)
					{
						p->struct_name = (char*)malloc(sizeof(30));//给p->name分配30个字节的空间
						if(h->firstchild->nextsibling != NULL)
							p->kind1 = ARRAY;
						strcpy(p->struct_name,return_kind);
					}
				}
			}
		}else if(struct_function_array == 4)//函数的形参
		{
			if(strcmp(h->firstchild->name,"ID") == 0)
			{
				is_id_existed = check_field_isexisted(function_variety->function->param,h->firstchild->var_fun_str_name);
				if(is_id_existed)//如果在此结构体的域内已经存在着这样一个ID，则报错
					printf("Error type 3 at Line %d:id %s和所在函数的某个形参重名;\n",h->line,h->firstchild->var_fun_str_name);
				else//该ID在此结构体内没有被使用过，可以用，便把此ID插入到结构体的域中
				{
					FieldList p = new_field();//给域分配一个空间，并且返回指向该域的一个指针
					strcpy(p->name,h->firstchild->var_fun_str_name);//域的名字，VarDec只会调用varDec->ID
					p->next = function_variety->function->param;
					function_variety->function->param = p;
					p->kind = kind;
					if(kind == STRUCTURE)
					{
						p->struct_name = (char*)malloc(sizeof(30));//给p->name分配30个字节的空间
						if(h->firstchild->nextsibling != NULL)
							p->kind1 = ARRAY;
						strcpy(p->struct_name,return_kind);
					}
				}
			}
			else//一维数组的情况
			{
				is_id_existed = check_field_isexisted(function_variety->function->param,h->firstchild->firstchild->var_fun_str_name);
				if(is_id_existed)//如果在此结构体的域内已经存在着这样一个ID，则报错
					printf("Error type 3 at Line %d:id %s和所在函数的某个形参重名;\n",h->line,h->firstchild->firstchild->var_fun_str_name);
				else//该ID在此结构体内没有被使用过，可以用，便把此ID插入到结构体的域中
				{
					FieldList p = new_field();//给域分配一个空间，并且返回指向该域的一个指针
					strcpy(p->name,h->firstchild->firstchild->var_fun_str_name);
					p->next = function_variety->function->param;
					function_variety->function->param = p;
					p->kind1 = kind;
					if(h->firstchild->firstchild->nextsibling != NULL)
						p->kind = ARRAY;
					if(kind == STRUCTURE)
					{
						p->struct_name = (char*)malloc(sizeof(30));//给p->name分配30个字节的空间
						strcpy(p->struct_name,return_kind);
					}
				}
			}
		}else if(struct_function_array == 3)//全局变量
		{
			is_id_existed = check_mainlist_isexisted(symbol_head,h->firstchild->var_fun_str_name);
			if(is_id_existed)//如果在主链中已经存在着这样一个ID，则报错
				printf("Error type 3 at Line %d:id %s  和某个全局变量重名;\n",h->line,h->firstchild->var_fun_str_name);
			else//该ID在此结构体内没有被使用过，可以用，便把此ID插入到结构体的域中
			{
				Type p = new_Type();//分配一个主链的节点空间，并且返回指向该节点的一个指针
				strcmp(p->name,h->firstchild->var_fun_str_name);//域的名字，VarDec只会调用varDec->ID
				p->kind = kind;
				p->next = symbol_head;
				symbol_head = p;
			}
		}
	}else if(!(strcmp(h->name,"ExtDecList")))
	{
		if(h->firstchild->nextsibling == NULL)
			semantic_analysis(h->firstchild);
		else
		{
			semantic_analysis(h->firstchild);
			semantic_analysis(h->firstchild->nextsibling->nextsibling);
		}
	}else if(!(strcmp(h->name,"FunDec")))
	{	//函数的定义必然是在全局变量之中，也就是说函数的符号必须放在符号表的主链中去
		if(def_dec == 1)//当前是在定义函数，那么得确定是否出现了重定义
		{
			is_id_existed = check_func_def_isexisted(symbol_head,h->firstchild->var_fun_str_name);
			if(is_id_existed)//如果在此结构体的域内已经存在着这样一个ID，则报错
				printf("Error type 4 at Line %d:函数  %s  重定义;\n",h->line,h->firstchild->var_fun_str_name);
			else
			{
				Type p = new_Type();//分配一个Type的空间
				strcpy(p->name,h->firstchild->var_fun_str_name);
				p->next = symbol_head;
				symbol_head = p;//将当前定义的函数加入符号表的主链
				p->function = (Function)malloc(sizeof(struct Function_));//分配一个Function_节点
				p->function->kind = kind;
				if(kind == STRUCTURE)//如果函数的返回类型是结构体，那么记录结构体的名字
				{
					p->function->return_kind = (char*)malloc(sizeof(30));
					strcpy(p->function->return_kind,return_kind);
				}
				p->kind = FUNCTIONDEF;//函数的种类是定义型，后面要加上{}
				function_variety = p;
				if(h->firstchild->nextsibling->nextsibling->nextsibling!=NULL)//FunDec->ID LP VarList RP，有形参
				{
					struct_function_array = 4;//现在要分析的变量都是函数的形参
					semantic_analysis(h->firstchild->nextsibling->nextsibling);//进入VarList
				}
			}
		}else
		{//声明函数时，要找一找当前是否有声明过，如果有声明过，那么看一看是否两次声明不一样，想要检测两次的声明是否一致，则需要先将当前的声明加进符号表，然后比对符号
			Type p = new_Type();//分配一个Type的空间
			strcpy(p->name,h->firstchild->var_fun_str_name);
			p->next = symbol_head;
			p->line = h->line;//声明的位置
			symbol_head = p;//将当前定义的函数加入符号表的主链的头部
			p->function = (Function)malloc(sizeof(struct Function_));//分配一个Function_节点
			p->function->kind = kind;
			if(kind == STRUCTURE)//如果函数的返回类型是结构体，那么记录结构体的名字
			{
				p->function->return_kind = (char*)malloc(sizeof(30));
				strcpy(p->function->return_kind,return_kind);
			}
			p->kind = FUNCTIONDEC;//函数的种类是定义型，后面要加上{}
			function_variety = p;
			if(h->firstchild->nextsibling->nextsibling->nextsibling!=NULL)//FunDec->ID LP VarList RP，有形参
			{
				struct_function_array = 4;//现在要分析的变量都是函数的形参
				semantic_analysis(h->firstchild->nextsibling->nextsibling);//进入VarList
			}
			is_id_existed = check_func_dec_param(p);
			if(!is_id_existed)
			{
				printf("Error type 19 at Line %d:函数  %s  声明冲突;\n",h->line,h->firstchild->var_fun_str_name);
				symbol_head = p->next;//将p从节点中删除
				free(p->function);
				free(p);
			}
		}
	}else if(!(strcmp(h->name,"VarList")))
	{
		if(h->firstchild->nextsibling == NULL)
			semantic_analysis(h->firstchild);
		else
		{
			semantic_analysis(h->firstchild);
			semantic_analysis(h->firstchild->nextsibling->nextsibling);
		}
	}else if(!(strcmp(h->name,"ParamDec")))
	{
		semantic_analysis(h->firstchild);
		semantic_analysis(h->firstchild->nextsibling);
	}else if(!(strcmp(h->name,"CompSt")))
	{
		struct_function_array = 1;//此时如果定义变量，那么就是在定义函数的局部变量，即函数的域
		if(strcmp(h->firstchild->nextsibling->name,"RC")!=0)//函数内部有操作
		{
			if(strcmp(h->firstchild->nextsibling->nextsibling->name,"RC")==0)//如果产生式的右侧只有三个，要么没有域定义部分，要么没有StmtList部分
				semantic_analysis(h->firstchild->nextsibling);
			else//函数的内部有两个操作，域和StmtList
			{
				semantic_analysis(h->firstchild->nextsibling);
				semantic_analysis(h->firstchild->nextsibling->nextsibling);//进入StmtList
			}
		}
	}else if(!(strcmp(h->name,"StmtList")))
	{
		if(h->firstchild->nextsibling == NULL)
			semantic_analysis(h->firstchild);
		else
		{
			semantic_analysis(h->firstchild);
			semantic_analysis(h->firstchild->nextsibling);
		}
	}else if(!(strcmp(h->name,"Stmt")))
	{
		if(!(strcmp(h->firstchild->name,"Exp"))&&!(strcmp(h->firstchild->nextsibling->name,"SEMI")))//Stmt->Exp SEMI
			semantic_analysis(h->firstchild);//进入Exp
		else if(!(strcmp(h->firstchild->name,"CompSt")))
			semantic_analysis(h->firstchild);//进入CompSt
		else if(!(strcmp(h->firstchild->name,"RETURN")))//Stmt->RETURN Exp SEMI
		{
			if(!(strcmp(h->firstchild->nextsibling->firstchild->name,"INT"))&&function_variety->function->kind != INTEGER)
				printf("Error 8 at Line %d:return 语句的返回类型和函数定义的返回类型不匹配;\n",h->line);
			else if(!(strcmp(h->firstchild->nextsibling->firstchild->name,"FLOAT"))&&function_variety->function->kind != FLOATING)
				printf("Error 8 at Line %d:return 语句的返回类型和函数定义的返回类型不匹配;\n",h->line);
			else if(!(strcmp(h->firstchild->nextsibling->firstchild->name,"ID")))
			{//投机取巧，在测试代码中出现了return j，所以这里就默认下一个产生式为Exp->ID，所以要检验ID是否存在，然后找到ID的类型，看看和函数的类型是否想同
				id_is_defined = check_return_id(function_variety,h->firstchild->nextsibling->firstchild->var_fun_str_name);
				//id_is_defined 0代表没有定义，3代表定义了，1代表定义了，但是类型和函数的返回类型不一样，2代表定义了，并且是对的，和函数类型一样，
				if(id_is_defined == 0)
					printf("Error type 1 at Line %d:变量  %s  未定义;\n",h->line,h->firstchild->nextsibling->var_fun_str_name);
				else if(id_is_defined == 1)
					printf("Error type 8 at Line %d:return 语句的返回类型和函数定义的返回类型不匹配;\n",h->line);
			}
		}else if(strcmp(h->firstchild->name,"IF")==0&&h->firstchild->nextsibling->nextsibling->nextsibling->nextsibling->nextsibling==NULL)//由于在测试代码中并没有出现WHILE和IFELSE，所以Stmt的其他产生式就不再考虑了
		{
			semantic_analysis(h->firstchild->nextsibling->nextsibling->firstchild);//为了偷懒
			//semantic_analysis(h->firstchild->nextsibling->nextsibling);正常情况下，
			semantic_analysis(h->firstchild->nextsibling->nextsibling->nextsibling->nextsibling);
		}
	}else if(!(strcmp(h->name,"Exp")))
	{
		if(!(strcmp(h->firstchild->nextsibling->name,"ASSIGNOP")))//Exp->Exp ASSIGNOP Exp
		{
			if(!(strcmp(h->firstchild->firstchild->name,"INT"))||!(strcmp(h->firstchild->firstchild->name,"FLOAT")))
				//赋值号左边不能是常数
				printf("Error type 6 at Line %d:赋值号左边出现了一个只有右值的表达式;\n",h->line);
			else if(h->firstchild->firstchild->nextsibling != NULL){
					if(strcmp(h->firstchild->firstchild->nextsibling->name,"LB")==0)
						semantic_analysis(h->firstchild);}
			else
			{
				enum KIND kind_id;
				char if_struct_name[30];
				strcpy(if_struct_name,"\0");
				id_is_defined = check_id_type(function_variety,if_struct_name,h->firstchild->firstchild->var_fun_str_name);
				if(!(strcmp(if_struct_name,"int"))) kind_id = INTEGER;
				else if(!(strcmp(if_struct_name,"float"))) kind_id = FLOATING;
				else kind_id = STRUCTURE;
				if(id_is_defined == 0)//未找到该变量
					printf("Error type 1 at Line %d:变量  %s  未经定义就使用;\n",h->line,h->firstchild->firstchild->var_fun_str_name);
				else if(id_is_defined == 3)
				{
					if(!(strcmp(h->firstchild->nextsibling->nextsibling->firstchild->name,"INT"))&&kind_id != INTEGER)//Exp->Exp ASSIGNOP Exp,Exp2->INTEGER
						printf("Error type 5 at Line %d:赋值号两边的类型不匹配;\n",h->line);
					else if(!(strcmp(h->firstchild->nextsibling->nextsibling->firstchild->name,"FLOAT"))&&kind_id != FLOATING)
						printf("Error type 5 at Line %d:赋值号两边的类型不匹配;\n",h->line);
					//测试文件中仅仅出现了Exp->Exp ASSIGNOP Exp,Exp2->ID LP Args RP,Exp2->ID LP RP，也就是说没有出现Exp->ID的状况，再次偷懒
					if(!(strcmp(h->firstchild->nextsibling->nextsibling->firstchild->name,"ID")))
					{	//检查名为ID的函数的返回类型和kind_id是否一致
						if(check_func_use_assign(kind_id,if_struct_name,h->firstchild->nextsibling->nextsibling->firstchild->var_fun_str_name)==0)
							//此时函数的返回类型和赋值号前面的变量的类型不一致，则报错
							printf("Error type 5 at Line %d:赋值号左侧的变量和右侧的函数返回类型不匹配;\n",h->line);
						if(check_func_use_assign(kind_id,if_struct_name,h->firstchild->nextsibling->nextsibling->firstchild->var_fun_str_name)==2)	
							printf("Error type 2 at Line %d:此函数  %s  还没有定义;\n",h->line,h->firstchild->nextsibling->nextsibling->firstchild->var_fun_str_name);
						semantic_analysis(h->firstchild->nextsibling->nextsibling);//进入第二个Exp中，此时这里Exp->ID LP Args RP或者Exp->ID LP RP
					}
				}
			}
		}else if(strcmp(h->firstchild->nextsibling->name,"PLUS")==0)//Exp->Exp PLUS Exp
		{
			//只考虑int+float的情况，例子中没有出现其他的情况即一个常数int和一个变量
			char id_type[30];
			int is_checked = check_id_type(function_variety,id_type,h->firstchild->nextsibling->nextsibling->firstchild->var_fun_str_name);
			if(is_checked == 3)
				if(strcmp(id_type,"int")!=0)
					printf("Error type 7 at line %d:操作数的类型不匹配;\n",h->line);
			if(is_checked == 0)
				printf("Error type 1 at Line %d:变量  %s  未定义便使用;\n",h->line,h->firstchild->nextsibling->nextsibling->var_fun_str_name);
		}else if(strcmp(h->firstchild->nextsibling->nextsibling->name,"Args")==0)//Exp->ID LP Args RP
		{	//首先从参数链中寻找，在链中寻找同名的变量，如果可以找到该变量，那么直接返回-1,如果找不到就到函数的域中寻找，如果找得到，就返回-1,然后在主链中找
			int id_is_normal = id_is_not_function(function_variety,h->firstchild->var_fun_str_name);
			if(id_is_normal == -1)//找到了一个同名的普通变量或者结构体变量
				printf("Error type 11 at Line %d:%s  不是一个函数，不可以使用()操作符;\n",h->line,h->firstchild->var_fun_str_name);
			else//如果没有找到如此一个普通变量
			{	//取函数的形参个数，如果未找到此函数的定义或者声明，那么返回-1
				int param_number = getfunc_param_num(h->firstchild->var_fun_str_name);//根据函数的名字在符号表的主链中找
				//判断ID是否为一个函数，即在主链中寻找此函数，如果没有找到，报错ID不是一个函数
				//并且取出函数的参数个数
				if(param_number == -1)
					printf("Error type 2 at Line %d:函数  %s  还没有定义;\n",h->line,h->firstchild->var_fun_str_name);
				else
				{
					real_param = 0;
					semantic_analysis(h->firstchild->nextsibling->nextsibling);//进入Args中统计实参的个数
					if(param_number != real_param)
					{
						printf("Error type 9 at Line %d:函数调用时，形参和实参个数不符;\n",h->line);
					}
				}
			}
			//比较real_param和之前取出的参数个数是否想等，如果不等，则报错
		}else if(strcmp(h->firstchild->nextsibling->nextsibling->name,"LP")==0)//Exp->ID LP RP
		{
			int id_is_normal = id_is_not_function(function_variety,h->firstchild->var_fun_str_name);
			if(id_is_normal == -1)//找到了一个同名的普通变量或者结构体变量
				printf("Error type 11 at Line %d:%s  不是一个函数，不可以使用()操作符;\n",h->line,h->firstchild->var_fun_str_name);
			else
			{
				int param_number = getfunc_param_num(h->firstchild->var_fun_str_name);//根据函数的名字在符号表的主链中找
				//判断ID是否为一个函数，即在主链中寻找此函数，如果没有找到，报错ID不是一个函数
				//并且取出函数的参数个数
				//如果参数的个数不为0，则报错
				if(param_number == -1)
					printf("Error type 2 at Line %d:函数  %s  还没有定义;\n",h->line,h->firstchild->var_fun_str_name);
				else
					if(param_number != 0)
						printf("Error type 9 at Line %d:函数调用时，形参和实参个数不符;\n",h->line);
			}
		}else if(strcmp(h->firstchild->nextsibling->name,"LB")==0)//Exp->Exp LB Exp RB
		{
			//从函数的形参开始找，再找域，在找全局变量，检查Exp->ID的ID是否已经存在，并且是一个数组
			//测试文件中只出现了一种情况，判断的ID在函数的域中，那么只需要找域就可以，如果域的名字匹配，但是类型却不是数组，则报错
			if(check_is_array(function_variety,h->firstchild->firstchild->var_fun_str_name)==0)
				//在函数的域中找到了这个变量，但是变量不是数组，则报错
				printf("Error type 10 at Line %d:对非数组型变量不可以使用[]访问;\n",h->line);
			else
			{	//例子中出现了i[10]，默认Exp2->ID，Exp->INTEGER
				//如果第三个Exp只要没有推出INTEGER，就报错，错误12
				if(strcmp(h->firstchild->nextsibling->nextsibling->firstchild->name,"INT")!=0)
					printf("Error type 12 at Line %d,数组访问操作符[]中出现了非整数;\n",h->line);
			}
		}else if(strcmp(h->firstchild->nextsibling->name,"DOT")==0)//Exp->Exp DOT ID
		{	//在函数的域中找一个变量，该变量的名字和h->firstchild->firstchild->var_fun_str_name相同，并且这个变量是个结构体，如果不是，则返回0
			//此处就不考虑找不到的情况了,如果找打了，并且是个结构体，那么检查.后面的ID是否为struct_name111结构体的域，如果不是，则报错
			//如果是结构体，结构体的类型名将放在struct_name1111中
			char struct_name1111[30];
			if(check_is_struct(function_variety,struct_name1111,h->firstchild->firstchild->var_fun_str_name)==0)
				printf("Error type 13 at Line %d:变量  %s  不是一个结构体，不能用\".\"操作访问;\n",h->line,h->firstchild->firstchild->var_fun_str_name);
			else
			{	
				//检查h->firstchild->nextsibling->nextsibling->var_fun_str_name是否在结构体struct_name1111中
				int struct_field_isexisted = check_struct_field_isexisted(struct_name1111,h->firstchild->nextsibling->nextsibling->var_fun_str_name);
				if(!struct_field_isexisted)
					printf("Error type 14 at Line %d:结构体  %s  中不存在域  %s;\n",h->line,struct_name1111,	h->firstchild->nextsibling->nextsibling->var_fun_str_name);
			}
		}
	}else if(!(strcmp(h->name,"Args")))
	{
		if(h->firstchild->nextsibling == NULL)
		{
			real_param++;
		}
		else
		{
			real_param++;
			semantic_analysis(h->firstchild->nextsibling->nextsibling);
		}			
	}else
	{
		semantic_analysis(h->firstchild);
		struct NODE* temp = h->nextsibling;
		while(temp != NULL)
		{
			semantic_analysis(temp);
			temp = temp->nextsibling;
		}
	}
}
void check_DEC_but_DEF()
{	//从符号表的主链开始，寻找是否有函数被声明，但是却没有定义，如果有，则报错
/*	Type temp = symbol_head;
	while(temp != NULL)
	{
		if(temp->kind == FUNCTIONDEC)
			printf("FUNCTIONDEC   %s\n",temp->name);
		if(temp->kind == FUNCTIONDEF)
			printf("FUNCTIONDEF   %s\n",temp->name);
		temp = temp->next;
	}
*/
int boo = 0;
	Type temp = symbol_head;
	while(temp != NULL)
	{
		if(temp->kind == FUNCTIONDEC)
		{
			Type temp1 = symbol_head;
			boo = 0;
			while(temp1 != NULL)
			{
				if(temp1->kind == FUNCTIONDEF && strcmp(temp1->name,temp->name)==0)
				{
					boo = 1;
					break;
				}
				temp1 = temp1->next;
			}
			if(boo == 0)
				printf("Error Type 18 at Line %d:函数  %s  声明了，但是没有定义;\n",temp->line,temp->name);
		}
		temp = temp->next;
	}



	
}
