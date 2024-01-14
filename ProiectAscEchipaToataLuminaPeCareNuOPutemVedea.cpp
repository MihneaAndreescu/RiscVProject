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
#include <fstream>
#include <bitset>
void writeBitsToFile(vector<bool> bits, string filename) {
	assert((int)bits.size() % 8 == 0);
	ofstream file(filename, ios::binary);
	assert(file.is_open());
	
	for (int i = 0; i < (int)bits.size(); i += 8) {
		bitset<8> currentByte;
		for (int j = 0; j < 8; j++) {
			currentByte[j] = bits[i + j];
		}
		unsigned char val = static_cast<unsigned char>(currentByte.to_ulong());
		file.write(reinterpret_cast<const char*>(&val), sizeof(val));
	}
	file.close();
}
vector<bool> readBitsFromFile(string filename) {
	ifstream file(filename, ios::binary);
	assert(file.is_open());
	vector<bool> bits;
	unsigned char byte;
	while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
		bitset<8> currentByte(byte);
		for (int i = 0; i < 8; i++) {
			bits.push_back(currentByte[i]);
		}
	}
	file.close();
	return bits;
}
vector<bool> encodebits(vector<bool> bits) {
	if ((int)bits.size() % 8 == 0) {
		bits.push_back(1);
		for (int i = 0; i < 7; i++) {
			bits.push_back(0);
		}
		assert((int)bits.size() % 8 == 0);
		return bits;
	}
	assert((int)bits.size() % 8 != 0);
	int rem = 8 - (int)bits.size() % 8;
	assert(rem >= 1);
	bits.push_back(1);
	for (int i = 1; i < rem; i++) {
		bits.push_back(0);
	}
	assert((int)bits.size() % 8 == 0);
	return bits;
}
vector<bool> decodebits(vector<bool> input) {
	assert((int)input.size() % 8 == 0);
	while (1) {
		assert(!input.empty());
		bool last = input.back();
		input.pop_back();
		if (last == 1) {
			break;
		}
	}
	return input;
}
string intToString(int x) {
	assert(0 <= x);
	if (x == 0) {
		return "0";
	}
	assert(1 <= x);
	string digs;
	while (x) {
		digs.push_back('0' + x % 10);
		x /= 10;
	}
	reverse(digs.begin(), digs.end());
	return digs;
}
int main() {
	// taskid = 0 => task = String Length
	// taskid = 1 => task = String Copy
	// taskid = 2 => task = String Copy (n bytes)
	// taskid = 3 => task = Reverse a string
	// taskid = 4 => task = Sum of an Integer Array
	// taskid = 5 => task = Bubble Sort
	// taskid = 6 => task = Distance Formula w/ 64-bit Floats
	// taskid = 7 => task = Min Max w/ 32-bit Floats
	// taskid = 8 => task = Binary Search
	// taskid = 9 => task = Vector x Matrix
	int taskid = 9;
	string codefilename = "code" + intToString(taskid) + ".txt";
	string binfilename = "bin" + intToString(taskid) + ".bin";
	string queriesfilename = "query" + intToString(taskid) + ".txt";
	string memoryfilename = "memory" + intToString(taskid) + ".in";
	bool watchRegisters = 0;
	bool printPath = 1;

	loadBijections();
	auto bits = encode(codefilename);
	writeBitsToFile(encodebits(bits), binfilename);
	vector<bool> read = decodebits(readBitsFromFile(binfilename));
	assert(read == bits);
	initMemory(memoryfilename);
	cout << (int)read.size() << " " << (int)read.size() / 8 << "\n";
	execute(bits, queriesfilename, watchRegisters, printPath);
	exit(0);
	return 0;
}