#pragma once
#include <vector>
#include <string>
void loadBijections();
std::string operationToBits(std::string operation);
std::string bitsToOperation(std::string bits);
std::string registerToBits(std::string reg);
std::string bitsToRegister(std::string bits);
bool doThisBitsRepresentAnOperation(std::string bits);
bool doThisBitsRepresentARegister(std::string bits);
std::vector<std::string> getIntRegisters();
std::vector<std::string> getFloatRegisters();