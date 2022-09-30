#include "analysis.h"


//多行注释的情况 没有添加 即 多行注释的情况下 不应该
 void analysis::getStrBuffer(){
    //自己读到东西 就不断的往缓冲区里添加
    //读到/n 或者 缓冲区满  进行deleNotes 
    //需要设置两个缓冲池 ，因为缓冲池满了，可能字母还没有读完
    //这时候就需要另外一个缓冲池，利用 界符 把缓冲池给分割开来，实现完整符号的读入
    //满了，处理完毕后，缓冲池交换，应用其对应的下符
    //然后将dele处理后的缓冲池 送到状态机进行分析 
    char c;
    int buffer_flag;//缓冲区是否需要轮转
    while(1)
    {
        c=fgetc(fin);
        if(c==EOF)
        {
            break;
        }
        //缓冲池满了
        if(buffer_read[buffer_choose].count==BUFFER_SIZE-2)
        {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count]=c;

            int i;
            for(i =1;i<buffer_read[buffer_choose].count;i++)
            {
                if(isDelimiter(buffer_read[buffer_choose].buffer[i]))
                {
                    int j;//分界点
                    int k;
                    for(j=0,k=i+1;k<=buffer_read[buffer_choose].count;k++,j++)
                    {
                        buffer_read[1-buffer_choose].buffer[j]=buffer_read[buffer_choose].buffer[k];
                    }
                    //count大小重新设置
                    buffer_read[1-buffer_choose].count=j-1;
                    buffer_read[buffer_choose].count=i;

                    //设置终结点
                    buffer_read[1-buffer_choose].buffer[j]='\0';
                    buffer_read[buffer_choose].buffer[i+1]='\0';

                    //缓冲区轮转
                    buffer_flag=1;
                    break;
                }
            }
        }
        else if(c=='\n')
        {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count]='\0';
        }
        else{
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count++]=c;
            continue;//继续吧
        }
        //继续处理换行后/缓冲池满后的处理
        deleNotes();
        deleSpaces();

        if(strlen(buffer_read[buffer_choose].buffer)>0)
        {
            strcpy(buffer_end.buffer,buffer_read[buffer_choose].buffer);
            //进入状态机处理 
            //注：给的缓冲区 有可能是不完整的字串 如果传入的太长了 
            //eg: "111*n"超过300个了，就会分割开，
            spearateStates();
        }

        if(buffer_flag==1)
        {
            //下一次 缓冲区轮转
            buffer_read[buffer_choose].count=0;
            buffer_choose=1-buffer_choose;
            buffer_flag=0;
        }

    }

 }//循环得到一串新的strbuffer  并经过deleNotes后 送到状态机函数中
 void analysis::deleNotes(){
    //删除注释
    char note[BUFFER_SIZE];
    char note_count=0;
    bool flag_qoute=0;
    //状态机 读到非“”包含的/进入循环
    for(int i=0;buffer_read[buffer_choose].buffer[i]!='\0';i++)
    {
        if(buffer_read[buffer_choose].buffer[i]=='"')
        {
            flag_qoute=1-flag_qoute;
            continue;
        }
        if(flag_qoute==1)
            continue;
        if(buffer_read[buffer_choose].buffer[i]=='/')
        {
            if(buffer_read[buffer_choose].buffer[i+1]=='/')
            {
                //进入 //状态 直到\0停止
                int j;
                for(j=i;buffer_read[buffer_choose].buffer[j]!='\0';j++)
                {
                    note[note_count++]=buffer_read[buffer_choose].buffer[j];
                    buffer_read[buffer_choose].buffer[j]='\0';
                }
                note[note_count]='\0';
                std::cout<<note<<std::endl;
                note_count=0;

                //也许不需要 因为 读入一行
                //开始前移
                j++;
                for(;buffer_read[buffer_choose].buffer[j]!='\0';j++,i++)
                {
                    buffer_read[buffer_choose].buffer[i]=buffer_read[buffer_choose].buffer[j];
                }
                buffer_read[buffer_choose].buffer[i]='\0';
                break;

            }
            if(buffer_read[buffer_choose].buffer[i+1]=='*')
            {
                //进入/* 状态 

                int j;
                for(j=i+2;buffer_read[buffer_choose].buffer[j]!='\0';j++)
                {
                    
                    note[note_count++]=buffer_read[buffer_choose].buffer[j];
                    if(buffer_read[buffer_choose].buffer[j]=='*'&&buffer_read[buffer_choose].buffer[j+1]=='/')
                    {
                        note[note_count]='\0';
                std::cout<<note<<std::endl;
                note_count=0;
                        break;
                    }
                }
                //开始前移
                j++;
                for(;buffer_read[buffer_choose].buffer[j]!='\0';j++,i++)
                {
                    buffer_read[buffer_choose].buffer[i]=buffer_read[buffer_choose].buffer[j];
                }
                buffer_read[buffer_choose].buffer[i]='\0';
            }
        }
    } 
 }
 void analysis::deleSpaces(){
    //界符 的空格可以删去
    //但需要判断这个是不是界符的范围内 因为 ";"肯定不算是界符

    //先不删除了 删不动了
 }