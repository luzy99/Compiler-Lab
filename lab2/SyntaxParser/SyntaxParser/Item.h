#pragma once
#include<iostream>
#include<string>
#include<map>
#include<vector>
#include<stack>
#include<unordered_set>
#include<queue>
using namespace std;

// LR(1) ��
struct item        
{
	int pid=0;			// ����ʽ���
	int dotPos=1;		//.��λ��
	unordered_set<int> fst;		//first��
};

// 2��term�Ƿ���ͬ
inline bool isEqualItem(item a, item b)
{
	if (a.fst == b.fst&&a.dotPos == b.dotPos&&a.pid == b.pid)return 1;
	return 0;
}

// �ж���Ŀ�Ƿ�����Ŀ����
bool isItemInSet(item a, vector<item> b) 
{
	for (int i = 0;i<b.size();i++)
	{
		if (isEqualItem(a, b[i]))return 1;
	}
	return 0;
}

// �ϲ���Ŀ��a,b
vector<item>  combineSet(vector<item>a, vector<item>&b) 
{
	for (int i = 0;i<b.size();i++)
	{
		if (isItemInSet(b[i], a))continue;
		else
			a.push_back(b[i]);
	}
	return a;
}

// ������Ŀ���Ƿ����
bool isEqualSet(vector<item> a, vector<item> b)
{
	if (a.size() != b.size())return 0;
	for (int i = 0;i<a.size();i++)
	{
		if (!isItemInSet(a[i], b))return 0;
	}
	return 1;
}

//����Ŀ�����в�����Ŀ����ţ����򷵻�-1
int findSetInCollection(vector<item>a, vector<vector<item> >b) 
{
	for (int i = 0;i<b.size();i++)
	{
		if (isEqualSet(a, b[i]))return i;
	}
	return -1;
}
