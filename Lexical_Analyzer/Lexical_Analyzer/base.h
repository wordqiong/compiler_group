#include<iostream>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)
/****************************************
 * base 类 
 * 完成基本的类型判断  
 * ***************************************/
class base {
public:
    virtual ~base();
public:
    int charKind(char c);//判断输入字符类型 是 数字 字母 还是 其他符号 状态机使用
    int wordWrongAnalysis(char str[], int type);//错误判断
    int isDelimiter(char c);//界符
    int isDelimiter(char* c);//界符

    bool spaceCanDelete(char c);//判断空格能否删除

protected:
    int isSeparator(char c);//分隔符
    int isBracketsLeft(char c);//左括号
    int isBracketsRight(char c);//右括号
    int isBracketsLeftBig(char c);//左大括号
    int isBracketsRightBig(char c);//右大括号
    int isEnd(char c);//结束符
    int isStr(char str[]);//字符串
    int isChar(char str[]);//字串是字符


    int isInt(char str[]);//整型
    int isFloat(char str[]);//float 型 +-xx.xx e +-xx.xx
    int isFloatTool(char str[]);//float型 +-xx.xx

    int isSignWord(char str[]);// 标识符 
    int isKeyWord(char str[]);//保留字  关键字 


    int isNum(char c);//是不是数字
    int isLetter(char c);//大小写字母

    int isSpecialSign(char c);//看标识符命名是否正确

    int isBinocularOperator(char str[]);//判断双目运算符
    int isMonocularOperator(char str[]);//判断单目运算符

    int isBlank(char str[]);//判断空格
};