#include <iostream>
using namespace std;
/********************************************
 * 判断输入字符类型 是 数字
 * *********************************************/
int isNum(char c) {
    if (c <= '9' && c >= '0')
        return 1;

    return 0;
}


/********************************************
 * 判断输入大小写字母类型 是
 * *********************************************/
int isLetter(char c) {
    if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A'))
        return 1;
    return 0;
}
/********************************************
 * 输入参数：char str[]，以'\0'结尾
 * 判断输入字符类型 是 int型整数吗
 * 返回值为0 意味着 压根不是
 * 返回值为1 意味着 是，包含4531十进制整数，0456八进制整数，0x46\0X46十六进制整数
 * 返回值为2 意味着 是但int命名错误 即：出现 0x0G
 * *********************************************/
int isInt(char str[]) {
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
}

/********************************************
 * 输入参数：char str[]，以'\0'结尾
 * 判断输入字符类型 是 float、double型浮点数吗
 * 返回值为0 意味着 压根不是（包括非最后一位出现非数字，最后一位出现非f\F、d\D、数字的情况）
 * 返回值为1 意味着 是
 * 返回值为2 意味着 是但float命名错误 即：出现 0.0.1或.1(出现多个小数点或首位出现小数点)
 * *********************************************/
int isFloat(char str[]) {

    int len = 0;
    int dotAppearNum = 0;
    len = strlen(str);
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

int main()
{
	char numInt[9][10] = {
		//三个正确样例
		"0123",
		"0",
		"0x456",
		//两个不完全错的样例
		"0891",
		"0Xt9",
		//三个完全错的样例
		"056,",
		"456htj",
		"0x*9",
		NULL
	};
	char numFloat[9][10] = {
		//三个正确样例
		"0.123",
		"6.4567f",
		"9.452D",
		//两个不完全错的样例
		".12",
		"0.0.1",
		//三个完全错的样例
		"0.56t",
		"456htj",
		"ss.456",
		NULL
	};
    for (int i = 0; i<=7; i++)
        cout << isInt(numInt[i]) << endl;
    cout << "==================================" << endl;
    for (int i = 0; i <= 7; i++)
        cout << isFloat(numFloat[i]) << endl;
	return 0;
}