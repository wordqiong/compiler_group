#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
#include<set>
#include<string>
#include<fstream>
#include<algorithm>
#include<map>
using namespace std;

typedef enum
{
    unknown_sym,          /* Undefined.  */
    token_sym,		/* Terminal. */
    nterm_sym,		/* Non-terminal */
    epsilon, /* null */
    end /* end terminal*/
}symbol_class;
typedef string uniqstr;

//单个符号相关属性
class symbol {
public:
    symbol_class type;//文法符号种类
    set<int> first_set;//记录该symbol的first符号对应的symbol表下标
    set<int>follow_set;//记录该symbol的follow符号对应的symbol表下标
    uniqstr tag;//符号名

    symbol(symbol_class type, const string tag);
};

//单个文法的结构
class rule {
public:
    int left_symbol;
    vector<int> right_symbol;

    rule(const int left, const vector<int>& right);

};

const string EpsilonToken = "@";
const string SplitToken = " | ";// 产生式右部分隔符
const string ProToken = "::=";// 产生式左右部分隔符
const string EndToken = "#";// 尾token 终止符号
const string StartToken = "Program";// 文法起始符号
const string ExtendStartToken = "S";// 扩展文法起始符号
const string AllTerminalToken = "%token";//所有的终结符

class grammar {
public:
    vector<symbol>symbols;//所有的符号表

    set<int>terminals;//终结符在symbol中的下标
    set<int>non_terminals;//非终结符在symbol中的下标
    vector<rule>rules;//所有的文法 
    int start_location;//起始产生式在rules中的位置
    grammar(){};
    grammar(const string file_path);
    //从file中读入grammar
    void ReadGrammar(const string file_path);
    void print();

    int Find_Symbol_Index_By_Token(const string token);

    //初始化first集合
    void InitFirst();
    void InitFirstTerm();
    void InitFirstNonTerm();
    void PrintFirst();
    void ProcessFirst();
    //返回一个符号串的first集合
    set<int>GetFirst(const vector<int>& str);
    //
public:
    bool haveStartToken;
    bool haveAllTerminalToken;
    bool haveExtendStartToken;
    bool haveEndToken;
    void initGrammar();
    void checkGrammar();

};