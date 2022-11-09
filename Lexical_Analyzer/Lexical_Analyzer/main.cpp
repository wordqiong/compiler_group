
#include"grammar.h"
#include"LR1.h"
int main()
{
	analysis res;
	res.getStrBuffer();

	LR1_Grammar lr1_grammar("grammar.txt");
	lr1_grammar.checkClosure();

	//cout << "*********************************" << endl;
	lr1_grammar.getClosureSum();
	cout << lr1_grammar.closure_sum.size() << endl;
	//lr1_grammar.closure_sum[0].print(lr1_grammar.symbols);

	//cout << (lr1_grammar.ACTION.find(pair<int, int>(41, lr1_grammar.Find_Symbol_Index_By_Token("("))) == lr1_grammar.ACTION.end()) << endl;

	lr1_grammar.computeACTION_GOTO();
	lr1_grammar.printTables();

	//for (int i = 0; i < lr1_grammar.symbols.size(); i++)
	//{
	//	cout << i << "  " << lr1_grammar.symbols[i].tag << endl;
	//}
	//cout << "*********************************" << endl;
	
	//map<int, vector<int>> temp = lr1_grammar.closure_sum[41].getShiftinSymbol();
	//int i = 1;
	//for (auto it=temp.begin();it!=temp.end();it++)
	//{
	//	cout << "第" << i << "个" << endl;
	//	cout << lr1_grammar.symbols[it->first].tag << endl;
	//	i++;
	//}

	//cout << lr1_grammar.start_closure.getShiftinSymbol().size() << endl;
	//cout << lr1_grammar.start_closure.getReduceSymbol().size() << endl;
	//cout << "forward：" << lr1_grammar.start_closure.getReduceSymbol()[0].first << endl;
	//cout << "grammar index：" << lr1_grammar.start_closure.getReduceSymbol()[0].second << endl;

	//看产生式
	//cout << "*********************************" << endl;
	//rule temp = lr1_grammar.rules[7];
	//cout << lr1_grammar.symbols[temp.left_symbol].tag << "  " << endl;
	//for (int i = 0; i < temp.right_symbol.size(); i++)
	//{
	//	cout << " " << lr1_grammar.symbols[temp.right_symbol[i]].tag;
	//}

	return 0;
}