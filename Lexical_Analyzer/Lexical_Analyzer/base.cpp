#include"base.h"


const char keyword[50][12] = { "break","case","char","class","continue","do","default","double","define",
"else","float","for","if","int","include","long","main","return","switch","typedef","void","unsigned","while" };//23个

const char Delimiter = { ';' }; //界符
const char Seprater = ',';
const char Brackets_Left = '(';
const char Brackets_Right = ')';
const char Brackets_Left_Big = '{';
const char Brackets_Right_Big = '}';
const char End = '#';

const char Monocular_Operator[20] = { '+','-','*','/','!','%','~','&','|','^','>','<','='};   //单目运算符 13个
const char Binocular_Operator[20][5] = { "++","--","&&","||","<=","!=","==",">=","+=","-=","*=","/=","<<",">>"}; //双目运算符 12个

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
    if (c == ' ')
        return 10;

    return 8;
}

/********************************************
 * 判断输入字符类型 是 界符
 *
 * *********************************************/
int base::isDelimiter(char c) {

    if (Delimiter == c)
        return 1;
    return 0;

}

/********************************************
 * 判断输入字符类型 是 界符
 *
 * *********************************************/
int base::isDelimiter(char* c) {
    if (strlen(c) == 1)
    {
        if (Delimiter == c[0])
            return 1;
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

bool base::spaceCanDelete(char c)
{
    //单边判断，认为有可能组成双目运算符的算符身边空格不可删除，但是像f = 5这种空格也不会删掉了
    //需要添加=，避免有= =出现
    if (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == '>' || c == '<' || c == '!')
        return false;
    if ((c > 'z' || (c < 'a' && c > 'Z') || (c < 'A' && c > '9') || (c < '0')) && c != '_' && c != '$')
        return true;
    return false;
}


/********************************************
 * 判断输入字符类型 是 int型整数吗
 * 返回值为0 意味着 压根不是
 * 返回值为1 意味着 是，包含4531十进制整数，0456八进制整数，0x46十六进制整数
 * 返回值为2 意味着 是但int命名错误 即：出现 0x0G
 * *********************************************/
int base::isInt(char str[]) {
    const int OCT = 8;
    const int DEC = 10;
    const int HEX = 16;
    int intType = DEC;
    int len = 0;
    len = strlen(str);

    //判断数的进制
    if (isNum(str[0]))
    {
        if (str[0] == '0' && str[1] != '\0')//判断是八进制还是十六进制，当首位是0且数字不止一位时考虑下一位，
        {
            if (isNum(str[1]))
                intType = OCT;
            else if (str[1] == 'x' || str[1] == 'X')
                intType = HEX;
            else
                return 0;
        }
        else
            intType = DEC;
    }
    else {
        return 0;
    }
    //检查数的格式是否正确
    switch (intType)
    {
    case OCT:
        for (int i = 1; i < len; i++)//八进制数首位为0，长度至少为2.
        {
            if (isNum(str[i]))
            {
                if (str[i] >= '0' && str[i] <= '7')
                    ;
                else
                    return 2;
            }
            else
                return 0;
        }
        return 1;
        break;
    case DEC:
        for (int i = 0; i < len; i++)
        {
            if (isNum(str[i]))
                ;
            else
                return 0;
        }
        return 1;
        break;
    case HEX:
        for (int i = 2; i < len; i++)//十六进制数前两位为0x\0X，长度至少为3.
        {
            if (isNum(str[i]) || (str[i] >= 'a' && str[i] <= 'f') || (str[i] >= 'A' && str[i] <= 'F'))
                ;
            else
            {
                if (!isLetter(str[i]))//十六进制数中出现非字母
                    return 0;
                else
                    return 2;
            }
        }
        return 1;
        break;
    }
    return 0;
}


/********************************************
 * 判断输入字符类型 是 float 型浮点数吗
 * 返回值为0 意味着 压根不是
 * 返回值为1 意味着 是 包括xx.xx和xx e\E xx两种情况
 * 返回值为2 意味着 是但float命名错误 即：出现 0.0.1
 * *********************************************/
int base::isFloat(char str[]) {
    int len = 0;
    int dotAppearNum = 0;
    len = strlen(str);
    //判断78e\E56的情况
    int pos = -1;
    for (int i = 0; i < len; i++)
    {
        if (str[i] == 'e' || str[i] == 'E')
        {
            pos = i;
            break;
        }
    }
    if (pos != -1)//出现了e/E
    {
        if (pos == 0 || pos == len - 1)//e在首位或末尾
            return 0;
        for (int j = 0; j < pos; j++)
            if (!isNum(str[j]))
                return 0;
        for (int j = pos+1; j <len; j++)
            if (!isNum(str[j]))
                return 0;
        return 1;
    }
    //判断xx.xx的情况
    for (int i = 0; i < len; i++)
    {
        if (len > 0)//确保有首位和最后一位
        {
            if (i == 0)//判断首位
            {
                if (!isNum(str[i]))//不是数字
                {
                    if (str[i] == '.')
                        return 2;
                    else
                        return 0;
                }
            }
            if (i < len - 1)//判断非最后一位且不是首位
            {
                if (!isNum(str[i]) && str[i] != '.')//既不是数字也不是小数点
                    return 0;
                if (str[i] == '.')//如果是小数点
                {
                    dotAppearNum++;
                    if (dotAppearNum > 1)//出现多个小数点的情况
                        return 2;
                }
            }
            if (i == len - 1)//判断末位
            {
                //既不是数字也不是f\F、d\D
                if (!isNum(str[i]) && str[i] != 'f' && str[i] != 'F' && str[i] != 'd' && str[i] != 'D')
                    return 0;
            }
        }
    }

    return 1;
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
 * 判断输入字符类型 是双目运算符 吗
 * *********************************************/
int base::isBinocularOperator(char str[]) {

    for (int i = 0; i < 20; i++)
    {
        if (strcmp(str, Binocular_Operator[i]) == 0)
            return 1;
    }
    return 0;
}

/********************************************
 * 判断输入字符类型 是单目运算符 吗
 * *********************************************/
int base::isMonocularOperator(char str[]) {
    if (strlen(str) == 1) {
        for (int i = 0; i < 20; i++)
        {
            if (str[0] == Monocular_Operator[i])
                return 1;
        }
    }
    return 0;
}
/********************************************
 * 判断输入字符类型 是空格 吗
 * *********************************************/
int base::isBlank(char str[])
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] != ' ')
            return 0;
    }
    return 1;
    //if (strlen(str) == 1 && str[0] == ' ') {
    //    return 1;
    //}
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

/********************************************
 * 判断输入字符类型 是 分隔符
 * *********************************************/
int base::isSeparator(char c)
{
    if (Seprater == c)
        return 1;
    return 0;
}

/********************************************
 * 判断输入字符类型 是 左括号
 * *********************************************/
int base::isBracketsLeft(char c)
{
    if (Brackets_Left == c)
        return 1;
    return 0;
}

/********************************************
 * 判断输入字符类型 是 右括号
 * *********************************************/
int base::isBracketsRight(char c)
{
    if (Brackets_Right == c)
        return 1;
    return 0;

}

/********************************************
 * 判断输入字符类型 是 左大括号
 * *********************************************/
int base::isBracketsLeftBig(char c)
{
    if (Brackets_Left_Big == c)
        return 1;
    return 0;

}

/********************************************
 * 判断输入字符类型 是 右大括号
 * *********************************************/
int base::isBracketsRightBig(char c)
{
    if (Brackets_Right_Big == c)
        return 1;
    return 0;

}

/********************************************
 * 判断输入字符类型 是 结束符
 * *********************************************/
int base::isEnd(char c)
{
    if (End == c)
        return 1;
    return 0;
}

/********************************************
 * 判断输入字串类型 是 字串 "xxxxxx"
 * *********************************************/
int base::isStr(char str[]) 
{
    //小型状态机
    int i = 0;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] == ' ')
            continue;
        else
            break;
    }
    int flag = 0;
    //此时i为非' '的首字符
    if (str[i] == '"')
    {
        i++;
        flag = 1;//第一个"
    }
    if (flag == 1)
    {
        for (i; i < strlen(str); i++)
        {
            if (str[i] == '"' && str[i - 1] != '\\')
                return 1;
        }
    }
    return 0;
}

/********************************************
 * 判断输入字串类型 是 字符 'x'
 * *********************************************/
int base::isChar(char str[]) 
{
    //小型状态机
    int i = 0;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] == ' ')
            continue;
        else
            break;
    }
    int flag = 0;
    //此时i为非' '的首字符
    if (str[i] == '\'')
    {
        i++;
        flag = 1;//第一个"
    }
    if (flag == 1)
    { 
        //只允许 '\x'
        if (str[i] == '\\')
        {
            if (isNum(str[i]))
            {
                flag = 2;//10进制
            }
            else if (str[i] == 'x')
            {
                flag = 3;//16进制
            }
            else
            {
                if (str[i + 2] == '\'')
                    return 1;
          
            }
            if (flag == 2 && isInt(&str[i]) && strlen(&str[i]) == 3)
                return 1;
            if (flag == 3 && isInt(&str[i]) && strlen(&str[i]) == 5)
                return 1;

        }
        else {
            if (str[i + 1] == '\'')
                return 1;
            return 0;
        }

    }
    return 0;
}

base::~base()
{
}