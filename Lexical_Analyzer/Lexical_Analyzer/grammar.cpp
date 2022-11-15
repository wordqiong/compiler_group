#include "grammar.h"
#define FILE_OPEN_ERROR 1
#define DEBUG 0

//工具函数
//字符串分割函数  
vector<string> split(const string& str, const string& pattern) {
	vector<string> res;
	if (str == "")
		return res;
	//在字符串末尾也加入分隔符，方便截取最后一段
	string strs = str + pattern;
	size_t pos = strs.find(pattern);

	while (pos != strs.npos)
	{
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		//去掉已分割的字符串,在剩下的字符串中进行分割
		strs = strs.substr(pos + pattern.length(), strs.size());
		pos = strs.find(pattern);
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
int mergeSet(set<int>& src, set<int>& dst, int null_index)
{
	set<int>temp;
	set_difference(src.begin(), src.end(), dst.begin(), dst.end(), inserter(temp, temp.begin()));
	//如果dst里没有空串，把插入后空串的删除
	bool isNullExist = dst.find(null_index) != dst.end();
	int pre_size = dst.size();
	dst.insert(temp.begin(), temp.end());
	if (!isNullExist)
	{
		dst.erase(null_index);
	}
	return pre_size < dst.size();
}

/************* symbol *****************/
symbol::symbol(symbol_class type, const string tag) {

	this->type = type;
	this->tag = tag;
}

/************* rule *****************/

rule::rule(const int left, const vector<int>& right) {

	this->left_symbol = left;
	this->right_symbol = right;
}

/************* grammar *****************/

grammar::grammar(const string file_path) {

	ReadGrammar(file_path);
	//初始化 所有终结符的first
	print();
	//初始化所有非终结符的first
	InitFirst();

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
		//log_error;
	}
	if (!haveAllTerminalToken)
	{
		//logerror;
	}
	if (!haveExtendStartToken)
	{
		//loginfo;
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
		return;
	}
	int rule_index = 0;
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
			cout << process[0] << " " << process[1] << endl;
			cout << process.size() << rule_str;
			//throw(GRAMMAR_ERROR);
			return;
		}
		string left = process[0];
		string right = process[1];
		Trimmed(left);
		int left_symbol = -1;
		if (left != AllTerminalToken)
		{
			left_symbol = Find_Symbol_Index_By_Token(left);
			if (left_symbol == -1)//没找到
			{
				Trimmed(left);
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
			//throw(GRAMMAR_ERROR);
			return;
		}
		for (auto it = right_part.begin(); it != right_part.end(); it++)
		{
			if (left_symbol == -1)
			{
				Trimmed(*it);
				symbols.push_back(symbol(symbol_class::token_sym, *it));
				terminals.insert(symbols.size() - 1);
			}
			else {
				vector<int>right_symbol_index;
				//分割右部
				vector<string>right_symbol_tag = split(*it, " ");
				for (auto right_it = right_symbol_tag.begin(); right_it != right_symbol_tag.end(); right_it++)
				{
					int right_index_now = Find_Symbol_Index_By_Token(*right_it);
					if (right_index_now == -1) {
						Trimmed(*right_it);
						if ((*right_it).length() == 0)
							continue;
						symbols.push_back(symbol(symbol_class::nterm_sym, *right_it));
						right_index_now = symbols.size() - 1;
						non_terminals.insert(right_index_now);
					}

					right_symbol_index.push_back(right_index_now);
				}
				this->rules.push_back(rule(left_symbol, right_symbol_index));
				//拓展文法产生式在rule中的位置
				if (symbols[left_symbol].tag == ExtendStartToken)
				{
					start_location = rules.size() - 1;
					//cout << "拓展产生式在rule中的位置" << start_location << endl;
				}

			}
		}
	}

	file_open.close();
	//init symbols
	checkGrammar();
	initGrammar();


}

void grammar::print() {
	//输出当前的终结符集合
	// 输出当前的非终结符集合

	fstream file_open;
	file_open.open("../work_dir/Grammar_Rules.txt", ios::out);
	file_open << symbols[0].tag << " ";
	file_open << "终结符" << endl;
	for (set<int>::iterator i = terminals.begin(); i != terminals.end(); i++)
	{
		int index = *i;
		file_open << symbols[index].tag << " ";
	}
	file_open << endl;
	file_open << "非终结符" << endl;
	for (set<int>::iterator i = non_terminals.begin(); i != non_terminals.end(); i++)
	{
		int index = *i;
		file_open << symbols[index].tag << "  ";
	}
	file_open << endl;

	//输出所有的rules
	for (int i = 0; i < rules.size(); i++)
	{
		file_open << "rule" << i << "  ";
		file_open << "left: " << symbols[rules[i].left_symbol].tag;
		file_open << "  right: ";
		for (int j = 0; j < rules[i].right_symbol.size(); j++)
			file_open << symbols[rules[i].right_symbol[j]].tag << " ";
		file_open << endl;
	}

}

//返回对应符号在symbol中的下标，如果不存在，返回-1
int grammar::Find_Symbol_Index_By_Token(const string token) {

	//遍历符号表 找到对应的位置
	//symbol符号表
	bool have_find = 0;
	int index = 0;
	for (int i = 0; i < symbols.size(); i++)
	{
		if (token == symbols[i].tag)
		{
			have_find = 1;
		}
		else {
			have_find = 0;
		}
		if (have_find)
		{
			index = i;
			break;
		}
		index = i;
	}
	if (have_find)
		return index;
	else {
		return -1;
	}

	return 0;
}

void grammar::InitFirst() {

	InitFirstTerm();
	InitFirstNonTerm();
	//ProcessFirst();
	PrintFirst();
}
void grammar::ProcessFirst()
{
	//处理非终结符中first集合包含非终结符的情况
	for (set<int>::iterator it = non_terminals.begin(); it != non_terminals.end(); it++)
	{
		bool is_no_term_appear = 0;
		while (1)
		{
			is_no_term_appear = 0;
			for (set<int>::iterator i = symbols[*it].first_set.begin(); i != symbols[*it].first_set.end(); i++)
			{
				if (symbols[*i].type == symbol_class::nterm_sym)
				{
					symbols[*it].first_set.erase(*i);
					is_no_term_appear = 1;
					break;
				}
			}
			if (!is_no_term_appear)
				break;
		}

	}
}
void grammar::InitFirstTerm() {
	//如果是终结符，first集合就是他们本身
	for (set<int>::iterator i = terminals.begin(); i != terminals.end(); i++)
	{
		symbols[*i].first_set.insert(*i);
	}
}

void grammar::InitFirstNonTerm() {
	//如果是非终结符
	//遍历所有非终结符的产生式
	//右部是终结符或空串，那么加入first集合
	//如果是非终结符，将这个非终结符的first集合合并， 那么判断这个非终结符能不能推出空串，能得话继续
	//不能的话，将其到期结束
	//下一次的时候 如果是终结符，加进去，也结束此次的添加
	bool useful_action;
	while (1) {
		//遍历非终结符
		bool useful_action = 0;;
		for (set<int>::iterator i = non_terminals.begin(); i != non_terminals.end(); i++)
		{
			//遍历rule
			for (int j = 0; j < rules.size(); j++)
			{
				//如果第一个是终结符 空串
				//加入后 寻找下一个
				if (rules[j].left_symbol != *i)
					continue;
				if (symbols[rules[j].right_symbol[0]].type == symbol_class::token_sym || symbols[rules[j].right_symbol[0]].type == symbol_class::epsilon)
				{
					useful_action = symbols[*i].first_set.insert(rules[j].right_symbol[0]).second || useful_action;
					continue;
				}
				//非终结符
				bool isAllNull = 1;
				int k = 0;
				for (k = 0; k < rules[j].right_symbol.size(); k++)
				{
					//如果右部是终结符
					if (symbols[rules[j].right_symbol[k]].type == symbol_class::token_sym)
					{
						useful_action = symbols[*i].first_set.insert(rules[j].right_symbol[k]).second || useful_action;

						isAllNull = false;
						break;
						//找下一条文法
					}
					else {
						//如果右部是非终结符
						useful_action = mergeSet(symbols[rules[j].right_symbol[k]].first_set, symbols[*i].first_set, Find_Symbol_Index_By_Token(EpsilonToken)) || useful_action;
						isAllNull = symbols[rules[j].right_symbol[k]].first_set.count(Find_Symbol_Index_By_Token(EpsilonToken)) && isAllNull;

						if (!isAllNull)
							break;
					}
				}
				if (k == rules[j].right_symbol.size() && isAllNull)
					useful_action = symbols[*i].first_set.insert(Find_Symbol_Index_By_Token(EpsilonToken)).second || useful_action;
				//如果右部是非终结符
				//迭代右部symbol表
				//看有无空串 是不是终结符

			}

		}
		if (useful_action == 0)
			break;
	}
}
void grammar::PrintFirst()
{
	fstream file_open;
	file_open.open("../work_dir/First_Set_Test.txt", ios::out);
	//输出每个symbol的first集合
	for (set<int>::iterator it = terminals.begin(); it != terminals.end(); it++)
	{
		file_open << "symbol name: ";
		file_open << symbols[*it].tag;
		file_open << " first set: ";

		for (set<int>::iterator i = symbols[*it].first_set.begin(); i != symbols[*it].first_set.end(); i++)
		{
			file_open << symbols[*i].tag << " ";
		}
		file_open << endl;
	}
	for (set<int>::iterator it = non_terminals.begin(); it != non_terminals.end(); it++)
	{
		file_open << "symbol name: ";
		file_open << symbols[*it].tag;
		file_open << " first set: ";

		for (set<int>::iterator i = symbols[*it].first_set.begin(); i != symbols[*it].first_set.end(); i++)
		{
			file_open << symbols[*i].tag << " ";
		}
		file_open << endl;
	}
}

//传入一组字串，返回他们的first集合
set<int>grammar::GetFirst(const vector<int>& str) {
	set<int>first_set;
	// above all 是不是空串
	if (str.empty()) {
		return first_set;
	}
	//1.判断空串是否需要加入
	//2.判断是终结符还是非终结符
	// 3.判断非终结符能否推导出空串
	bool is_epsilon = true;
	int empty_location = Find_Symbol_Index_By_Token(EpsilonToken);
	for (auto i = str.begin(); i != str.end(); i++)
	{
		if (symbols[*i].type == symbol_class::token_sym)
		{
			is_epsilon = false;
			mergeSet(symbols[*i].first_set, first_set, empty_location);

			break;
		}
		if (symbols[*i].type == symbol_class::epsilon)
		{
			is_epsilon = false;
			first_set.insert(*i);
			break;
		}
		mergeSet(symbols[*i].first_set, first_set, empty_location);
		is_epsilon = symbols[*i].first_set.count(empty_location) && is_epsilon;
		if (!is_epsilon)
			break;

	}
	if (is_epsilon)
		first_set.insert(empty_location);
	return first_set;
}