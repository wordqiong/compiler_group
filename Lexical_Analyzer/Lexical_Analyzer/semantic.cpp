#include"semantic.h"
using namespace std;
Quaternion::Quaternion(int i, string op_type, string a1, string a2, string res)
{
	this->index = i;
	this->operator_type = op_type;
	this->arg1 = a1;
	this->arg2 = a2;
	this->result = res;
}
//=======================符号表=====================
SemanticSymbolTable::SemanticSymbolTable(const TableType type, const string name)
{
	this->type = type;
	this->name = name;
}
/*
* 函数名：FindSymbol
* 功能：返回符号id_name在符号表中的索引，
* 如果符号表没有该符号，返回-1
*
*/
int SemanticSymbolTable::FindSymbol(const string id_name)
{

	return -1;
}

/*
* 函数名：InsertSymbol
* 功能：向符号表中插入一个符号
*
*
*/
int SemanticSymbolTable::InsertSymbol(const IdentifierInfo ident)
{

}

//====================语义分析 ===========================
//构造函数
SemanticAnalysis::SemanticAnalysis()
{
	//创建全局的符号表
	tables.push_back(SemanticSymbolTable(GlobalTable, "global table"));
	//当前作用域为全局作用域
	current_table_stack.push_back(0);
	//创建临时变量表
	tables.push_back(SemanticSymbolTable(TempTable, "temp variable table"));

	//从标号1开始生成四元式标号；0号用于 (j, -, -, main_address)
	next_quaternion_index = 1;
	//main函数的标号先置为-1
	main_index = -1;
	//初始回填层次为0，表示不需要回填
	backpatching_level = 0;
	//临时变量计数器归零
	tmp_var_count = 0;
}


/*
* 函数名：AddSymbolToList
* 功能：将符号信息放入symbol_list
*
*
*/
void SemanticAnalysis::AddSymbolToList(const SemanticSymbol symbol)
{
	symbol_list.push_back(symbol);
}

/*
* 函数名：Analysis
* 功能：分析过程，根据产生式左部选择对应的翻译方法
*
*
*/
void SemanticAnalysis::Analysis(const string production_left, const vector<string> production_right)
{
	//Program ::= ExtDefList 
	if (production_left == "Program")
		TranslateProgram(production_left, production_right);
	//ExtDef ::= VarSpecifier <ID> ; | FunSpecifier FunDec Block ExitFunTable_m
	else if (production_left == "ExtDef")
		TranslateExtDef(production_left, production_right);
	//VarSpecifier ::= int
	else if (production_left == "VarSpecifier")
		TranslateVarSpecifier(production_left, production_right);
	else if (production_left == "FunSpecifier")
		TranslateFunSpecifier(production_left, production_right);
	else if (production_left == "FunDec")
		TranslateFunDec(production_left, production_right);
	else if (production_left == "CreateFunTable_m")
		TranslateCreateFunTable_m(production_left, production_right);
	else if (production_left == "ParamDec")
		TranslateParamDec(production_left, production_right);
	else if (production_left == "Block")
		TranslateBlock(production_left, production_right);
	else if (production_left == "Def")
		TranslateDef(production_left, production_right);
	else if (production_left == "AssignStmt")
		TranslateAssignStmt(production_left, production_right);
	else if (production_left == "Exp")
		TranslateExp(production_left, production_right);
	else if (production_left == "AddSubExp")
		TranslateAddSubExp(production_left, production_right);
	else if (production_left == "Item")
		TranslateItem(production_left, production_right);
	else if (production_left == "Factor")
		TranslateFactor(production_left, production_right);
	else if (production_left == "CallStmt")
		TranslateCallStmt(production_left, production_right);
	else if (production_left == "CallFunCheck")
		TranslateCallFunCheck(production_left, production_right);
	else if (production_left == "Args")
		TranslateArgs(production_left, production_right);
	else if (production_left == "ReturnStmt")
		TranslateReturnStmt(production_left, production_right);
	else if (production_left == "Relop")
		TranslateRelop(production_left, production_right);
	else if (production_left == "IfStmt")
		TranslateIfStmt(production_left, production_right);
	else if (production_left == "IfStmt_m1")
		TranslateIfStmt_m1(production_left, production_right);
	else if (production_left == "IfStmt_m2")
		TranslateIfStmt_m2(production_left, production_right);
	else if (production_left == "IfNext")
		TranslateIfNext(production_left, production_right);
	else if (production_left == "IfStmt_next")
		TranslateIfStmt_next(production_left, production_right);
	else if (production_left == "WhileStmt")
		TranslateWhileStmt(production_left, production_right);
	else if (production_left == "WhileStmt_m1")
		TranslateWhileStmt_m1(production_left, production_right);
	else if (production_left == "WhileStmt_m2")
		TranslateWhileStmt_m2(production_left, production_right);
	else {
		if (production_right[0] != "@") {
			int count = production_right.size();
			while (count--)
				symbol_list.pop_back();
		}
		symbol_list.push_back({ production_left,"",-1,-1 });
	}

}

/*
* 函数名：PrintQuaternion
* 功能：打印四元式表到文件file_path中
*
*
*/
void SemanticAnalysis::PrintQuaternion(const string file_path)
{



}

/*
* 函数名：PrintQuaternion
* 功能：翻译Program ::= ExtDefList 
*
*
*/
void SemanticAnalysis::TranslateProgram(const string production_left,
	const vector<string> production_right)
{
	if (main_index == -1)
	{
		cout << "Semantic Analysis Error:main函数未定义" << endl;
		//TODO:是用她的throw退出吗
		exit(-1);
	}
	for (int i = 0; i < production_right.size(); i++)
		symbol_list.pop_back();

	quaternion_list.insert(quaternion_list.begin(), { 0, "j","-" , "-", to_string(main_index) });
	//修改符号流
	symbol_list.push_back({ production_left,"",-1,-1 });//推入一个空，表示main
}


//ExtDef ::= VarSpecifier <ID> ; | FunSpecifier FunDec Block
//变量定义与函数定义
void SemanticAnalysis::TranslateExtDef(const string production_left,
	const vector<string> production_right)
{
	//TODO:这个个数怎么判断的，为啥函数定义右边能拿到标识符？？？？
	
	//变量定义
	if (production_right.size() == 3)
	{
		//变量类型
		SemanticSymbol type = symbol_list[symbol_list.size() - 3];
		//变量名
		SemanticSymbol name = symbol_list[symbol_list.size() - 2];

		//判断是否已经有定义
		//当前符号表
		SemanticSymbolTable table = tables[current_table_stack.back()];
		if (table.FindSymbol(name.val) != -1)
		{
			cout << "Semantic Analysis Error:变量" << name.val <<"重定义" << endl;
			//TODO;退出
			exit(-1);
		}

		//未重定义，加入符号表
		IdentifierInfo variable;
		variable.identifier_type = IdentifierType::Variable;//是个变量
		variable.specifier_type = type.val;
		variable.identifier_name = name.val;

		//将新变量加入到当前的符号表中
		tables[current_table_stack.back()].InsertSymbol(variable);

		//修改符号流
		for (int i = 0; i < production_right.size(); i++)
			symbol_list.pop_back();
		//保存当前标识符的名字
		symbol_list.push_back({ production_left,name.val,-1,-1 });
	}
	else //函数定义
	{
		//TODO：函数定义好像不需要加入到符号表(符号表是给每个作用域一个符号表)，那标识符的Function是干什么的
		SemanticSymbol name = symbol_list[symbol_list.size() - 2];

		//函数定义语法归约，表示从函数内部出来了，所以要弹出此时的作用域
		current_table_stack.pop_back();

		//修改符号流
		for (int i = 0; i < production_right.size(); i++)
			symbol_list.pop_back();
		//保存当前函数标识符的名字
		symbol_list.push_back({ production_left,name.val,-1,-1 });
	}
}

//VarSpecifier ::= int
void  SemanticAnalysis::TranslateVarSpecifier(const string production_left, const vector<string> production_right)
{
	//归约修改符号流即可
	SemanticSymbol type = symbol_list.back();
	for (int i = 0; i < production_right.size(); i++)
		symbol_list.pop_back();
	//保存类型值
	symbol_list.push_back({ production_left,type.val,-1,-1 });//就是替换，先不改值，不改位置，只是将右侧归约成左侧
}

//FunSpecifier ::= void | int
void  SemanticAnalysis::TranslateFunSpecifier(const string production_left, const vector<string> production_right)
{
	//同样是修改符号流
	SemanticSymbol type = symbol_list.back();
	for (int i = 0; i < production_right.size(); i++)
		symbol_list.pop_back();
	//保存函数类型
	symbol_list.push_back({ production_left,type.val,-1,-1 });//就是替换，先不改值，不改位置，只是将右侧归约成左侧
}
void  SemanticAnalysis::TranslateFunDec(const string production_left, const vector<string> production_right)
{


}
void  SemanticAnalysis::TranslateCreateFunTable_m(const string production_left, const vector<string> production_right)
{

}
void  SemanticAnalysis::TranslateParamDec(const string production_left, const vector<string> production_right)
{

}
void  SemanticAnalysis::TranslateBlock(const string production_left, const vector<string> production_right)
{

}
void  SemanticAnalysis::TranslateDef(const string production_left, const vector<string> production_right)
{

}
void  SemanticAnalysis::TranslateAssignStmt(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateExp(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateAddSubExp(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateItem(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateFactor(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateCallStmt(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateCallFunCheck(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateArgs(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateReturnStmt(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateRelop(const string production_left, const vector<string> production_right)
{

}

//IfStmt ::= if IfStmt_m1 ( Exp ) IfStmt_m2 Block IfNext
void SemanticAnalysis::TranslateIfStmt(const string production_left, const vector<string> production_right)
{
	SemanticSymbol IfStmt_m2 = symbol_list[symbol_list.size() - 3];
	SemanticSymbol IfNext = symbol_list[symbol_list.size() - 1];

	//如果没有else，只需要回填IfStmt_m2的两个四元式
	if (IfNext.val.empty())
	{
		//真出口，进入到block中
		quaternion_list[backpatching_list.back()].result = IfStmt_m2.val;
		backpatching_list.pop_back();

		//假出口，跳出if，这里的next_quaternary_index是整体的下一条
		quaternion_list[backpatching_list.back()].result = to_string(next_quaternion_index);
		backpatching_list.pop_back();
	}
	else
		//有else，需要回填三个四元式
	{
		//if块出口，填的是整体出口，对应Block执行完的无条件jump的跳转目标
		quaternion_list[backpatching_list.back()].result = to_string(next_quaternion_index);
		backpatching_list.pop_back();

		//if真出口，是IfStmt_m2装入的待回填四元式，表示exp为真时的出口
		quaternion_list[backpatching_list.back()].result = IfStmt_m2.val;
		backpatching_list.pop_back();

		//if假出口
		quaternion_list[backpatching_list.back()].result = IfNext.val;
		backpatching_list.pop_back();
	}

	//修改symbol list
	for (int i = 0; i < production_right.size(); i++)
		symbol_list.pop_back();
	symbol_list.push_back({ production_left,"",-1,-1 });
}

//IfStmt_m1 ::= @
void SemanticAnalysis::TranslateIfStmt_m1(const string production_left, const vector<string> production_right)
{
	//IfStmt_m1只需要作为一个语法左侧占位即可，且因为右侧是空，所以不用pop
	//保存当前下一条四元式的位置,TODO:其实没用
	symbol_list.push_back({ production_left,to_string(next_quaternion_index),-1,-1 });
}

//IfStmt_m2 ::= @
//用来判断布尔表达式的真假出口，对接的是Exp表达式部分
void SemanticAnalysis::TranslateIfStmt_m2(const string production_left, const vector<string> production_right)
{
	//if控制语句布尔表达式
	SemanticSymbol expression = symbol_list[symbol_list.size() - 2];

	//布尔表达式假出口,待回填
	quaternion_list.push_back({ next_quaternion_index++ ,"j=",expression.val,"0","" });
	backpatching_list.push_back(quaternion_list.size() - 1);

	//布尔表达式真出口，待回填,TODO
	quaternion_list.push_back({ next_quaternion_index++ ,"j=","-","-","" });
	backpatching_list.push_back(quaternion_list.size() - 1);

	//修改symbol list
	symbol_list.push_back({ production_left,to_string(next_quaternion_index),-1,-1 });

}

//IfNext ::= @ | IfStmt_next else Block
//else可有可无
void SemanticAnalysis::TranslateIfNext(const string production_left, const vector<string> production_right)
{	
	//TODO:这个应该是要判断的，不然IfStmt_next可能拿到不正确的东西
	if (production_right.size() == 3)
	{
		SemanticSymbol IfStmt_next = symbol_list[symbol_list.size() - 3];
		//修改symbol list
		for (int i = 0; i < production_right.size(); i++)
			symbol_list.pop_back();
		//存的和IfStmt_next一样，是第一段结束后无条件跳转到哪里
		symbol_list.push_back({ production_left,IfStmt_next.val,-1,-1 });
	}
	else//是空的情况
	{
		//只需要占位，并且给个空串作为value，用于if整体语句判断
		symbol_list.push_back({ production_left,"",-1,-1 });
	}

}

//IfStmt_next ::= @
//if第一段走完之后要去哪里，是一个无条件跳转
void SemanticAnalysis::TranslateIfStmt_next(const string production_left, const vector<string> production_right)
{
	//添加一个无条件跳转语句，跳转位置待回填
	quaternion_list.push_back({ next_quaternion_index++ ,"j","-","-","" });
	backpatching_list.push_back(quaternion_list.size() - 1);

	//修改symbol list,存当前情况下下一条四元式位置
	symbol_list.push_back({ production_left,to_string(next_quaternion_index),-1,-1 });
}
void SemanticAnalysis::TranslateWhileStmt(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateWhileStmt_m1(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateWhileStmt_m2(const string production_left, const vector<string> production_right)
{

}


