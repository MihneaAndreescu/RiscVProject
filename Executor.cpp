#include "Help.h"
#include "Bijections.h"
#include "Executor.h"
#include "MemoryManager.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <iomanip>
#include <functional>
using namespace std;
bool programEnd = 0;
bool watchIntRegisters = 1;
bool watchFloatRegisters = 1;
bool logWay = 1;
class Operation {
public:
	virtual void print() = 0;
	virtual void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) = 0;
};
void executeStep(vector<Operation*>& operations, vector<int>& callStack, int& ptr) {
	if (watchIntRegisters) {
		cout << "-------------------------------------------------------------------------\n";
		cout << "watch int Registers:\n";
		for (auto& intRegister : getInterestingIntRegisters()) {
			cout << intRegister << " = " << getIntRegister(intRegister) << "\n";
		}
		cout << "\n";
		cout << "watch float Registers:\n";
		for (auto& floatRegister : getInterestingFloatRegisters()) {
			cout << floatRegister << " = " << fixed << setprecision(6) << getFloatRegister(floatRegister) << "\n";
		}
		cout << "\n";
		printMemory(0, 15);
	}
	if (logWay) {
		if (ptr < 10) {
			cout << " ";
		}
		if (ptr < 100) {
			cout << " ";
		}
		if (ptr < 1000) {
			cout << " ";
		}
		cout << ptr << " ---> ";
	}
	assert(0 <= ptr && ptr < (int)operations.size());
	operations[ptr]->executeInner(operations, callStack, ptr);
}
class OperationFuncDef : public Operation {
public:
	int funcName;
	void print() override {
		cout << "function " << funcName << ":\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		ptr++;
	}
};
class OperationLi : public Operation {
public:
	string reg;
	int immediate;
	void print() override {
		cout << reg << " = " << immediate << " [int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setIntRegister(reg, immediate);
		ptr++;
	}
};
class OperationTagDef : public Operation {
public:
	int tagName;
	void print() override {
		cout << "tag " << tagName << ":\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		ptr++;
	}
};
class OperationAdd : public Operation {
public:
	string regDest;
	string rega;
	string regb;
	void print() override {
		cout << regDest << " = " << rega << " + " << regb << " [int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setIntRegister(regDest, getIntRegister(rega) + getIntRegister(regb));
		ptr++;
	}
};
class OperationLb : public Operation {
public:
	string regDest;
	int offset;
	string regSource;
	void print() override {
		cout << regDest << " = *(" << regSource << " + " << offset << ") [8 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		int location = getIntRegister(regSource) + offset;
		setIntRegister(regDest, getMemoryByte(location));
		ptr++;
	}
};
class OperationBeqz : public Operation {
public:
	string reg;
	bool inFront;
	int tagName;
	void print() override {
		cout << "branch to " << tagName << "<->" << ((inFront) ? ("front") : ("back")) << " if " << reg << " == 0[int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		if (getIntRegister(reg) != 0) {
			ptr++;
			return;
		}
		int location = ptr;
		while (1) {
			assert(0 <= location && location < (int)operations.size());
			if (OperationTagDef* operation = dynamic_cast<OperationTagDef*>(operations[location])) {
				if (operation->tagName == tagName) {
					ptr = location;
					return;
				}
			}
			location += ((inFront) ? (+1) : (-1));
		}
		assert(0);
	}
};
class OperationAddi : public Operation {
public:
	string regDest;
	string regSource;
	int immediate;
	void print() override {
		cout << regDest << " = " << regSource << " + " << immediate << " [int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setIntRegister(regDest, getIntRegister(regSource) + immediate);
		ptr++;
	}
};
class OperationJ : public Operation {
public:
	bool inFront;
	int tagName;
	void print() override {
		cout << "jump to " << tagName << "<->" << ((inFront) ? ("front") : ("back")) << "\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		int location = ptr;
		while (1) {
			assert(0 <= location && location < (int)operations.size());
			if (OperationTagDef* operation = dynamic_cast<OperationTagDef*>(operations[location])) {
				if (operation->tagName == tagName) {
					ptr = location;
					return;
				}
			}
			location += ((inFront) ? (+1) : (-1));
		}
		assert(0);
	}
};
class OperationMv : public Operation {
public:
	string regDest;
	string regSource;
	void print() override {
		cout << regDest << " = " << regSource << " [int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setIntRegister(regDest, getIntRegister(regSource));
		ptr++;
	}
};
class OperationRet : public Operation {
public:
	void print() override {
		cout << "return \n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		if (callStack.empty()) {
			cout << "program over\n";
			programEnd = 1;
			return;
		}
		ptr = callStack.back();
		callStack.pop_back();
	}
};
class OperationMain : public Operation {
public:
	void print() override {
		cout << "function main:\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		ptr++;
	}
};
class OperationCall : public Operation {
public:
	int funcName;
	void print() override {
		cout << "call function " << funcName << "\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		callStack.push_back(ptr + 1);
		for (int i = 0; i < (int)operations.size(); i++) {
			if (OperationFuncDef* operation = dynamic_cast<OperationFuncDef*>(operations[i])) {
				if (operation->funcName == funcName) {
					ptr = i;
					return;
				}
			}
		}
		assert(0);
	}
};
class OperationSb : public Operation {
public:
	string reg1;
	int offset;
	string reg2;
	void print() override {
		cout << "*(" << reg2 << " + " << offset << ") = " << reg1 << " [8 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		int location = getIntRegister(reg2) + offset;
		unsigned char byte = (getIntRegister(reg1) & ((1 << 8) - 1));
		setMemoryByte(location, byte);
		ptr++;
	}
};
class OperationBge : public Operation {
public:
	string reg1;
	string reg2;
	bool inFront;
	int tagName;
	void print() override {
		cout << "branch to " << tagName << "<->" << ((inFront) ? ("front") : ("back")) << " if " << reg1 << " >= " << reg2 << " [int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		if (!(getIntRegister(reg1) >= getIntRegister(reg2))) {
			ptr++;
			return;
		}
		int location = ptr;
		while (1) {
			assert(0 <= location && location < (int)operations.size());
			if (OperationTagDef* operation = dynamic_cast<OperationTagDef*>(operations[location])) {
				if (operation->tagName == tagName) {
					ptr = location;
					return;
				}
			}
			location += ((inFront) ? (+1) : (-1));
		}
	}
};
class OperationSd : public Operation {
public:
	string reg1;
	int offset;
	string reg2;
	void print() override {
		cout << "*(" << reg2 << " + " << offset << ") = " << reg1 << " [32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		int location = getIntRegister(reg2) + offset;
		int valueAtRegister = getIntRegister(reg1);
		int mask = (1 << 8) - 1, mult = 1;
		for (int i = 0; i < 4; i++) {
			setMemoryByte(location, (valueAtRegister & (mask * mult)) / mult);
			mult *= (1 << 8);
			location++;
		}
		ptr++;
	}
};
class OperationSrai : public Operation {
public:
	string regDest;
	string regSource;
	int immediate;
	void print() override {
		cout << regDest << " = " << regSource << " / 2^" << immediate << " [int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setIntRegister(regDest, getIntRegister(regSource) >> immediate);
		ptr++;
	}
};
class OperationSub : public Operation {
public:
	string regDest;
	string rega;
	string regb;
	void print() override {
		cout << regDest << " = " << rega << " - " << regb << " [int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setIntRegister(regDest, getIntRegister(rega) - getIntRegister(regb));
		ptr++;
	}
};
class OperationLd : public Operation {
public:
	string regDest;
	int offset;
	string regSource;
	void print() override {
		cout << regDest << " = *(" << regSource << " + " << offset << ") [32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		int location = getIntRegister(regSource) + offset;
		int value = 0;
		int mult = 1;
		for (int i = 0; i < 4; i++) {
			value += mult * getMemoryByte(location);
			mult *= (1 << 8);
			location++;
		}
		setIntRegister(regDest, value);
		ptr++;
	}
};
class OperationSlli : public Operation {
public:
	string regDest;
	string regSource;
	int immediate;
	void print() override {
		cout << regDest << " = " << regSource << " * 2^" << immediate << " [int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setIntRegister(regDest, getIntRegister(regSource) << immediate);
		ptr++;
	}
};
class OperationLw : public Operation {
public:
	string regDest;
	int offset;
	string regSource;
	void print() override {
		cout << regDest << " = *(" << regSource << " + " << offset << ") [32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		int location = getIntRegister(regSource) + offset;
		int value = 0;
		int mult = 1;
		for (int i = 0; i < 4; i++) {
			value += mult * getMemoryByte(location);
			mult *= (1 << 8);
			location++;
		}
		setIntRegister(regDest, value);
		ptr++;
	}
};
class OperationBle : public Operation {
public:
	string reg1;
	string reg2;
	bool inFront;
	int tagName;
	void print() override {
		cout << "branch to " << tagName << "<->" << ((inFront) ? ("front") : ("back")) << " if " << reg1 << " <= " << reg2 << " [int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		if (!(getIntRegister(reg1) <= getIntRegister(reg2))) {
			ptr++;
			return;
		}
		int location = ptr;
		while (1) {
			assert(0 <= location && location < (int)operations.size());
			if (OperationTagDef* operation = dynamic_cast<OperationTagDef*>(operations[location])) {
				if (operation->tagName == tagName) {
					ptr = location;
					return;
				}
			}
			location += ((inFront) ? (+1) : (-1));
		}
	}
};
class OperationBnez : public Operation {
public:
	string reg;
	bool inFront;
	int tagName;
	void print() override {
		cout << "branch to " << tagName << "<->" << ((inFront) ? ("front") : ("back")) << " if " << reg << " != 0[int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		if (getIntRegister(reg) == 0) {
			ptr++;
			return;
		}
		int location = ptr;
		while (1) {
			assert(0 <= location && location < (int)operations.size());
			if (OperationTagDef* operation = dynamic_cast<OperationTagDef*>(operations[location])) {
				if (operation->tagName == tagName) {
					ptr = location;
					return;
				}
			}
			location += ((inFront) ? (+1) : (-1));
		}
		assert(0);
	}
};
class OperationFl : public Operation {
public:
	string regDest;
	int offset;
	string regSource;
	void print() override {
		cout << regDest << " = *(" << regSource << " + " << offset << ") [float 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		cout << "start!\n";
		int location = getIntRegister(regSource) + offset;
		cout << " : " << regSource << " and " << regDest << "\n";
		string s;
		for (int i = 0; i < sizeof(float); i++) {
			s.push_back(getMemoryByte(location));
			location++;
		}
		cout << "regDest = " << regDest << "\n";
		setFloatRegister(regDest, stringToFloat(s));
		ptr++;
	}
};
class OperationFsub : public Operation {
public:
	string regDest;
	string rega;
	string regb;
	void print() override {
		cout << regDest << " = " << rega << " - " << regb << " [float 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setFloatRegister(regDest, getFloatRegister(rega) - getFloatRegister(regb));
		ptr++;
	}
};
class OperationFmul : public Operation {
public:
	string regDest;
	string rega;
	string regb;
	void print() override {
		cout << regDest << " = " << rega << " * " << regb << " [float 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setFloatRegister(regDest, getFloatRegister(rega) * getFloatRegister(regb));
		ptr++;
	}
};
class OperationFadd : public Operation {
public:
	string regDest;
	string rega;
	string regb;
	void print() override {
		cout << regDest << " = " << rega << " + " << regb << " [float 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setFloatRegister(regDest, getFloatRegister(rega) + getFloatRegister(regb));
		ptr++;
	}
};
class OperationFsqrt : public Operation {
public:
	string regDest;
	string regSource;
	void print() override {
		cout << regDest << " = sqrt(" << regSource << ") [float 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setFloatRegister(regDest, sqrt(getFloatRegister(regSource)));
		ptr++;
	}
};
class OperationFmv : public Operation {
public:
	string regDest;
	string regSource;
	void print() override {
		cout << regDest << " = " << regSource << " [float 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setFloatRegister(regDest, getFloatRegister(regSource));
		ptr++;
	}
};
class OperationFgt : public Operation {
public:
	string regDest;
	string reg1;
	string reg2;
	void print() override {
		cout << regDest << " = (" << reg1 << " > " << reg2 << ") [float 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setIntRegister(regDest, getFloatRegister(reg1) > getFloatRegister(reg2));
		ptr++;
	}
};
class OperationFlt : public Operation {
public:
	string regDest;
	string reg1;
	string reg2;
	void print() override {
		cout << regDest << " = (" << reg1 << " < " << reg2 << ") [float 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setIntRegister(regDest, getFloatRegister(reg1) < getFloatRegister(reg2));
		ptr++;
	}
};
class OperationFsw : public Operation {
public:
	string reg1;
	int offset;
	string reg2;
	void print() override {
		cout << "*(" << reg2 << " + " << offset << ") = " << reg1 << " [float 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		int location = getIntRegister(reg2) + offset;
		float valueAtRegister = getFloatRegister(reg1);
		string s = floatToString(valueAtRegister);
		assert((int)s.size() == sizeof(float));
		for (int i = 0; i < sizeof(float); i++) {
			setMemoryByte(location, s[i]);
			location++;
		}
		ptr++;
	}
};
class OperationBgt : public Operation {
public:
	string reg1;
	string reg2;
	bool inFront;
	int tagName;
	void print() override {
		cout << "branch to " << tagName << "<->" << ((inFront) ? ("front") : ("back")) << " if " << reg1 << " > " << reg2 << " [int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		if (!(getIntRegister(reg1) > getIntRegister(reg2))) {
			ptr++;
			return;
		}
		int location = ptr;
		while (1) {
			assert(0 <= location && location < (int)operations.size());
			if (OperationTagDef* operation = dynamic_cast<OperationTagDef*>(operations[location])) {
				if (operation->tagName == tagName) {
					ptr = location;
					return;
				}
			}
			location += ((inFront) ? (+1) : (-1));
		}
	}
};
class OperationFmvsx : public Operation {
public:
	string regDest;
	string regSource;
	void print() override {
		cout << regDest << " = " << regSource << " [float = int 32 bit]\n";
	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		if (logWay) {
			print();
		}
		setFloatRegister(regDest, getIntRegister(regSource));
		ptr++;
	}
};
/*
template:
class Operation : public Operation {
public:
	void print() override {

	}
	void executeInner(vector<Operation*>& operations, vector<int>& callStack, int& ptr) override {
		assert(0);
	}
};
*/
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
query reg double t0

*/
#include <fstream>
#include "Help.h"
void answerQueries(string filename) {
	ifstream file(filename);
	string q;
	while (file >> q) {
		assert(q == "query");
		string what;
		file >> what;
		assert(what == "memory" || what == "reg");
		if (what == "memory") {
			string type;
			file >> type;
			assert(type == "byte" || type == "char" || type == "int" || type == "longlong" || type == "float" || type == "double");
			int pos;
			file >> pos;
			cout << type << " at " << pos << " is ";
			if (type == "byte") {
				unsigned int x = getMemoryByte(pos);
				cout << x << "\n";
				continue;
			}
			if (type == "char") {
				char ch = getMemoryByte(pos);
				cout << ch << "\n";
				continue;
			}
			if (type == "int") {
				int value = 0;
				int mult = 1;
				for (int i = 0; i < 4; i++) {
					value += mult * getMemoryByte(pos);
					mult *= (1 << 8);
					pos++;
				}
				cout << value << "\n";
				continue;
			}
			if (type == "longlong") {
				long long value = 0;
				long long mult = 1;
				for (int i = 0; i < 8; i++) {
					value += mult * getMemoryByte(pos);
					mult *= (1 << 8);
					pos++;
				}
				cout << value << "\n";
				continue;
			}
			if (type == "float") {
				string s;
				s += getMemoryByte(pos);
				s += getMemoryByte(pos + 1);
				s += getMemoryByte(pos + 2);
				s += getMemoryByte(pos + 3);
				float value = stringToFloat(s);
				cout << value << "\n";
				continue;
			}
			if (type == "double") {
				string s;
				s += getMemoryByte(pos);
				s += getMemoryByte(pos + 1);
				s += getMemoryByte(pos + 2);
				s += getMemoryByte(pos + 3);
				double value = stringToFloat(s);
				cout << value << "\n";
				continue;
			}
			assert(0);
		}
		else {
			assert(what == "reg");
			string type;
			file >> type;
			assert(type == "int" || type == "longlong" || type == "float" || type == "double");
			string reg;
			file >> reg;
			cout << type << " " << reg << " = ";
			if (type == "int") {
				cout << getIntRegister(reg) << "\n";
				continue;
			}
			if (type == "longlong") {
				cout << getIntRegister(reg) << "\n";
				continue;
			}
			if (type == "float") {
				cout << getFloatRegister(reg) << "\n";
				continue;
			}
			if (type == "double") {
				cout << getFloatRegister(reg) << "\n";
				continue;
			}
			assert(0);
			/*
			query reg int t0
			query reg longlong t0
			query reg float ft0
			query reg double t0
			*/
		}
	}
	file.close();
}
void executeInner(vector<bool> code) {
	cout << "dimension = " << (int)code.size() << "\n";
	for (auto b : code) {
		cout << b << " ";
	}
	cout << "\n";
	int ptr = 0;
	function<string()> readOperation = [&]() {
		string currentOperation;
		while (1) {
			assert(ptr < (int)code.size());
			if (code[ptr++]) {
				currentOperation.push_back('1');
			}
			else {
				currentOperation.push_back('0');
			}
			if (doThisBitsRepresentAnOperation(currentOperation)) {
				break;
			}
		}
		return bitsToOperation(currentOperation);
	};
	function<string()> readRegister = [&]() {
		string currentRegister;
		while (1) {
			assert(ptr < (int)code.size());
			if (code[ptr++]) {
				currentRegister.push_back('1');
			}
			else {
				currentRegister.push_back('0');
			}
			if (doThisBitsRepresentARegister(currentRegister)) {
				break;
			}
		}
		return bitsToRegister(currentRegister);
	};
	function<int()> countZerosToOne = [&]() {
		int cnt0 = 0;
		while (1) {
			assert(ptr < (int)code.size());
			if (code[ptr++]) {
				break;
			}
			cnt0++;
		}
		return cnt0;
	};
	function<int()>readInt = [&]() {
		int sol = 0;
		for (int bit = 0; bit < 32; bit++) {
			assert(ptr < (int)code.size());
			if (code[ptr++]) {
				sol += (1 << bit);
			}
		}
		return sol;
	};
	function<bool()>readBit = [&]() {
		assert(ptr < (int)code.size());
		return code[ptr++];
	};
	vector<Operation*> operations;
	while (ptr < (int)code.size()) {
		string operation = readOperation();
		if (operation == "funcdef") {
			OperationFuncDef* operation = new OperationFuncDef;
			operation->funcName = countZerosToOne();
			operations.push_back(operation);
			continue;
		}
		if (operation == "li") {
			OperationLi* operation = new OperationLi;
			operation->reg = readRegister();
			operation->immediate = readInt();
			operations.push_back(operation);
			continue;
		}
		if (operation == "tagdef") {
			OperationTagDef* operation = new OperationTagDef;
			operation->tagName = countZerosToOne();
			operations.push_back(operation);
			continue;
		}
		if (operation == "add") {
			OperationAdd* operation = new OperationAdd;
			operation->regDest = readRegister();
			operation->rega = readRegister();
			operation->regb = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "lb") {
			OperationLb* operation = new OperationLb;
			operation->regDest = readRegister();
			operation->offset = readInt();
			operation->regSource = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "beqz") {
			OperationBeqz* operation = new OperationBeqz;
			operation->reg = readRegister();
			operation->inFront = readBit();
			operation->tagName = countZerosToOne();
			operations.push_back(operation);
			continue;
		}
		if (operation == "addi") {
			OperationAddi* operation = new OperationAddi;
			operation->regDest = readRegister();
			operation->regSource = readRegister();
			operation->immediate = readInt();
			operations.push_back(operation);
			continue;
		}
		if (operation == "j") {
			OperationJ* operation = new OperationJ;
			operation->inFront = readBit();
			operation->tagName = countZerosToOne();
			operations.push_back(operation);
			continue;
		}
		if (operation == "mv") {
			OperationMv* operation = new OperationMv;
			operation->regDest = readRegister();
			operation->regSource = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "ret") {
			OperationRet* operation = new OperationRet;
			operations.push_back(operation);
			continue;
		}
		if (operation == "main") {
			OperationMain* operation = new OperationMain;
			operations.push_back(operation);
			continue;
		}
		if (operation == "call") {
			OperationCall* operation = new OperationCall;
			operation->funcName = countZerosToOne();
			operations.push_back(operation);
			continue;
		}
		if (operation == "sb") {
			OperationSb* operation = new OperationSb;
			operation->reg1 = readRegister();
			operation->offset = readInt();
			operation->reg2 = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "bge") {
			OperationBge* operation = new OperationBge;
			operation->reg1 = readRegister();
			operation->reg2 = readRegister();
			operation->inFront = readBit();
			operation->tagName = countZerosToOne();
			operations.push_back(operation);
			continue;
		}
		if (operation == "sd") {
			OperationSd* operation = new OperationSd;
			operation->reg1 = readRegister();
			operation->offset = readInt();
			operation->reg2 = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "srai") {
			OperationSrai* operation = new OperationSrai;
			operation->regDest = readRegister();
			operation->regSource = readRegister();
			operation->immediate = readInt();
			operations.push_back(operation);
			continue;
		}
		if (operation == "sub") {
			OperationSub* operation = new OperationSub;
			operation->regDest = readRegister();
			operation->rega = readRegister();
			operation->regb = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "ld") {
			OperationLd* operation = new OperationLd;
			operation->regDest = readRegister();
			operation->offset = readInt();
			operation->regSource = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "slli") {
			OperationSlli* operation = new OperationSlli;
			operation->regDest = readRegister();
			operation->regSource = readRegister();
			operation->immediate = readInt();
			operations.push_back(operation);
			continue;
		}
		if (operation == "lw") {
			OperationLw* operation = new OperationLw;
			operation->regDest = readRegister();
			operation->offset = readInt();
			operation->regSource = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "ble") {
			OperationBle* operation = new OperationBle;
			operation->reg1 = readRegister();
			operation->reg2 = readRegister();
			operation->inFront = readBit();
			operation->tagName = countZerosToOne();
			operations.push_back(operation);
			continue;
		}
		if (operation == "bnez") {
			OperationBnez* operation = new OperationBnez;
			operation->reg = readRegister();
			operation->inFront = readBit();
			operation->tagName = countZerosToOne();
			operations.push_back(operation);
			continue;
		}
		if (operation == "fld") {
			OperationFl* operation = new OperationFl;
			operation->regDest = readRegister();
			operation->offset = readInt();
			operation->regSource = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "fsub") {
			OperationFsub* operation = new OperationFsub;
			operation->regDest = readRegister();
			operation->rega = readRegister();
			operation->regb = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "fmul") {
			OperationFmul* operation = new OperationFmul;
			operation->regDest = readRegister();
			operation->rega = readRegister();
			operation->regb = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "fadd") {
			OperationFadd* operation = new OperationFadd;
			operation->regDest = readRegister();
			operation->rega = readRegister();
			operation->regb = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "fsqrt") {
			OperationFsqrt* operation = new OperationFsqrt;
			operation->regDest = readRegister();
			operation->regSource = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "fmv") {
			OperationFmv* operation = new OperationFmv;
			operation->regDest = readRegister();
			operation->regSource = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "fgt") {
			OperationFgt* operation = new OperationFgt;
			operation->regDest = readRegister();
			operation->reg1 = readRegister();
			operation->reg2 = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "flt") {
			OperationFlt* operation = new OperationFlt;
			operation->regDest = readRegister();
			operation->reg1 = readRegister();
			operation->reg2 = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "fsw") {
			OperationFsw* operation = new OperationFsw;
			operation->reg1 = readRegister();
			operation->offset = readInt();
			operation->reg2 = readRegister();
			operations.push_back(operation);
			continue;
		}
		if (operation == "bgt") {
			OperationBgt* operation = new OperationBgt;
			operation->reg1 = readRegister();
			operation->reg2 = readRegister();
			operation->inFront = readBit();
			operation->tagName = countZerosToOne();
			operations.push_back(operation);
			continue;
		}
		if (operation == "fmvsx") {
			OperationFmvsx* operation = new OperationFmvsx;
			operation->regDest = readRegister();
			operation->regSource = readRegister();
			operations.push_back(operation);
			continue;
		}


		cout << "unknown operation = " << operation << "\n";
		cout << "(you are now inside of the executor)\n";
		exit(0);
		//cout << currentOperation << "\n";
	}
	assert(ptr == (int)code.size());
	int posMain = -1;
	for (int i = 0; i < (int)operations.size(); i++) {
		if (dynamic_cast<OperationMain*>(operations[i])) {
			assert(posMain == -1);
			posMain = i;
		}
	}
	assert(posMain != -1);
	cout << "----------------------------------------------------------\n";
	cout << "posMain = " << posMain << "\n";
	cout << "finished unpacking the code\n";
	int lineId = -1;
	for (auto& operation : operations) {
		lineId++;
		if (lineId < 10) {
			cout << " ";
		}
		if (lineId < 100) {
			cout << " ";
		}
		if (lineId < 1000) {
			cout << " ";
		}
		cout << lineId << " ---> ";
		operation->print();
	}
	cout << "----------------------------------------------------------\n";
	vector<int> callStack;
	while (!programEnd) {
		executeStep(operations, callStack, posMain);
	}
	answerQueries("queries.txt");
	printMemory(0, 0);
	exit(0);
	cout << "special for matmul finitos\n";
	ptr = 0;
	for (int i = 0; i < 3; i++) {
		string s;
		s += getMemoryByte(ptr++);
		s += getMemoryByte(ptr++);
		s += getMemoryByte(ptr++);
		s += getMemoryByte(ptr++);
		cout << i << " : " << stringToFloat(s) << "\n";
	}
	exit(0);
}