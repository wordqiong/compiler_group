#include"grammar.h"
#include"LR1.h"
int main()
{
	cout << "****************************************" << endl;
	cout << "LR(1)词法语法分析器" << endl;
	cout << "****************************************" << endl << endl;
	cout << "系统默认读入文法文件grammar.txt" << endl << endl;

	LR1_Grammar lr1_grammar("grammar.txt");
	lr1_grammar.checkClosure();
	lr1_grammar.getClosureSum();
	lr1_grammar.computeACTION_GOTO();
	lr1_grammar.printTables();

	cout << "已读入文法文件，可在../work_dir文件夹中查看\n1. 产生式规则文件Grammar_Rules.txt\n2. 初始闭包集文件Zero_Closure.txt\n3. ACTION表和GOTO表Tables.csv。" << endl;
	cout << endl;
	cout << "系统默认读入源程序的文件路径code_in.txt" << endl;
	cout << "正在进行词法分析，请稍后..." << endl;
	//cout << "请查看文件路径是否正确(y/n)" << endl;
	//string pin;
	//cin >> pin;
	//if (pin != "y")
	//{
	//	cout << "请先配置文件" << endl;
	//	return 0;
	//}
	analysis res;
	res.getStrBuffer();

	cout << "词法分析已结束，可在../work_dir文件夹中查看\n1. 预处理代码文件Pre-Processed_Code.txt\n2. 词法分析结果文件Lexical_Result.txt\n3. 词法分析符号对应表Word_Lable.txt" << endl;
	cout << endl;
	cout << "正在进行语法分析，请稍后..." << endl;

	int mark = lr1_grammar.analyze(res.analysis_res);
	if (mark == 0)
	{
		lr1_grammar.generateTree(res.analysis_res);
		system("dot -Tpng ../work_dir/Parse_Tree.dot -o ../work_dir/Parse_Tree.png");
		cout << "语法分析已结束，可在../work_dir文件夹中查看\n1. 文法分析过程文件Analysis_Process.txt\n2. 语法树图片Parse_Tree.png。" << endl;
	}
	else if (mark == 1)
		cout << "语法分析过程中ACTION表查找出错，请在../work_dir文件夹中查看文法分析过程文件Analysis_Process.txt。" << endl;
	else
		cout << "语法分析过程中GOTO表查找出错，请在../work_dir文件夹中查看文法分析过程文件Analysis_Process.txt。" << endl;

	return 0;
}