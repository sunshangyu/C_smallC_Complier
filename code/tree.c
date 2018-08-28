#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
struct NODE
{
	char name[40];
	struct NODE* firstchild;
	struct NODE* nextsibling;
	int line;
	char var_fun_str_name[30];//实际的ID
};

struct NODE* Create_newnode(char* name,int line);
struct NODE* insert(int num,...);
void print(struct NODE* root,int ceng);

struct NODE* Create_newnode(char* name,int line)
{
	struct NODE *node = (struct NODE*)malloc(sizeof(struct NODE));
	strcpy(node->name , name);
	node->nextsibling = NULL;
	node->firstchild = NULL;
	node->line = line;
	return node;
}

struct NODE* insert(int num, ...)
{
	if(num == 0) return NULL;
	struct NODE* node_parent=(struct NODE*)malloc(sizeof(struct NODE));
	struct NODE* node_child;
	struct NODE* node_secondchild;
	va_list argptr;
	va_start(argptr, num);
	num--;
	node_child = va_arg(argptr,struct  NODE*);
	node_parent->firstchild = node_child;
	node_parent->line = node_child->line;
	while (num--)
	{
		while(node_child->nextsibling!=NULL)
			node_child = node_child->nextsibling;
		node_secondchild = va_arg(argptr,struct NODE*);
		node_child->nextsibling = node_secondchild;
	}
	va_end(argptr);
	return node_parent;
}

void print(struct NODE* root,int ceng)
{
	if(!root) return;
	if(root->firstchild != NULL)
	{
		for(int i=0;i<ceng;i++)
			printf("|  ");
		printf("%s (%d)\n",root->name,root->line);
	}
	else
	{
		for(int i=0;i<ceng;i++)
			printf("|  ");
		printf("%s\n",root->name);
	}
	print(root->firstchild,ceng+1);
	print(root->nextsibling,ceng);
}

char* sixteenint_to_int(char* str)
{
	int number[30];
	char c;
	int i=0,start=0;
	int num=0;
	if(str[0]=='+'||str[0]=='-') start=3;
	else start=2;
	i = start;
	c = str[i];
	while(c != '\0')
	{
		if(c == 'A') number[i]=10;
		else if(c == 'B') number[i]=11;
		else if(c == 'C') number[i]=12;
		else if(c == 'D') number[i]=13;
		else if(c == 'E') number[i]=14;
		else if(c == 'F') number[i]=15;
		else if(c == '9') number[i]=9;
		else if(c == '8') number[i]=8;
		else if(c == '7') number[i]=7;
		else if(c == '6') number[i]=6;
		else if(c == '5') number[i]=5;
		else if(c == '4') number[i]=4;
		else if(c == '3') number[i]=3;
		else if(c == '2') number[i]=2;
		else if(c == '1') number[i]=1;
		else if(c == '0') number[i]=0;
		i++;
		c = str[i];
	}
	int temp = 1;
	printf("\n");
	for(int j=start;j<i;j++)
	{
		temp = 1;
		for(int a=i-j;a>=2;a--)
			temp *= 16;
		num += number[j]*temp;
	}
	if(str[0]=='-') num*=-1;
	sprintf(str,"%d",num);
	return str;
}

char* eightint_to_int(char* str)
{
	int number[30];
	char c;
	int i=0,start=0;
	int num=0;
	if(str[0]=='+'||str[0]=='-') start=2;
	else start=1;
	i = start;
	c = str[i];
	while(c != '\0')
	{
		if(c == '8') number[i]=8;
		else if(c == '7') number[i]=7;
		else if(c == '6') number[i]=6;
		else if(c == '5') number[i]=5;
		else if(c == '4') number[i]=4;
		else if(c == '3') number[i]=3;
		else if(c == '2') number[i]=2;
		else if(c == '1') number[i]=1;
		else if(c == '0') number[i]=0;
		i++;
		c = str[i];
	}
	int temp = 1;
	printf("\n");
	for(int j=start;j<i;j++)
	{
		temp = 1;
		for(int a=i-j;a>=2;a--)
			temp *= 8;
		num += number[j]*temp;
	}
	if(str[0]=='-') num*=-1;
	sprintf(str,"%d",num);
	return str;
}
