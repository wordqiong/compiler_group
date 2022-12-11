#include "LR1.h"

LR1_item::LR1_item(int l, vector<int>& r, int ds, int fw, int gi)
{
	this->left = l;
	this->right = r;
	this->dot_site = ds;
	this->forward = fw;
	this->grammar_index = gi;
}
void LR1_item::LR1_itemInit(int l, vector<int>& r, int ds, int fw, int gi)
{
	this->left = l;
	this->right = r;
	this->dot_site = ds;
	this->forward = fw;
	this->grammar_index = gi;
}
bool LR1_item::operator==(const LR1_item& item)
{
	if (this->grammar_index == item.grammar_index && this->dot_site == item.dot_site && this->forward == item.forward)
		return true;
	return false;
}

bool LR1_closure::isIn(LR1_item item)
{
	for (int i = 0; i < this->closure.size(); i++)
	{
		if (item == this->closure[i])
			return true;
	}
	return false;
}


bool LR1_closure::operator==(LR1_closure& clos)
{
	if (this->closure.size() != clos.closure.size())
		return false;

	//TODO:变成两层循环了，看看怎么能更简单一些
	//这里写的不对，只判断了clos的项目是不是在本closure内，没有反向判断，或这样的等号条件是不正确的，但是两个size是相等的才会到这里
	for (int i = 0; i < clos.closure.size(); i++)
	{
		if (!this->isIn(clos.closure[i]))
			return false;
	}
	return true;
}

map<int, vector<int>> LR1_closure::getShiftinSymbol()
{
	map<int, vector<int>> MAP;
	for (int i = 0; i < this->closure.size(); i++)
	{
		LR1_item temp = this->closure[i];
		if (temp.dot_site >= temp.right.size())//点到头了，是归约项目
			continue;
		int present_symbol = temp.right[temp.dot_site];//当前点后面的字符
		//存在该字符
		if (MAP.find(present_symbol) != MAP.end())
			MAP[present_symbol].push_back(i);//插入项目的序号
		else
		{
			vector<int> target;
			target.push_back(i);
			MAP[present_symbol] = target;//插入
		}
	}
	return MAP;
}

vector<pair<int, int>> LR1_closure::getReduceSymbol()
{
	vector<pair<int, int>> VEC;
	for (int i = 0; i < this->closure.size(); i++)
	{
		LR1_item temp = this->closure[i];
		if (temp.dot_site >= temp.right.size())//点到头了，是归约项目
		{
			VEC.push_back(pair<int, int>(temp.forward, temp.grammar_index));
		}
		else
			continue;
	}
	return VEC;
}

int LR1_Grammar::getClosureIndex(LR1_closure& clos)
{
	for (int i = 0; i < this->closure_sum.size(); i++)
	{
		if (clos == this->closure_sum[i])
			return i;
	}
	return -1;
}

ACTION_item::ACTION_item(ACTION_Option ac_op, int num)
{
	this->op = ac_op;
	this->serial = num;
}
ACTION_item::ACTION_item()
{
}
GOTO_item::GOTO_item(GOTO_Option goto_op, int num)
{
	this->op = goto_op;
	this->serial = num;
}
GOTO_item::GOTO_item()
{
}
void LR1_Grammar::getClosureSum()
{
	//得到初始闭包
	//this->start_closure = computeClosure(vector<LR1_item>(1, this->start_item));、
	//初始闭包加入到闭包集合中
	this->closure_sum.push_back(this->start_closure);

	//应该是用队列循环求出链接情况
	queue<LR1_closure> q;
	q.push(this->start_closure);

	while (!q.empty())
	{
		LR1_closure source = q.front();
		q.pop();

		map<int, vector<int>> shift_symbol = source.getShiftinSymbol();
		//set<int> reduce_symbol = source.getReduceSymbol();

		for (map<int, vector<int>>::iterator it = shift_symbol.begin(); it != shift_symbol.end(); it++)
		{
			//有了此时移进的字符
			//需要求得点向后移动一位后的项目，然后组成vector求出闭包，再加入队列q，加入闭包集合，并标记在DFA中
			vector<LR1_item> next_closure_key;
			for (int j = 0; j < it->second.size(); j++)
			{
				LR1_item temp = source.closure[it->second[j]];
				temp.dot_site++;//点向后移动一位，其他均不变
				/* ************************************/
				//TODO:这里要不要改成序号，就是项目集要不要编号
				/* *************************************/
				next_closure_key.push_back(temp);//所有在该字符下可以移进的LR1项目
			}

			LR1_closure next_closure = computeClosure(next_closure_key);
			int next_closure_site = getClosureIndex(next_closure);
			if (next_closure_site == -1)//不存在
			{
				q.push(next_closure);
				this->closure_sum.push_back(next_closure);
			}
			//不管在不在，DFA都要加上边
			this->DFA[pair<int, int>(getClosureIndex(source), it->first)] = getClosureIndex(next_closure);

		}

	}

}

void LR1_Grammar::computeACTION_GOTO()
{
	//计算完成DFA和闭包集后，构造ACTION表，<closure的编号，符号的编号>,动作
	//记录的是所有的终结符，这里只记录存在的状态
	//DFA中记录的都是转移，还需要记录归约状态，里面是产生式的序号
	//所有的REJECT状态都没有记录

	//根据DFA得到移进动作
	for (auto it = this->DFA.begin(); it != this->DFA.end(); it++)
	{
		//转移符号是终结符，记录在ACTION表中
		if (terminals.find(it->first.second) != terminals.end())
		{
			ACTION_item act_item(ACTION_Option::SHIFT_IN, it->second);
			this->ACTION[pair<int, int>(it->first.first, it->first.second)] = act_item;
		}
		else
		{
			//非终结符记录在GOTO表中
			GOTO_item goto_item(GOTO_Option::GO, it->second);
			this->GOTO[pair<int, int>(it->first.first, it->first.second)] = goto_item;
		}
	}

	//根据闭包的归约项得到归约/接受动作
	for (int i = 0; i < this->closure_sum.size(); i++)
	{
		vector<pair<int, int>> reduce_line = this->closure_sum[i].getReduceSymbol();
		for (int j = 0; j < reduce_line.size(); j++)
		{
			//在第i个闭包状态，遇到终结符reduce_line[j].first，要使用reduce_line[j].second产生式归约
			//**********************************************
			// （TODO:需不需要看前向符号是#？）
			//如果该归约项目是初始归约项目，则应设置为接受状态

			if (reduce_line[j].second == start_location)
			{
				ACTION_item act_item(ACTION_Option::ACCEPT, reduce_line[j].second);
				this->ACTION[pair<int, int>(i, reduce_line[j].first)] = act_item;
			}
			else
			{
				ACTION_item act_item(ACTION_Option::REDUCE, reduce_line[j].second);
				this->ACTION[pair<int, int>(i, reduce_line[j].first)] = act_item;
			}
		}

	}
}
void LR1_Grammar::printTables()
{
	const int width = 10;
	const int interval = 10;
	const int start_state = 5;
	const int state_action = 20;
	const int action_goto = 230;

	ofstream ofs(out_Table_path, ios::out);
	ofs.setf(std::ios::left);
	ofs << "STATE" << "," << "ACTION" << "," << "GOTO" << endl;

	int rowNum = this->closure_sum.size();
	int columnNum = this->symbols.size();
	int actionNum = this->terminals.size();
	int gotoNum = this->non_terminals.size();
	vector<int> terminal_site;//记录终结符在symbols里面的序号
	vector<int> non_terminal_site;//记录非终结符在symbols里面的序号

	ofs << ",";

	/*************
	要确保terminal和non_terminal加起来是symbols的总和
	***************/
	for (int i = 0; i < columnNum; i++)
	{
		if (terminals.find(i) != terminals.end())
		{
			if (symbols[i].tag == ",")
				ofs << "\"" << symbols[i].tag << "\"" << ",";
			else
				ofs << symbols[i].tag << ",";
			terminal_site.push_back(i);
		}
	}

	for (int i = 0; i < columnNum; i++)
	{
		if (non_terminals.find(i) != non_terminals.end())
		{

			ofs << symbols[i].tag << ",";
			non_terminal_site.push_back(i);
		}
	}

	ofs << endl;

	for (int i = 0; i < rowNum; i++)
	{
		ofs << i << ",";
		for (int j = 0; j < actionNum; j++)
		{
			auto it = ACTION.find(pair<int, int>(i, terminal_site[j]));
			if (it != ACTION.end())
			{
				if (it->second.op == ACTION_Option::SHIFT_IN)
					ofs << "s" << it->second.serial << ",";
				else if (it->second.op == ACTION_Option::REDUCE)
					ofs << "r" << it->second.serial << ",";
				else if (it->second.op == ACTION_Option::ACCEPT)
					ofs << "acc" << ",";
			}
			else
				ofs << ",";
		}
		for (int j = 0; j < gotoNum; j++)
		{
			auto it = GOTO.find(pair<int, int>(i, non_terminal_site[j]));
			if (it != GOTO.end())
				ofs << it->second.serial << ",";
			else
				ofs << ",";
		}

		ofs << endl;
	}
	ofs.close();
}

int LR1_Grammar::analyze(vector<unit>& lexical_res)
{
	vector<int> status_stack;//状态栈
	vector<int> symbol_stack;//符号栈
	int step = 0;

	unit end(EndToken, EndToken);
	lexical_res.push_back(end);//在输入串的最后加上结束符号
	status_stack.push_back(0);//状态栈先压入状态0
	symbol_stack.push_back(Find_Symbol_Index_By_Token(EndToken));//在符号栈中先放入结束符号

	//语义分析初始化
	semantic_analysis = SemanticAnalysis();
	semantic_analysis.AddSymbolToList({ StartToken,"",-1,-1 });

	ofstream ofs(analysis_process_path, ios::out);

	const int width = 5;
	const int interval = 10;
	const int start_step = 10;
	const int step_status = 20;
	const int status_symbol = 30;
	const int symbol_lex = 150;

	ofs << setw(start_step) << "STEP" << setw(step_status) << "STATUS_STACK" << setw(status_symbol) << "SYMBOL_STACK" << setw(symbol_lex) << "INPUT" << endl;

	//输出第一行
	ofs << setw(start_step) << step;
	ofs << setw(start_step);
	for (int t = 0; t < status_stack.size(); t++)
		ofs << " " << status_stack[t];
	ofs << setw(status_symbol);
	for (int t = 0; t < symbol_stack.size(); t++)
		ofs << symbols[symbol_stack[t]].tag;
	ofs << setw(symbol_lex);
	for (int t = 0; t < lexical_res.size(); t++)
		ofs << lexical_res[t].value;
	ofs << endl;
	step++;

	int err_code = 0;
	int reduce_error_status = -1;
	int reduce_error_symbol = -1;
	//开始进行语法分析
	for (int i = 0; i < lexical_res.size(); i++)
	{
		//每次从输入串读入一个终结符，放入符号栈中，看当前状态下遇到该符号后ACTION表中的操作
		//如果是转移，就状态栈中加入转移后的状态
		//如果是归约，就使用归约规则，将符号栈中涉及归约的项换成右侧表达式，状态栈中删去相同数量的状态，并从GOTO表中查此时状态遇到该非终结符应转移到哪里
		//并将转移后的状态压入状态栈
		//当遇到ACTION中为acc时，结束，或reject（即ACTION表中找不到转移），则结束（GOTO中找不到也是错误）
		string present_terminal = lexical_res[i].type;
		int present_terminal_serial = Find_Symbol_Index_By_Token(present_terminal);
		int present_status = status_stack.back();
		auto it = ACTION.find(pair<int, int>(present_status, present_terminal_serial));

		//不存在，即REJECT，错误退出
		if (it == ACTION.end())
		{
			err_code = 1;
		}
		else
		{
			switch (it->second.op)
			{
			case ACTION_Option::SHIFT_IN:
			{
				//移进
				status_stack.push_back(it->second.serial);//新状态入栈
				symbol_stack.push_back(present_terminal_serial);//读入的终结符压栈
				
				//SHIFT_IN 移进，也需要移进语义分析的符号流
				semantic_analysis.AddSymbolToList({ lexical_res[i].type ,lexical_res[i].value ,-1,-1 });
				
				break;
			}
			case ACTION_Option::REDUCE:
			{
				//归约，要归约则当前输入串不加一！！
				i--;
				rule rule_need = rules[it->second.serial];//要使用的产生式
				int right_length = rule_need.right_symbol.size();//要归约掉的长度
				if (right_length == 1)
				{
					//特判epsilon，因为存的size是1，但实际length是0
					if (rule_need.right_symbol[0] == Find_Symbol_Index_By_Token("@"))
						right_length = 0;
				}
				for (int k = 0; k < right_length; k++)
				{
					status_stack.pop_back();//状态栈移出
					symbol_stack.pop_back();//符号栈移出
				}
				symbol_stack.push_back(rule_need.left_symbol);//符号栈压入非终结符

				int temp_status = status_stack.back();

				//归约之后查看GOTO表
				auto goto_it = GOTO.find(pair<int, int>(temp_status, rule_need.left_symbol));
				if (goto_it == GOTO.end())//不存在转移，则应退出GOTO，编译错误
				{
					err_code = 2;
					reduce_error_status = temp_status;
					reduce_error_symbol = rule_need.left_symbol;
					break;
				}
				else
				{
					if (goto_it->second.op == GOTO_Option::GO)
					{
						status_stack.push_back(goto_it->second.serial);//将新状态压栈
					
						//语义分析，归约，使用归约时的产生式，在语法分析归约完全成功时调用
						vector<string> production_right;
						for (int i = 0; i < rule_need.right_symbol.size(); i++)
							production_right.push_back(symbols[rule_need.right_symbol[i]].tag);

						semantic_analysis.Analysis(symbols[rule_need.left_symbol].tag, production_right);

					}
					else//不会出现
					{
						err_code = 2;
						break;
					}
				}
				break;
			}
			case ACTION_Option::ACCEPT:
			{
				//接受状态，直接返回
				ofs << endl << "Parse successfully!" << endl;
				ofs.close();
				return err_code;
				break;
			}
			case ACTION_Option::REJECT:
			{
				err_code = 1;
				break;
			}
			default:
				break;
			}
		}
		//有error，直接退出
		if (err_code == 1)
		{
			ofs << endl << "Parse Error:Non-existed action!" << endl;
			ofs << "Present Status: " << present_status << endl;
			ofs << "Present Terminal Type: " << present_terminal << endl;
			break;
		}
		else if (err_code == 2)
		{
			ofs << endl << "Parse Error:Non-existed goto!" << endl;
			ofs << "Present Status: " << reduce_error_status << endl;
			ofs << "Present NonTerminal: " << symbols[reduce_error_symbol].tag << endl;
			break;
		}

		//输出这一行
		ofs << setw(start_step) << step;
		ofs << setw(start_step);
		for (int t = 0; t < status_stack.size(); t++)
		{
			ofs << " " << status_stack[t];
		}
		ofs << setw(status_symbol);
		for (int t = 0; t < symbol_stack.size(); t++)
		{
			ofs << symbols[symbol_stack[t]].tag;
		}
		ofs << setw(symbol_lex);
		for (int t = i + 1; t < lexical_res.size(); t++)
		{
			ofs << lexical_res[t].type;
		}
		ofs << endl;
		step++;
	}
	ofs.close();
	return err_code;
}
void LR1_Grammar::generateTree(vector<unit>& lexical_res)
{
	int node_serial = 0;
	vector<int> status_stack;//状态栈
	vector<int> symbol_stack;//符号栈
	vector<int> serial_stack;//用于记录符号栈中的tmp值
	int step = 0;

	unit end(EndToken, EndToken);
	lexical_res.push_back(end);//在输入串的最后加上结束符号
	status_stack.push_back(0);//状态栈先压入状态0
	symbol_stack.push_back(Find_Symbol_Index_By_Token(EndToken));//在符号栈中先放入结束符号

	ofstream ofs(tree_dot_path, ios::out);
	ofs << "digraph mygraph {\n";
	step++;
	int err_code = 0;
	for (int i = 0; i < lexical_res.size(); i++)
	{
		string present_terminal = lexical_res[i].type;
		int present_terminal_serial = Find_Symbol_Index_By_Token(present_terminal);
		int present_status = status_stack.back();
		auto it = ACTION.find(pair<int, int>(present_status, present_terminal_serial));

		//不存在，即REJECT，错误退出
		if (it == ACTION.end())
		{
			err_code = 1;
		}
		else
		{
			switch (it->second.op)
			{
			case ACTION_Option::SHIFT_IN:
			{
				//移进
				status_stack.push_back(it->second.serial);//新状态入栈
				symbol_stack.push_back(present_terminal_serial);//读入的终结符压栈
				serial_stack.push_back(node_serial);
				ofs << "n" << node_serial++ << "[label=\"" << lexical_res[i].value << "\",color=red];" << endl;
				break;
			}
			case ACTION_Option::REDUCE:
			{
				//归约，要归约则当前输入串不加一！！
				i--;
				rule rule_need = rules[it->second.serial];//要使用的产生式
				int right_length = rule_need.right_symbol.size();//要归约掉的长度
				if (right_length == 1)
				{
					//特判epsilon，因为存的size是1，但实际length是0
					if (rule_need.right_symbol[0] == Find_Symbol_Index_By_Token("@"))
						right_length = 0;
				}
				vector<int> drop;
				for (int k = 0; k < right_length; k++)
				{
					status_stack.pop_back();//状态栈移出
					symbol_stack.pop_back();//符号栈移出
					drop.push_back(serial_stack.back());
					serial_stack.pop_back();
				}
				symbol_stack.push_back(rule_need.left_symbol);//符号栈压入非终结符
				int temp_status = status_stack.back();

				//归约之后查看GOTO表
				auto goto_it = GOTO.find(pair<int, int>(temp_status, rule_need.left_symbol));
				if (goto_it == GOTO.end())//不存在转移，则应退出GOTO，编译错误
				{
					err_code = 2;
					break;
				}
				else
				{
					if (goto_it->second.op == GOTO_Option::GO)
					{
						status_stack.push_back(goto_it->second.serial);//将新状态压栈
						serial_stack.push_back(node_serial);
					}
					else//不会出现
					{
						err_code = 2;
						break;
					}
				}

				ofs << "n" << node_serial++ << "[label=\"" << symbols[rule_need.left_symbol].tag << "\"];\n";
				if (right_length == 0)
				{
					ofs << "e" << node_serial << "[label=\"@\"];\n";
					ofs << "n" << node_serial - 1 << " -> " << "e" << node_serial << ";\n";
				}
				else
				{
					for (auto t = drop.begin(); t != drop.end(); t++)
						ofs << "n" << node_serial - 1 << " -> " << "n" << *t << ";\n";
				}

				break;
			}
			case ACTION_Option::ACCEPT:
			{
				//接受状态，直接返回
				ofs << "}";
				ofs.close();
				return;
				break;
			}
			case ACTION_Option::REJECT:
			{
				err_code = 1;
				break;
			}
			default:
				break;
			}
		}
		//有error，直接退出
		if (err_code != 0)
			break;
		step++;
	}
	ofs.close();

}
void LR1_item::print() {

}
void LR1_closure::print(const vector<symbol>symbols)
{
	//输出所有的key
	fstream file_open;
	file_open.open(closure_test_path, ios::out);
	for (int i = 0; i < key_item.size(); i++)
	{
		file_open << "rule" << i << "  ";
		file_open << "left: " << symbols[key_item[i].left].tag;
		file_open << "  right: ";
		for (int j = 0; j < key_item[i].right.size(); j++)
		{
			if (j == key_item[i].dot_site)
			{
				file_open << " * ";
			}
			file_open << symbols[key_item[i].right[j]].tag << " ";
		}
		if (key_item[i].dot_site == key_item[i].right.size())
			file_open << " * ";
		file_open << "     terim:  " << symbols[closure[i].forward].tag;
		file_open << endl;
	}
	//输出所有的key
	for (int i = 0; i < closure.size(); i++)
	{
		file_open << "rule" << i << "  ";
		file_open << "left: " << symbols[closure[i].left].tag;
		file_open << "  right: ";
		for (int j = 0; j < closure[i].right.size(); j++)
		{
			if (j == closure[i].dot_site)
			{
				file_open << " * ";
			}
			file_open << symbols[closure[i].right[j]].tag << " ";
		}
		if (closure[i].dot_site == closure[i].right.size())
			file_open << " * ";
		file_open << "     terim:  " << symbols[closure[i].forward].tag;
		file_open << endl;
	}
}
LR1_closure LR1_Grammar::computeClosure(vector<LR1_item> lr1)
{
	//传入核心项
	//计算其闭包
	LR1_closure closure_now;
	closure_now.key_item = lr1;
	closure_now.closure = lr1;
	//遍历核心项
	for (int i = 0; i < closure_now.closure.size(); i++) {
		//处理当前rule
		LR1_item item_now = closure_now.closure[i];
		//如果*在最后一个位置
		if (item_now.dot_site >= item_now.right.size())
		{
			continue;
		}
		//当前rule下，dot后的符号对应的下标
		int dot_next_symbol_index = item_now.right[item_now.dot_site];
		symbol dot_next_symbol = symbols[dot_next_symbol_index];
		//开始符号判断
		// 如果这玩意后面是个空串，那么设置为后继
		if (dot_next_symbol.type == symbol_class::epsilon)
		{
			closure_now.closure[i].dot_site++;
			continue;
		}
		//如果这玩意后面是个终结符 那就不用加
		if (dot_next_symbol.type == symbol_class::token_sym)
		{
			continue;
		}
		//如果这玩意后面是个非终结符，把这个非终结符的first加进来
		//将dot后面从第二个开始所有的字符和加入的终结符合并，求一个first集合
		vector<int>BetaA(item_now.right.begin() + item_now.dot_site + 1, item_now.right.end());
		BetaA.push_back(item_now.forward);
		//初始化完成
		set<int> BetaAset = GetFirst(BetaA);
		//A->α·Bβ,a 
		//B->XX ,first(β,a)
		//完成此步的添加
		//遍历所有rule，找到对应的rule规则
		for (int j = 0; j < rules.size(); j++)
		{
			rule rule_now = rules[j];
			if (dot_next_symbol_index != rule_now.left_symbol)
				continue;
			//开始加入到closure里 
			//此处仍需要判断右部产生式是不是空串
			//空串 dot位置在末端
			//遍历first
			for (auto it = BetaAset.begin(); it != BetaAset.end(); it++) {
				//closure里是否有这项？
				bool have_exist = false;
				bool is_epsilon = false;
				is_epsilon = (symbols[rule_now.right_symbol[0]].type == symbol_class::epsilon);
				for (auto temp = closure_now.closure.begin(); temp != closure_now.closure.end(); temp++)
				{
					if (*temp == LR1_item(rule_now.left_symbol, rule_now.right_symbol, have_exist, *it, j))
					{
						have_exist = true;
						break;
					}
				}
				//如果没有
				if (!have_exist)
				{
					closure_now.closure.push_back(LR1_item(rule_now.left_symbol, rule_now.right_symbol, is_epsilon, *it, j));
				}

			}
		}

	}
	return closure_now;
}
//check
int LR1_Grammar::checkClosure()
{
	//related var 
	// vector<LR1_item> item_sum
	// LR1_item start_item
	// vector<rule>rules;
	// start_location

	start_item.LR1_itemInit(rules[0].left_symbol, rules[0].right_symbol, 0, Find_Symbol_Index_By_Token(EndToken), start_location);
	vector<LR1_item>lr1;
	lr1.push_back(start_item);
	start_closure = computeClosure(lr1);
	start_closure.print(this->symbols);
	return 0;

}
LR1_Grammar::LR1_Grammar(const string file_path)
{
	ReadGrammar(file_path);
	//初始化 所有终结符的first
	print();
	//初始化所有非终结符的first
	InitFirst();
}