#pragma once
//first goto closure集合
//derives处理结束后，整个逻辑框架就很清楚了
//剩下的是 derives里存了各种非终结符产生的产生式
//nullable里存了各种可以产生空串的式子
//此刻 需要根据这个 完成first 集合 closure的计算

//此处自由发挥吧


static void
set_firsts(void);
static void
set_fderives(void);
void
new_closure(unsigned int n);

void
closure(item_number* core, size_t n);


void
free_closure(void);

new_itemsets();

append_states();

save_reductions();

set_states();


