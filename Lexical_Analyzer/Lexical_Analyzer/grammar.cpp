#include "grammar.h"

#define DEBUG 1

//工具函数
//字符串分割函数  
vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if ("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型
	char* strs = new char[str.length() + 1];
	strcpy(strs, str.c_str());

	char* d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char* p = strtok(strs, d);
	while (p) {
		string s = p; //分割得到的字符串转换为string类型
		res.push_back(s); //存入结果数组
		p = strtok(NULL, d);
	}

	return res;
}


//去掉首尾 空格白
void Trimmed(std::string& str) {
	str.erase(0, str.find_first_not_of(" \r\n\t\v\f"));
	str.erase(str.find_last_not_of(" \r\n\t\v\f") + 1);
	if (DEBUG)
		std::cout << "Trimmed [" << str << "]" << std::endl;
}


//first集合 follow集合合并所需的函数 
//将一个非终结符的相关集合合并到另一个非终结符
// 如果前者不存在空串，那么意味着 后者也不应该存在空串
void mergeSet(set<int>& src, set<int>& dst, int null_index)
{
	set<int>temp;
	set_difference(src.begin(), src.end(),dst.begin(),dst.end(), inserter(temp, temp.begin()));
	//如果dst里没有空串，把插入后空串的删除
	bool isNullExist = dst.find(null_index)!=dst.end();
	
	dst.insert(temp.begin(), temp.end());
	if (!isNullExist)
	{
		dst.erase(null_index);
	}

}

/************* symbol *****************/
symbol::symbol(symbol_class type, const string tag) {

	this->type = type;
	this->tag = tag.c_str();
}

/************* rule *****************/

rule::rule(const int left, const vector<int>& right){

	this->left_symbol = left;
	this->right_symbol = right;
}

void rule::print() {
	
}

/************* grammar *****************/

grammar::grammar(const string file_path) {
	
	ReadGrammar(file_path);
	//初始化 所有终结符的first

	//初始化所有非终结符的first

}


void  grammar::initGrammar()
{
	if (!haveEndToken)
	{
		symbols.push_back(symbol(symbol_class::end, EndToken));
		terminals.insert(symbols.size() - 1);
	}
	if (!haveStartToken)
	{
		log_error;
	}
	if (!haveAllTerminalToken)
	{
		logerror;
	}
	if (!haveExtendStartToken)
	{
		loginfo;
		int left_symbol;
		vector<int>right_symbol_index;

		symbols.push_back(symbol(symbol_class::nterm_sym, ExtendStartToken));
		left_symbol = symbols.size() - 1;
		int right = Find_Symbol_Index_By_Token(StartToken);
		right_symbol_index.push_back(right);
		this->rules.push_back(rule(left_symbol, right_symbol_index));
	}
}

void  grammar::checkGrammar()
{
	int index = 0;
	index = Find_Symbol_Index_By_Token(EndToken);
	if (index == -1) {
		haveEndToken = false;
	}
	else {
		haveEndToken = true;
	}
	index = Find_Symbol_Index_By_Token(StartToken);
	if (index == -1) {
		haveStartToken = false;
	}
	else {
		haveStartToken = true;
	}
	index = Find_Symbol_Index_By_Token(ExtendStartToken);
	if (index == -1) {
		haveExtendStartToken = false;
	}
	else {
		haveExtendStartToken = true;
	}
	index = Find_Symbol_Index_By_Token(AllTerminalToken);
	if (index == -1) {
		haveAllTerminalToken = false;
	}
	else {
		haveAllTerminalToken = true;
	}

}

void grammar::ReadGrammar(const string file_path) {

	ifstream file_open;
	file_open.open(file_path, ios::in);
	symbols.push_back(symbol(symbol_class::epsilon, EpsilonToken));

	if (!file_open.is_open())
	{
		cout << "file open fail" << endl;
		throw FILE_OPEN_ERROR;
	}
	int rule_index=0;
	int row_num = 0;
	string rule_str;
	while (getline(file_open, rule_str, '\n'))
	{
		row_num++;
		if (rule_str.empty())
			continue;
		Trimmed(rule_str);
		if (rule_str.find("$") != rule_str.npos)
			//从$第一次出现的位置 删到结尾
			rule_str.erase(rule_str.find_first_of("$"));
		if (rule_str.empty())
			continue;

		//注释处理结束

		//开始处理rule
		vector<string> process = split(rule_str, ProToken);
		if (process.size() != 2) {
			cout << "第" << row_num << "行的产生式格式有误（每行应有且只有一个\"" << ProToken << "\"符号）" << endl;
			throw(GRAMMAR_ERROR);
		}
		string left = process[0];
		string right = process[1];

		int left_symbol = -1;
		if (left != AllTerminalToken)
		{
			left_symbol = Find_Symbol_Index_By_Token(left);
			if (left_symbol == -1)//没找到
			{
				symbols.push_back(symbol(symbol_class::nterm_sym, left));
				left_symbol = symbols.size() - 1;
				non_terminals.insert(left_symbol);
			}
		}
		//左部非终结符处理完成 
		//完成右部的分割
		vector<string>right_part = split(right, SplitToken);
		if (right_part.size() == 0)
		{
			cout << "第" << row_num << "行的产生式格式有误（没有文法号）" << endl;
			throw(GRAMMAR_ERROR);
		}
		for (auto it = right_part.begin(); it != right_part.end(); it++)
		{
			if (left_symbol == -1)
			{
				symbols.push_back(symbol(symbol_class::token_sym, *it));
				terminals.insert(symbols.size() - 1);
			}else{
				vector<int>right_symbol_index;
				//分割右部
				vector<string>right_symbol_tag = split(*it, " ");
				for (auto right_it = right_symbol_tag.begin(); right_it != right_symbol_tag.end(); right_it++)
				{
					int right_index_now = Find_Symbol_Index_By_Token(*right_it);
					if (right_index_now == -1) {
						symbols.push_back(symbol(symbol_class::nterm_sym, *right_it));
						right_index_now = symbols.size() - 1;
						non_terminals.insert(right_index_now);
					}
					right_symbol_index.push_back(right_index_now);
				}
				this->rules.push_back(rule(left_symbol, right_symbol_index));
			}
		}
	}

	file_open.close();
	//init symbols
	checkGrammar();
	initGrammar();
	

}

void grammar::print() {

}

int grammar::Find_Symbol_Index_By_Token(const string token) {

}

void grammar::InitFirst() {

}

set<int>grammar::GetFirst(const vector<int>& str) {

}