#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <string.h>

#include <vector>
#include <set>
#include <map>

using namespace std;

#define SEMANTIC_ERROR_UNDEFINED 1
#define SEMANTIC_ERROR_PARAMETER_NUM 2
#define SEMANTIC_ERROR_NO_RETURN 3
enum ErrorProcess {
	ID_UNDEFIEND,
	WRONG_PARAMETER_NUM
};

//语义分析中的符号
struct SemanticSymbol
{
	string type;//类型
	string val;//值
	int table_index;//符号所在table的index
	int symbol_index;//符号在table内部的index
};


//标识符信息，即函数、变量、临时变量、常量的具体信息
struct IdentifierInfo
{
	//定义标识符类型，分别为函数、变量、临时变量、常量、返回值
	enum IdentifierType { Function, Variable, TempVar, ConstVar, ReturnVar };
	IdentifierType identifier_type;//标识符的类型
	string specifier_type;//变(常)量类型/函数返回类型
	string identifier_name;//标识符名称/常量值
	int function_parameter_num;//函数参数个数
	int function_entry;//函数入口地址(四元式的标号)
	int function_table_index;//函数的函数符号表在整个程序的符号表列表中的索引
};

//语义分析中的符号表
struct SemanticSymbolTable {
	//几种表的类型，分别为全局表、函数表、块级表、临时表
	enum TableType { GlobalTable, FunctionTable, BlockTable, TempTable };

	TableType type;//表类型
	vector<IdentifierInfo> content;//符号表的内容
	string name;//表名

	//构造函数
	SemanticSymbolTable(const TableType type, const string name);

	//寻找指定符号名的位置
	int FindSymbol(const string symbol_name);

	//加入一个符号，返回加入的位置
	int InsertSymbol(const IdentifierInfo ident);
};

//四元式
struct Quaternion
{
	int index;//四元式标号
	string operator_type;//操作类型
	string arg1;//操作数1
	string arg2;//操作数2
	string result;//结果

	Quaternion(int i, string op_type, string a1, string a2, string res);
};


class SemanticAnalysis {
public:
	vector<Quaternion> quaternion_list;//四元式表
	int main_index;//main函数对应的四元式标号
	int backpatching_level;//回填层次

	vector<int> backpatching_list;//待回填的四元式在四元式表quaternion_list中的序号
	int next_quaternion_index;//当前位置下一个四元式标号
	int tmp_var_count;//临时变量计数

	vector<SemanticSymbol> symbol_list;//语义分析过程的符号流，可以理解为在语义分析中的符号栈，跟着语法分析中的要求走
	vector<SemanticSymbolTable> tables;//程序所有符号表
	vector<int> current_table_stack;//当前作用域对应的符号表索引栈

	//构造函数
	SemanticAnalysis();
	//将所有的符号信息放入symbol_list
	void AddSymbolToList(const SemanticSymbol symbol);
	//分析过程
	void Analysis(const string production_left, const vector<string> production_right);
	//打印四元式表
	void PrintQuaternion(const string file_path);

private:
	//每次翻译都对应的语法分析中的一个归约式
	void TranslateProgram(const string production_left, const vector<string> production_right);
	void TranslateExtDef(const string production_left, const vector<string> production_right);
	void TranslateVarSpecifier(const string production_left, const vector<string> production_right);
	void TranslateFunSpecifier(const string production_left, const vector<string> production_right);
	void TranslateIfStmt(const string production_left, const vector<string> production_right);
	void TranslateIfStmt_m1(const string production_left, const vector<string> production_right);
	void TranslateIfStmt_m2(const string production_left, const vector<string> production_right);
	void TranslateIfNext(const string production_left, const vector<string> production_right);
	void TranslateIfStmt_next(const string production_left, const vector<string> production_right);

	//===============================================================
	void TranslateFunDec(const string production_left, const vector<string> production_right);
	void TranslateCreateFunTable_m(const string production_left, const vector<string> production_right);
	void TranslateParamDec(const string production_left, const vector<string> production_right);
	void TranslateBlock(const string production_left, const vector<string> production_right);
	void TranslateDef(const string production_left, const vector<string> production_right);
	void TranslateWhileStmt(const string production_left, const vector<string> production_right);
	void TranslateWhileStmt_m1(const string production_left, const vector<string> production_right);
	void TranslateWhileStmt_m2(const string production_left, const vector<string> production_right);

	//==============================================================
	void TranslateAssignStmt(const string production_left, const vector<string> production_right);
	void TranslateExp(const string production_left, const vector<string> production_right);
	void TranslateAddSubExp(const string production_left, const vector<string> production_right);
	void TranslateItem(const string production_left, const vector<string> production_right);
	void TranslateFactor(const string production_left, const vector<string> production_right);
	void TranslateCallStmt(const string production_left, const vector<string> production_right);
	void TranslateCallFunCheck(const string production_left, const vector<string> production_right);
	void TranslateArgs(const string production_left, const vector<string> production_right);
	void TranslateReturnStmt(const string production_left, const vector<string> production_right);
	void TranslateRelop(const string production_left, const vector<string> production_right);

	//=====================tool======================================
	bool CheckIdDefine(SemanticSymbol identifier, int* tb_index, int* tb_index_index);
	int CheckParNum(SemanticSymbol check, int* value);
	void ProcessError(SemanticSymbol identifier, int* tb_index, int* tb_index_index, ErrorProcess type);
	void PopSymbolList(int count);
};