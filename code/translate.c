static int temp_count = 1;//当前临时变量的个数
static int label_count = 1;//当前label的个数
char* itoa(int num,char*str,int radix)
{	/*索引表*/
	char index[]="0123456789ABCDEF";
	unsigned unum;/*中间变量*/
	int i=0,j,k;
	/*确定unum的值*/
	if(radix==10&&num<0)/*十进制负数*/
	{
		unum=(unsigned)-num;
		str[i++]='-';
	}
	else unum=(unsigned)num;/*其他情况*/
	/*转换*/
	do{
		str[i++]=index[unum%(unsigned)radix];
		unum/=radix;
	}while(unum);
	str[i]='\0';
	/*逆序*/
	if(str[0]=='-')	k=1;/*十进制负数*/
	else	k=0;
	char temp;
	for(j=k;j<=(i-1)/2;j++)
	{
		temp=str[j];
		str[j]=str[i-1+k-j];
		str[i-1+k-j]=temp;
	}
	return str;
}
//生成新的临时变量，名称放在提供好的name里面。
void new_temp(char* name)
{
	strcpy(name,"t");
	char temp[20];
	itoa(temp_count,temp,10);
	temp_count++;
	strcat(name,temp);
}
//生成新的label，名称放在提供好的name里面。
void new_label(char* name)
{
	strcpy(name,"label");
	char temp[20];
	itoa(label_count,temp,10);
	label_count++;
	strcat(name,temp);
}
//中间代码的结构体，双向链表
struct code_node
{
	int args_count;				//有多少个词。具体见上面注释内的分类。
	char args[6][30];			//每个词都是什么
	struct code_node* prev,* next;//双向指针
};
struct code_node head;//中间代码链表的头部，是一个实体
struct code_node* tail = &head;//中间代码链表的尾部，刚开始的时候指向头部
//往链表中插入一个代码，参数的个数是不确定的
void add_code(int args_count,...)
{
	struct code_node* temp=(struct code_node*)malloc(sizeof(struct code_node));
	temp->args_count=args_count;
	va_list ap;
	va_start(ap,args_count);
	for(int i=0;i<args_count;i++)
	{
		strcpy(temp->args[i],va_arg(ap,char*));
	//	printf("%s ",temp->args[i]);
	}
	//printf("\n");
	tail->next = temp;
	temp->prev = tail;
	tail = temp;//将新节点连上去
}
void translate_Exp(struct NODE* node,char* place);
void translate_Stmt(struct NODE* node);
void translate_Cond(struct NODE* node,char* label_true,char* label_false);
int translate_Args(struct NODE* node,char arg_list[][30],int Args_number);
void translate_CompSt(struct NODE* node);
void translate_StmtList(struct NODE* node);
void translate_DefList(struct NODE* node);
void translate_Def(struct NODE* node);
void translate_DecList(struct NODE* node);
void translate_Dec(struct NODE* node);
void translate_CompSt(struct NODE* node)
{
	if(node == NULL) return;
	if(strcmp(node->firstchild->nextsibling->name,"StmtList") == 0)
		translate_StmtList(node->firstchild->nextsibling);
	else
	{
		translate_DefList(node->firstchild->nextsibling);
		translate_StmtList(node->firstchild->nextsibling->nextsibling);
	}
}
void translate_DefList(struct NODE* node)
{
	translate_Def(node->firstchild);
	if(node->firstchild->nextsibling != NULL)
		translate_DefList(node->firstchild->nextsibling);
}
void translate_Def(struct NODE* node)
{
	translate_DecList(node->firstchild->nextsibling);
}
void translate_DecList(struct NODE* node)
{
	translate_Dec(node->firstchild);
	if(node->firstchild->nextsibling != NULL)
		translate_DecList(node->firstchild->nextsibling->nextsibling);
}
void translate_Dec(struct NODE* node)
{
	if(strcmp(node->firstchild->firstchild->name,"ID") == 0)
	{
		FieldList temp=find_fieldList(node->firstchild->firstchild->var_fun_str_name);
		if(temp != NULL && temp->kind == STRUCTURE)
		{
			char length[10];
			itoa(struct_get_size(temp->struct_name),length,10);
			add_code(3,"DEC",temp->name,length);
		}
	}
	if(strcmp(node->firstchild->firstchild->name,"ID") == 0 && node->firstchild->nextsibling != NULL)
	{
			char length[10];
			new_temp(length);
			translate_Exp(node->firstchild->nextsibling->nextsibling,length);
			add_code(3,node->firstchild->firstchild->var_fun_str_name,":=",length);
	}
}
void translate_StmtList(struct NODE* node)
{
	if(node == NULL) return;
	translate_Stmt(node->firstchild);
	translate_StmtList(node->firstchild->nextsibling);
}
void translate_Exp(struct NODE* node,char* place)
{
	if(node == NULL) return;
	if(strcmp(node->firstchild->name,"LP") == 0 && strcmp(node->firstchild->nextsibling->name,"Exp") == 0 && strcmp(node->firstchild->nextsibling->nextsibling->name,"RP") == 0)
	{
		translate_Exp(node->firstchild->nextsibling,place);
	}
	else if(strcmp(node->firstchild->name,"INT") == 0||strcmp(node->firstchild->name,"FLOAT") == 0)//Exp->INT||Exp->FLOAT
	{
		char temp[30];
		sprintf(temp,"#%s",node->firstchild->var_fun_str_name);//在常数的前面加上一个#，然后赋给temp
		add_code(3,place,":=",temp);//三个参数，place，：=，temp
	}
	else if(strcmp(node->firstchild->name,"ID") == 0 && node->firstchild->nextsibling == NULL)//Exp->ID
	{
		add_code(3,place,":=",node->firstchild->var_fun_str_name);
	}
	else if(strcmp(node->firstchild->name,"ID") == 0 && strcmp(node->firstchild->nextsibling->name,"LP") == 0 && strcmp(node->firstchild->nextsibling->nextsibling->name,"RP") == 0)
	{
		char function[30];
		strcpy(function,node->firstchild->var_fun_str_name);
		if(strcmp(function,"read") == 0) add_code(2,"READ",place);
		else add_code(4,place,":=","CALL",function);
	}
	else if(strcmp(node->firstchild->name,"ID") == 0 && strcmp(node->firstchild->nextsibling->name,"LP") == 0 && strcmp(node->firstchild->nextsibling->nextsibling->name,"Args") == 0 && strcmp(node->firstchild->nextsibling->nextsibling->nextsibling->name,"RP") == 0)
	{
		char function[30];
		strcpy(function,node->firstchild->var_fun_str_name);
		char arg_list[10][30];//实参的列表
		int Args_number = 0;//实参的个数
		Args_number = translate_Args(node->firstchild->nextsibling->nextsibling,arg_list,Args_number);//在translate_Args函数中统计实参的个数
		if(strcmp(function,"write") == 0)
			add_code(2,"WRITE",arg_list[0]);
		else
		{
			for(int i = 0;i < Args_number;i++)
				add_code(2,"ARG",arg_list[i]);
			add_code(4,place,":=","CALL",function);
		}
	}
	else if(strcmp(node->firstchild->name,"Exp") == 0 && strcmp(node->firstchild->nextsibling->name,"ASSIGNOP") == 0 && strcmp(node->firstchild->nextsibling->nextsibling->name,"Exp") == 0)//Exp->Exp ASSIGNOP Exp
	{
		if(strcmp(node->firstchild->firstchild->name,"ID") == 0)
		{
			char variable[30];
			strcpy(variable,node->firstchild->firstchild->var_fun_str_name);
			char t1[30];
			new_temp(t1);
			translate_Exp(node->firstchild->nextsibling->nextsibling,t1);
			add_code(3,variable,":=",t1);
			if(place == NULL)
			{
				place = (char*)malloc(sizeof(30));
				new_temp(place);
			}
			add_code(3,place,":=",variable);
		}
		else if(strcmp(node->firstchild->firstchild->nextsibling->name,"DOT") == 0)
		{
			char t1[30];
			new_temp(t1);
			translate_Exp(node->firstchild,t1);
			char t2[30];
			new_temp(t2);
			translate_Exp(node->firstchild->nextsibling->nextsibling,t2);
			add_code(3,t1,":=",t2);
		}
	}
	else if((strcmp(node->firstchild->nextsibling->name,"DIV") == 0 || strcmp(node->firstchild->nextsibling->name,"PLUS") == 0 || strcmp(node->firstchild->nextsibling->name,"STAR") == 0 || strcmp(node->firstchild->nextsibling->name,"MINUS") == 0) && node->firstchild->nextsibling->nextsibling->nextsibling == NULL)
	{
		char t1[30];
		new_temp(t1);
		char t2[30];
		new_temp(t2);
		translate_Exp(node->firstchild,t1);
		translate_Exp(node->firstchild->nextsibling->nextsibling,t2);
		if(strcmp(node->firstchild->nextsibling->name,"PLUS") == 0)
			add_code(5,place,":=",t1,"+",t2);
		if(strcmp(node->firstchild->nextsibling->name,"STAR") == 0)
			add_code(5,place,":=",t1,"*",t2);
		if(strcmp(node->firstchild->nextsibling->name,"MINUS") == 0)
			add_code(5,place,":=",t1,"-",t2);
		if(strcmp(node->firstchild->nextsibling->name,"DIV") == 0)
			add_code(5,place,":=",t1,"/",t2);
	}
	else if(strcmp(node->firstchild->name,"MINUS") == 0 && strcmp(node->firstchild->nextsibling->name,"Exp") == 0 && node->firstchild->nextsibling->nextsibling == NULL)
	{
		char t1[30];
		new_temp(t1);
		translate_Exp(node->firstchild->nextsibling,t1);
		add_code(5,place,":=","#0","-",t1);
	}
	else if(strcmp(node->firstchild->nextsibling->name,"RELOP") == 0 || strcmp(node->firstchild->name,"NOT") == 0 || strcmp(node->firstchild->nextsibling->name,"AND") == 0 || strcmp(node->firstchild->nextsibling->name,"OR") == 0)
	{
		char label1[30];
		new_label(label1);
		char label2[30];
		new_label(label2);
		add_code(3,place,":=","#0");
		translate_Cond(node,label1,label2);
		add_code(3,"LABEL",label1,":");
		add_code(3,place,":=","#1");
		add_code(3,"LABEL",label2,":");
	}
	else if(strcmp(node->firstchild->name,"Exp") == 0 && strcmp(node->firstchild->nextsibling->name,"DOT") == 0 && strcmp(node->firstchild->nextsibling->nextsibling->name,"ID") == 0)//Exp->Exp DOT ID
	{	//结构体的使用
		//要先检查node->firstchild->firstchild->var_fun_str_name是域中的还是形参中，如果是形参中的，则需要用指针
		//利用所有变量均不重名
		int pafi = param_field(node->firstchild->firstchild->var_fun_str_name);
		if(pafi == 0)//在形参中
		{
			char address[30];
			sprintf(address,"*%s",node->firstchild->firstchild->var_fun_str_name);
			char t2[30];
			new_temp(t2);
			add_code(3,t2,":=",address);
			char current_struct[30];
			strcpy(current_struct,find_fieldList(node->firstchild->firstchild->var_fun_str_name)->struct_name);
			int offseti=struct_get_offset(current_struct,node->firstchild->nextsibling->nextsibling->var_fun_str_name);
			char offset[30];
			if(offseti != 0)
			{	
				char temp[30];
				new_temp(temp);
				sprintf(offset,"#%d",offseti);
				add_code(5,temp,":=",address,"+",offset);
				char place_temp[30];
				sprintf(place_temp,"*%s",temp);
				strcpy(place,place_temp);
			}
			else
			{
				char place_temp[30];
				sprintf(place_temp,"*%s",t2);
				strcpy(place,place_temp);
			}
		}
		if(pafi == 1)//在域中
		{
			char address[30];
			sprintf(address,"&%s",node->firstchild->firstchild->var_fun_str_name);
			char t2[30];
			new_temp(t2);
			add_code(3,t2,":=",address);
			//对地址做偏移处理
			char current_struct[30];
			strcpy(current_struct,find_fieldList(node->firstchild->firstchild->var_fun_str_name)->struct_name);
			int offseti=struct_get_offset(current_struct,node->firstchild->nextsibling->nextsibling->var_fun_str_name);
			char offset[30];
			if(offseti != 0)
			{
				
				char temp[30];
				new_temp(temp);
				sprintf(offset,"#%d",offseti);
				add_code(5,temp,":=",address,"+",offset);
				char place_temp[30];
				sprintf(place_temp,"*%s",temp);
				strcpy(place,place_temp);
			}
			else
			{
				char place_temp[30];
				sprintf(place_temp,"*%s",t2);
				strcpy(place,place_temp);
			}
		}
	}
}
void translate_Stmt(struct NODE* node)
{
	if(node == NULL) return;
	if(strcmp(node->firstchild->name,"Exp") == 0 && strcmp(node->firstchild->nextsibling->name,"SEMI") == 0)//Stmt->Exp SEMI
	{
		translate_Exp(node->firstchild,NULL);
	}
	else if(strcmp(node->firstchild->name,"CompSt") == 0 && node->firstchild->nextsibling == NULL)//Stmt->CompSt
	{
		translate_CompSt(node->firstchild);
	}
	else if(strcmp(node->firstchild->name,"RETURN") == 0 && node->firstchild->nextsibling != NULL)//Stmt->RETURN Exp SEMI
	{
		char t1[30];
		new_temp(t1);
		translate_Exp(node->firstchild->nextsibling,t1);
		add_code(2,"RETURN",t1);
	}
	else if(strcmp(node->firstchild->name,"IF") == 0 && node->firstchild->nextsibling->nextsibling->nextsibling->nextsibling->nextsibling == NULL)//Stmt->IF LP Exp RP Stmt
	{
		char label1[30];
		new_label(label1);
		char label2[30];
		new_label(label2);
		translate_Cond(node->firstchild->nextsibling->nextsibling,label1,label2);
		add_code(3,"LABEL",label1,":");
		translate_Stmt(node->firstchild->nextsibling->nextsibling->nextsibling->nextsibling);
		add_code(3,"LABEL",label2,":");
	}
	else if(strcmp(node->firstchild->name,"IF") == 0 && node->firstchild->nextsibling->nextsibling->nextsibling->nextsibling->nextsibling != NULL)//Stmt->IF LP Exp RP Stmt ELSE Stmt
	{
		char label1[30];
		new_label(label1);
		char label2[30];
		new_label(label2);
		char label3[30];
		new_label(label3);
		translate_Cond(node->firstchild->nextsibling->nextsibling,label1,label2);
		add_code(3,"LABEL",label1,":");
		translate_Stmt(node->firstchild->nextsibling->nextsibling->nextsibling->nextsibling);
		add_code(2,"GOTO",label3);
		add_code(3,"LABEL",label2,":");
		translate_Stmt(node->firstchild->nextsibling->nextsibling->nextsibling->nextsibling->nextsibling->nextsibling);
		add_code(3,"LABEL",label3,":");
	}
	else if(strcmp(node->firstchild->name,"WHILE") == 0)
	{
		char label1[30];
		new_label(label1);
		char label2[30];
		new_label(label2);
		char label3[30];
		new_label(label3);
		add_code(3,"LABEL",label1,":");
		translate_Cond(node->firstchild->nextsibling->nextsibling,label2,label3);
		add_code(3,"LABEL",label2,":");
		translate_Stmt(node->firstchild->nextsibling->nextsibling->nextsibling->nextsibling);
		add_code(2,"GOTO",label1);
		add_code(3,"LABEL",label3,":");
	}
}
void translate_Cond(struct NODE* node,char* label_true,char* label_false)
{
	if(strcmp(node->firstchild->name,"Exp") == 0 && strcmp(node->firstchild->nextsibling->name,"RELOP") == 0 && strcmp(node->firstchild->nextsibling->nextsibling->name,"Exp") == 0 && node->firstchild->nextsibling->nextsibling->nextsibling == NULL)
	{
		char t1[30];
		new_temp(t1);
		char t2[30];
		new_temp(t2);
		translate_Exp(node->firstchild,t1);
		translate_Exp(node->firstchild->nextsibling->nextsibling,t2);
		char op[10];
		strcpy(op,node->firstchild->nextsibling->var_fun_str_name);
		add_code(6,"IF",t1,op,t2,"GOTO",label_true);
		add_code(2,"GOTO",label_false);
	}
	else if(strcmp(node->firstchild->name,"NOT") == 0 && strcmp(node->firstchild->nextsibling->name,"Exp") == 0 && node->firstchild->nextsibling->nextsibling == NULL)
	{
		translate_Cond(node->firstchild->nextsibling,label_true,label_false);
	}
	else if(strcmp(node->firstchild->nextsibling->name,"AND") == 0)
	{
		char label1[30];
		new_label(label1);
		translate_Cond(node->firstchild,label1,label_false);
		add_code(3,"LABEL",label1,":");
		translate_Cond(node->firstchild->nextsibling->nextsibling,label_true,label_false);
	}
	else if(strcmp(node->firstchild->nextsibling->name,"OR") == 0)
	{
		char label1[30];
		new_label(label1);
		translate_Cond(node->firstchild,label_true,label1);
		add_code(3,"LABEL",label1,":");
		translate_Cond(node->firstchild->nextsibling->nextsibling,label_true,label_false);
	}
	else
	{
		char t1[30];
		new_temp(t1);
		translate_Exp(node,t1);
		add_code(6,"IF",t1,"!=","#0","GOTO",label_true);
		add_code(2,"GOTO",label_false);
	}
}
int translate_Args(struct NODE* node,char arg_list[][30],int Args_number)
{
	if(strcmp(node->firstchild->name,"Exp") == 0 && node->firstchild->nextsibling == NULL)
	{
		if(strcmp(node->firstchild->firstchild->name,"ID") == 0 && node->firstchild->firstchild->nextsibling == NULL && find_fieldList(node->firstchild->firstchild->var_fun_str_name)->kind == STRUCTURE)
		{
			char temp[30];
			sprintf(temp,"&%s",node->firstchild->firstchild->var_fun_str_name);
			strcpy(arg_list[Args_number],temp);
		}
		else
		{
			char t1[30];
			new_temp(t1);
			translate_Exp(node->firstchild,t1);
			strcpy(arg_list[Args_number],t1);
		}
		return ++Args_number;
	}
	if(strcmp(node->firstchild->name,"Exp") == 0 && strcmp(node->firstchild->nextsibling->name,"COMMA") == 0 && strcmp(node->firstchild->nextsibling->nextsibling->name,"Args") == 0)
	{
		char t1[30];
		new_temp(t1);
		translate_Exp(node->firstchild,t1);
		strcpy(arg_list[Args_number],t1);
		return translate_Args(node->firstchild->nextsibling->nextsibling,arg_list,Args_number+1);
	}
}
void translate(struct NODE* node)
{
	if(node == NULL) return;
	if(strcmp(node->name,"FunDec") == 0)
	{
		add_code(3,"FUNCTION",node->firstchild->var_fun_str_name,":");
		Type temp = symbol_head;
		while(temp != NULL)
		{
			if(temp->kind == FUNCTIONDEF && strcmp(temp->name,node->firstchild->var_fun_str_name) == 0)
			{
				FieldList temp_para = temp->function->param;
				while(temp_para != NULL)
				{
					add_code(2,"PARAM",temp_para->name);
					temp_para = temp_para->next;
				}
				break;
			}
			temp = temp->next;
		}
	}
	else if(strcmp(node->name,"Stmt") == 0)
	{
		translate_Stmt(node);
	}
	else if(strcmp(node->name,"CompSt") == 0)
	{
		translate_CompSt(node);
	}
	else
	{
		translate(node->firstchild);
		struct NODE* temp = node->nextsibling;
		while(temp != NULL)
		{
			translate(temp);
			temp = temp->nextsibling;
		}
	}
}
void print_one_line(FILE* fp,struct code_node* p)//打印一行代码
{
	if(p->args_count!=3 && p->args[0][0]=='*')
	{
		char temp[30];
		new_temp(temp);
		fprintf(fp,"%s",temp);
		for(int i=1;i<p->args_count;i++)
			fprintf(fp," %s",p->args[i]);
		fprintf(fp,"\n");
		fprintf(fp,"%s := %s\n",p->args[0],temp);
	}
	else if(p->args_count==2 && p->args[1][0]=='*' && strcmp(p->args[0],"READ") == 0)
	{
		char temp[30];
		new_temp(temp);
		fprintf(fp,"READ %s\n",&temp[1]);
		fprintf(fp,"%s := %s\n",p->args[1],&temp[1]);
	}
	else
	{
		fprintf(fp,"%s",p->args[0]);
		for(int i=1;i<p->args_count;i++)
			fprintf(fp," %s",p->args[i]);
		fprintf(fp,"\n");
	}
}
void print_code(char* name)//将内存中的代码打印到文件中，传入新文件路径
{
	if(head.next == NULL)return;
	FILE* fp;
	fp=fopen(name,"w");
	struct code_node* p = head.next;
	do
	{
		print_one_line(fp,p);
		p=p->next;
	}while(p != tail);
	fclose(fp);
}
