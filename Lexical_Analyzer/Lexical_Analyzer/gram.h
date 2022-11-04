#include<iostream>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)
typedef int item_number;
typedef int rule_number;
typedef char const* uniqstr;
typedef int location;
typedef int symbol_number;
typedef char const* symbol_name_list;

//symbol有 终结符 非终结符两种 还有undefined 默认文法不出错，简化处理
typedef enum
{
    unknown_sym,          /* Undefined.  */
    token_sym,		/* Terminal. */
    nterm_sym		/* Non-terminal */
} symbol_class;


//一个symbol 需要存什么
//类型 名字 对应的constchar*下标 
//我们将符号表集中存储在一个地方，通过number访问对应的位置
struct symbol
{
    uniqstr tag;//the symbol of name

    //我们采用一个简单的char*数组吧，记录下标
    //location number等价，简化处理
    location location;
    symbol_number number;
    symbol_class class_;
    int user_token_number;
};

//传入rule_number的编号，返回 负数，将其与符号区区分
item_number rule_number_as_item_number(rule_number ruleno) {
    return -1 - ruleno;
};
//传入symbol_number 返回itemnumber
item_number symbol_number_as_item_number(symbol_number number) {
    return number;
};

//将item_number转回rule_number
rule_number item_number_as_rule_number(item_number item)
{
    return -1 - item;
}