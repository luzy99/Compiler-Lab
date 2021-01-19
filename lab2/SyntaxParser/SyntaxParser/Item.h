#pragma once
#include<iostream>
#include<string>
#include<map>
#include<vector>
#include<stack>
#include<unordered_set>
#include<queue>
using namespace std;

// LR(1) 项
struct item        
{
	int pid=0;			// 产生式编号
	int dotPos=1;		//.的位置
	unordered_set<int> fst;		//first集
};

// 2个term是否相同
inline bool isEqualItem(item a, item b)
{
	if (a.fst == b.fst&&a.dotPos == b.dotPos&&a.pid == b.pid)return 1;
	return 0;
}

// 判断项目是否在项目集中
bool isItemInSet(item a, vector<item> b) 
{
	for (int i = 0;i<b.size();i++)
	{
		if (isEqualItem(a, b[i]))return 1;
	}
	return 0;
}

// 合并项目集a,b
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

// 两个项目集是否相等
bool isEqualSet(vector<item> a, vector<item> b)
{
	if (a.size() != b.size())return 0;
	for (int i = 0;i<a.size();i++)
	{
		if (!isItemInSet(a[i], b))return 0;
	}
	return 1;
}

//在项目集族中查找项目集编号，否则返回-1
int findSetInCollection(vector<item>a, vector<vector<item> >b) 
{
	for (int i = 0;i<b.size();i++)
	{
		if (isEqualSet(a, b[i]))return i;
	}
	return -1;
}
