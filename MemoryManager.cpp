#include <cassert>
#include "MemoryManager.h"
#include "Help.h"
#include "Bijections.h"
#include <vector>
#include <bitset>
#include <string>
#include <map>
using namespace std;
const int BYTES = 8 * 1024; // number of BITS of memory
map<string, int> intRegisters;
map<string, float> floatRegisters;
vector<unsigned char> memory;
set<string> interestingIntRegisters;
set<string> interestingFloatRegisters;
void setFloatRegister(std::string reg, float value) {
	interestingFloatRegisters.insert(reg);
	assert(floatRegisters.count(reg));
	floatRegisters[reg] = value;
}
void setIntRegister(string reg, int value) {
	interestingIntRegisters.insert(reg);
	assert(intRegisters.count(reg));
	intRegisters[reg] = value;
}
int getIntRegister(string reg) {
	interestingIntRegisters.insert(reg);
	assert(intRegisters.count(reg));
	return intRegisters[reg];
}
float getFloatRegister(string reg) {
	interestingFloatRegisters.insert(reg);
	assert(floatRegisters.count(reg));
	return floatRegisters[reg];
}
unsigned char getMemoryByte(int position) {
	assert(0 <= position && position < (int)memory.size());
	return memory[position];
}
void setMemoryByte(int position, unsigned char byte) {
	assert(0 <= position && position < (int)memory.size());
	memory[position] = byte;
}
set<string> getInterestingIntRegisters() {
	return interestingIntRegisters;
}
set<string> getInterestingFloatRegisters() {
	return interestingFloatRegisters;
}
#include <iostream>
#include <fstream>
void loadMemoryFromFile(string filename) {
	ifstream file(filename);
	string word;
	while (file >> word) {
		assert(word == "reg" || word == "memory");
		if (word == "reg") {
			string type;
			file >> type;
			assert(type == "int" || type == "float" || type == "longlong" || type == "double");
			string reg;
			file >> reg;
			if (type == "int" || type == "longlong") {
				assert(intRegisters.count(reg));
				int x;
				file >> x;
				setIntRegister(reg, x);
				continue;
			}
			else {
				assert(type == "float" || type == "double");
				assert(floatRegisters.count(reg));
				float x;
				file >> x;
				setFloatRegister(reg, x);
				continue;
			}
			assert(0);
		}
		else {
			assert(word == "memory");
			/*
			memory char 444 23
			memory byte 444 23
			memory int 33 111
			memory longlong 33 111
			memory float 33 111.33
			memory double 33 111.33

			*/
			int a = 3;
			string type;
			file >> type;
			int pos;
			assert(type == "char" || type == "byte" || type == "int" || type == "longlong" || type == "float" || type == "double");
			file >> pos;
			if (type == "char") {
				string s;
				file >> s;
				assert((int)s.size() == 1);
				setMemoryByte(pos, s[0]);
				continue;
			}
			if (type == "byte") {
				int x;
				file >> x;
				assert(0 <= x && x < 256);
				setMemoryByte(pos, x);
				continue;
			}
			if (type == "int") {
				int x;
				file >> x;
				int mask = (1 << 8) - 1, mult = 1;
				for (int i = 0; i < 4; i++) {
					setMemoryByte(pos, (x & (mask * mult)) / mult);
					mult *= (1 << 8);
					pos++;
				}
				continue;
			}
			if (type == "longlong") {
				long long x;
				file >> x;
				long long mask = (1 << 8) - 1, mult = 1;
				for (int i = 0; i < 8; i++) {
					setMemoryByte(pos, (x & (mask * mult)) / mult);
					mult *= (1 << 8);
					pos++;
				}
				continue;
			}
			if (type == "float") {
				float x;
				file >> x;
				string s = floatToString(x);
				assert((int)s.size() == 4);
				for (int i = 0; i < 4; i++) {
					setMemoryByte(pos, s[i]);
					pos++;
				}
				continue;
			}
			if (type == "double") {
				float x;
				file >> x;
				string s = floatToString(x);
				assert((int)s.size() == 4);
				for (int i = 0; i < 4; i++) {
					setMemoryByte(pos, s[i]);
					pos++;
				}
				continue;
			}
			assert(0);
		}
	}
	file.close();
}
/*
query memory byte 4444
query memory char 4444
query memory longlong 4444
query memory int 4444
query memory float 4444
query memory double 4444


query reg int t0
query reg longlong t0
query reg float ft0
query reg doublef t0

---
reg int a0 333
reg float ft0 555
memory char 444 23
memory byte 444 23
memory int 33 111
memory longlong 33 111
memory float 33 111.33
memory double 33 111.33

*/
vector<float> ret;
void initMemory(string memoryFilename) {
	for (auto& intRegister : getIntRegisters()) {
		intRegisters[intRegister] = 0;
	}
	for (auto& floatRegister : getFloatRegisters()) {
		floatRegisters[floatRegister] = 0;
	}
	memory.resize(BYTES);
	for (int i = 0; i < BYTES; i++) {
		memory[i] = 0;
	}
	// define your memory Here:
/*
		# a0 = dst[3]
		# a1 = matrix[3][3]
		# a2 = vector[3]
		# t0 = r
		# t1 = c
		# t3 = 3
		# ft0 = d
		# Row for loop
*/
	

	loadMemoryFromFile(memoryFilename);

	//vector<float> vec = { 3, 2, -5.77 };
	//vector<vector<float>> matrix = { {3.3, 1.1, 4.44}, {5.7, 1.333333, 0}, {55, 13, 66} };
	//ret = { 0, 0, 0 };
	//for (int r = 0; r < 3; r++) {
	//	for (int c = 0; c < 3; c++) {
	//		ret[r] += matrix[r][c] * vec[c];
	//	}
	//}
	//for (int i = 0; i < 3; i++) {
	//	cout << i << " : " << ret[i] << "\n";
	//}
	// end of defining memory
}
void printMemory(int l, int r) {
	// special for matrixmul
	//cout << "special for matmul\n";
	//for (int i = 0; i < 3; i++) {
	//	cout << i << " : " << ret[i] << "\n";
	//}
	// special for matrix mul
	for (int i = l; i <= r; i++) {
		unsigned char byte = getMemoryByte(i);
		cout << "memory[" << i << "] = " << (int)byte << " = \"" << byte << "\"\n";
	}
}