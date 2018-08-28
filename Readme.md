运行环境：
	Ubuntu16.04
运行软件：
	Flex，Bison，Spim Simulator
运行方式：
	flex fi.l &&
	bison -d fi.y &&
	gcc fi.tab.c -lfl -ly -o parser &&
	./parser test4.1.c aim1.asm &&
	./parser test4.2.c aim2.asm
文件的用途：
	aim1.asm				test4.1.c对应的目标代码
	aim2.asm				test4.2.c对应的目标代码
	aimcode.c				目标代码生成文件
	fi.l					词法分析文件
	fi.tab.c				bison -d fi.y生成的文件
	fi.tab.h				bison -d fi.y生成的文件
	fi.y					C--文法文件
	lex.yy.c				flex fi.l生成的文件
	order.c					命令文件
	parser					最终生成的二进制程序
	semantic_analysis.c		语法分析文件
	symbol.c				符号表文件
	test4.1.c				测试代码
	test4.2.c				测试代码
	translate.c				中间代码生成文件
	tree.c					语法树文件
