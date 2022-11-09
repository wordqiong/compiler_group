
#include"grammar.h"
#include"LR1.h"
int main()
{
	analysis res;
	res.getStrBuffer();
	grammar rule("grammar(1).txt");
	LR1_Grammar lr1_grammar();

	return 0;
}