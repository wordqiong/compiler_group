
#include"grammar.h"
#include"LR1.h"
int main()
{
	analysis res;
	res.getStrBuffer();

	LR1_Grammar lr1_grammar("grammar(1).txt");
	lr1_grammar.checkClosure();
	return 0;
}