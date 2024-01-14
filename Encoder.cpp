#include "Encoder.h"
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <algorithm>
#include <map>
#include "Bijections.h"
#include "Help.h"
using namespace std;
string readwholefile(string filename);
string processfilecontent(string filecontent);
vector<bool> encode(string filename) {
	string wholeString = readwholefile(filename);
	wholeString = processfilecontent(wholeString);
	int ptr = 0;
	auto getword = [&]() {
		while (ptr < (int)wholeString.size() && wholeString[ptr] == ' ') {
			ptr++;
		}
		string word;
		while (ptr < (int)wholeString.size() && wholeString[ptr] != ' ') {
			word.push_back(wholeString[ptr++]);
		}
		return word;
	};
	map<string, int> functionId;
	map<string, int> tagId;
	{
		vector<string> functions, tags;
		while (1) {
			string word = getword();
			if (word.empty()) {
				break;
			}
			if (word == ".global") {
				functions.push_back(getword());
				continue;
			}
			if (word.back() == ':') {
				word.pop_back();
				tags.push_back(word);
				continue;
			}
		}
		for (int i = 0; i < (int)tags.size(); i++) {
			bool isFunction = 0;
			for (auto& f : functions) {
				if (f == tags[i]) {
					isFunction = 1;
					break;
				}
			}
			if (isFunction) {
				swap(tags[i], tags.back());
				i--;
				tags.pop_back();
			}
		}
		ptr = 0;
		sort(functions.begin(), functions.end());
		sort(tags.begin(), tags.end());
		functions.resize(unique(functions.begin(), functions.end()) - functions.begin());
		tags.resize(unique(tags.begin(), tags.end()) - tags.begin());
		for (int i = 1; i < (int)functions.size(); i++) {
			assert(functions[i - 1] != functions[i]);
		}
		for (int i = 1; i < (int)tags.size(); i++) {
			assert(tags[i - 1] != tags[i]);
		}
		for (int i = 0; i < (int)functions.size(); i++) {
			functionId[functions[i]] = i;
		}
		for (int i = 0; i < (int)tags.size(); i++) {
			tagId[tags[i]] = i;
		}
	}
	vector<bool> code;
	while (1) {
		string word = getword();
		if (word.empty()) {
			break;
		}
		if (word == ".section") {
			assert(getword() == ".text");
			continue;
		}
		if (word == ".global") {
			assert(functionId.count(getword()));
			continue;
		}
		if (word == "main:") {
			append(code, binarystringtovector(operationToBits("main")));
			continue;
		}
		if (word.back() == ':') {
			word.pop_back();
			if (functionId.count(word)) {
				string functionName = word;
				assert(functionId.count(functionName));
				append(code, binarystringtovector(operationToBits("funcdef")));
				for (int zero = 1; zero <= functionId[functionName]; zero++) {
					append(code, { 0 });
				}
				append(code, { 1 });
			}
			else {
				string tagName = word;
				append(code, binarystringtovector(operationToBits("tagdef")));
				assert(tagId.count(tagName));
				for (int zero = 1; zero <= tagId[tagName]; zero++) {
					append(code, { 0 });
				}
				append(code, { 1 });
			}
			continue;
		}
		if (word == "call") {
			append(code, binarystringtovector(operationToBits(word)));
			string functionName = getword();
			assert(functionId.count(functionName));
			for (int zero = 1; zero <= functionId[functionName]; zero++) {
				append(code, { 0 });
			}
			append(code, { 1 });
			continue;
		}
		if (word == "li") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, inttobinary(stringtoint(getword())));
			continue;
		}
		if (word == "add") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(getword())));
			continue;
		}
		if (word == "lb") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			vector<string> split = splitString(getword(), "()");
			assert((int)split.size() == 2);
			append(code, inttobinary(stringtoint(split[0])));
			append(code, binarystringtovector(registerToBits(split[1])));
			continue;
		}
		if (word == "beqz") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			string tag = getword();
			assert(!tag.empty());
			assert(tag.back() == 'b' || tag.back() == 'f');
			if (tag.back() == 'b') {
				append(code, { 0 });
			}
			else {
				append(code, { 1 });
			}
			string tagName = myPopBack(tag);
			assert(tagId.count(tagName));
			for (int zero = 1; zero <= tagId[tagName]; zero++) {
				append(code, { 0 });
			}
			append(code, { 1 });
			continue;
		}
		if (word == "addi") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, inttobinary(stringtoint(getword())));
			continue;
		}
		if (word == "j") {
			append(code, binarystringtovector(operationToBits(word)));
			string tag = getword();
			assert(!tag.empty());
			assert(tag.back() == 'b' || tag.back() == 'f');
			if (tag.back() == 'b') {
				append(code, { 0 });
			}
			else {
				append(code, { 1 });
			}
			string tagName = myPopBack(tag);
			assert(tagId.count(tagName));
			for (int zero = 1; zero <= tagId[tagName]; zero++) {
				append(code, { 0 });
			}
			append(code, { 1 });
			continue;
		}
		if (word == "mv") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(getword())));
			continue;
		}
		if (word == "ret") {
			append(code, binarystringtovector(operationToBits(word)));
			continue;
		}
		if (word == "sb") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			vector<string> split = splitString(getword(), "()");
			assert((int)split.size() == 2);
			append(code, inttobinary(stringtoint(split[0])));
			append(code, binarystringtovector(registerToBits(split[1])));
			continue;
		}
		if (word == "bge") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			string tag = getword();
			assert(!tag.empty());
			assert(tag.back() == 'b' || tag.back() == 'f');
			if (tag.back() == 'b') {
				append(code, { 0 });
			}
			else {
				append(code, { 1 });
			}
			string tagName = myPopBack(tag);
			assert(tagId.count(tagName));
			for (int zero = 1; zero <= tagId[tagName]; zero++) {
				append(code, { 0 });
			}
			append(code, { 1 });
			continue;
		}
		if (word == "sd") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			vector<string> split = splitString(getword(), "()");
			assert((int)split.size() == 2);
			append(code, inttobinary(stringtoint(split[0])));
			append(code, binarystringtovector(registerToBits(split[1])));
			continue;
		}
		if (word == "srai") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, inttobinary(stringtoint(getword())));
			continue;
		}
		if (word == "sub") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(getword())));
			continue;
		}
		if (word == "ld") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			vector<string> split = splitString(getword(), "()");
			assert((int)split.size() == 2);
			append(code, inttobinary(stringtoint(split[0])));
			append(code, binarystringtovector(registerToBits(split[1])));
			continue;
		}
		if (word == "slli") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, inttobinary(stringtoint(getword())));
			continue;
		}
		if (word == "lw") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			vector<string> split = splitString(getword(), "()");
			assert((int)split.size() == 2);
			append(code, inttobinary(stringtoint(split[0])));
			append(code, binarystringtovector(registerToBits(split[1])));
			continue;
		}
		if (word == "ble") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			string tag = getword();
			assert(!tag.empty());
			assert(tag.back() == 'b' || tag.back() == 'f');
			if (tag.back() == 'b') {
				append(code, { 0 });
			}
			else {
				append(code, { 1 });
			}
			string tagName = myPopBack(tag);
			assert(tagId.count(tagName));
			for (int zero = 1; zero <= tagId[tagName]; zero++) {
				append(code, { 0 });
			}
			append(code, { 1 });
			continue;
		}
		if (word == "bnez") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			string tag = getword();
			assert(!tag.empty());
			assert(tag.back() == 'b' || tag.back() == 'f');
			if (tag.back() == 'b') {
				append(code, { 0 });
			}
			else {
				append(code, { 1 });
			}
			string tagName = myPopBack(tag);
			assert(tagId.count(tagName));
			for (int zero = 1; zero <= tagId[tagName]; zero++) {
				append(code, { 0 });
			}
			append(code, { 1 });
			continue;
		}
		if (word == "fld" || word == "flw") {
			append(code, binarystringtovector(operationToBits("fld")));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			vector<string> split = splitString(getword(), "()");
			assert((int)split.size() == 2);
			append(code, inttobinary(stringtoint(split[0])));
			append(code, binarystringtovector(registerToBits(split[1])));
			continue;
		}
		if (word == "fsub.d") {
			append(code, binarystringtovector(operationToBits("fsub")));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(getword())));
			continue;
		}
		if (word == "fmul.d" || word == "fmul.s") {
			append(code, binarystringtovector(operationToBits("fmul")));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(getword())));
			continue;
		}
		if (word == "fadd.d" || word == "fadd.s") {
			append(code, binarystringtovector(operationToBits("fadd")));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(getword())));
			continue;
		}
		if (word == "fsqrt.d") {
			append(code, binarystringtovector(operationToBits("fsqrt")));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(getword())));
			continue;
		}
		if (word == "fmv.s") {
			append(code, binarystringtovector(operationToBits("fmv")));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(getword())));
			continue;
		}
		if (word == "fgt.s") {
			append(code, binarystringtovector(operationToBits("fgt")));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(getword())));
			continue;
		}
		if (word == "flt.s") {
			append(code, binarystringtovector(operationToBits("flt")));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(getword())));
			continue;
		}
		if (word == "fsw") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			vector<string> split = splitString(getword(), "()");
			assert((int)split.size() == 2);
			append(code, inttobinary(stringtoint(split[0])));
			append(code, binarystringtovector(registerToBits(split[1])));
			continue;
		}
		if (word == "bgt") {
			append(code, binarystringtovector(operationToBits(word)));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			string tag = getword();
			assert(!tag.empty());
			assert(tag.back() == 'b' || tag.back() == 'f');
			if (tag.back() == 'b') {
				append(code, { 0 });
			}
			else {
				append(code, { 1 });
			}
			string tagName = myPopBack(tag);
			assert(tagId.count(tagName));
			for (int zero = 1; zero <= tagId[tagName]; zero++) {
				append(code, { 0 });
			}
			append(code, { 1 });
			continue;
		}
		if (word == "fmv.s.x") {
			append(code, binarystringtovector(operationToBits("fmvsx")));
			append(code, binarystringtovector(registerToBits(myPopBack(getword()))));
			append(code, binarystringtovector(registerToBits(getword())));
			continue;
		}
		cout << "unknown word = " << word << "\n";
		assert(0);
		break;
	}
	//cout << "finished encoding!\n";
	return code;
}
string processfilecontent(string s) {
	string s2;
	bool incomment = 0;
	for (auto& ch : s) {
		if (ch == '#') {
			incomment = 1;
		}
		if (ch == '\n') {
			incomment = 0;
		}
		if (!incomment) {
			if (ch == '\n' || ch == '\t' || ch == ' ') {
				ch = ' ';
			}
			if (ch == ' ' && !s2.empty() && s2.back() == ' ') {
				continue;
			}
			s2.push_back(ch);
		}
	}
	return s2;
}
string readwholefile(string filename) {
	FILE* stream;
	errno_t _err = freopen_s(&stream, filename.c_str(), "r", stdin);
	assert(!(_err != 0 || stream == nullptr));
	stringstream buf;
	buf << std::cin.rdbuf();
	fclose(stream);
	return buf.str();
}