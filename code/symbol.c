#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct Type_;//类型的结构体，实际上，可以理解成是一个是符号表的节点结构，节点内存储着节点的类型（普通变量，结构体，数组，函数）（函数的结构体中含有着自己的定义时的返回类型，以及是否被声明）
struct Array_;//数组的结构体
struct Function_;//函数的结构体
struct FieldList_;//通过域的链接，建立结构体
typedef struct Type_* Type;//针对所有类型的一个结构体，用来表示所有的变量的类型和函数的类型
typedef struct Array_* Array;
typedef struct Function_* Function;//指向函数的一个指针
typedef struct FieldList_* FieldList;//此时要注意，Type和FieldList是指针类型
enum KIND {INTEGER,FLOATING,ARRAY,STRUCTURE,FUNCTIONDEF,FUNCTIONDEC};
struct Type_
{
 	enum KIND kind;//通过kind表示类型，数据结构的种类而不是具体的类型
	char* name;//用来存储着一个符号表节点的名称，无论是普通变量，数组，还是结构体，函数，均有着名字
	int basic;//int和float，0为int，1为float
	Array array;//数组型的结构体，指向一个数组
	FieldList structure;//结构体的指针
	Function function;//指向一个函数的结构体
	int line;//为了输出声明了但是没有定义的函数的行号，所以在声明的时候，都应该记录line
	Type next;//指向符号表主链的下一个节点
};
Type symbol_head;//symbol_head是一个Type_型的指针，指向符号表主链的头部，以后插入，都从这里插入
struct Function_
{
	FieldList param;//参数的链表
	enum KIND kind;//函数的返回类型。仅仅可以包含INTEGER，FLOATING
	char* return_kind;//返回类型如果是结构体的话，那么用这个字符串来记录
	FieldList field_list;//域的链表
};
struct Array_
{
	int size;//某一维的长度
	enum KIND kind;//如果定义一个数组，数组是全局变量的话，例如int array[10],Type p,p->kind = ARRAY,p->array->kind = INTEGER,如果是结构体，return_kind则存储着结构体的名称
	//如果Type->kind==STRUCTURE时，此时的kind_name派上用场，记录结构体的名字
	char* kind_name;//这里是存储结构体数组的类型，默认一个数组中出现的所有的维度的类型都是一样的
	Array next_demon;//指向下一个维度
};
struct FieldList_//域结构
{
	char * name;//域，即某一个函数或者结构体内部的域
	enum KIND kind;//域的种类
	char * struct_name;//如果kind==STRUCTURE，则struct_name则为结构体的ID
	enum KIND kind1;
	Array array;
	FieldList next;//指向下一个域
};
int check_isexisted_struct(char* name)//找当前结构体ID是否在主链中定义过了，如果找到了则返回1，未找到则为0
{
	Type temp = symbol_head;//从符号表主链的头部开始，往下找，比对ID
	while(temp != NULL)
	{
		if((strcmp(temp->name,name))==0)
		{
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}
Type new_struct_symbol(char* name)//在符号表的头部插入一个节点，类型是一个结构体型的，要将enum KIND置为STRUCTURE
{
	Type p = (Type)malloc(sizeof(struct Type_));//分配一个Type_节点
	p->kind = STRUCTURE;
	p->name = (char*)malloc(sizeof(30));//给p->name分配30个字节的空间
	strcpy(p->name,name);
	p->structure = NULL;
	//symbol_head仅仅是一个指针，并没有实体的空间
	p->next = symbol_head;
	symbol_head = p;//symbol_head仍然是头部，但此时p也是头部
	return p;
}
Type new_Type()
{
	Type p = (Type)malloc(sizeof(struct Type_));//分配一个Type_节点
	p->name = (char*)malloc(sizeof(30));//给p->name分配30个字节的空间
	p->next = NULL;
	return p;
}
FieldList new_field()//仅仅是新建一个FieldList_型的节点
{
	FieldList p = (FieldList)malloc(sizeof(struct FieldList_));//分配一个FieldList_节点
	p->name = (char*)malloc(sizeof(30));//给p->name分配30个字节的空间
	p->next = NULL;
	return p;
}
int check_field_isexisted(FieldList fieldlist,char* name)//从FieldList开始找，直到NULL，找到是否有name，如有，返回1，如没有，0
{
	FieldList temp = fieldlist;
	while(temp != NULL)
	{
		if((strcmp(temp->name,name))==0)
		{
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}
int check_mainlist_isexisted(Type symbol_head,char* name)//从主链的头部开始找
{
	Type temp = symbol_head;
	while(temp != NULL)
	{
		if((strcmp(temp->name,name))==0)
		{
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}
int check_func_def_isexisted(Type symbol_head,char* name)//在主链中找，找到一个同名的函数，并且这个函数是一个已经定义好的，找到了则返回1，否则，0
{
	Type temp = symbol_head;
	while(temp != NULL)
	{
		if(temp->kind == FUNCTIONDEF &&(strcmp(temp->name,name))==0)
		{
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}
int check_func_dec_param(Type p)
{
	//沿着主链从p->next开始往下找，如果某个节点的kind为FUNCTIONDEC，并且这个节点的名字==p->name
	//那么比对这个这个声明的形参和p的形参类型是否一致
	//如果不一致，则返回0
	Type temp = p->next;
	while(temp != NULL)
	{
		if((strcmp(temp->name,p->name))==0&&temp->kind == FUNCTIONDEC)
		{
			if(temp->function->kind!=p->function->kind)//两次声明的返回类型不一样
			{
				return 0;
			}else//两次声明的返回类型一样
			{
				if(temp->function->kind == STRUCTURE)//返回类型是结构体，那么比较结构体的名字
				{
					if((strcmp(temp->function->return_kind,p->function->return_kind))!=0)
					{
						return 0;
					}
				}
			}
			//以上都没有返回的话，说明两个声明的返回类型是一致的，接下来比较形参
			FieldList para = p->function->param;//p的形参表头
			FieldList temp_para = temp->function->param;//temp的形参表头
			while(para != NULL||temp_para != NULL)
			{
				if(para->kind != temp_para->kind)//两个参数的类型不一样，此处忽略掉形参的名字
				{
					return 0;
				}else
				{
					if(para->kind == STRUCTURE)//两个参数的类型一样，并且都是结构体，就该比较结构体的名字
					{
						if((strcmp(para->struct_name,temp_para->struct_name))!=0)
						{
							return 0;
						}
					}
				}
				para = para->next;
				temp_para = temp_para->next;
			}
		}
		temp = temp->next;
	}
	return 1;
}
int check_return_id(Type p,char * name)
{
	//首先从p开始，沿着p的参数链找，如果找到了相同名字的变量，那么比对这个变量和函数的返回类型是否一致，如果不一样则返回1
	//如果在参数链中没有找到，那么在域链中找，如果找到了相同名字的变量，那么比对这个变量的类型，如果不一样，则返回1
	//如果以上两步都没有找到，沿着符号表的主链找，如果找到了相同名字的变量，那么比对这个变量和函数的返回类型是否一致，如果不一样则返回1
	//如果以上三步都找不到，则直接返回0
	//如果在某一步中找到了类型还是对的，则返回2
	FieldList temp = p->function->param;//函数的形参链
	while(temp != NULL)
	{
		if((strcmp(temp->name,name))==0)
		{
			if(p->function->kind != temp->kind)//说明现在返回的对象的类型和定义的函数的返回类型不一致，报错
			{
				return 1;
			}else//类型相同
			{
				if(p->function->kind == STRUCTURE)//参数的类型是结构体，函数的返回类型也是结构体
				{
					if((strcmp(p->function->return_kind,temp->struct_name))!=0)
					{
						return 1;
					}else
					{
						return 2;
					}
				}else
				{
					return 2;
				}
			}
		}
		temp = temp->next;	
	}
	temp = p->function->field_list;//函数的局部变量链
	while(temp != NULL)
	{
		if((strcmp(temp->name,name))==0)
		{
			if(p->function->kind != temp->kind)//说明现在返回的对象的类型和定义的函数的返回类型不一致，报错
			{
				return 1;
			}else//类型相同
			{
				if(p->function->kind == STRUCTURE)//参数的类型是结构体，函数的返回类型也是结构体
				{
					if((strcmp(p->function->return_kind,temp->struct_name))!=0)
					{
						return 1;
					}else
					{
						return 2;
					}
				}else
				{
					return 2;
				}
			}
		}
		temp = temp->next;	
	}
	Type temp11 = symbol_head;//全局变量链
	while(temp11 != NULL)
	{
		if((strcmp(temp11->name,name))==0)
		{
			if(p->function->kind != temp11->kind)//说明现在返回的对象的类型和定义的函数的返回类型不一致，报错
			{
				return 1;
			}else//类型相同
			{
				if(p->function->kind == STRUCTURE)//参数的类型是结构体，函数的返回类型也是结构体
				{
					if((strcmp(p->function->return_kind,temp11->name))!=0)
					{
						return 1;
					}else
					{
						return 2;
					}
				}else
				{
					return 2;
				}
			}
		}
		temp11 = temp11->next;
	}
	return 0;//没有找到这个变量
}
int check_id_type(Type p,char* type,char * name)
{	//type存储着找到的同名变量的类型
	//此函数是在函数内部开始，沿着p->function->params开始往下找，找到了同名的，则将该ID的类型放到type中，type=int，float或者某个其他的字符（说明是结构体）
	//如果在参数链中找不到，就在函数的域链中寻找，如果找不到，就到主链中寻找，在主链中寻找，类型要找INTEGER或者FLOATING型的
	//如果最终没有找到，则返回0
	//如果找到了，则返回3
	FieldList temp = p->function->param;//函数的形参链
	while(temp != NULL)
	{
		if((strcmp(temp->name,name))==0)
		{
			if(temp->kind == STRUCTURE)//参数的类型是结构体，函数的返回类型也是结构体
			{
				strcpy(type,temp->struct_name);
				return 3;
			}else
			{
				if(temp->kind == INTEGER)
				{
					strcpy(type,"int");
					return 3;
				}
				if(temp->kind == FLOATING)
				{
					strcpy(type,"float");
					return 3;
				}
			}
		}
		temp = temp->next;	
	}
	temp = p->function->field_list;//函数的局部变量链
	while(temp != NULL)
	{
		if((strcmp(temp->name,name))==0)
		{
			if(temp->kind == STRUCTURE)//参数的类型是结构体，函数的返回类型也是结构体
			{
				strcpy(type,temp->struct_name);
				return 3;
			}else
			{
				if(temp->kind == INTEGER)
				{
					strcpy(type,"int");
					return 3;
				}
				if(temp->kind == FLOATING)
				{
					strcpy(type,"float");
					return 3;
				}
			}
		}
		temp = temp->next;	
	}
	Type temp11111 = symbol_head;//全局变量链
	while(temp11111 != NULL)
	{
		if((strcmp(temp11111->name,name))==0)
		{
			if(temp11111->kind == INTEGER)
			{
				strcpy(type,"int");
				return 3;
			}
			if(temp11111->kind == FLOATING)
			{
				strcpy(type,"float");
				return 3;
			}
		}
		temp11111 = temp11111->next;	
	}
	return 0;//没有找到这个变量
}
int check_func_use_assign(enum KIND kind_id,char* if_struct_name,char* name)
{
	//从符号表的主链开始找，找到同名函数后，比对函数的返回类型，如果类型不一样，直接返回0，报错，如果一样，返回1，最终没有找到该函数，返回2
	Type temp = symbol_head;
	while(temp != NULL)
	{
		if((temp->kind == FUNCTIONDEC || temp->kind == FUNCTIONDEF)&&strcmp(temp->name,name)==0)
		{
			if(temp->function->kind == kind_id)
			{
				if(kind_id == STRUCTURE)
				{
					if(strcmp(temp->function->return_kind,if_struct_name)==0)
					{
						return 1;
					}else
					{
						return 0;
					}
				}else
				{
					return 1;
				}
			}else
			{
				return 0;
			}
		}
		temp = temp->next;
	}
	return 2;
}
int getfunc_param_num(char * name)
{
	Type temp = symbol_head;
	while(temp != NULL)
	{
		if((temp->kind == FUNCTIONDEC || temp->kind == FUNCTIONDEF)&&strcmp(temp->name,name)==0)
		{
			int param_number = 0;
			FieldList param = temp->function->param;
			while(param != NULL)
			{
				param_number++;
				param = param->next;
			}
			return param_number;
		}
		temp = temp->next;
	}
	return -1;
}
int id_is_not_function(Type function,char* name)
{
	FieldList temp = function->function->param;//函数的参数链表
	while(temp != NULL)
	{
		if(strcmp(temp->name,name)==0)
		{
			return -1;
		}
		temp = temp->next;
	}
	temp = function->function->field_list;
	while(temp != NULL)
	{
		if(strcmp(temp->name,name)==0)
		{
			return -1;
		}
		temp = temp->next;
	}
	Type temp1 = symbol_head;
	while(temp1 != NULL)
	{
		if(temp1->kind != FUNCTIONDEF&&temp1->kind != FUNCTIONDEC &&strcmp(temp1->name,name)==0)
		{
			return -1;
		}
		temp1 = temp1->next;
	}
	return 0;
}
int check_is_array(Type function,char * name)//只在域中找，太懒了
{
	FieldList temp = function->function->field_list;
	while(temp != NULL)
	{
		if(strcmp(temp->name,name)==0)
		{
			if(temp->kind1 == ARRAY)
			{
				return 1;
			}
		}
	temp = temp->next;
	}
	return 0;
}
int check_is_struct(Type function,char * structname,char * name)
{
	FieldList temp = function->function->field_list;
	while(temp != NULL)
	{
		if(strcmp(temp->name,name)==0)
		{
			if(temp->kind != STRUCTURE)
			{
				return 0;
			}else
			{
				strcpy(structname,temp->struct_name);
				return 1;
			}
		}
	temp = temp->next;
	}
	return 0;
}
int check_struct_field_isexisted(char * struct_name,char * field)
{
	Type temp = symbol_head;
	while(temp != NULL)
	{
		if(temp->kind == STRUCTURE && strcmp(temp->name,struct_name)==0)
		{
			FieldList tempfield = temp->structure;
			while(tempfield != NULL)
			{
				if(strcmp(tempfield->name,field)==0)
				{
					return 1;
				}
				tempfield = tempfield->next;
			}
			return 0;
		}
		temp = temp->next;
	}
	return 0;
}
FieldList find_fieldList(char* name)//根据name，在所有函数的域链中找到一个域，并将指向该域的指针返回
{
	Type temp = symbol_head;
	while(temp != NULL)
	{
		if(temp->kind == FUNCTIONDEF)
		{
			FieldList temp_field = temp->function->field_list;
			while(temp_field != NULL)
			{
				if(strcmp(temp_field->name,name) == 0)
					return temp_field;
				temp_field = temp_field->next;
			}
			FieldList temp_param = temp->function->param;
			while(temp_param != NULL)
			{
				if(strcmp(temp_param->name,name) == 0)
					return temp_param;
				temp_param = temp_param->next;
			}
		}
		temp = temp->next;
	}
	return NULL;
}
int struct_get_size(char* struct_name)//根据struct_name，在符号表中找到该结构体，并将该结构体的大小返回
{
	int count = 0;
	Type temp = symbol_head;
	while(temp != NULL)
	{
		if(temp->kind == STRUCTURE && strcmp(temp->name,struct_name) == 0)
		{
			FieldList temp_field = temp->structure;
			while(temp_field != NULL)
			{
				if(temp_field->kind == INTEGER)
					count += 4;
				else if(temp_field->kind == FLOATING)
					count += 8;
				else if(temp_field->kind == ARRAY)
				{
					int capbility = 1;
					Array temp_array = temp_field->array;
					while(temp_array != NULL)
					{
						capbility *= temp_array->size;
						temp_array = temp_array->next_demon;
					}
					if(temp_field->kind1 == INTEGER)
						count += 4*capbility;
					else if(temp_field->kind1 == FLOATING)
						count += 8*capbility;
					else
						count += struct_get_size(temp_field->struct_name);
				}
				temp_field = temp_field->next;
			}
			break;
		}
		temp = temp->next;
	}
	return count;
}
int struct_get_offset(char* current_struct,char* name)
{
	int offset_reverse = 0;
	int size = struct_get_size(current_struct);
	Type temp = symbol_head;
	while(temp != NULL)
	{
		if(temp->kind == STRUCTURE && strcmp(temp->name,current_struct) == 0)
		{
			FieldList temp_field = temp->structure;
			while(temp_field != NULL)
			{
				if(strcmp(temp_field->name,name) == 0)
					break;
				if(temp_field->kind == INTEGER)
					offset_reverse += 4;
				else if(temp_field->kind == FLOATING)
					offset_reverse += 8;
				else if(temp_field->kind == STRUCTURE)
					offset_reverse += struct_get_size(temp_field->struct_name);
				else if(temp_field->kind == ARRAY)
				{
					int capbility = 1;
					Array temp_array = temp_field->array;
					while(temp_array != NULL)
					{
						capbility *= temp_array->size;
						temp_array = temp_array->next_demon;
					}
					if(temp_field->kind1 == INTEGER)
						offset_reverse += 4*capbility;
					else if(temp_field->kind1 == FLOATING)
						offset_reverse += 8*capbility;
					else
						offset_reverse += struct_get_size(temp_field->struct_name);
				}
				temp_field = temp_field->next;
			}
			while(temp_field != NULL)
			{
				if(temp_field->next == NULL)
				{
					if(temp_field->kind == INTEGER)
						offset_reverse += 4;
					else if(temp_field->kind == FLOATING)
						offset_reverse += 8;
					else if(temp_field->kind == STRUCTURE)
						offset_reverse += struct_get_size(temp_field->struct_name);
					else if(temp_field->kind == ARRAY)
					{
						int capbility = 1;
						Array temp_array = temp_field->array;
						while(temp_array != NULL)
						{
							capbility *= temp_array->size;
							temp_array = temp_array->next_demon;
						}
						if(temp_field->kind1 == INTEGER)
							offset_reverse += 4*capbility;
						else if(temp_field->kind1 == FLOATING)
							offset_reverse += 8*capbility;
						else
							offset_reverse += struct_get_size(temp_field->struct_name);
					}
				}
				temp_field = temp_field->next;
			}
			break;
		}
		temp = temp->next;
	}
	return size - offset_reverse;
}
int param_field(char* name)
{
	Type temp = symbol_head;
	while(temp != NULL)
	{
		if(temp->kind == FUNCTIONDEF)
		{
			FieldList temp_field = temp->function->field_list;
			while(temp_field != NULL)
			{
				if(strcmp(temp_field->name,name) == 0)
					return 1;
				temp_field = temp_field->next;
			}
			FieldList temp_param = temp->function->param;
			while(temp_param != NULL)
			{
				if(strcmp(temp_param->name,name) == 0)
					return 0;
				temp_param = temp_param->next;
			}
		}
		temp = temp->next;
	}
	return 2;
}
