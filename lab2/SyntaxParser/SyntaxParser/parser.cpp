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

/*************ȫ�ֱ�����**************/
unordered_map<string, int>ch_to_num;	// �ַ�������ת����
vector<string> num_to_ch;          // ���ֵ��ַ�ת����
vector<vector<string>>productions;        //����ʽ	[ [��, �Ҳ�1,�Ҳ�2...],[...]...]    
vector<vector<int>> table;		 //Ԥ������� -1
vector<vector<int>> tb_s_r;        //���ƽ���ǹ�Լ��,-1,-2.
int num = 0;int e_idx = 0;   //numvt���ս�����ϣ�0�ǡ�#����numvt�����
vector<vector<int>> relate_prods;   // ���ս����������ߵ����в���ʽ���
vector<unordered_set<int>> first;   //ÿ�����ŵ�first��

vector<vector<item> >itemSetCollection;             //��Ŀ����
vector<edge> edges;		// DFA �� ��
unordered_map<int, int> head;    // head[i]��ʾ��iΪ���ĵ�һ����

/************************************************/
// ���� �ս�������ս����Ͳ���ʽ
void readin()
{
	ch_to_num["#"] = 0;
	num_to_ch.push_back("#");
	cout << "�������ս������" << endl;
	string x;
	do
	{
		cin >> x;
		ch_to_num[x] = ++num;
		num_to_ch.push_back(x);
	} while (cin.peek() != '\n');
	e_idx = ++num;
	ch_to_num["@"] = e_idx;        // ��
	num_to_ch.push_back("@");
	cout << "��������ս������" << endl;
	do
	{
		cin >> x;
		ch_to_num[x] = ++num;
		num_to_ch.push_back(x);
	} while (cin.peek() != '\n');
	cout << "�������в���ʽ�������á�@����ʾ��,�ԡ�end������:" << endl;
	string pro;
	while (getline(cin,pro)&&pro != "end")
	{
		vector<string> sp = split(pro, "->");		// ������ʽ��Ϊ�󲿺��Ҳ�
		vector<string> ss;		// ��ǰ����ʽ
		if (sp.size() != 2)continue;
		ss.push_back(sp[0]);
		vector<string> right = split(sp[1], " "); // ����Ҳ�
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

// ���� ���ս����������ߵ����в���ʽ��� ��
void gen_relate_prods()
{
	relate_prods = vector<vector<int>>(num + 1);
	for (int i = 0;i<productions.size();i++)
	{
		int temp = ch_to_num[productions[i][0]];
		relate_prods[temp].push_back(i);
	}
}

// ����ŵ�FIRST������ǰ���ţ�����ʽ��ţ�
void getfirst(int nv, int nump)
{
	int fch = ch_to_num[productions[nump][1]];  //����ʽ�Ƴ������׷�
	if (fch <= e_idx)first[nv].insert(fch);  //���ս��
	else
	{
		for (int i = 0;i<relate_prods[fch].size();i++)
		{
			if (productions[nump][0] == productions[nump][1])continue;	//������ݹ�
			getfirst(fch, relate_prods[fch][i]);
		}

		first[nv].insert(first[fch].begin(), first[fch].end());       // ����fch��first��
	}
}

// ����ÿ�����ŵ�first��
void gen_firstSet()
{
	first = vector<unordered_set<int>>(num+1);
	for (int i = 1;i <= e_idx;i++)             //    �ս��first�����Լ�
	{
		first[i].insert(i);
	}
	for (int i = 0;i<productions.size();i++)
	{
		if (productions[i][0] == productions[i][1])continue; // ������ݹ�Ĳ���ʽ
		int nv = ch_to_num[productions[i][0]];
		getfirst(nv, i);
	}
}

//�����
void addEdge(int from, int to, int w)
{
	int f = head.count(from) == 0 ? -1 : head[from];
	edge e = { f,to,w };
	edges.push_back(e);
	head[from] = edges.size()-1;
}

// ��ʼ��������
void init_table()
{
	int st = itemSetCollection.size()+1;
	table = vector<vector<int>>(st, vector<int>(st, -1));
	tb_s_r = vector<vector<int>>(st, vector<int>(st, -1));
}


// ��ȡLR��ıհ���������Ŀ����BFS
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
		if (cur.dotPos == productions[cur.pid].size())          // . λ�����
			continue;
		int tt = ch_to_num[productions[cur.pid][cur.dotPos]];       // . ֮��ķ���
		if (tt <= e_idx)   continue;                  //�����ս�����򲻱�����
		for (int i = 0;i<relate_prods[tt].size();i++)         //��Ӧ����ʽ�ı��
		{
			item c;
			c.pid = relate_prods[tt][i];
			if (productions[cur.pid].size() - cur.dotPos == 1) // . λ�����
				c.fst += cur.fst;
			else
			{
				int tttnum = ch_to_num[productions[cur.pid][cur.dotPos + 1]];
				c.fst.insert(first[tttnum].begin(), first[tttnum].end());
			}
			if (!isItemInSet(c, temp))           // ����
			{
				q.push(c);
				temp.push_back(c);
			}
		}
	}
	return temp;
}

// ������Ŀ����
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
		for (int i = 1;i <= num;i++)     //���з���
		{
			if (i == e_idx)continue;      // ��������
			vector<item> temp;
			for (int j = 0;j<cur.size();j++)     //����Ŀ���е�������Ŀ
			{
				if (cur[j].dotPos == productions[cur[j].pid].size())continue;  //�ǹ�Լ��
				int tt = ch_to_num[productions[cur[j].pid][cur[j].dotPos]];
				if (tt == i)                                          // �¸�����Ϊi
				{
					item it;
					it.fst = cur[j].fst;
					it.dotPos = cur[j].dotPos + 1;
					it.pid = cur[j].pid;
					temp = combineSet(temp, getClosure(it));
				}
			}
			if (temp.size() == 0)continue;             // �Ϊ��
			int numcur = findSetInCollection(cur, itemSetCollection);   //��ǰ�ڵ���
			int tttnum = findSetInCollection(temp, itemSetCollection);  //��Ŀ����
			if (tttnum == -1)                    //�µ���Ŀ��
			{
				itemSetCollection.push_back(temp);
				q.push(temp);
				addEdge(numcur, itemSetCollection.size() - 1, i);   //��ӱߣ�ȨΪ����ķ���
			}
			else                             //�ϵ���Ŀ��
			{
				addEdge(numcur, tttnum, i);
			}
		}
	}
}


//��ӡ��Ŀ����
void print_itemSetCollection()             
{
	for (int i = 0;i<itemSetCollection.size();i++)
	{
		cout << "��Ŀ��" << i << ":" << endl;
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
bool get_table()            //��÷�����table[i][j]=w:״̬i-->j,�������W��
{
	init_table();	// ��ʼ��������
	for (int i = 0;i<itemSetCollection.size();i++)          //����ͼ
	{
		if (head.count(i) == 0)continue;
		for (int j = head[i];j != -1;j = edges[j].next)
		{
			if (table[i][edges[j].w] != -1)return 0;           //������ڣ�����.
			table[i][edges[j].w] = edges[j].to;
			tb_s_r[i][edges[j].w] = -1;             //�ƽ���-1��
		}
	}
	for (int i = 0;i<itemSetCollection.size();i++)             //����������Ŀ
	{
		for (int j = 0;j<itemSetCollection[i].size();j++)
		{
			if (itemSetCollection[i][j].dotPos == productions[itemSetCollection[i][j].pid].size())  //��Լ��
			{
				for (auto &k : itemSetCollection[i][j].fst) {
					if (table[i][k] != -1)return 0;           //������ڣ�����.
					if (k == 0&&itemSetCollection[i][j].pid == 0)
						table[i][k] = -3;           //����̬��
					else
					{
						table[i][k] = itemSetCollection[i][j].pid;
						tb_s_r[i][k] = -2;            //��Լ̬
					}
				}
			}
		}
	}
	return 1;
}
void print_table()
{
	cout << "LR(1)������" << endl;
	cout << "״̬   " << "         actoin     " << endl;
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
			if (table[i][j] == -3)  cout << "acc" << "  ";       //����
			else if (table[i][j] == -1)cout << "     ";        //��
			else if (tb_s_r[i][j] == -1)cout << "s" << table[i][j] << "   ";  //�ƽ�
			else if (tb_s_r[i][j] == -2)cout << "r" << table[i][j] << "   ";  //��Լ
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

// ʹ��PPT ����
bool analyze()
{
	cout << word << "�ķ������̣�" << endl;
	cout << "����\t\t" << "״̬ջ\t\t" << "����ջ\t\t" << "���봮\t\t" << "����" << endl;
	stack<int>state;   //2��ջ��״̬ջ�ͷ���ջ
	stack<int>wd;
	int count = 0;
	state.push(0);     //��ʼ��
	wd.push(0);        //'#'
	for (int i = 0;;)       //i�������ı���
	{
		int cur = state.top();
		if (ch_to_num.count(word[i])==0 || table[cur][ch_to_num[word[i]]] == -1)    // �հף�������
			return 0;
		if (table[cur][ch_to_num[word[i]]] == -3)  //����
		{
			print_now_state(count++, state, wd, i);
			cout << "accept!" << endl;
			return 1;
		}
		if (tb_s_r[cur][ch_to_num[word[i]]] == -1)       //�ƽ���
		{
			print_now_state(count++, state, wd, i);
			int newstate = table[cur][ch_to_num[word[i]]];
			cout << "action[" << cur << "," << ch_to_num[word[i]] << "]=" << newstate;
			cout << "��״̬" << newstate << "��ջ" << endl;
			wd.push(ch_to_num[word[i]]);
			state.push(newstate);
			i++;
		}
		else if (tb_s_r[cur][ch_to_num[word[i]]] == -2)         //��Լ
		{
			print_now_state(count++, state, wd, i);

			int numpro = table[cur][ch_to_num[word[i]]];   //�øò���ʽ��Լ
			int len = productions[numpro].size() - 1;
			for (int ii = 0;ii<len;ii++)                 //��ջ
			{
				wd.pop();
				state.pop();
			}
			wd.push(ch_to_num[productions[numpro][0]]);    //����
			int cur = state.top();
			cout <<  productions[numpro][0] << "->";
			for (int ii = 1;ii <= len;ii++)
				cout << productions[numpro][ii];
			cout << " ���й�Լ," << "goto[" << cur << "," << ch_to_num[word[i]] << "]=" << table[cur][ch_to_num[productions[numpro][0]]];
			cout << "��ջ" << endl;
			state.push(table[cur][ch_to_num[productions[numpro][0]]]);
		}
	}
	return 1;
}
int main()
{
	readin();	// ����CFG
	gen_relate_prods();	// Ԥ�������ʽ
	gen_firstSet();	// ����first��
	gen_itemSetCollection();	// ������Ŀ����
	
	if (!get_table())	// ���������
	{
		cout << "LR(1)�ķ�����" << endl;
		system("pause");
		return 0;
	}
	print_itemSetCollection();	// ��ӡ��Ŀ����
	print_table();				// ��ӡ������
	string in;
	while (in!="exit")
	{
		cout << "��������Ŵ���" << endl;
		getline(cin, in);
		
		word = split(in, " ");
		word.push_back("#");

		if (!analyze())			// ����token����
			cout << "error!" << endl;
	}
	

	system("pause");
	return 0;
}