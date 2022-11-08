#pragma once
#include "grammar.h"
#include <vector>
#include <map>

class LR1_item
{
private:
	int left;//左侧符号序号编号
	vector<int> right;//右侧符号序号编号
	int dot_site;//中心点的位置
	int forward;//向前看的符号编号 
	int grammar_index;//这个LR1项是哪个产生式出来的
public:
	LR1_item(int l, vector<int>& r, int ds, int fw, int gi);
	bool operator==(LR1_item& item);

};

////所有项目的一个集合
//class Item_Sum
//{
//private:
//	set<LR1_item> item_sum;
//public:
//	int getItemIndex(LR1_item& item);
//};

class LR1_closure
{
private:
	vector<LR1_item> key_item;//该闭包的关键项目，有没有用不知道
	vector<LR1_item> closure;//项目闭包
public:
	//TODO:这个需要考虑要不要保留
	bool isIn(LR1_item item);//返回该项目是否在该闭包中
	bool operator==(LR1_closure& closure);
};

//class Closure_Sum
//{
//private:
//	set<LR1_closure> closure_sum;
//public:
//	int getClosureIndex(LR1_closure&);
//};

//构造出DFA后，每一个DFA中的闭包都是一个状态，要在ACTION和GOTO表里面使用的 

//ACTION表中可以存在的动作
enum ACTION_Option
{
	SHIFT_IN,//移进
	REDUCE,//归约
	ACCEPT,//接受
	REJECT//拒绝
};
//GOTO表中可以存在的动作
enum GOTO_Option
{
	GO,
	REJECT
};

class ACTION_item
{
public:
	ACTION_Option op;
	int serial;//编号，根据op是移进，归约或接受而有不同的含义
};

class LR1_Grammar:grammar
{
private:
	set<LR1_item> item_sum;//存所有的项目，也进行编个号
	set<LR1_closure> closure_sum;//所有可能出现的闭包，相当于编个号
	map<pair<int, int>, int> DFA;//前面的pair是<closure的编号，符号的编号>，对应的是能连接的目标closure编号
								//相当于就是表示连接关系    
	//ACTION表和DFA有区别，在于归约项，如何当该归约时表示归约产生式序号
	map<pair<int, int>, ACTION_item> ACTION;//ACTION表
	//GOTO表就是非终结符与状态之间，只有状态转移或空
	map<pair<int, int>, GOTO_Option> GOTO;
	
public:
	//得到所有项目
	int getItemSum(); //从grammar继承的rules，从开始产生式开始，使得项目集中第一个是闭包

	LR1_closure computeClosure(vector<LR1_item>);//给定项目计算闭包

	//得到所有闭包，初始闭包是0号闭包，在过程中就把DFA确定了，但是这样没有序号？？？？？？？有的
	int getClosureSum();

	//计算ACTION表
	int computeACTION();

	//计算GOTO表
	int computeGOTO();

};
