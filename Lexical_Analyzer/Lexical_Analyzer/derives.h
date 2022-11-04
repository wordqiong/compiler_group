#include "reader.h"

typedef struct rule_list
{
    struct rule_list* next;
    rule* value;
}rule_list;

rule_list*** derives;


static void print_derives(void)
{
    //输出derives 即当前的链接产生式
    //做一个检验吧
}

void derives_compute(void)
{
    symbol_number i;
    rule_number r;
    rule** q;

    rule_list** dset =(rule_list**) calloc(nvars, sizeof * dset);

    rule_list* delts = (rule_list*)calloc(nrules, sizeof * delts);

    for (r = nrules - 1; r >= 0; --r)
    {
        //开始建立关联
        symbol_number lhs = rules[r].lhs->number;
        rule_list* p = &delts[r];

        p->next = dset[lhs - ntokens];
        p->value = &rules[r];
        dset[lhs - ntokens] = p;
    }

    derives = (rule_list***)calloc(nvars, sizeof * derives);
    q = (rule **)calloc(nvars + nrules, sizeof * q);

    //开始转为链表
    for (i = ntokens; i < nsyms; i++)
    {
        rule_list* p = dset[i - ntokens];
        derives[i - ntokens] = q;
        while (p)
        {
            *q++ = p->value;
            p = p->next;
        }
        *q++ = NULL;//标志着当前产生式建立完毕

    }
    //输出相关符号
    if (debug)
    {
        print_derives();
    }
    free(dset);
    free(delts);
}
bool* nullable = NULL;
static void nullable_print()
{
    //输出空串文件
}

//计算空串 也很巧妙
void nullable_compute(void)
{
    rule_number ruleno;
    symbol_number* s1;
    symbol_number* s2;
    rule_list* p;

    symbol_number* squeue =(symbol_number*) calloc(nvars, sizeof * squeue);

    size_t* rcount = (size_t*)calloc(nrules, sizeof * rcount);
    rule_list** rsets =(rule_list**) calloc(nvars, sizeof * rsets);

    rule_list* relts =(rule_list*) calloc(nritems + nvars + 1, sizeof * relts);

    nullable = (bool*)calloc(nvars, sizeof * nullable);

    s1 = s2 = squeue;
    p = relts;

    for (ruleno = 0; ruleno < nrules; ruleno++)
    {
        //默认输入的所有规则都用得上
        rule* rules_ruleno = &rules[ruleno];
        if (rules_ruleno->rhs[0] >= 0)
        {
            //右部存的是非空RHS
            item_number* rp = NULL;
            bool any_tokens = false;
            for (rp = rules_ruleno->rhs; *rp >= 0;rp++)
            {
                if (ISTOKEN(*rp))
                    any_tokens = true;
            }
            if (!any_tokens)
            {
                //说明右边都是非终结符，那么需要mark一下
                for (rp = rules_ruleno->rhs; *rp >= 0; rp++)
                {
                    //计数，记下这个ruleno出现的次数
                    rcount[ruleno]++;
                    //链入rsets中
                    p->next = rsets[*rp - ntokens];
                    p->value = rules_ruleno;
                    rsets[*rp - ntokens] = p;
                    p++;
                }
            }
        }
        else {
            //是规则编号 为负数 为空串
            //rhs为转置后的负号 需要变回来
            assert(rules_ruleno->rhs[0] == ruleno);
            if (!nullable[rules_ruleno->lhs->number - ntokens])
            {
                nullable[rules_ruleno->lhs->number - ntokens] = true;
                *s2++ = rules_ruleno->lhs->number;
            }
        }
    }
    while (s1 < s2)
    {
        for (p = rsets[*s1++ - ntokens]; p; p = p->next)
        {
            rule* r = p->value;
            //如果这个为零，说明该规则的右部都可以推导到NULL
            //也就是说 
            if (--rcount[r->number] == 0)
            {
                if (!nullable[r->lhs->number - ntokens])
                {
                    nullable[r->lhs->number - ntokens] = true;
                    *s2++ = r->lhs->number;
                }
            }
        }
    }
    free(squeue);
    free(rcount);
    free(rsets);
    free(relts);
}

void
nullable_free(void)
{
    free(nullable);
}
