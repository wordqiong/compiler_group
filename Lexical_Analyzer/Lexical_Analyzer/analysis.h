#include"base.h"
#include<map>
using namespace std;
#define BUFFER_SIZE 300
const int KeyWord = 1;
const int SignWord = 2;
const int Integer = 3;
const int FloatPoint = 4;
const int MonocularOperator = 5;//单目运算符
const int BinocularOperator = 6;//双目运算符
const int Delimiter = 7;//界符
const int WrongWord = 8;//错误
const int Blank = 9;//空格
const int Separator = 10; //分隔符
const int BracketsLeft = 11; //左括号
const int BracketsRight = 12; //右括号
const int BracketsLeftBig = 13; //左大括号
const int BracketsRightBig = 14; //右大括号
const int End = 15; //结束符
const int Str = 16;
const int Char = 17;
const int Brackets_Left_Square = 18;
const int Brackets_Right_Square = 19;
const int Point_Arrow = 20;
const int Region = 21;
const int Region_Xigou = 22;
const int Point = 23;
const int Colon = 24;
const char keyword[50][12] = { "break","case","char","class","continue","do","default","double","define",
"else","float","for","if","int","include","long","main","return","switch","typedef","void","unsigned","while","iostream" };//24个
/*
const char* delimiter = ";"; //界符
const char* separator = ",";//分隔符
const char* bracketsLeft = "(";//左括号
const char* bracketsRight = ")";//右括号
const char* bracketsLeftBig ="{"; //左大括号
const char* bracketsRightBig ="}"; //右大括号
const char* endsign = "#";
*/
const char monocular_operator[20][5] = { "+","-","*","/","!","%","~","&","|","^","=" ,">","<"};   //单目运算符 13个
const char binocular_operator[20][5] = { "++","--","&&","||","<=","!=","==",">=","+=","-=","*=","/=","<<",">>"}; //双目运算符 14个

class unit
{
public:
    string type;
    string value;
    unit(string tp, string v);
};

class Buffer {

public:
    char* buffer;
    int count;

    Buffer() {
        count = 0;
        buffer = new char[BUFFER_SIZE];
    }
    ~Buffer() {
        delete buffer;
    }
};
class analysis :public base {


    //状态机 不只传分割的字符  还有 初步判断的类型  数字开头的只可能是 数字 字母开头的 只可能是 标识符 关键字  如果是别的 _ 标识符  特殊字符  进入运算符处理阶段
    //传入后 可以进行错误 判断 保留字查完后  查标识符  标识符有问题  就报错 
    // 即： 1  查标识符  2  查标识符 保留字 3 查特殊字符 4 查数字 
    //清理注释 
    //清理空格
    //分割单词
    //判断类型 错误输出

    //可移植性  错误那里 传入参 自己查 返回值就行 
protected:
    Buffer buffer_read[2];
    Buffer buffer_end;
    int buffer_choose;

    int note_flag;
    FILE* fin;
    FILE* fout;
    FILE* fout_pre;
    FILE* fout_lable;
    map<string, int> WordCode;
public:

    analysis();
    void getStrBuffer();//循环得到一串新的strbuffer  并经过deleNotes后 送到状态机函数中
    void deleNotes();//清除注释 
    void deleSpaces();//删除空格

    //注：请使用 buffer end缓冲区中的字符串
    void spearateStates();//状态机 调用结构体的buffer变量 返回的int 转交给kindJudge函数
    void kindJudge(char* str);//kindJudge函数 判断传入的 str 类型 并将结果转交给 print函数完成输出
    void printResult(int kind, char* str, int opt);//打印kindJudge分析出的结果
    int getWordKindCode(int kind, char* str);//获取单词种别值
    virtual ~analysis();
};