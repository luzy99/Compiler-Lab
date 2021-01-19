#define STATE_NUM 48
#define ACTION_NUM 21
#include<iostream>
#include<fstream>
#include<vector>
#include<unordered_map>

using namespace std;

//保留字表
static char reserveWord[28][20] = {
	"auto", "break", "case", "char", "const", "continue",
	"default", "do", "double", "else", "enum", "extern",
	"float", "for", "goto", "if", "int", "long",
	"register", "return", "short", "signed", "sizeof", "static",
	"struct", "switch",  "void", "while"
};
//界符运算符 50+
static char operators[39][10] = {
	"+","-","*","/","<","<=",">",">=","=","==",
	"!=",";","(",")","^",",","\"","\'","#","&",
	"&&","|","||","%","<<",">>","[","]","{","}",
	":","!","++","--","+=","-=","*=","/=","::"
};
//char IdentifierTable[1000][50] = { "" };//标识符表


unordered_map<string,int> IdentifierTable; //标识符表

// 种别码
int errorId = 0;
int constantId = 101;
int stringId = 102;
int charId = 103;
int commentId = 104;
int blankId = 200;

// 标识符编号起始位置
static int identifierId=300;

/***********************************************************************************/
// 状态转换表
int transferTable[STATE_NUM *ACTION_NUM] = {
	14,5,1,25,28,14,31,15,10,7,-1,22,47,22,36,33,47,45,41,39,47,// 0
	-1,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 1
	3,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 2
	-1,4,-1,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 3
	-1,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 4
	3,5,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 5
	-1,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 6
	7,7,7,7,7,7,7,7,7,9,8,-1,7,7,-1,-1,-1,-1,-1,-1,7,// 7
	7,7,7,7,7,7,7,7,7,7,7,-1,7,7,-1,-1,-1,-1,-1,-1,7,// 8
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 9
	12,12,12,12,12,12,12,12,-1,12,11,-1,12,12,-1,-1,-1,-1,-1,-1,12,// 10
	12,12,12,12,12,12,12,12,12,12,12,-1,12,12,-1,-1,-1,-1,-1,-1,12,// 11
	-1,-1,-1,-1,-1,-1,-1,-1,13,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 12
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 13
	14,14,-1,-1,-1,14,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 14
	-1,-1,-1,-1,-1,-1,18,16,-1,-1,-1,-1,21,-1,-1,-1,-1,-1,-1,-1,-1,// 15
	16,16,16,16,16,16,16,16,16,16,16,17,16,16,-1,-1,-1,-1,-1,-1,16,// 16
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 17
	18,18,18,18,18,18,19,18,18,18,18,18,18,18,-1,-1,-1,-1,-1,-1,18,// 18
	18,18,18,18,18,18,18,20,18,18,18,18,18,18,-1,-1,-1,-1,-1,-1,18,// 19
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 20
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 21
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 22
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,24,-1,-1,-1,-1,// 23
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 24
	-1,-1,-1,26,-1,-1,-1,-1,-1,-1,-1,-1,27,-1,-1,-1,-1,-1,-1,-1,-1,// 25
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 26
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 27
	-1,-1,-1,-1,29,-1,-1,-1,-1,-1,-1,-1,30,-1,-1,-1,-1,-1,-1,-1,-1,// 28
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 29
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 30
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,32,-1,-1,-1,-1,-1,-1,-1,-1,// 31
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 32
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,34,-1,-1,35,-1,-1,-1,-1,-1,// 33
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 34
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 35
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,38,-1,37,-1,-1,-1,-1,-1,-1,// 36
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 37
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 38
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,40,-1,// 39
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 40
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,42,-1,-1,// 41
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 42
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,44,-1,-1,-1,-1,-1,-1,-1,-1,// 43
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 44
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,46,-1,-1,-1,-1,-1,-1,-1,-1,// 45
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 46
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 47
};

// 节点状态
int stateCode[48] = {
	0,0,101,0,101,101,0,0,0,102,0,0,0,103,300,54,0,104,0,0,104,87,200,80,88,50,82,83,51,84,85,52,86,54,55,74,56,75,57,71,72,69,70,58,59,81,60,100
};



/***********************************************************************************/


bool isDigit(char ch)//是否为数字
{
	if (ch >= '0' && ch <= '9')
		return true;
	return false;
}

bool isLetter(char ch)//是否为字母或下划线
{
	
	if ((ch >= 'a' && ch <= 'z') || (ch <= 'Z' && ch >= 'A') || ch == '_')
		return true;
	return false;
}

int isReserve(char *s)//判断是否为保留字
{
	for (int i = 0;i<28;++i)
	{
		if (strcmp(reserveWord[i], s) == 0)
			return i + 1;//返回种别码
	}
	return -1;
}

int isOperator(char *s)//判断是否为合法运算符
{
	for (int i = 0;i<32;++i)
	{
		if (strcmp(operators[i], s) == 0)
			return i + 50;//返回种别码
	}
	return -1;
}

// 获取字符类别
int getAction(char &ch) {
	if (ch == 'e') {
		return 0;
	}
	else if (isDigit(ch)) {
		return 1;
	}
	else if (ch == '.') {
		return 2;
	}
	else if (ch == '+') {
		return 3;
	}
	else if (ch == '-') {
		return 4;
	}
	else if (isLetter(ch)) {
		return 5;
	}
	else if (ch == '*') {
		return 6;
	}
	else if (ch == '/') {
		return 7;
	}
	else if (ch == '\'') {
		return 8;
	}
	else if (ch == '"') {
		return 9;
	}
	else if (ch == '\\') {
		return 10;
	}
	else if (ch == '\n') {
		return 11;
	}
	else if (ch == '=') {
		return 12;
	}
	else if (ch == ' ' || ch == '\t') {
		return 13;
	}
	else if (ch == '>') {
		return 14;
	}
	else if (ch == '<') {
		return 15;
	}
	else if (ch == ':') {
		return 16;
	}
	else if (ch == '!') {
		return 17;
	}
	else if (ch == '&') {
		return 18;
	}
	else if (ch == '|') {
		return 19;
	}
	else {	// other, 其他符号
		return 20;
	}

}

// 获取单词token
void getToken(int state, char* word) {
	int code = stateCode[state];
	switch (code)
	{
	case 101:
		printf("< %s, %s, %d >\n", word, "CONSTANT", code);
		break;
	case 102:
		printf("< %s, %s, %d >\n", word, "STRING", code);
		break;
	case 103:
		printf("< %s, %s, %d >\n", word, "CHARACTER", code);
		break;
	case 104:
		printf("< %s, %s, %d >\n", word, "COMMENT", code);
		break;
	case 100:	// other opt
	{
		int optCode =	isOperator(word);
		if (optCode > 0) {
			printf("< %s, %s, %d >\n", word, word, optCode);
		}
		else {
			printf("< %s, %s, %d >\n", word, "ERROR", 0);
		}
		break;
	}
	case 200:	// 空白 跳过
		break;
	case 0:	// 错误
		printf("< %s, %s, %d >\n", word, "ERROR", 0);
		break;
	case 300:
	{
		int tmpNum = 0;
		tmpNum = isReserve(word);
		if (tmpNum > 0) {	// 保留字
			printf("< %s, %s, %d >\n", word, word, tmpNum);
		}
		else {	// 标识符
			int idnum = 300;
			string str = word;
			int tt = IdentifierTable.count(str);
			if (IdentifierTable.count(str) == 0) {
				// not found
				IdentifierTable[str] = identifierId;
				idnum = identifierId;
				identifierId++;
			}
			else {
				idnum = IdentifierTable[str];
			}
			printf("< %s, %s, %d >\n", word, "IDENTIFIER", idnum);
		}
		break;
	}
	default:
		printf("< %s, %s, %d >\n", word, word, code);
		break;
	}
}

void scanner(char * source)// 扫描源程序
{
	int count = 0;
	char ch;
	int p = 0;	// 当前字符下标
	int t = 0;	// 当前单词下标
	ch = source[p];
	int state = 0;
	char word[30];
	while (ch != '\0')
	{
		t++;
		int next_state = transferTable[state * ACTION_NUM + getAction(ch)];
		//cout << next_state << endl;
		if (next_state > 0) {
			state = next_state;
			ch = source[++p];
		}
		else
		{	// next_state=-1
			strncpy(word, source+p-t+1, t-1);
			word[t-1] = '\0';
			getToken(state, word);
			t = 0;
			state = 0;
		}
		
	}
}

int main(int argc, char **argv)
{
	//打开一个文件，读取源程序
	char source[10000];
	char token[20] = { 0 };
	int innerCode = -1;
	int p = 0; //程序位置索引
	//char file_path[100];
	//cout << "输入文件路径：\n";
	//cin >> file_path;

	if (argc < 2) {
		cout << "未输入文件路径";
		system("pause");
		exit(0);
	}

	ifstream in(argv[1]);
	//ifstream in("D:\\文档\\@SEU\\编译原理\\lab1\\MyLex\\Debug\\test.cpp");
	//ofstream out("out.txt");
	if (!in.is_open())
	{
		cout << "error opening file!" << endl;
		system("pause");
		exit(0);
	}
	while (!in.eof())
	{
		in.get(source[p++]);
	}
	source[p] = '\0';
	scanner(source);

	system("pause");
}