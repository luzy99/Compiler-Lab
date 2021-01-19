#pragma once
#include<string>
#include<vector>
#include<unordered_set>

using namespace std;

vector<string>  split(const string& str, const string& delim) { //将分割后的子字符串存储在vector中
	vector<string> res;
	if ("" == str) return  res;

	string strs = str + delim; //*****扩展字符串以方便检索最后一个分隔出的字符串
	size_t pos;
	size_t size = strs.size();

	for (int i = 0; i < size; ++i) {
		pos = strs.find(delim, i); //pos为分隔符第一次出现的位置，从i到pos之前的字符串是分隔出来的字符串
		if (pos < size) { //如果查找到，如果没有查找到分隔符，pos为string::npos
			string s = strs.substr(i, pos - i);//*****从i开始长度为pos-i的子字符串
			res.push_back(s);//两个连续空格之间切割出的字符串为空字符串，这里没有判断s是否为空，所以最后的结果中有空字符的输出，
			i = pos + delim.size() - 1;
		}

	}
	return res;
}

ostream &operator<<(ostream &output, vector<string> &vec) {
	for (string &s : vec) {
		output << s << " ";
	}
	return output;
}



template <typename T>
unordered_set<T> &operator+=(unordered_set<int> &a, unordered_set<T> &b) {
	a.insert(b.begin(), b.end());
	return a;
}

template <typename T>
ostream &operator<<(ostream &output, unordered_set<T> &vec) {
	for (auto &s : vec) {
		output << "{"<<s << ","<<"}";
	}
	return output;
}