typedef struct var_location
{
	char name[30];
	int location;
}var;
FILE* fprint;
static int arg_count = 0;
static int size = 0;
static int count = 0;
static var table[1000];
int var_get(char* name)
{
	if(name[0] == '#')return -1;
	if(name[0] == '*'||name[0] == '&')name++;
	for(int i = 0;i<count;i++)
		if(strcmp(name,table[i].name) == 0) return table[i].location;
	return -1;
}
void var_add(char* name,int sz)
{
	if(name[0] == '#')return;
	if(name[0] == '*'||name[0] == '&')name++;
	if(var_get(name)!= -1)return;
	table[count].location = size;
	size += sz;
	strcpy(table[count].name,name);
	count++;
}
void prep_register(char* name,int num)
{
	char temp[8];
	sprintf(temp,"$t%d",num);
	if(name[0] == '*')
	{
		 fprintf(fprint,"lw %s, %d($sp)\n",temp,var_get(name));
		 fprintf(fprint,"add %s, %s,$sp\n",temp,temp);
		 fprintf(fprint,"lw %s, 0(%s)\n",temp,temp);
	}
	else if(name[0] == '&'){fprintf(fprint,"li %s, %d\n",temp,var_get(name));}
	else if(name[0] == '#'){fprintf(fprint,"li %s, %s\n",temp,&name[1]);}
	else{fprintf(fprint,"lw %s, %d($sp)\n",temp,var_get(name));}
}
void function_tra(struct code_node* begin,struct code_node* end)
{
	count = 0;
	size = 0;
	arg_count = 0;
	fprintf(fprint,"%s:\n",begin->args[1]);
	struct code_node* p = begin->next;
	while(p != end)
	{
		switch(p->args_count)
		{
			case 2:
				if(strcmp("GOTO",p->args[0]) != 0) var_add(p->args[1],4);
				break;
			case 3:
				if(strcmp(p->args[1],":=") == 0)
				{
					var_add(p->args[0],4);
					var_add(p->args[2],4);
				}
				else if(strcmp(p->args[0],"DEC") == 0)
				{
					int a = strtol(p->args[2],NULL,10);
					var_add(p->args[1],a);
				}
				break;
			case 4:
				var_add(p->args[0],4);
				break;
			case 5:
				var_add(p->args[0],4);
				var_add(p->args[2],4);
				var_add(p->args[4],4);
				break;
			case 6:
				var_add(p->args[1],4);
				var_add(p->args[3],4);
				break;
			default:;
		}
		p = p->next;
	}
	fprintf(fprint,"addi $sp, $sp, -%d\n",size);
	p = begin->next;
	while(p != end)
	{
		switch(p->args_count)
		{
			case 2:
				if(strcmp(p->args[0],"GOTO") == 0){fprintf(fprint,"j %s\n",p->args[1]);}
				else if(strcmp(p->args[0],"RETURN") == 0)
				{
					prep_register(p->args[1],0);
					fprintf(fprint,"move $v0, $t0\n");
					fprintf(fprint,"addi $sp, $sp, %d\n",size);
					fprintf(fprint,"jr $ra\n");
				}
				else if(strcmp(p->args[0],"ARG") == 0)
				{
					prep_register(p->args[1],0);
					fprintf(fprint,"move $a%d, $t0\n",arg_count);
					arg_count++;
				}
				else if(strcmp(p->args[0],"PARAM") == 0)
				{
					int para_count = 0;
					struct code_node* q = p;
					while(strcmp(q->next->args[0],"PARAM") == 0)
					{
						q = q->next;
						para_count++;
					}
					 fprintf(fprint,"sw $a%d, %d($sp)\n",para_count,var_get(p->args[1]));
				}
				else if(strcmp(p->args[0],"READ") == 0)
				{
					fprintf(fprint,"addi $sp, $sp, -4\n");
					fprintf(fprint,"sw $ra, 0($sp)\n");
					fprintf(fprint,"jal read\n");
					fprintf(fprint,"lw $ra, 0($sp)\n");
					fprintf(fprint,"addi $sp, $sp, 4\n");
					if(p->args[1][0] == '*')
					{
						fprintf(fprint,"lw $t0, %d($sp)\n",var_get(p->args[1]));
						fprintf(fprint,"add $t0, $t0, $sp\n");
						fprintf(fprint,"sw $v0, 0($t0)\n");
					}
					else fprintf(fprint,"sw $v0, %d($sp)\n",var_get(p->args[1]));
				}
				else if(strcmp(p->args[0],"WRITE") == 0)
				{
					prep_register(p->args[1],0);
					fprintf(fprint,"move $a0, $t0\n");
					fprintf(fprint,"addi $sp, $sp, -4\n");
					fprintf(fprint,"sw $ra, 0($sp)\n");
					fprintf(fprint,"jal write\n");
					fprintf(fprint,"lw $ra, 0($sp)\n");
					fprintf(fprint,"addi $sp, $sp, 4\n");
				}
				break;
			case 3:
				if(strcmp(p->args[1],":=") == 0)
				{
					prep_register(p->args[2],0);
					if(p->args[0][0] == '*')
					{
						fprintf(fprint,"lw $t1, %d($sp)\n",var_get(p->args[0]));
						fprintf(fprint,"add $t1, $t1, $sp\n");
						fprintf(fprint,"sw $t0, 0($t1)\n");
					}
					else{fprintf(fprint,"sw $t0, %d($sp)\n",var_get(p->args[0]));}
				}
				else if(strcmp(p->args[0],"DEC") != 0){fprintf(fprint,"%s:\n",p->args[1]);}
				break;
			case 4:
				arg_count = 0;
				fprintf(fprint,"addi $sp, $sp, -4\n");
				fprintf(fprint,"sw $ra, 0($sp)\n");
				fprintf(fprint,"jal %s\n",p->args[3]);
				fprintf(fprint,"lw $ra, 0($sp)\n");
				fprintf(fprint,"addi $sp, $sp, 4\n");
				if(p->args[0][0] == '*')
				{
					fprintf(fprint,"lw $t0, %d($sp)\n",var_get(p->args[0]));
					fprintf(fprint,"add $t0, $t0 ,$sp\n");
					fprintf(fprint,"sw $v0, 0($t0)\n");
				}
				else fprintf(fprint,"sw $v0, %d($sp)\n",var_get(p->args[0]));
				break;
			case 5:
				prep_register(p->args[2],0);
				prep_register(p->args[4],1);
				switch(p->args[3][0])
				{
					case '+':
						fprintf(fprint,"add $t0, $t0, $t1\n");
						break;
					case '-':
						fprintf(fprint,"sub $t0, $t0, $t1\n");
						break;
					case '*':
						fprintf(fprint,"mul $t0, $t0, $t1\n");
						break;
					case '/':
						fprintf(fprint,"div $t0, $t1\n");
						fprintf(fprint,"mflo $t0\n");
						break;
					default:;
				}
				if(p->args[0][0] == '*')
				{
					 fprintf(fprint,"lw $t1, %d($sp)\n",var_get(p->args[0]));
					 fprintf(fprint,"add $t1, $t1 ,$sp\n");
					 fprintf(fprint,"sw $t0, 0($t1)\n");
				}
				else fprintf(fprint,"sw $t0, %d($sp)\n",var_get(p->args[0]));
				break;
			case 6:
			{
				char temp[4];
				if(strcmp(p->args[2],"==") == 0)
					strcpy(temp,"beq");
				else if(strcmp(p->args[2],"!=") == 0)
					strcpy(temp,"bne");
				else if(strcmp(p->args[2],">") == 0)
					strcpy(temp,"bgt");
				else if(strcmp(p->args[2],"<") == 0)
					strcpy(temp,"blt");
				else if(strcmp(p->args[2],">=") == 0)
					strcpy(temp,"bge");
				else if(strcmp(p->args[2],"<=") == 0)
					strcpy(temp,"ble");
				prep_register(p->args[1],0);
				prep_register(p->args[3],1);
				fprintf(fprint,"%s $t0, $t1,%s\n",temp,p->args[5]);
				break;
			}
			default:;
		}
		p = p->next;
	}
}
void all_tra(char* name)
{
	if(head.next == NULL)return;
	FILE* fp;
	fp = fopen(name,"w");
	fprint  =  fp;
	fprintf(fp,"%s\n",".data");
	fprintf(fp,"%s\n","_prompt: .asciiz \"Enter an integer:\"");
	fprintf(fp,"%s\n","_ret: .asciiz \"\\n\"");
	fprintf(fp,"%s\n",".globl main");
	fprintf(fp,"%s\n",".text");
	fprintf(fp,"%s\n","read:");
	fprintf(fp,"%s\n","li $v0, 4");
	fprintf(fp,"%s\n","la $a0, _prompt");
	fprintf(fp,"%s\n","syscall");
	fprintf(fp,"%s\n","li $v0, 5");
	fprintf(fp,"%s\n","syscall");
	fprintf(fp,"%s\n","jr $ra");
	fprintf(fp,"%s\n","write:");
	fprintf(fp,"%s\n","li $v0, 1");
	fprintf(fp,"%s\n","syscall");
	fprintf(fp,"%s\n","li $v0, 4");
	fprintf(fp,"%s\n","la $a0, _ret");
	fprintf(fp,"%s\n","syscall");
	fprintf(fp,"%s\n","move $v0, $0");
	fprintf(fp,"%s\n","jr $ra");
	struct code_node* temp = head.next,*q;
	do
	{
		while(q != NULL)
		{
			if(strcmp(q->args[0],"FUNCTION") == 0) break;
			else if(q == head.next) break;
			q = q->next;
		}
		function_tra(temp,q);
		temp = q;
	}while(temp != NULL);
	fclose(fp);
}
