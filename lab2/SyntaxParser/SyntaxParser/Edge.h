#pragma once

// DFA 边
struct edge
{
	int next;		//下一条边的存储下标
	int to;		//这条边的终点
	int w;
};