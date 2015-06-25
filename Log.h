#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

class logger
{
public:
	logger();
	~logger();
	void operator<<(const string &);
	void operator<<(const int &);
private:
};
