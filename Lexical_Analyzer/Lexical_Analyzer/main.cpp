#include"grammar.h"
#include"LR1.h"
#define FILE_OPEN_ERROR 1
#define CODE_FILE_OPEN_ERROR 2

int main()
{
	cout << "****************************************" << endl;
	cout << "LR(1)词法、语法、语义分析器" << endl;
	cout << "****************************************" << endl << endl;
	cout << "系统默认读入文法文件grammar.txt" << endl << endl;

	string gfile_name = "grammar.txt";
	string b;
	cout << "请输入要读入的文法文件名：（如果选择默认文件，请输入e）\n";
	cin >> b;
	if (b != "e")
	{
		gfile_name = b;
		cout << "已读入文件" << gfile_name << endl;
	}

	else {
		cout << "已读入默认文件" << gfile_name << endl;
	}
	//TODO 读入位置的处理，如果出错，那么程序结束
	//TODO 读入源文件的处理，如果出错，那么报错
	//TODO 程序默认输出文件夹为XXX ，如需更改，请输入对应的文件夹路径，并确保存在
	//TODO 程序的更改
	try
	{
		cout << "正在对文法进行分析，请稍后..." << endl;
		LR1_Grammar lr1_grammar(gfile_name);
		lr1_grammar.checkClosure();
		lr1_grammar.getClosureSum();
		lr1_grammar.computeACTION_GOTO();
		lr1_grammar.printTables();

		cout << "已完成文法分析，可在../work_dir文件夹中查看\n1. 产生式规则文件Grammar_Rules.txt\n2. 初始闭包集文件Zero_Closure.txt\n3. ACTION表和GOTO表Tables.csv。" << endl;
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
		cout << "正在进行语法和语义分析，请稍后..." << endl;

		int mark = lr1_grammar.analyze(res.analysis_res);

		lr1_grammar.semantic_analysis.PrintQuaternion(quaternion_path);

		if (mark == 0)
		{
			cout << "语法和语义分析已结束，可在../work_dir文件夹中查看\n1. 文法分析过程文件Analysis_Process.txt\n2. 语义分析四元式序列文件Quaternion_List.txt。" << endl;
			cout << "是否需要生成归约语法树图片（需要安装graphviz）（y/n）" << endl;
			string ans;
			cin >> ans;
			if (ans == "y")
			{
				lr1_grammar.generateTree(res.analysis_res);
				system("dot -Tpng ../work_dir/Parse_Tree.dot -o ../work_dir/Parse_Tree.png");
				cout << "语法分析树已生成，可在../work_dir文件夹中查看\n1. 语法树图片Parse_Tree.png。" << endl;
			}
		}
		else if (mark == 1)
			cout << "文法分析过程中ACTION表查找出错，请在../work_dir文件夹中查看文法分析过程文件Analysis_Process.txt。" << endl;
		else
			cout << "文法分析过程中GOTO表查找出错，请在../work_dir文件夹中查看文法分析过程文件Analysis_Process.txt。" << endl;

	}
	catch (int a)
	{
		if (a == FILE_OPEN_ERROR)
			cout << "读入的文法文件打开失败！请检查对应的路径" << endl;
		else if (a == CODE_FILE_OPEN_ERROR)
			cout << "读入的code文件打开失败！请检查对应的路径" << endl;
		return 0;
	}

	cout << endl;
	cout << "程序结束！" << endl;
	return 0;
}
