typedef int item_number;
typedef int rule_number;
typedef char const *uniqstr;
typedef int location;
struct rule
{
    rule_number user_number;//文法中的index
    rule_number rule_number;//rule中的index
    symbol* symbol_lhs;
    item_number *rhs;

};
typedef struct  rule_list
{
    struct rule_list* next;
    rule* value;
}rule_list;

//非终结符
struct symbol
{
    uniqstr tag;//the symbol of name
    //bison里面存他们是在bit里面存的，需要有行和列的记录
    //我们采用一个简单的char*数组吧，记录下标
    location location;//the position of symbol 你的symbol 处于一个文件里，位于第几行 第几列
    /* data */
};

struct symbol_list
{
    symbol *sym;
    uniqstr type_name;
    location location;
    /* data */
    //如果有一个rule 他位于产生式的右部 并且这个symbol又是一个非终结符，MIDRULE = a pointer to that midrule.
    struct symbol_list *midrule;
    
  /* If this symbol is the generated lhs for a midrule and this is that
     midrule, MIDRULE_PARENT_RULE = a pointer to the rule in whose rhs it
     appears, and MIDRULE_PARENT_RHS_INDEX = its rhs index (1-origin) in the
     parent rule.  */
  struct symbol_list *midrule_parent_rule;
  int midrule_parent_rhs_index;

    /* The action is attached to the LHS of a rule, but action properties for
   * each RHS are also stored here.  */
  code_props action_props;

    /* The list.  */
  struct symbol_list *next;
};

