#pragma once
#include "grammar.h"
#include "analysis.h"
#include <vector>
#include <map>
#include <queue>
#include <fstream>
#include <iomanip>

const string out_Table_path = "../work_dir/Tables.csv";//输出表地址
const string analysis_process_path = "../work_dir/Analysis_Process.txt";//输出归约地址
const string closure_test_path = "../work_dir/Zero_Closure.txt";//输出闭包测试地址

class LR1_item
{
public:
	int left;//左侧符号序号编号
	vector<int> right;//右侧符号序号编号
	int dot_site;//中心点的位置
	int forward;//向前看的符号编号 
	int grammar_index;//这个LR1项是哪个产生式出来的,其实是有冗余，有这个index就已经有了left和right
public:
	void print();
	LR1_item() { left = 0; dot_site = 0; forward = 0; grammar_index = 0; right.push_back(0); };
	LR1_item(int l, vector<int>& r, int ds, int fw, int gi);
	bool operator==(const LR1_item& item);
	void LR1_itemInit(int l, vector<int>& r, int ds, int fw, int gi);
};


class LR1_closure
{
public:
	vector<LR1_item> key_item;//该闭包的关键项目，有没有用不知道
	vector<LR1_item> closure;//项目闭包
public:
	//TODO:这个需要考虑要不要保留
	bool isIn(LR1_item item);//返回该项目是否在该闭包中
	bool operator==(LR1_closure& clos);
	map<int, vector<int>> getShiftinSymbol();//得到可移进的字符以及项目在闭包中的位置
	vector<pair<int, int>> getReduceSymbol();//得到可以归约的符号和对应的产生式的序号
	void print(const vector<symbol>symbols);
};

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
	REJECT_
};

class ACTION_item
{
public:
	ACTION_Option op;
	int serial;//编号，根据op是移进，归约或接受而有不同的含义

	ACTION_item(ACTION_Option ac_op, int num);
	ACTION_item();
};

class GOTO_item
{
public:
	GOTO_Option op;
	int serial;//编号，在GOTO表里只有转移编号，就是闭包集中的编号

	GOTO_item(GOTO_Option goto_op, int num);
	GOTO_item();
};

class LR1_Grammar :public grammar
{
public:
	vector<LR1_item> item_sum;//存所有的项目，set没有编号
	vector<LR1_closure> closure_sum;//所有可能出现的闭包，相当于编个号
	map<pair<int, int>, int> DFA;//前面的pair是<closure的编号，符号的编号>，对应的是能连接的目标closure编号
	//相当于就是表示连接关系    
//ACTION表和DFA有区别，在于归约项，如何当该归约时表示归约产生式序号
	map<pair<int, int>, ACTION_item> ACTION;//ACTION表
	//GOTO表就是非终结符与状态之间，只有状态转移或空
	map<pair<int, int>, GOTO_item> GOTO;

	LR1_item start_item; //初始项目
	LR1_closure start_closure; //初始项目闭包
	LR1_Grammar(){};
	LR1_Grammar(const string file_path);

public:
	//初始化start_item和start_closure
	int checkClosure(); //从grammar继承的rules，从开始产生式开始，使得项目集中第一个是闭包

	LR1_closure computeClosure(vector<LR1_item>);//给定项目计算闭包

	//判断闭包集合中是否有该闭包，若有返回序号，若没有返回-1
	int getClosureIndex(LR1_closure& clos);

	//得到所有闭包，初始闭包是0号闭包，在过程中就把DFA确定了，但是这样没有序号？？？？？？？有的
	void getClosureSum();

	//计算ACTION表和GOTO表
	void computeACTION_GOTO();

	//打印ACTION和GOTO表为csv文件
	void printTables();

	//进行归约，在过程中进行打印
	void analyze(vector<unit>& lexical_res);
}; 