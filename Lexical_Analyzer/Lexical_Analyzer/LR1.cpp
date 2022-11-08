#include "LR1.h"

LR1_item::LR1_item(int l, vector<int>& r, int ds, int fw, int gi)
{
	this->left = l;
	this->right = r;
	this->dot_site = ds;
	this->forward = fw;
	this->grammar_index = gi;
}

bool LR1_item::operator==(LR1_item& item)
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
		if (MAP.find(present_symbol)!=MAP.end())
		{
			vector<int> target = MAP[present_symbol];
			target.push_back(i);//插入项目的序号
		}
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
	this->start_closure = computeClosure(vector<LR1_item>(1, this->start_item));
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
			if (next_closure_site == -1)
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

void LR1_Grammar::printTable()
{

}

void LR1_Grammar::analyze(string file_path)
{

}
