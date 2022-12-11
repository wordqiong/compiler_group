#include"semantic.h"
using namespace std;
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
		symbol_list.push_back({ production_left,"",-1,-1,-1,-1 });
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
* 功能：翻译产生式左部为Program的式子
*
*
*/
void SemanticAnalysis::TranslateProgram(const string production_left,
	const vector<string> production_right)
{


}


void SemanticAnalysis::TranslateExtDef(const string production_left,
	const vector<string> production_right)
{


}

void  SemanticAnalysis::TranslateVarSpecifier(const string production_left, const vector<string> production_right)
{

}
void  SemanticAnalysis::TranslateFunSpecifier(const string production_left, const vector<string> production_right)
{

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
void SemanticAnalysis::TranslateIfStmt(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateIfStmt_m1(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateIfStmt_m2(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateIfNext(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateIfStmt_next(const string production_left, const vector<string> production_right)
{

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


