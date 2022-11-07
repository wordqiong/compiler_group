#include "reader.h"


#define ITEM_NUMBER_MAX INT_MAX


//存放终结符和非终结符的符号集合
symbol_name_list rsymbol[];


static symbol_list* grammar;

static symbol_list* grammar_end = NULL;

//将传入的symbol 加入symbol list中
symbol_list* symbol_list_new_sym(symbol* sym, location loc)
{
    symbol_list* res = (symbol_list*)malloc(sizeof * res);
    res->content.sym = sym;
    res->location = loc;
    res->next = NULL;

    return res;
}

static void
grammar_symbol_append(symbol* sym, location loc)
{
    symbol_list* p = symbol_list_new_sym(sym, loc);

    //如果grammar有了，那么在end后部加上就行
    if (grammar_end)
        grammar_end->next = p;
    else
        //没有的话，p就是现在的grammar
        grammar = p;
    //p成了新的grammar_end
    grammar_end = p;

    //前面已经说过 NULL是一个分隔符，不要放在里面
    //A->ABaCd
    //A A B a C d NULL
    if (sym)
        //如果sym不是null，右部产生式符号加一
        ++nritems;
}

//TODO 读入语法规则 调用grammar_symbol_append，建立grammar链表
void gram_parse(void)
{
    //对读入的grammar进行处理 
    //统计其中多少个终结符
    //统计其中多少个非终结符
    //将读入的文法添加到grammar list里
    //将终结符和非终结符的符号 存在rsymbols里 可以考虑写个hash
    //注意symbol 在symbol_list里的存储
}
symbol* endtoken;//
static bool start_flag = false;


//完成rule 的定义
static void packgram(void)
{

    unsigned int itemno = 0;
    rule_number ruleno = 0;
    symbol_list* p = grammar;
    ritem = (int *)calloc(nritems + 1, sizeof * ritem);
    rules = (rule*)calloc(nrules, sizeof * rules);
    while (p)
    {
        int rule_length = 0;
        rules[ruleno].user_number = ruleno;
        rules[ruleno].number = ruleno;
        rules[ruleno].lhs = p->content.sym;
        rules[ruleno].rhs = ritem+itemno;
        for (p = p->next; p && p->content.sym; p = p->next)
        {
            rule_length++;
            if (rule_length == INT_MAX)
            {
                assert(rule_length == INT_MAX, "rule too long");
            }
            ritem[itemno++] = symbol_number_as_item_number(p->content.sym->number);

        }
        ritem[itemno++] = rule_number_as_item_number(ruleno);
        //ExtDefList ::= ExtDef ExtDefList
        // ExtDefList ::= ExtDef
        // map<int,char*> symbol;
        //ritem  0 2 [-1-rule_index] 2 1 3 -3(-1-rule_index)
        assert(itemno < ITEM_NUMBER_MAX);
        ruleno++;
        assert(ruleno < ITEM_NUMBER_MAX);
        if (p)//p不会是NULL，只是p.symbol为null
            p = p->next;
    }
}


static void check_and_convert_grammer(void)
{
    if (nrules == 0)
        assert("no rules in the input grammar");

    /* If the user did not define her ENDTOKEN, do it now. */
    if (!endtoken)
    {
        //此处无需进行 按照咱们约定的格式 end token是需要被定义的
        /*endtoken = symbol_get("$end", empty_location);
        endtoken->class_ = token_sym;
        endtoken->number = 0;

        endtoken->user_token_number = 0;*/
    }
    if (!start_flag)
    {
        //如果没有定义start符号 即开始符号
        symbol_list* node;
        for (node = grammar;
            node != NULL ;
            node = node->next)
        {
            for (node = node->next;
                node != NULL && node->content.sym != NULL;
                node = node->next)
                ;
        }
        assert(node != NULL);
        grammar_start_symbol_set(node->content.sym,
            node->content.sym->location);
    }

    {
        //accept 
        //拓展文法 加进去 
        //S->S'
        symbol_list* p = symbol_list_new_sym(accept, empty_location);
        p->location = grammar->location;
        p->next = symbol_list_new_sym(startsymbol, empty_location);
        //p->next->next = symbol_list_new_sym(endtoken, empty_location);
        p->next->next->next = symbol_list_new_sym(NULL, empty_location);
        p->next->next->next->next = grammar;
        nrules += 1;
        nritems += 3;
        grammar = p;
    }
    packgram();
}

symbol* errtoken = NULL;
symbol* undeftoken = NULL;
symbol* endtoken = NULL;
symbol* accept = NULL;
symbol* startsymbol = NULL;
location startsymbol_location;

void reader(void)
{
    symbols_new();

    //约定 输入的文法 需要满足
    //拓展文法用的
    /* Construct the accept symbol. */
    accept = symbol_get("$accept", empty_location);
    accept->class_ = nterm_sym;
    accept->number = nvars++;

    /* Construct the error token */
    errtoken = symbol_get("error", empty_location);
    errtoken->class_ = token_sym;
    errtoken->number = ntokens++;



    //openfile
    FILE* file = fopen(filename, "r");
    //gram_parse()
    gram_parse();
    check_and_convert_grammer();

    fclose(file);

}