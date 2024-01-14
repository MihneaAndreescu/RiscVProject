#include "Help.h"
#include <cassert>
#include <vector>
#include <string>
using namespace std;
vector<bool> binarystringtovector(string s) {
	vector<bool> a;
	for (auto& ch : s) {
		assert(ch == '0' || ch == '1');
		a.push_back(ch - '0');
	}
	return a;
}
void append(vector<bool>& bucket, vector<bool> nw) {
	for (int i = 0; i < (int)nw.size(); i++) {
		bucket.push_back(nw[i]);
	}
}
int stringtoint(string number) {
	int sgn = +1, val = 0, ptr = 0;
	while (ptr < (int)number.size() && (number[ptr] == '-' || number[ptr] == '+')) {
		if (number[ptr] == '-') {
			sgn *= -1;
		}
		ptr++;
	}
	while (ptr < (int)number.size() && '0' <= number[ptr] && number[ptr] <= '9') {
		val = 10 * val + number[ptr] - '0';
		ptr++;
	}
	assert(ptr == (int)number.size());
	return sgn * val;
}
vector<bool> inttobinary(int number) {
	vector<bool> sol;
	for (int b = 0; b < 32; b++) {
		if (number & (1 << b)) {
			sol.push_back(1);
		}
		else {
			sol.push_back(0);
		}
	}
	return sol;
}
string myPopBack(string s) {
	assert(!s.empty());
	s.pop_back();
	return s;
}
vector<string> splitString(string input, string badChars) {
	vector<string> sol;
	bool requestNew = 1;
	for (auto& ch : input) {
		bool isBad = 0;
		for (auto& bad : badChars) {
			if (bad == ch) {
				isBad = 1;
				break;
			}
		}
		if (isBad) {
			requestNew = 1;
			continue;
		}
		if (requestNew) {
			sol.push_back("");
			requestNew = 0;
		}
		assert(!sol.empty());
		sol.back().push_back(ch);
	}
	for (auto& s : sol) {
		assert(!s.empty());
	}
	return sol;
}
bool isPrefix(string input, string prefix) {
	if ((int)input.size() >= (int)prefix.size()) {
		for (int i = 0; i < (int)prefix.size(); i++) {
			if (prefix[i] != input[i]) {
				return 0;
			}
		}
		return 1;
	}
	return 0;
}
string floatToString(float value) {
	unsigned char bytes[sizeof(float)];
	memcpy(bytes, &value, sizeof(float));
	string str;
	for (int i = 0; i < (int)(sizeof(float)); i++) {
		str.push_back(bytes[i]);
	}
	return str;
}
float stringToFloat(string str) {
	unsigned char bytes[sizeof(float)];
	assert((int)str.size() == (int)(sizeof(float)));
	for (int i = 0; i < (int)(sizeof(float)); i++) {
		bytes[i] = str[i];
	}
	float value;
	memcpy(&value, bytes, sizeof(float));
	return value;
}