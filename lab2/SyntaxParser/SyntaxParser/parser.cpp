#include<iostream>
#include<unordered_map>
#include<vector>
#include<stack>
#include<unordered_set>
#include<queue>

#include"Item.h"
#include"Utils.h"
#include"Edge.h"

using namespace std;

/*************全局变量区**************/
unordered_map<string, int>ch_to_num;	// 字符到数字转换表
vector<string> num_to_ch;          // 数字到字符转换表
vector<vector<string>>productions;        //产生式	[ [左部, 右部1,右部2...],[...]...]    
vector<vector<int>> table;		 //预测分析表 -1
vector<vector<int>> tb_s_r;        //是移进项还是规约项,-1,-2.
int num = 0;int e_idx = 0;   //numvt是终结符集合，0是‘#’，numvt表空字
vector<vector<int>> relate_prods;   // 非终结符出现在左边的所有产生式编号
vector<unordered_set<int>> first;   //每个符号的first集

vector<vector<item> >itemSetCollection;             //项目集族
vector<edge> edges;		// DFA 边 集
unordered_map<int, int> head;    // head[i]表示以i为起点的第一条边

/************************************************/
// 读入 终结符表、非终结符表和产生式
void readin()
{
	ch_to_num["#"] = 0;
	num_to_ch.push_back("#");
	cout << "请输入终结符集：" << endl;
	string x;
	do
	{
		cin >> x;
		ch_to_num[x] = ++num;
		num_to_ch.push_back(x);
	} while (cin.peek() != '\n');
	e_idx = ++num;
	ch_to_num["@"] = e_idx;        // ε
	num_to_ch.push_back("@");
	cout << "请输入非终结符集：" << endl;
	do
	{
		cin >> x;
		ch_to_num[x] = ++num;
		num_to_ch.push_back(x);
	} while (cin.peek() != '\n');
	cout << "输入所有产生式（空字用‘@’表示）,以‘end’结束:" << endl;
	string pro;
	while (getline(cin,pro)&&pro != "end")
	{
		vector<string> sp = split(pro, "->");		// 将产生式分为左部和右部
		vector<string> ss;		// 当前产生式
		if (sp.size() != 2)continue;
		ss.push_back(sp[0]);
		vector<string> right = split(sp[1], " "); // 拆分右部
		for (auto &s:right)
		{
			if (s == "") {
				continue;
			}
			else if (s == "|")
			{
				productions.push_back(ss);
				ss.clear();
				ss.push_back(sp[0]);
			}
			else
			{
				ss.push_back(s);
			}
		}
		productions.push_back(ss);
	}
}

// 生成 非终结符出现在左边的所有产生式编号 表
void gen_relate_prods()
{
	relate_prods = vector<vector<int>>(num + 1);
	for (int i = 0;i<productions.size();i++)
	{
		int temp = ch_to_num[productions[i][0]];
		relate_prods[temp].push_back(i);
	}
}

// 求符号的FIRST集（当前符号，产生式编号）
void getfirst(int nv, int nump)
{
	int fch = ch_to_num[productions[nump][1]];  //产生式推出来的首符
	if (fch <= e_idx)first[nv].insert(fch);  //是终结符
	else
	{
		for (int i = 0;i<relate_prods[fch].size();i++)
		{
			if (productions[nump][0] == productions[nump][1])continue;	//跳过左递归
			getfirst(fch, relate_prods[fch][i]);
		}

		first[nv].insert(first[fch].begin(), first[fch].end());       // 加上fch的first集
	}
}

// 构造每个符号的first集
void gen_firstSet()
{
	first = vector<unordered_set<int>>(num+1);
	for (int i = 1;i <= e_idx;i++)             //    终结符first集是自己
	{
		first[i].insert(i);
	}
	for (int i = 0;i<productions.size();i++)
	{
		if (productions[i][0] == productions[i][1])continue; // 跳过左递归的产生式
		int nv = ch_to_num[productions[i][0]];
		getfirst(nv, i);
	}
}

//构造边
void addEdge(int from, int to, int w)
{
	int f = head.count(from) == 0 ? -1 : head[from];
	edge e = { f,to,w };
	edges.push_back(e);
	head[from] = edges.size()-1;
}

// 初始化分析表
void init_table()
{
	int st = itemSetCollection.size()+1;
	table = vector<vector<int>>(st, vector<int>(st, -1));
	tb_s_r = vector<vector<int>>(st, vector<int>(st, -1));
}


// 获取LR项的闭包，构造项目集，BFS
vector<item> getClosure(item &t)           
{
	vector<item> temp;
	temp.push_back(t);
	queue<item> q;
	q.push(t);
	while (!q.empty())
	{
		item cur = q.front();
		q.pop();
		if (cur.dotPos == productions[cur.pid].size())          // . 位于最后
			continue;
		int tt = ch_to_num[productions[cur.pid][cur.dotPos]];       // . 之后的符号
		if (tt <= e_idx)   continue;                  //若是终结符，则不必找了
		for (int i = 0;i<relate_prods[tt].size();i++)         //对应产生式的编号
		{
			item c;
			c.pid = relate_prods[tt][i];
			if (productions[cur.pid].size() - cur.dotPos == 1) // . 位于最后
				c.fst += cur.fst;
			else
			{
				int tttnum = ch_to_num[productions[cur.pid][cur.dotPos + 1]];
				c.fst.insert(first[tttnum].begin(), first[tttnum].end());
			}
			if (!isItemInSet(c, temp))           // 查重
			{
				q.push(c);
				temp.push_back(c);
			}
		}
	}
	return temp;
}

// 生成项目集族
void gen_itemSetCollection()            
{
	vector<item>temp;
	item t;
	t.fst.insert(0);
	temp = getClosure(t);
	queue<vector<item> >q;        // BFS
	q.push(temp);
	itemSetCollection.push_back(temp);
	while (!q.empty())
	{
		vector<item> cur = q.front();
		q.pop();
		for (int i = 1;i <= num;i++)     //所有符号
		{
			if (i == e_idx)continue;      // 跳过空字
			vector<item> temp;
			for (int j = 0;j<cur.size();j++)     //该项目集中的所有项目
			{
				if (cur[j].dotPos == productions[cur[j].pid].size())continue;  //是规约项
				int tt = ch_to_num[productions[cur[j].pid][cur[j].dotPos]];
				if (tt == i)                                          // 下个符号为i
				{
					item it;
					it.fst = cur[j].fst;
					it.dotPos = cur[j].dotPos + 1;
					it.pid = cur[j].pid;
					temp = combineSet(temp, getClosure(it));
				}
			}
			if (temp.size() == 0)continue;             // 项集为空
			int numcur = findSetInCollection(cur, itemSetCollection);   //当前节点标号
			int tttnum = findSetInCollection(temp, itemSetCollection);  //新目标标号
			if (tttnum == -1)                    //新的项目集
			{
				itemSetCollection.push_back(temp);
				q.push(temp);
				addEdge(numcur, itemSetCollection.size() - 1, i);   //添加边，权为读入的符号
			}
			else                             //老的项目集
			{
				addEdge(numcur, tttnum, i);
			}
		}
	}
}


//打印项目集族
void print_itemSetCollection()             
{
	for (int i = 0;i<itemSetCollection.size();i++)
	{
		cout << "项目集" << i << ":" << endl;
		for (int j = 0;j<itemSetCollection[i].size();j++)
		{
			cout << productions[itemSetCollection[i][j].pid] << " " << itemSetCollection[i][j].dotPos << " " << itemSetCollection[i][j].fst << endl;
		}
		cout << endl;
	}
	for (int i = 0;i<itemSetCollection.size();i++)
	{
		if (head.count(i) == 0)continue;
		for (int j = head[i];j != -1;j = edges[j].next)
		{
			cout << "  " << num_to_ch[edges[j].w] << endl;
			cout << i << "--->" << edges[j].to << endl;
		}
	}
}
bool get_table()            //获得分析表table[i][j]=w:状态i-->j,读入符号W。
{
	init_table();	// 初始化分析表
	for (int i = 0;i<itemSetCollection.size();i++)          //遍历图
	{
		if (head.count(i) == 0)continue;
		for (int j = head[i];j != -1;j = edges[j].next)
		{
			if (table[i][edges[j].w] != -1)return 0;           //多重入口，报错.
			table[i][edges[j].w] = edges[j].to;
			tb_s_r[i][edges[j].w] = -1;             //移近项-1。
		}
	}
	for (int i = 0;i<itemSetCollection.size();i++)             //遍历所有项目
	{
		for (int j = 0;j<itemSetCollection[i].size();j++)
		{
			if (itemSetCollection[i][j].dotPos == productions[itemSetCollection[i][j].pid].size())  //归约项
			{
				for (auto &k : itemSetCollection[i][j].fst) {
					if (table[i][k] != -1)return 0;           //多重入口，报错.
					if (k == 0&&itemSetCollection[i][j].pid == 0)
						table[i][k] = -3;           //接受态。
					else
					{
						table[i][k] = itemSetCollection[i][j].pid;
						tb_s_r[i][k] = -2;            //归约态
					}
				}
			}
		}
	}
	return 1;
}
void print_table()
{
	cout << "LR(1)分析表：" << endl;
	cout << "状态   " << "         actoin     " << endl;
	for (int j = 0;j <= num;j++)
	{
		if (j == e_idx)continue;
		cout << "    " << num_to_ch[j];
	}
	cout << endl;
	for (int i = 0;i<itemSetCollection.size();i++)
	{
		cout << i << "   ";
		for (int j = 0;j <= num;j++)
		{
			if (j == e_idx)continue;
			if (table[i][j] == -3)  cout << "acc" << "  ";       //接受
			else if (table[i][j] == -1)cout << "     ";        //空
			else if (tb_s_r[i][j] == -1)cout << "s" << table[i][j] << "   ";  //移近
			else if (tb_s_r[i][j] == -2)cout << "r" << table[i][j] << "   ";  //归约
		}
		cout << endl;
	}
}
vector<string> word;
void  print_now_state(int count, stack<int>state, stack<int>wd, int i)
{
	cout << count << '\t' << '\t';
	stack<int>temp;
	while (!state.empty())
	{
		temp.push(state.top());
		state.pop();
	}
	while (!temp.empty())
	{
		cout << temp.top()<<" ";
		temp.pop();
	}
	cout << '\t' << '\t';
	while (!wd.empty())
	{
		temp.push(wd.top());
		wd.pop();
	}
	while (!temp.empty())
	{
		cout << num_to_ch[temp.top()];
		temp.pop();
	}
	cout << '\t' << '\t';
	for (int j = i;j<word.size();j++)
		cout << word[j];
	cout << '\t' << '\t';
}

// 使用PPT 分析
bool analyze()
{
	cout << word << "的分析过程：" << endl;
	cout << "步骤\t\t" << "状态栈\t\t" << "符号栈\t\t" << "输入串\t\t" << "动作" << endl;
	stack<int>state;   //2个栈：状态栈和符号栈
	stack<int>wd;
	int count = 0;
	state.push(0);     //初始化
	wd.push(0);        //'#'
	for (int i = 0;;)       //i，读入文本的
	{
		int cur = state.top();
		if (ch_to_num.count(word[i])==0 || table[cur][ch_to_num[word[i]]] == -1)    // 空白，报错误
			return 0;
		if (table[cur][ch_to_num[word[i]]] == -3)  //接受
		{
			print_now_state(count++, state, wd, i);
			cout << "accept!" << endl;
			return 1;
		}
		if (tb_s_r[cur][ch_to_num[word[i]]] == -1)       //移进项
		{
			print_now_state(count++, state, wd, i);
			int newstate = table[cur][ch_to_num[word[i]]];
			cout << "action[" << cur << "," << ch_to_num[word[i]] << "]=" << newstate;
			cout << "，状态" << newstate << "入栈" << endl;
			wd.push(ch_to_num[word[i]]);
			state.push(newstate);
			i++;
		}
		else if (tb_s_r[cur][ch_to_num[word[i]]] == -2)         //归约
		{
			print_now_state(count++, state, wd, i);

			int numpro = table[cur][ch_to_num[word[i]]];   //用该产生式归约
			int len = productions[numpro].size() - 1;
			for (int ii = 0;ii<len;ii++)                 //出栈
			{
				wd.pop();
				state.pop();
			}
			wd.push(ch_to_num[productions[numpro][0]]);    //新入
			int cur = state.top();
			cout <<  productions[numpro][0] << "->";
			for (int ii = 1;ii <= len;ii++)
				cout << productions[numpro][ii];
			cout << " 进行归约," << "goto[" << cur << "," << ch_to_num[word[i]] << "]=" << table[cur][ch_to_num[productions[numpro][0]]];
			cout << "入栈" << endl;
			state.push(table[cur][ch_to_num[productions[numpro][0]]]);
		}
	}
	return 1;
}
int main()
{
	readin();	// 输入CFG
	gen_relate_prods();	// 预处理产生式
	gen_firstSet();	// 构造first集
	gen_itemSetCollection();	// 构造项目集族
	
	if (!get_table())	// 构造分析表
	{
		cout << "LR(1)文法错误！" << endl;
		system("pause");
		return 0;
	}
	print_itemSetCollection();	// 打印项目集族
	print_table();				// 打印分析表
	string in;
	while (in!="exit")
	{
		cout << "请输入符号串：" << endl;
		getline(cin, in);
		
		word = split(in, " ");
		word.push_back("#");

		if (!analyze())			// 分析token序列
			cout << "error!" << endl;
	}
	

	system("pause");
	return 0;
}