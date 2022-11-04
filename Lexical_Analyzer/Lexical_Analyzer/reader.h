#include"gram.h"
#include<sys/types.h>
#include <assert.h>

# define ISTOKEN(i)	((i) < ntokens)
# define ISVAR(i)	((i) >= ntokens)



//右部产生式的符号 与符号集合对应起来
//eg rsymbol[2]=A; rsymbol[1]=a;
//B->Aa (1);
// C->Ba (2);
//ritem[0]=2;ritem[1]=1;ritem[2]=-1-(1)=-2;
extern int* ritem;
extern int nritems;//右部产生式产生了多少个符号
extern int nrules;//经过 B->Aa|bC;拆分后，有多少个规则
extern int ntokens;//有多少个终结符
extern int nvars;//多少个非终结符
extern int nsyms;//一共多少个符号


//一个symbol_list长什么样
//A->ABaCd
// 拆分后 每个list都有一个symbol，最后以NULL结尾，标志着这个
// 产生式结束
//A A B a C d NULL
struct symbol_list
{
    union
    {
        symbol* sym;
        uniqstr type_name;
    }content;
    location location;
    /* The list.  */
    struct symbol_list* next;
};

//rule 存放对应的规则 分成左部符号和右部符号存储
struct rule
{
    rule_number user_number;//文法中的index
    rule_number number;//rule中的index
    symbol* lhs;
    item_number* rhs;
};

extern rule* rules;

