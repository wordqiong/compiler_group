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

set<int> LR1_closure::getReduceSymbol()
{
	set<int> SET;
	for (int i = 0; i < this->closure.size(); i++)
	{
		LR1_item temp = this->closure[i];
		if (temp.dot_site >= temp.right.size())//点到头了，是归约项目
			SET.insert(temp.forward);
		else
			continue;		
	}
	return SET;
}

int LR1_Grammar::getClosureSum()
{
	//得到初始闭包
	this->start_closure = computeClosure(vector<LR1_item>(1, this->start_item));
	this->closure_sum.insert(this->start_closure);

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
		}

	}


	return 0;
}

