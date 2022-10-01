#include"base.h"


const char keyword[50][12] = { "break","case","char","class","continue","do","default","double","define",
"else","float","for","if","int","include","long","main","return","switch","typedef","void","unsigned","while","iostream" };

const char Delimiter[20] = { ',','(',')','{','}',';','<','>','#' }; //界符

const char Monocular_Operator[20] = { '+','-','*','/','!','%','~','&','|','^','=' };   //单目运算符
const char Binocular_Operator[20][5] = { "++","--","&&","||","<=","!=","==",">=","+=","-=","*=","/=" }; //双目运算符

/********************************************
 * 判断输入字符类型 是 数字 字母 还是 其他符号 状态机使用
 * 数字 字母 下划线$  /   =
 *
 * *********************************************/
int base::charKind(char c) {
    if (isLetter(c))
        return 1;
    if (isNum(c))
        return 2;
    if (c == '$' || c == '_')
        return 3;
    if (c == '\\')//解决转义问题
        return 4;
    if (c == '"')
        return 5;
    if (c == '\'')
        return 6;
    //要结束的字符
    if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ';' || c == ',' || c == '.')//这里需要注意.符号，若是在数字后面就需要考虑浮点数，这个单独考虑
        return 7;
    //if (c == '&' || c == '|' || c == '=')//两个相同的双目运算符
    //    return 7;
    //if(c=='<'||c == '>'||c=='!'||c=='*'||c=='/'|| c == '+' || c == '-')//两个可能不一样的双目运算符


    return 8;
}

/********************************************
 * 判断输入字符类型 是 界符
 *
 * *********************************************/
int base::isDelimiter(char c) {

    for (int i = 0; i < 20; i++)
    {
        if (Delimiter[i] == c)
        {
            return 1;
        }
    }
    return 0;
}


/********************************************
 * 错误判断
 *  type 1 判断标识符是否命名出错
 *  type 2 判断实常数是否命名出错
 *
 *  return 1 意味着有错误
 *  return 0 意味着没有错误
 * *********************************************/
int base::wordWrongAnalysis(char str[], int type) {
    if (type == 1)
    {
        if (isSignWord(str) == 2)
        {
            std::cout << "error wrong 命名" << std::endl;
            return 1;
        }
    }
    if (type == 2)
    {
        if (isFloat(str) == 2 || isInt(str) == 2)
        {
            std::cout << "error int/float 命名" << std::endl;
            return 1;
        }
    }
    return 0;
}


/********************************************
 * 判断输入字符类型 是 int型整数吗
 * 返回值为0 意味着 压根不是
 * 返回值为1 意味着 是，包含4531十进制整数，0456八进制整数，0x46十六进制整数
 * 返回值为2 意味着 是但int命名错误 即：出现 0x0G
 * *********************************************/
int base::isInt(char str[]) {

    return 0;
}


/********************************************
 * 判断输入字符类型 是 float 型浮点数吗
 * 返回值为0 意味着 压根不是
 * 返回值为1 意味着 是
 * 返回值为2 意味着 是但float命名错误 即：出现 0.0.1
 * *********************************************/
int base::isFloat(char str[]) {
    return 0;
}

/********************************************
 * 判断输入字符类型 是 标识符 吗？
 * 返回值为0 意味着 压根不是
 * 返回值为1 意味着 是
 * 返回值为2 意味着 是但标识符命名错误
 * *********************************************/
int base::isSignWord(char str[]) {

    if (str[0] == '_' || isLetter(str[0]) || str[0] == '$')
    {
        for (int i = 0; str[i] != '\0'; i++)
        {
            if (isSpecialSign(str[i]))
                return 2;

        }
        return 1;
    }
    return 0;
}



/********************************************
 * 判断输入字符类型 是关键字 吗
 * *********************************************/
int base::isKeyWord(char str[]) {

    for (int i = 0; i < 50; i++)
    {
        if (strcmp(str, keyword[i]) == 0)
            return 1;
    }
    return 0;
}


/********************************************
 * 判断输入字符类型 是 数字
 * *********************************************/
int base::isNum(char c) {
    if (c <= '9' && c >= '0')
        return 1;

    return 0;
}


/********************************************
 * 判断输入大小写字母类型 是 
 * *********************************************/
int base::isLetter(char c) {
    if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A'))
        return 1;
    return 0;
}



/********************************************
 * 判断输入字符类型 是 特殊符号 界符也是特殊符号
 * *********************************************/
int base::isSpecialSign(char c) {
    if (c != '_' && c != '$' && !isLetter(c) && !isNum(c))
        return 1;
    return 0;
}

base::~base()
{
}