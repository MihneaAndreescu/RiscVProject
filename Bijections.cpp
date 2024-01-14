#include "Bijections.h"
#include "Help.h"
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
using namespace std;
vector<pair<string, string>> ob;
vector<pair<string, string>> rb;
vector<string> intRegisters;
vector<string> floatRegisters;
void loadOperations() {
	ifstream stream("operations.txt");
	string o, b;
	while (stream >> o >> b) {
		ob.push_back({ o, b });
		for (auto& ch : b) {
			assert(ch == '0' || ch == '1');
		}
	}
	for (int i = 0; i < (int)ob.size(); i++) {
		for (int j = i + 1; j < (int)ob.size(); j++) {
			assert(ob[i].first != ob[j].first);
			assert(ob[i].second != ob[j].second);
		}
	}
	for (int i = 0; i < (int)ob.size(); i++) {
		for (int j = 0; j < (int)ob.size(); j++) {
			if (i == j) {
				continue;
			}
			assert(!isPrefix(ob[i].second, ob[j].second));
		}
	}
}
void loadRegisters() {
	ifstream stream("registers.txt");
	string r, b, dataType;
	while (stream >> r >> b >> dataType) {
		rb.push_back({ r, b });
		for (auto& ch : b) {
			assert(ch == '0' || ch == '1');
		}
		assert(dataType == "int" || dataType == "float");
		if (dataType == "int") {
			intRegisters.push_back(r);
		}
		if (dataType == "float") {
			floatRegisters.push_back(r);
		}
	}
	for (int i = 0; i < (int)rb.size(); i++) {
		for (int j = i + 1; j < (int)rb.size(); j++) {
			assert(rb[i].first != rb[j].first);
			assert(rb[i].second != rb[j].second);
		}
	}
	for (int i = 0; i < (int)rb.size(); i++) {
		for (int j = 0; j < (int)rb.size(); j++) {
			if (i == j) {
				continue;
			}
			assert(!isPrefix(rb[i].second, rb[j].second));
		}
	}
}
void loadBijections() {
	loadOperations();
	loadRegisters();
}
string operationToBits(string operation) {
	for (auto& [o, b] : ob) {
		if (o == operation) {
			return b;
		}
	}
	cout << "operation " << operation << " was not found\n";
	assert(0);
}
string bitsToOperation(string bits) {
	for (auto& [o, b] : ob) {
		if (b == bits) {
			return o;
		}
	}
	assert(0);
}
string registerToBits(string reg) {
	for (auto& [r, b] : rb) {
		if (r == reg) {
			return b;
		}
	}
	cout << "register " << reg << " was not found.\n";
	assert(0);
}
string bitsToRegister(string bits) {
	for (auto& [r, b] : rb) {
		if (b == bits) {
			return r;
		}
	}
	assert(0);
}
bool doThisBitsRepresentAnOperation(string bits) {
	for (auto& [o, b] : ob) {
		if (b == bits) {
			return 1;
		}
	}
	return 0;
}
bool doThisBitsRepresentARegister(string bits) {
	for (auto& [r, b] : rb) {
		if (b == bits) {
			return 1;
		}
	}
	return 0;
}
vector<string> getIntRegisters() {
	return intRegisters;
}
vector<string> getFloatRegisters() {
	//cout << "the ones : ";
	//for (auto& rr : floatRegisters) {
	//	cout << rr << " ";
	//}
	//cout << "\n";
	return floatRegisters;
}