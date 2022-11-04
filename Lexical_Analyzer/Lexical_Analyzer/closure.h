#pragma once
#include"derives.h"

//first goto closure集合
//derives处理结束后，整个逻辑框架就很清楚了
//剩下的是 derives里存了各种非终结符产生的产生式
//nullable里存了各种可以产生空串的式子
//此刻 需要根据这个 完成first 集合 closure的计算

//此处自由发挥吧
typedef int* firstSet;
typedef int first_number;
struct First
{
	symbol_number number;//当前非终结符在symbol表中的位置
	symbol name;//symbol符号 里面存了是终结符还是非终结符
	//还记得 rules[]吗？ 根据ruleno 可以得到其右部的终结符
	//还记得 derives[]吗？ 存了一个非终结符可以产生的所有式子  也有右部的表达式 
	//还记得nullable吗？ 它存了 对应的ruleno 能否产生空串。
	//所以first集合实现难度 应该不大
	//存放first集合 用什么来存放？

	firstSet firstsSet;
	first_number fisrt_numbers;//当前集合存了多少个
};

//derives[]是一个数组 derives[0] A, derives[1]B  
//derives是一个rule_list 链表 数组
//derives[1]是一个链表
//derives[2]是一个链表
//....
//Relop(symbol)  relop > 链接 relop < 链接relop >= 链接relop <= 链接relop == 链接relop !=
//nullable  ruleno  
//rules[] 所有的rule
First* set;//存所有symbol的 每个下标 与symbol表里的非终止符对应 

//求first集合
//可以这么求
//你先看找到的第一个符号 是终结符 结束查找
//是非终结符，看看nullable里 存的能不能推到空串 
//推不倒，那就 把这个非终结符 加到first集合里   如果要print 那就一层层找就可以  //有可能出现互相包含的情况，这种认为文法错误吧
//能推到，再看下一个符号 直到找完


