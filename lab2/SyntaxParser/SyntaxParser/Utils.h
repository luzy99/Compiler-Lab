#pragma once
#include<string>
#include<vector>
#include<unordered_set>

using namespace std;

vector<string>  split(const string& str, const string& delim) { //���ָ������ַ����洢��vector��
	vector<string> res;
	if ("" == str) return  res;

	string strs = str + delim; //*****��չ�ַ����Է���������һ���ָ������ַ���
	size_t pos;
	size_t size = strs.size();

	for (int i = 0; i < size; ++i) {
		pos = strs.find(delim, i); //posΪ�ָ�����һ�γ��ֵ�λ�ã���i��pos֮ǰ���ַ����Ƿָ��������ַ���
		if (pos < size) { //������ҵ������û�в��ҵ��ָ�����posΪstring::npos
			string s = strs.substr(i, pos - i);//*****��i��ʼ����Ϊpos-i�����ַ���
			res.push_back(s);//���������ո�֮���и�����ַ���Ϊ���ַ���������û���ж�s�Ƿ�Ϊ�գ��������Ľ�����п��ַ��������
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