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
	for (int i = 0; i < this->content.size(); i++)
	{
		if (id_name == this->content[i].identifier_name)
			return i;
	}
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
	int is_existed = this->FindSymbol(ident.identifier_name);
	if (is_existed == -1)
	{
		this->content.push_back(ident);
		return this->content.size() - 1;
	}
	else
		return -1;
}

//====================语义分析 ===========================
//构造函数
SemanticAnalysis::SemanticAnalysis()
{
	//创建全局的符号表
	tables.push_back(SemanticSymbolTable(SemanticSymbolTable::GlobalTable, "global table"));
	//当前作用域为全局作用域
	current_table_stack.push_back(0);
	//创建临时变量表
	tables.push_back(SemanticSymbolTable(SemanticSymbolTable::TempTable, "temp variable table"));

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
bool SemanticAnalysis::CheckIdDefine(SemanticSymbol identifier, int* tb_index, int* tb_index_index) {
	bool exist = false;
	int index_table = -1;
	int index = -1;
	//从当前层向上遍历
	for (int now_layer = current_table_stack.size() - 1; now_layer >= 0; now_layer--)
	{
		SemanticSymbolTable now_table = tables[current_table_stack[now_layer]];
		*tb_index_index = now_table.FindSymbol(identifier.val);
		if (*tb_index_index != -1)
		{
			*tb_index = current_table_stack[now_layer];
			exist = true;
			break;
		}
	}
	return exist;
}
int SemanticAnalysis::CheckParNum(SemanticSymbol check, int* value) {
	int para_num = tables[check.table_index].content[check.symbol_index].function_parameter_num;
	if (para_num > *value)
	{
		return 1;
	}
	else if (para_num < *value)
	{
		return 2;
	}
	return 0;
}
void SemanticAnalysis::ProcessError(SemanticSymbol identifier, int* tb_index, int* tb_index_index, ErrorProcess type) {
	if (type == ID_UNDEFIEND) {
		if (!CheckIdDefine(identifier, tb_index, tb_index_index))
		{
			cout << "语义分析中发生错误：\"" << identifier.val << "\"未定义" << endl;
			throw(SEMANTIC_ERROR_UNDEFINED);
		}
		else {
			return;
		}
	}
	else if (type == WRONG_PARAMETER_NUM) {
		int res = CheckParNum(identifier, tb_index);
		if (res == 2)
		{
			cout << "语义分析中发生错误：函数调用参数过多，出错函数名为   " <<identifier.val<< endl;
			throw(SEMANTIC_ERROR_PARAMETER_NUM);
		}
		if (res == 1)
		{
			cout << "语义分析中发生错误：函数调用参数过少，出错函数名为   " << identifier.val<< endl;
			throw(SEMANTIC_ERROR_PARAMETER_NUM);
		}
		return;
	}
	else if (type == FUN_ID_REDEFINED) {
		//在全局的table判断函数名是否重定义
		if (tables[0].FindSymbol(identifier.val) != -1) {
			cout << "语义分析中发生错误：" <<
				"函数" << identifier.val <<
				"重定义" << endl;
			throw(SEMANTIC_ERROR_REDEFINED);//抛出错误是否要考虑
		}
	}
	else if (type == PARA_ID_REDEFINED) {
		SemanticSymbolTable& function_table = tables[*tb_index];
		//如果已经进行过定义
		if (function_table.FindSymbol(identifier.val) != -1) {
			cout << "语义分析中发生错误：" <<
				"函数参数" << identifier.val << "重定义" << endl;
			throw(SEMANTIC_ERROR_REDEFINED);
		}
	}
	else if (type == ID_REDEFINED) {
		SemanticSymbolTable& current_table = tables[*tb_index];
		//重定义则报错
		if (current_table.FindSymbol(identifier.val) != -1)
		{
			cout << "语义分析中发生错误："
				<< "变量" << identifier.val << "重定义" << endl;
			throw(SEMANTIC_ERROR_REDEFINED);
		}
	}
	else if (type == MAIN_UNDEFINED)
	{
		cout << "语义分析中发生错误：main函数未定义" << endl;
		throw(SEMANTIC_ERROR_MAIN_UNDEFINED);
	}

}
void SemanticAnalysis::PopSymbolList(int count) {
	int cnt = count;
	while (cnt--)
		symbol_list.pop_back();
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
	ofstream ofs(file_path, ios::out);

	for (int i = 0; i < this->quaternion_list.size(); i++)
	{
		Quaternion temp = quaternion_list[i];
		ofs << temp.index << "(";
		ofs << temp.operator_type << "," << temp.arg1 << "," << temp.arg2 << "," << temp.result << ")" << endl;

	}
	ofs.close();
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
		//cout << "Semantic Analysis Error:main函数未定义" << endl;
		ProcessError({ "","",-1,-1 }, NULL, NULL, MAIN_UNDEFINED);
	}

	//插入初始四元式
	quaternion_list.insert(quaternion_list.begin(), { 0, "j","-" , "-", to_string(main_index) });
	//修改符号流
	PopSymbolList(production_right.size());
	symbol_list.push_back({ production_left,"",-1,-1 });//推入一个空，表示main
}


//ExtDef ::= VarSpecifier <ID> ; | FunSpecifier FunDec Block
//变量定义与函数定义
void SemanticAnalysis::TranslateExtDef(const string production_left,
	const vector<string> production_right)
{
	//TODO:这个个数怎么判断的，产生式右边都是三个怎么区分；修改成了判断最后一个string是不是分号

	//变量定义
	//if (production_right.size() == 3)
	if (production_right.back() == ";")
	{
		//变量名
		SemanticSymbol name = symbol_list[symbol_list.size() - 2];
		//变量类型
		SemanticSymbol type = symbol_list[symbol_list.size() - 3];

		//判断是否已经有定义
		//当前符号表
		ProcessError(name, &(current_table_stack.back()), NULL, ID_REDEFINED);


		//未重定义，加入符号表
		IdentifierInfo variable;
		variable.identifier_type = IdentifierInfo::Variable;//是个变量
		variable.specifier_type = type.val;
		variable.identifier_name = name.val;

		//将新变量加入到当前的符号表中
		tables[current_table_stack.back()].InsertSymbol(variable);

		//修改符号流
		PopSymbolList(production_right.size());
		//保存当前标识符的名字
		symbol_list.push_back({ production_left,name.val,-1,-1 });
	}
	else //函数定义
	{
		//TODO：函数定义好像不需要加入到符号表(符号表是给每个作用域一个符号表)，那标识符的Function是干什么的
		//TODO:函数定义右边能拿到标识符应该是FunDec的value存的是函数名
		SemanticSymbol name = symbol_list[symbol_list.size() - 2];

		//函数定义语法归约，表示从函数内部自底向上归约出来了，所以要弹出此时的作用域
		current_table_stack.pop_back();

		//修改符号流
		PopSymbolList(production_right.size());
		//保存当前函数标识符的名字
		symbol_list.push_back({ production_left,name.val,-1,-1 });
	}
}

//VarSpecifier ::= int
void  SemanticAnalysis::TranslateVarSpecifier(const string production_left, const vector<string> production_right)
{
	//归约修改符号流即可
	SemanticSymbol type = symbol_list[symbol_list.size() - 1];
	PopSymbolList(production_right.size());
	//保存类型值
	symbol_list.push_back({ production_left,type.val,-1,-1 });//就是替换，先不改值，不改位置，只是将右侧归约成左侧
}

//FunSpecifier ::= void | int
void  SemanticAnalysis::TranslateFunSpecifier(const string production_left, const vector<string> production_right)
{
	//同样是修改符号流
	SemanticSymbol type = symbol_list[symbol_list.size() - 1];
	PopSymbolList(production_right.size());
	//保存函数类型
	symbol_list.push_back({ production_left,type.val,-1,-1 });//就是替换，先不改值，不改位置，只是将右侧归约成左侧
}
void  SemanticAnalysis::TranslateFunDec(const string production_left, const vector<string> production_right)
{
	//symbol_list的最后一个是int或void
	//同样是修改符号流
	SemanticSymbol type = symbol_list[symbol_list.size() - 1];
	PopSymbolList(production_right.size());
	//保存函数类型
	symbol_list.push_back({ production_left,type.val,-1,-1 });//就是替换，先不改值，不改位置，只是将右侧归约成左侧
}
void  SemanticAnalysis::TranslateCreateFunTable_m(const string production_left, const vector<string> production_right)
{
	//创建函数表
	//func_name函数名，func_para函数参数
	SemanticSymbol func_name = symbol_list.back();
	SemanticSymbol func_para = symbol_list[symbol_list.size() - 2];

	/*
	//在全局的table判断函数名是否重定义
	if (tables[0].FindSymbol(func_name.val) != -1) {
		cout << "语义分析中发生错误：" <<
			"函数" << func_name.val <<
			"重定义" << endl;
		throw(SEMANTIC_ERROR_REDEFINED);//抛出错误是否要考虑
	}*/
	ProcessError(func_name, NULL, NULL, FUN_ID_REDEFINED);

	//创建函数表
	tables.push_back(SemanticSymbolTable(SemanticSymbolTable::FunctionTable, func_name.val));
	//在全局符号表创建当前函数的符号项（这里参数个数和入口地址会进行回填）
	tables[0].InsertSymbol({ IdentifierInfo::Function,func_para.val,
		func_name.val,0,0,int(tables.size() - 1) });
	//这个后面的3个整型参数是干嘛的

	//函数表压入栈
	current_table_stack.push_back(tables.size() - 1);

	//返回值
	IdentifierInfo return_value;
	return_value.identifier_type = IdentifierInfo::ReturnVar;
	return_value.identifier_name = tables.back().name + "_return_value";
	return_value.specifier_type = func_para.val;
	//如果为main函数，则进行记录
	if (func_name.val == "main")
		main_index = next_quaternion_index;
	//加入四元式
	quaternion_list.push_back({ next_quaternion_index++ , func_name.val,"-","-" ,"-" });
	//向函数表中加入返回变量
	tables[current_table_stack.back()].InsertSymbol(return_value);
	//空串不需要pop
	//进行pushback
	symbol_list.push_back({ production_left,func_name.val,
		0,int(tables[0].content.size() - 1) });
}
void  SemanticAnalysis::TranslateParamDec(const string production_left, const vector<string> production_right)
{
	//symbol_list最后一个为变量名，倒数第二个为类型
	SemanticSymbol name = symbol_list.back();
	SemanticSymbol type = symbol_list[symbol_list.size() - 2];


	//当前函数表
	SemanticSymbolTable& function_table = tables[current_table_stack.back()];
	/*
	//如果已经进行过定义
	if (function_table.FindSymbol(func_name.val) != -1) {
		cout << "语义分析中发生错误：" <<
			"函数参数" << func_name.val << "重定义" << endl;
		//throw(SEMANTIC_ERROR_REDEFINED);
	}*/

	ProcessError(name, &(current_table_stack.back()), NULL, PARA_ID_REDEFINED);

	//函数表加入形参变量
	int new_position = function_table.InsertSymbol(
		{ IdentifierInfo::Variable,type.val,name.val,-1,-1,-1 });
	//当前函数在全局符号中的索引
	int table_position = tables[0].FindSymbol(function_table.name);
	//形参个数++
	tables[0].content[table_position].function_parameter_num++;

	//加入四元式
	quaternion_list.push_back({ next_quaternion_index++, "defpar","-" , "-", name.val });

	//symbol_list更新
	PopSymbolList(production_right.size());
	symbol_list.push_back({ production_left,name.val,
		current_table_stack.back(),new_position });
}
void  SemanticAnalysis::TranslateBlock(const string production_left, const vector<string> production_right)
{
	//更新symbol_list
	SemanticSymbol type = symbol_list[symbol_list.size() - 1];
	PopSymbolList(production_right.size());
	symbol_list.push_back({ production_left,
		to_string(next_quaternion_index),-1,-1 });
}
void  SemanticAnalysis::TranslateDef(const string production_left, const vector<string> production_right)
{
	//symbol_list的倒数第二个、倒数第三个是变量名和类型
	SemanticSymbol name = symbol_list[symbol_list.size() - 2];
	SemanticSymbol type = symbol_list[symbol_list.size() - 3];
	SemanticSymbolTable& current_table = tables[current_table_stack.back()];
	/*
	//重定义则报错
	if (current_table.FindSymbol(name.val) != -1)
	{
		cout << "语义分析中发生错误："
			<< "变量" << name.val << "重定义" << endl;
		//throw(SEMANTIC_ERROR_REDEFINED);
	}*/
	ProcessError(name, &(current_table_stack.back()), NULL, ID_REDEFINED);

	current_table.InsertSymbol({ IdentifierInfo::Variable,type.val,name.val ,-1,-1,-1 });

	for (int i = 0; i < production_right.size(); i++)
		symbol_list.pop_back();
	symbol_list.push_back({ production_left, name.val,
		current_table_stack.back(),int(tables[current_table_stack.back()].content.size() - 1) });
}
void  SemanticAnalysis::TranslateAssignStmt(const string production_left, const vector<string> production_right)
{
	//AssignStmt ::= <ID> = Exp
	//1.相关symbol已存入symbol_list的后部
	//2.查错机制：ID是否被定义过
	int s_size = symbol_list.size();
	SemanticSymbol exp = symbol_list[s_size - 1];
	SemanticSymbol idtf = symbol_list[s_size - 3];
	int tb_index = -1, tb_index_index = -1;
	//检查ID未定义
	ProcessError(idtf, &tb_index, &tb_index_index, ID_UNDEFIEND);
	quaternion_list.push_back({ next_quaternion_index++,"=",exp.val,"-",idtf.val });
	//process symbol_list
	int cnt = production_right.size();
	PopSymbolList(cnt);

	symbol_list.push_back({ production_left,idtf.val,tb_index,tb_index_index });

}
void SemanticAnalysis::TranslateExp(const string production_left, const vector<string> production_right)
{
	//Exp ::= AddSubExp | Exp Relop AddSubExp
	//1.两种判断方式
	//2.查错机制：无法进行查错
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	if (r_size == 1) {
		SemanticSymbol add_sub_exp = symbol_list[s_size - 1];
		PopSymbolList(r_size);

		symbol_list.push_back({ production_left,add_sub_exp.val,add_sub_exp.table_index,add_sub_exp.symbol_index });
	}
	else {
		SemanticSymbol exp1 = symbol_list[s_size - 3];
		SemanticSymbol op = symbol_list[s_size - 2];
		SemanticSymbol exp2 = symbol_list[s_size - 1];
		string var = "T" + to_string(tmp_var_count++);
		int lable_num_next = next_quaternion_index++;

		quaternion_list.push_back({ lable_num_next,"j" + op.val,exp1.val,exp2.val,to_string(lable_num_next+3) });
		quaternion_list.push_back({ next_quaternion_index++,"=" ,"0","-",var });
		quaternion_list.push_back({ next_quaternion_index++,"j" ,"-","-",to_string(lable_num_next + 4) });
		quaternion_list.push_back({ next_quaternion_index++,"=" ,"1","-",var });
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,var,-1,-1 });
	}
}
void SemanticAnalysis::TranslateAddSubExp(const string production_left, const vector<string> production_right)
{

	//AddSubExp ::= Item | Item + Item | Item - Item
	//1.三种判断方式
	//2.查错机制：无法进行查错
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	if (r_size == 1) {
		SemanticSymbol add_sub_exp = symbol_list[s_size - 1];
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,add_sub_exp.val,add_sub_exp.table_index,add_sub_exp.symbol_index });
	}
	else {
		SemanticSymbol exp1 = symbol_list[s_size - 3];
		SemanticSymbol op = symbol_list[s_size - 2];
		SemanticSymbol exp2 = symbol_list[s_size - 1];
		string var = "T" + to_string(tmp_var_count++);

		quaternion_list.push_back({ next_quaternion_index++,op.val, exp1.val, exp2.val,var });
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,var,-1,-1 });
	}
}
void SemanticAnalysis::TranslateItem(const string production_left, const vector<string> production_right)
{
	//Item ::= Factor | Factor * Factor | Factor / Factor
	//1.三种判断方式
	//2.查错机制：无法进行查错
	int r_size = production_right.size();
	int s_size = symbol_list.size();

	if (r_size == 1) {
		SemanticSymbol item_exp = symbol_list[s_size - 1];
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,item_exp.val,item_exp.table_index,item_exp.symbol_index });
	}
	else {
		SemanticSymbol exp1 = symbol_list[s_size - 3];
		SemanticSymbol op = symbol_list[s_size - 2];
		SemanticSymbol exp2 = symbol_list[s_size - 1];
		string var = "T" + to_string(tmp_var_count++);

		quaternion_list.push_back({ next_quaternion_index++,op.val, exp1.val, exp2.val,var });
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,var,-1,-1 });
	}

}
void SemanticAnalysis::TranslateFactor(const string production_left, const vector<string> production_right)
{
	//Factor ::= <INT> | ( Exp ) | <ID> | CallStmt
	//1.四种判断方式
	//2.查错机制：ID是否定义
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	if (r_size == 1) {
		SemanticSymbol exp = symbol_list[s_size - 1];
		if ("<ID>" == production_right[0]) {
			int tb_index = -1, tb_index_index = -1;
			ProcessError(exp, &tb_index, &tb_index_index, ID_UNDEFIEND);

		}
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,exp.val,exp.table_index,exp.symbol_index });
	}
	else {
		SemanticSymbol exp = symbol_list[s_size - 2];
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,exp.val,exp.table_index,exp.symbol_index });
	}
}
void SemanticAnalysis::TranslateCallStmt(const string production_left, const vector<string> production_right)
{
	//CallStmt ::= <ID> ( CallFunCheck Args )
	//1.一种判断方式
	//2.查错机制：ID是否定义 CallFunCheck检查
	//			  检查functiontalbe，函数参数是否满足要求
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	SemanticSymbol idtf = symbol_list[s_size - 5];
	SemanticSymbol chk = symbol_list[s_size - 3];
	SemanticSymbol args = symbol_list[s_size - 2];

	int value = stoi(args.val);
	int temp = -1;
	ProcessError(chk, &value, &temp, WRONG_PARAMETER_NUM);
	string var = "T" + to_string(tmp_var_count++);
	quaternion_list.push_back({ next_quaternion_index++,"call",idtf.val,"-",var });
	PopSymbolList(r_size);
	symbol_list.push_back({ production_left,var,-1,-1 });


}
void SemanticAnalysis::TranslateCallFunCheck(const string production_left, const vector<string> production_right)
{
	//CallFunCheck ::= @
	//1.一种判断方式
	//2.查错机制：func是否定义过(ID是否定义，其类型是否为func)
	int s_size = symbol_list.size();
	bool check_ID = false;
	bool check_ID_type = false;
	SemanticSymbol func_id = symbol_list[s_size - 2];
	int func_pos = tables[0].FindSymbol(func_id.val);
	if (func_pos == -1)
		check_ID = false;
	else
		check_ID = true;
	if (!check_ID)
	{
		cout << "未定义的函数名" << endl;
		throw(SEMANTIC_ERROR_UNDEFINED);
	}
	if (tables[0].content[func_pos].identifier_type != IdentifierInfo::Function)
		check_ID_type = false;
	else
		check_ID_type = true;
	if (!check_ID_type) {
		cout << "ID定义，类型不对" << endl;
		throw(SEMANTIC_ERROR_UNDEFINED);
	}
	symbol_list.push_back({ production_left,func_id.val,0,func_pos });
}
void SemanticAnalysis::TranslateArgs(const string production_left, const vector<string> production_right)
{
	//Args ::= Exp , Args | Exp | @
	//1.三种判断方式
	//2.查错机制：无法进行查错
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	if (r_size == 3)
	{
		SemanticSymbol exp = symbol_list[s_size - 3];
		int args_num = stoi(symbol_list[s_size - 1].val) + 1;
		PopSymbolList(r_size);
		quaternion_list.push_back({ next_quaternion_index++,"param",exp.val,"-","-" });
		symbol_list.push_back({ production_left,to_string(args_num),-1,-1 });
	}
	else {
		if ("Exp" == production_right[0]) {
			SemanticSymbol exp = symbol_list[s_size - 1];
			PopSymbolList(r_size);
			quaternion_list.push_back({ next_quaternion_index++,"param",exp.val,"-","-" });
			symbol_list.push_back({ production_left,"1",-1,-1 });
		}
		if ("@" == production_right[0]) {
			symbol_list.push_back({ production_left,"0",-1,-1 });
		}
	}
}
void SemanticAnalysis::TranslateReturnStmt(const string production_left, const vector<string> production_right)
{
	//ReturnStmt ::= return Exp | return
	//1.两种判断方式
	//2.查错机制：无法进行查错
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	SemanticSymbolTable tb = tables[current_table_stack.back()];

	if (r_size == 1) {
		auto type = tables[0].content[tables[0].FindSymbol(tb.name)].specifier_type;
		if (type != "void")
		{
			cout << "需要有返回值" << endl;
			throw(SEMANTIC_ERROR_NO_RETURN);
		}
		PopSymbolList(r_size);
		quaternion_list.push_back({ next_quaternion_index++,"return","-","-",tb.name });
		symbol_list.push_back({ production_left,"" - 1,-1 });
	}
	else {
		SemanticSymbol exp = symbol_list[s_size - 1];

		quaternion_list.push_back({ next_quaternion_index++,"=",exp.val,"-",tb.content[0].identifier_name });
		quaternion_list.push_back({ next_quaternion_index++,"return",tb.content[0].identifier_name,"-",tb.name });
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,exp.val,-1,-1 });
	}
}
void SemanticAnalysis::TranslateRelop(const string production_left, const vector<string> production_right)
{
	//Relop ::= > | < | >= | <= | == | !=
	//1.六种判断方式
	//2.查错机制：无法进行查错
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	SemanticSymbol exp = symbol_list[s_size - 1];
	PopSymbolList(r_size);
	symbol_list.push_back({ production_left,exp.val,-1,-1 });

}

//IfStmt ::= if IfStmt_m1 ( Exp ) IfStmt_m2 Block IfNext
void SemanticAnalysis::TranslateIfStmt(const string production_left, const vector<string> production_right)
{
	//从符号流中读取组成部分
	SemanticSymbol IfStmt_m2 = symbol_list[symbol_list.size() - 3];
	SemanticSymbol IfNext = symbol_list[symbol_list.size() - 1];

	//如果没有else，只需要回填IfStmt_m2的一个四元式
	if (IfNext.val.empty())
	{
		//回填假出口，跳出if，这里的next_quaternary_index是整体的下一条
		quaternion_list[backpatching_list.back()].result = to_string(next_quaternion_index);
		backpatching_list.pop_back();
	}
	else
		//有else，需要回填两个四元式
	{
		//回填if块出口，填的是整体出口，对应Block执行完的无条件jump的跳转目标
		quaternion_list[backpatching_list.back()].result = to_string(next_quaternion_index);
		backpatching_list.pop_back();

		//回填if假出口
		quaternion_list[backpatching_list.back()].result = IfNext.val;
		backpatching_list.pop_back();
	}

	//修改symbol list
	PopSymbolList(production_right.size());
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

	//布尔表达式真出口，可以直接写入结果
	//TODO，其实感觉可以不写这个j=，或直接写成j，或不写，因为要执行的就是下一条语句，在回填的时候也体现出来了
	quaternion_list.push_back({ next_quaternion_index++ ,"j=","-","-",to_string(next_quaternion_index)});

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
		PopSymbolList(production_right.size());
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
	SemanticSymbol whilestmt_m1 = symbol_list[symbol_list.size() - 6];
	SemanticSymbol whilestmt_m2 = symbol_list[symbol_list.size() - 2];

	// 无条件跳转到 while 的条件判断语句处
	quaternion_list.push_back({ next_quaternion_index++,"j","-","-" ,whilestmt_m1.val });

	//回填真出口
	quaternion_list[backpatching_list.back()].result = whilestmt_m2.val;
	backpatching_list.pop_back();

	//回填假出口
	quaternion_list[backpatching_list.back()].result = to_string(next_quaternion_index);
	backpatching_list.pop_back();

	backpatching_level--;

	PopSymbolList(production_right.size());

	symbol_list.push_back({ production_left,"",-1,-1 });
}
void SemanticAnalysis::TranslateWhileStmt_m1(const string production_left, const vector<string> production_right)
{
	backpatching_level++;
	symbol_list.push_back({ production_left,to_string(next_quaternion_index),-1,-1 });
}
void SemanticAnalysis::TranslateWhileStmt_m2(const string production_left, const vector<string> production_right)
{
	//假出口
	SemanticSymbol while_exp = symbol_list[symbol_list.size() - 2];

	//假出口
	quaternion_list.push_back({ next_quaternion_index++,"j=",while_exp.val,"0","" });
	backpatching_list.push_back(quaternion_list.size() - 1);
	//真出口
	quaternion_list.push_back({ next_quaternion_index++ ,"j","-","-" ,"" });
	backpatching_list.push_back(quaternion_list.size() - 1);

	symbol_list.push_back({ production_left,to_string(next_quaternion_index),-1,-1 });
}

