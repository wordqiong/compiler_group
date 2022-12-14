#include "analysis.h"
#define CODE_FILE_OPEN_ERROR 2
unit::unit(string tp, string v)
{
    this->type = tp;
    this->value = v;
}
//多行注释的情况 没有添加 即 多行注释的情况下 不应该
void analysis::getStrBuffer() {
    //自己读到东西 就不断的往缓冲区里添加
    //读到/n 或者 缓冲区满  进行deleNotes 
    //需要设置两个缓冲池 ，因为缓冲池满了，可能字母还没有读完
    //这时候就需要另外一个缓冲池，利用 界符 把缓冲池给分割开来，实现完整符号的读入
    //满了，处理完毕后，缓冲池交换，应用其对应的下符
    //然后将dele处理后的缓冲池 送到状态机进行分析 
    char c = '\0';
    int buffer_flag = 0;//缓冲区是否需要轮转

    while (1)
    {

        c = fgetc(fin);
        if (c == EOF)
        {
            //结束了
            deleNotes();
            deleSpaces();
            if (buffer_read[buffer_choose].count > 0)
            {
                strcpy(buffer_end.buffer, buffer_read[buffer_choose].buffer);
                buffer_end.count = buffer_read[buffer_choose].count;
                //进入状态机处理 
                //注：给的缓冲区 有可能是不完整的字串 如果传入的太长了 
                //eg: "111*n"超过300个了，就会分割开，
                buffer_read[buffer_choose].count = 0;
                fprintf(fout_pre, "%s\n", buffer_read[buffer_choose].buffer);
                spearateStates();
            }
            break;
        }

        //缓冲池满了
        if (buffer_read[buffer_choose].count == BUFFER_SIZE - 2)
        {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count] = c;
            int i;
            for (i = 0; i < buffer_read[buffer_choose].count; i++)
            {
                if (isDelimiter(buffer_read[buffer_choose].buffer[i]))
                {
                    int j;//分界点
                    int k;
                    //把buffer_choose的转移到1-buffer_choose中，
                    for (j = 0, k = i + 1; k <= buffer_read[buffer_choose].count; k++, j++)
                    {
                        buffer_read[1 - buffer_choose].buffer[j] = buffer_read[buffer_choose].buffer[k];
                    }
                    //count大小重新设置
                    buffer_read[1 - buffer_choose].count = j;
                    buffer_read[buffer_choose].count = i;

                    //设置终结点
                    buffer_read[1 - buffer_choose].buffer[j] = '\0';
                    buffer_read[buffer_choose].buffer[i + 1] = '\0';

                    //缓冲区轮转
                    buffer_flag = 1;

                    break;
                }
            }

        }
        else if (c == '\n' && !note_flag)
        {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count] = '\0';
        }
        else if (c == '\n')
        {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count] = '\0';
        }
        else {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count++] = c;
            continue;//继续吧
        }
        //继续处理换行后/缓冲池满后的处理
        deleNotes();
        deleSpaces();

        if (buffer_read[buffer_choose].count > 0)
        {
            strcpy(buffer_end.buffer, buffer_read[buffer_choose].buffer);
            buffer_end.count = buffer_read[buffer_choose].count;
            //进入状态机处理 
            //注：给的缓冲区 有可能是不完整的字串 如果传入的太长了 
            //eg: "111*n"超过300个了，就会分割开，
            buffer_read[buffer_choose].count = 0;
            fprintf(fout_pre, "%s\n", buffer_read[buffer_choose].buffer);
            spearateStates();
        }

        if (buffer_flag == 1)
        {
            //下一次 缓冲区轮转
            buffer_read[buffer_choose].count = 0;
            buffer_choose = 1 - buffer_choose;
            buffer_flag = 0;
        }

    }
    //cout << "The result of lexical analysis has been saved in the res_out.txt file." << endl;
    //cout << "The pre-processed code has been saved in the pre-processed_code.txt file." << endl;
    //cout << "The word_lable has been saved in the word-lable.txt file." << endl;
    //cout << "The analysis_res has been saved in the analysis_res.txt file." << endl;
}
//循环得到一串新的strbuffer  并经过deleNotes后 送到状态机函数中
void analysis::deleNotes() {
    //删除注释
    char note[BUFFER_SIZE];
    int note_count = 0;
    bool flag_qoute = 0;
    //状态机 读到非“”包含的/进入循环
    for (int i = 0; buffer_read[buffer_choose].buffer[i] != '\0'; i++)
    {
        if (buffer_read[buffer_choose].buffer[i] == '"')
        {
            flag_qoute = 1 - flag_qoute;
            if (note_flag != 1)
                continue;
        }
        if (flag_qoute == 1)
            if (note_flag != 1)
                continue;
        if (buffer_read[buffer_choose].buffer[i] == '/' || note_flag == 1)
        {
            if (buffer_read[buffer_choose].buffer[i + 1] == '\0')
            {
                continue;
            }
            if (buffer_read[buffer_choose].buffer[i + 1] == '/' && !note_flag)
            {
                //进入 //状态 直到\0停止
                int j;

                for (j = i; buffer_read[buffer_choose].buffer[j] != '\0'; j++)
                {
                    note[note_count++] = buffer_read[buffer_choose].buffer[j];
                    buffer_read[buffer_choose].buffer[j] = '\0';
                }
                note[note_count] = '\0';
                fprintf(fout, "[注释]----[ %s ]\n", note);
                buffer_read[buffer_choose].count -= note_count;
                note_count = 0;

                break;

            }
            if (buffer_read[buffer_choose].buffer[i + 1] == '*' || note_flag == 1)
            {
                //进入/* 状态 
                note_flag = 1;
                int j;
                for (j = i + 2 * (1 - note_flag); buffer_read[buffer_choose].buffer[j] != '\0'; j++)
                {

                    note[note_count++] = buffer_read[buffer_choose].buffer[j];
                    if (!flag_qoute && buffer_read[buffer_choose].buffer[j] == '*' && buffer_read[buffer_choose].buffer[j + 1] == '/')
                    {
                        note_flag = 0;
                        note[note_count++] = '/';
                        note[note_count] = '\0';
                        fprintf(fout, "[注释]----[ %s ]\n", note);

                        buffer_read[buffer_choose].count -= note_count;
                        note_count = 0;
                        break;
                    }
                }

                if (note_flag == 0)
                    j = j + 2;

                //开始前移

                for (; buffer_read[buffer_choose].buffer[j] != '\0'; j++, i++)
                {
                    if (buffer_read[buffer_choose].buffer[j] == '\n')
                    {
                        i--;
                        continue;
                    }
                    buffer_read[buffer_choose].buffer[i] = buffer_read[buffer_choose].buffer[j];
                }

                if (note_flag) {
                    //意味着多行注释，直接printf
                    note[note_count] = '\0';
                    fprintf(fout, "[注释]----[ %s ]\n", note);

                    buffer_read[buffer_choose].buffer[i] = '\0';
                    buffer_read[buffer_choose].count -= note_count;
                    break;




                }
                buffer_read[buffer_choose].buffer[i] = '\0';
            }

        }
    }
}
void analysis::deleSpaces() {
    //界符 的空格可以删去
    //但需要判断这个是不是界符的范围内 因为 ";"肯定不算是界符

    bool flag1 = true, flag2 = true;
    for (int i = 0; buffer_read[buffer_choose].buffer[i] != '\0'; i++)
    {
        //不能删除字符串内的空格
        if (buffer_read[buffer_choose].buffer[i] == '"')
            flag1 = !flag1;
        //不能删除空格字符
        if (buffer_read[buffer_choose].buffer[i] == '\'')
            flag2 = !flag2;
        if ((buffer_read[buffer_choose].buffer[i] == ' ' || buffer_read[buffer_choose].buffer[i] == '\t') && flag1 && flag2)
        {
            //找到空格的最后，末尾或是第一个不是空格的位置
            int j = i + 1;
            for (; buffer_read[buffer_choose].buffer[j] != '\0' && (buffer_read[buffer_choose].buffer[j] == ' ' || buffer_read[buffer_choose].buffer[j] == '\t'); j++)
            {
            }
            //如果是到末尾了，直接修改尾零位置即可
            if (buffer_read[buffer_choose].buffer[j] == '\0')
            {
                buffer_read[buffer_choose].buffer[i] = '\0';
                buffer_read[buffer_choose].count -= (j - i);
                break;
            }
            //如果是开头,直接全删就行
            if (i == 0)
            {
                int k = i;
                buffer_read[buffer_choose].count -= (j - i);
                for (; buffer_read[buffer_choose].buffer[j] != '\0'; j++, k++)
                    buffer_read[buffer_choose].buffer[k] = buffer_read[buffer_choose].buffer[j];
                buffer_read[buffer_choose].buffer[k] = '\0';
                // i--是因为原来i的位置是空格，现在被删除掉了，所以回退一个单位
                i--;
            }
            else
            {
                //如果之间有多个空格，先删到只有一个
                if (j - i >= 2)
                {
                    int k = i + 1;
                    buffer_read[buffer_choose].count -= (j - (i + 1));
                    for (; buffer_read[buffer_choose].buffer[j] != '\0'; j++, k++)
                        buffer_read[buffer_choose].buffer[k] = buffer_read[buffer_choose].buffer[j];
                    buffer_read[buffer_choose].buffer[k] = '\0';
                    //先将j移动到i+1的位置
                    j = i + 1;
                    //这里不需要i--回退
                }

                //判断空格可不可以删除，只要左右两边有一个是 周围可以没有空格就能与其他区分 的即可
                //但是例如 a > = b 这种错误写法就无法辨别，会将>与=之间的空格给吃掉
                //bool b = 1 > = 2;

                // TODO:这个要修改，可能需要修改spaceCanDelete判断函数以解决上述问题
                if (buffer_read[buffer_choose].buffer[j] != '\0' && ((spaceCanDelete(buffer_read[buffer_choose].buffer[j]) || (i > 0 && spaceCanDelete(buffer_read[buffer_choose].buffer[i - 1])))))
                    //if (buffer_read[buffer_choose].buffer[j] != '\0' && ((isDelimiter(buffer_read[buffer_choose].buffer[j]) || (i > 0 && isDelimiter(buffer_read[buffer_choose].buffer[i - 1])))))
                {
                    //把后面的移动到前面
                    int k = i;
                    buffer_read[buffer_choose].count -= (j - i);
                    for (; buffer_read[buffer_choose].buffer[j] != '\0'; j++, k++)
                        buffer_read[buffer_choose].buffer[k] = buffer_read[buffer_choose].buffer[j];
                    buffer_read[buffer_choose].buffer[k] = '\0';
                    // i--是因为原来i的位置是空格，现在被删除掉了，所以回退一个单位
                    i--;
                }

            }
        }

    }

}

//状态机，从buffer_end中读取语句并划分成单词  
//不需要考虑注释，因此/只是除法
//输入是一行预处理后的代码，将其分割为单词进行类型判定

void analysis::spearateStates()
{
    char word[BUFFER_SIZE];
    int count = 0;//当前word中的字符个数
    bool finish = false;
    int state = 0;//初态，state为0就表示了在初态

    for (int i = 0; i <= buffer_end.count; i++)
    {
        switch (state)
        {
            //初态
        case 0:
            switch (charKind(buffer_end.buffer[i]))
            {
            case 1://字母
                word[count++] = buffer_end.buffer[i];
                state = 1;
                break;
            case 2://数字
                word[count++] = buffer_end.buffer[i];
                state = 2;
                break;
            case 3://$,_
                word[count++] = buffer_end.buffer[i];
                state = 3;
                break;
            case 4://转义符只会在字符串内部使用，否则作为一个字符单独出来
                word[count++] = buffer_end.buffer[i];
                state = 4;
                break;
            case 5:
                word[count++] = buffer_end.buffer[i];
                state = 5;
                break;
            case 6:
                word[count++] = buffer_end.buffer[i];
                state = 6;
                break;
            case 7:
                word[count++] = buffer_end.buffer[i];
                state = 7;
                break;
            case 8:
                word[count++] = buffer_end.buffer[i];
                state = 8;
                break;
            case 10:
                word[count++] = buffer_end.buffer[i];
                state = 10;
                break;
            default:
                word[count++] = buffer_end.buffer[i];
                break;
            }
            break;
        case 1:
            switch (charKind(buffer_end.buffer[i]))
            {
            case 1:case 2:case 3:
                word[count++] = buffer_end.buffer[i];
                break;
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;//结束状态
            }
            break;
        case 2:
            switch (charKind(buffer_end.buffer[i]))
            {
            case 1:
            case 2:
                word[count++] = buffer_end.buffer[i];
                break;
            case 7:
                if (buffer_end.buffer[i] == '.')
                {
                    word[count++] = buffer_end.buffer[i];
                    break;
                }
                else
                {
                    word[count] = '\0';
                    i--;
                    finish = 1;
                    state = 9;//结束状态
                }
                break;
            case 8:
                //现在是+-，前面是Ee
                if ((buffer_end.buffer[i] == '+' || buffer_end.buffer[i] == '-') && (buffer_end.buffer[i - 1] == 'e' || buffer_end.buffer[i - 1] == 'E'))
                {
                    word[count++] = buffer_end.buffer[i];
                    break;
                }
                else
                {
                    word[count] = '\0';
                    i--;
                    finish = 1;
                    state = 9;//结束状态
                    break;
                }
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;//结束状态
                break;
            }
            break;
        case 3://好像$_和字母是一样的操作
            switch (charKind(buffer_end.buffer[i]))
            {
            case 1:case 2:case 3:
                word[count++] = buffer_end.buffer[i];
                break;
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;//结束状态
                break;
            }
            break;
        case 4:
            //字符串内转义符的情况在5态内部处理，这里处理单独的'\'
            word[count] = '\0';
            i--;
            finish = 1;
            state = 9;//结束状态
            break;
        case 5:
            word[count++] = buffer_end.buffer[i];
            if (buffer_end.buffer[i] == '"')
            {
                //此时一定不是初态，所以不需要判断i与1的关系
                if (buffer_end.buffer[i - 1] == '\\')
                {
                }
                else
                {
                    word[count] = '\0';
                    finish = 1;
                    state = 9;
                }
            }
            break;
        case 6:
            word[count++] = buffer_end.buffer[i];
            if (buffer_end.buffer[i] == '\'')
            {
                //还有一种情况是'\''，还是得判断
                if (buffer_end.buffer[i - 1] == '\\')
                {
                }
                else
                {
                    word[count] = '\0';
                    finish = 1;
                    state = 9;
                }
            }
            break;
        case 7:
            //要结束的字符，直接结束
            word[count] = '\0';
            i--;
            finish = 1;
            state = 9;
            break;
        case 8:
            switch (charKind(buffer_end.buffer[i]))
            {
            case 8:case 11:
                word[count++] = buffer_end.buffer[i];
                break;
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;
                break;
            }
            break;
        case 9://结束态
            //此时word已经得到，并且最后以\0结尾，故状态换成初始状态
            state = 0;
            count = 0;
            finish = 0;
            i--;
            kindJudge(word);
            break;
        case 10://空格另加
            switch (charKind(buffer_end.buffer[i]))
            {
            case 10:
                word[count++] = buffer_end.buffer[i];
                break;
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;
                break;
            }
            break;

        default:
            break;
        }
        if (buffer_end.buffer[i + 1] == '\0')
        {
            word[count] = '\0';
            kindJudge(word);
            break;
        }
    }

}


unit analysis::Generate_unit(int kind, char* str) {
    //判断是否是数据
    if (kind == Integer)
    {
        unit r("<INT>", str);
        return r;
    }
    else if (kind == FloatPoint)
    {
        unit r("<FLOAT>", str);
        return r;
    }
    else if (kind == Str)
    {
        unit r("<STRING>", str);
        return r;
    }
    else if (kind == Char)
    {
        unit r("<Char>", str);
        return r;
    }
    //判断是否是标识符
    else if (kind == SignWord)
    {
        unit r("<ID>", str);
        return r;
    }
    //判断是否出错
    else if (kind == WrongWord)
    {
        unit r("<WRONG>", str);
        return r;
    }
    //特殊处理main
    else if (kind == KeyWord && !strcmp(str, "main"))
    {
        unit r("<ID>", str);
        return r;
    }
    //处理各种符号
    else
    {
        unit r(str, str);
        return r;
    }
}

//在自动机中调用，判断从自动机输出的单词类型并输出到文件，类似<类型，原值>，同时完成analysis_res的赋值
void analysis::kindJudge(char* str)
{
    int kind = 0;
    if (isKeyWord(str) == 1) //判断是否为关键字
    {
        kind = KeyWord;
        //fprintf(fout, "  [ %s ]  ----  [ 关键字 ]\n", str);
    }
    else if (isSignWord(str) == 1)  //判断是否为标识符
        kind = SignWord;
    else if (isInt(str) == 1)//判断是否为整数
        kind = Integer;
    else if (isFloat(str) == 1)//判断是否为浮点数
        kind = FloatPoint;
    else if (isMonocularOperator(str) == 1)//判断是否为单目运算符
        kind = MonocularOperator;
    else if (isBinocularOperator(str) == 1)//判断是否为双目运算符
        kind = BinocularOperator;
    else if (isDelimiter(str) == 1)//判断是否为界符        
        kind = Delimiter;
    else if (isBlank(str) == 1)//判断是否是空格
        kind = Blank;
    else if (isSeparator(str[0]) == 1 && strlen(str) == 1)
        kind = Separator;
    else if (isBracketsLeft(str[0]) == 1 && strlen(str) == 1)
        kind = BracketsLeft;
    else if (isBracketsRight(str[0]) == 1 && strlen(str) == 1)
        kind = BracketsRight;
    else if (isBracketsLeftBig(str[0]) == 1 && strlen(str) == 1)
        kind = BracketsLeftBig;
    else if (isBracketsRightBig(str[0]) == 1 && strlen(str) == 1)
        kind = BracketsRightBig;
    else if (isEnd(str[0]) == 1 && strlen(str) == 1)
        kind = End;
    else if (isBracketsLeftSquare(str[0]) == 1 && strlen(str) == 1)
        kind = Brackets_Left_Square;
    else if (isBracketsRightSquare(str[0]) == 1 && strlen(str) == 1)
        kind = Brackets_Right_Square;
    else if (isPointArrow(str) == 1)
        kind = Point_Arrow;
    else if (isRegion(str) == 1)
        kind = Region;
    else if (isRegionXigou(str) == 1)
        kind = Region_Xigou;
    else if (strlen(str) == 1 && isColon(str[0]) == 1)
        kind = Colon;
    else if (isPoint(str[0]) == 1 && strlen(str) == 1)
        kind = Point;
    else if (isStr(str))
        kind = Str;
    else if (isChar(str))
        kind = Char;
    else
        kind = WrongWord;
    if (kind != Blank)//送到语法分析器中的中间结果不含空格
    {
        unit t = Generate_unit(kind, str);
        analysis_res.push_back(t);
    }
    printResult(kind, str, 0);
}

void analysis::showAnalysisRes()
{
    vector<unit>::iterator it;
    for (it = analysis_res.begin(); it != analysis_res.end(); it++)
    {
        char ch1[BUFFER_SIZE];
        strcpy(ch1, (*it).type.c_str());
        char ch2[BUFFER_SIZE];
        strcpy(ch2, (*it).value.c_str());
        fprintf(fout_analysis_res, "%s----%s\n", ch1, ch2);
    }
}
void analysis::printResult(int kind, char* str, int opt)
{
    if (opt == 1)
    {
        switch (kind)
        {
        case KeyWord:
            fprintf(fout, "[关键字]----[%s]\n", str);
            break;
        case SignWord:
            fprintf(fout, "[标识符]----[%s]\n", str);
            break;
        case Integer:
            fprintf(fout, "[整数]----[%s]\n", str);
            break;
        case FloatPoint:
            fprintf(fout, "[浮点数]----[%s]\n", str);
            break;
        case MonocularOperator:
            fprintf(fout, "[单目运算符]----[%s]\n", str);
            break;
        case BinocularOperator:
            fprintf(fout, "[双目运算符]----[%s]\n", str);
            break;
        case Delimiter:
            fprintf(fout, "[界符]----[%s]\n", str);
            break;
        case WrongWord:
            fprintf(fout, "[错误词]----[%s]\n", str);
            break;
        case Blank:
            fprintf(fout, "[空格]----[%s]\n", str);
            break;
        case Separator:
            fprintf(fout, "[分隔符]----[%s]\n", str);
            break;
        case BracketsLeft:
            fprintf(fout, "[左括号]----[%s]\n", str);
            break;
        case BracketsRight:
            fprintf(fout, "[右括号]----[%s]\n", str);
            break;
        case BracketsLeftBig:
            fprintf(fout, "[左大括号]----[%s]\n", str);
            break;
        case BracketsRightBig:
            fprintf(fout, "[右大括号]----[%s]\n", str);
            break;
        case End:
            fprintf(fout, "[结束符]----[%s]\n", str);
            break;
        case Str:
            fprintf(fout, "[字符串]----[%s]\n", str);
            break;
        case Char:
            fprintf(fout, "[字符]----[%s]\n", str);
            break;
        case Brackets_Left_Square:
            fprintf(fout, "[左方括号]----[%s]\n", str);
            break;
        case Brackets_Right_Square:
            fprintf(fout, "[右方括号]----[%s]\n", str);
            break;
        case Point_Arrow:
            fprintf(fout, "[箭头]----[%s]\n", str);
            break;
        case Region:
            fprintf(fout, "[域符]----[%s]\n", str);
            break;
        case Region_Xigou:
            fprintf(fout, "[析构符]----[%s]\n", str);
            break;
        case Colon:
            fprintf(fout, "[冒号]----[%s]\n", str);
            break;
        case Point:
            fprintf(fout, "[点符]----[%s]\n", str);
            break;
        default:
            fprintf(fout, "[其他]----[%s]\n", str);
            break;
        }
    }
    else
    {
        int WordKindCode = 0;
        WordKindCode = getWordKindCode(kind, str);
        switch (kind)
        {
        case KeyWord:
            fprintf(fout, "[关键字]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case SignWord:
            fprintf(fout, "[标识符]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Integer:
            fprintf(fout, "[整数]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case FloatPoint:
            fprintf(fout, "[浮点数]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case MonocularOperator:
            fprintf(fout, "[单目运算符]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case BinocularOperator:
            fprintf(fout, "[双目运算符]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Delimiter:
            fprintf(fout, "[界符]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case WrongWord:
            fprintf(fout, "[错误词]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Blank:
            fprintf(fout, "[空格]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Separator:
            fprintf(fout, "[分隔符]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case BracketsLeft:
            fprintf(fout, "[左括号]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case BracketsRight:
            fprintf(fout, "[右括号]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case BracketsLeftBig:
            fprintf(fout, "[左大括号]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case BracketsRightBig:
            fprintf(fout, "[右大括号]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case End:
            fprintf(fout, "[结束符]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Str:
            fprintf(fout, "[字符串]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Char:
            fprintf(fout, "[字符]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Brackets_Left_Square:
            fprintf(fout, "[左方括号]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Brackets_Right_Square:
            fprintf(fout, "[右方括号]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Point_Arrow:
            fprintf(fout, "[箭头]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Region:
            fprintf(fout, "[域符]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Region_Xigou:
            fprintf(fout, "[析构符]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Colon:
            fprintf(fout, "[冒号]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Point:
            fprintf(fout, "[点符]----[%d]----[%s]\n", WordKindCode, str);
            break;
        default:
            fprintf(fout, "[其他]----[%s]\n", str);
        }
    }
}

int analysis::getWordKindCode(int kind, char* str)
{
    int ret = 0;
    switch (kind)
    {
    case KeyWord:
        ret = WordCode[str];
        break;
    case SignWord:
        ret = WordCode["signword"];
        break;
    case Integer:
        ret = WordCode["integer"];
        break;
    case FloatPoint:
        ret = WordCode["float"];
        break;
    case MonocularOperator:
        ret = WordCode[str];
        break;
    case BinocularOperator:
        ret = WordCode[str];
        break;
    case Delimiter:
        ret = WordCode[";"];
        break;
    case WrongWord:
        ret = WordCode["wrongword"];
        break;
    case Blank:
        ret = WordCode["blank"];
        break;
    case Separator:
        ret = WordCode[","];
        break;
    case BracketsLeft:
        ret = WordCode["("];
        break;
    case BracketsRight:
        ret = WordCode[")"];
        break;
    case BracketsLeftBig:
        ret = WordCode["{"];
        break;
    case BracketsRightBig:
        ret = WordCode["}"];
        break;
    case End:
        ret = WordCode["#"];
        break;
    case Str:
        ret = WordCode["string"];
        break;
    case Char:
        ret = WordCode["char"];
        break;
    case Brackets_Left_Square:
        ret = WordCode["["];
        break;
    case Brackets_Right_Square:
        ret = WordCode["]"];
        break;
    case Point_Arrow:
        ret = WordCode["->"];
        break;
    case Region:
        ret = WordCode["::"];
        break;
    case Region_Xigou:
        ret = WordCode["::~"];
        break;
    case Colon:
        ret = WordCode[":"];
        break;
    case Point:
        ret = WordCode["."];
        break;
    default:
        ret = -100;
        break;
    }
    //TODO:这里修改
    return ret;
}

analysis::analysis()
{
    buffer_choose = 0;
    note_flag = 0;

    string cfile_name = "code_in.txt";
    string c;
    cout << "系统默认读入源程序文件code_in.txt" << endl << endl;
    cout << "请输入要进行词法、语法分析的源程序文件名：（如果选择默认文件，请输入e）\n";
    cin >> c;
    if (c != "e")
    {
        cfile_name = c;
        cout << "正在读入文件" << cfile_name << endl;
    }

    else {
        cout << "正在读入默认文件" << cfile_name << endl;
    }

    fin = fopen(cfile_name.c_str(), "r");
    if ((fin = fopen(cfile_name.c_str(), "r")) == NULL)
    {

        throw CODE_FILE_OPEN_ERROR;
        return;
    }

    fout = fopen("../work_dir/Lexical_Result.txt", "w");
    fout_pre = fopen("../work_dir/Pre-Processed_Code.txt", "w");
    fout_lable = fopen("../work_dir/Word_Lable.txt", "w");
    fout_analysis_res = fopen("../work_dir/Lex_to_Parse.txt", "w");
    //map赋初值
    const int keyword_size = 24;
    const int monocular_operator_size = 13;
    const int binocular_operator_size = 14;
    int cnt = 0;
    //关键字
    for (int i = 0; i < keyword_size; i++)
        WordCode[keyword[i]] = ++cnt;
    //算符
    for (int i = 0; i < monocular_operator_size; i++)
        WordCode[monocular_operator[i]] = ++cnt;
    for (int i = 0; i < binocular_operator_size; i++)
        WordCode[binocular_operator[i]] = ++cnt;
    WordCode["signword"] = ++cnt;
    WordCode["wrongword"] = ++cnt;
    WordCode["blank"] = ++cnt;
    WordCode[";"] = ++cnt;
    WordCode[","] = ++cnt;
    WordCode["("] = ++cnt;
    WordCode[")"] = ++cnt;
    WordCode["{"] = ++cnt;
    WordCode["}"] = ++cnt;
    WordCode["#"] = ++cnt;
    WordCode["."] = ++cnt;
    WordCode["["] = ++cnt;
    WordCode["]"] = ++cnt;
    WordCode["->"] = ++cnt;
    WordCode[";;"] = ++cnt;
    WordCode["::~"] = ++cnt;
    WordCode[":"] = ++cnt;
    //数字
    WordCode["integer"] = ++cnt;
    WordCode["float"] = ++cnt;

    //打印单词种别码值
    map<string, int>::iterator iter;
    iter = WordCode.begin();
    while (iter != WordCode.end()) {
        //cout << iter->first << " : " << iter->second << endl;
        fprintf(fout_lable, "%s : %d\n", iter->first.c_str(), iter->second);
        iter++;
    }
    /*
    * 测试isFloat判断含有e的浮点数
    * 预期输出1，0，0，0，0，1，1，1
    * 实际输出1，0，0，0，1，1，1*/
    /*
    char t[][20] = { "7e5","8eeee","e80","7e","+70","+70.0","70.5e-56.9","70e56"};
    for (int i = 0; i < 8; i++)
    {
        if (i == 6)
            cout << "wrong" << endl;
        cout << isFloat(t[i]) << endl;
    }
        */

}
analysis::~analysis()
{
    fclose(fin);
    fclose(fout);
}