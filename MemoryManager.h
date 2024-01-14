#pragma once
#include <string>
#include <set>
void initMemory(std::string memoryFilename);
void setIntRegister(std::string reg, int value);
void setFloatRegister(std::string reg, float value);
int getIntRegister(std::string reg);
float getFloatRegister(std::string reg);
unsigned char getMemoryByte(int position);
void setMemoryByte(int position, unsigned char byte);
std::set<std::string> getInterestingIntRegisters();
void printMemory(int l, int r);
std::set<std::string> getInterestingFloatRegisters();