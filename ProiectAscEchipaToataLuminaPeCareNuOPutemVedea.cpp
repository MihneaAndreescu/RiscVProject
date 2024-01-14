// do not run this code on release!!!!!!!!!!! It is correct only on debug!
#include <iostream>
#include "Encoder.h"
#include "Executor.h"
#include "Bijections.h"
#include "MemoryManager.h"
#include "Help.h"
#include <cassert>
#include <functional>
#include <map>
#include <algorithm>
using namespace std;
vector<pair<string, int>> readFrequencies(string filename) {
	FILE* stream;
	errno_t _err = freopen_s(&stream, filename.c_str(), "r", stdin);
	vector<pair<string, int>> v;
	string s;
	int f;
	while (cin >> s >> f) {
		v.push_back({ s, f });
	}
	fclose(stream);
	return v;
}
void print(vector<pair<string, int>> v) {
	for (auto& [s, f] : v) {
		cout << " ---> " << s << " " << f << "\n";
	}
	cout << "---------------------------\n";
}

vector<pair<string, string>> getOptimalCodes(vector<pair<string, int>> frequencies) {
	set<string> strings;
	for (auto& [s, f] : frequencies) {
		strings.insert(s);
	}
	assert((int)strings.size() == (int)frequencies.size());
	map<string, int> code;
	vector<vector<int>> g;
	int y = 0;
	for (int i = 0; i < (int)frequencies.size(); i++) {
		code[frequencies[i].first] = y++;
		g.push_back({});
	}
	vector<pair<int, int>> leafs;
	for (int i = 0; i < y; i++) {
		leafs.push_back({ i, frequencies[i].second });
	}
	while ((int)leafs.size() >= 2) {
		//cout << " ---> ";
		//for (auto& it : leafs) {
		//	cout << "(" << it.first << ", " << it.second << ") ";
		//}
		//cout << "\n";
		sort(leafs.begin(), leafs.end(), [&](auto i, auto j) {return i.second > j.second; });
		assert(y == (int)g.size());
		auto it1 = leafs.back(); leafs.pop_back();
		auto it2 = leafs.back(); leafs.pop_back();
		g.push_back({ it1.first, it2.first });
		leafs.push_back({ y, it1.second + it2.second });
		y++;
	}
	assert(y == (int)g.size());
	vector<string> sols((int)frequencies.size());
	function<void(int, string)> dfs = [&](int vertex, string code) {
		if (vertex < (int)frequencies.size()) {
			assert((int)g[vertex].size() == 0);
			sols[vertex] = code;
		}
		else {
			assert((int)g[vertex].size() == 2);
			int c0 = g[vertex][0], c1 = g[vertex][1];
			dfs(c0, code + "0");
			dfs(c1, code + "1");
		}
	};
	dfs(y - 1, "");
	vector<pair<string, string>> sol;
	for (int i = 0; i < (int)frequencies.size(); i++) {
		sol.push_back({ frequencies[i].first, sols[i] });
	}
	return sol;
	exit(0);
}
int main() {
	//auto v = getOptimalCodes(readFrequencies("register_frequencies.txt"));
	//for (auto& [s, co] : v) {
	//	cout << s << " " << co << "\n";
	//}
	//exit(0);
	//print(readFrequencies("operation_frequencies.txt"));
	//print(readFrequencies("register_frequencies.txt"));
	//exit(0);
	//float value = 7.2145;
	//string s = floatToString(value);
	//cout << value << "\n";
	//cout << s << "\n";
	//float value2 = stringToFloat(s);
	//cout << value2 << "\n";
	//exit(0);
	loadBijections();
	auto bits = encode("code.txt");
	initMemory();
	executeInner(bits);
	return 0;
}