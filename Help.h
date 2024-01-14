#pragma once
#include <string>
#include <vector>
std::vector<bool> binarystringtovector(std::string s);
void append(std::vector<bool>& bucket, std::vector<bool> nw);
int stringtoint(std::string number);
std::vector<bool> inttobinary(int number);
std::string myPopBack(std::string s);
std::vector<std::string> splitString(std::string input, std::string badChars);
bool isPrefix(std::string input, std::string prefix);
std::string floatToString(float value);
float stringToFloat(std::string str);