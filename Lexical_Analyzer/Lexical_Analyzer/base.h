#include<iostream>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)
/****************************************
 * base 类 
 * 完成基本的类型判断  
 * ***************************************/
class base{
public:
    
    
    virtual ~base();
public:
    int charKind(char c );//判断输入字符类型 是 数字 字母 还是 其他符号 状态机使用
    int wordWrongAnalysis(char str[],int type);//错误判断
    int isDelimiter(char c );//界符 可以用来删除空格
    bool spaceCanDelete(char c);//判断空格能否删除

protected:
    
    int isInt(char str[]);//整型
    int isFloat(char str[]);//float 型

    int isSignWord(char str[]);// 标识符 
    int isKeyWord(char str[]);//保留字  关键字 


    int isNum(char c);//是不是数字
    int isLetter(char c);//大小写字母

    int isSpecialSign(char c);//看标识符命名是否正确

   

};