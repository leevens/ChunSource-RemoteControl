#pragma once

#include<string>
using namespace std;

//#include <conf.h>
class CBase64Coder
{
private:
	static char ch64[];
	char* buf;
	int size;

private:
	static int BinSearch(char p);
	void allocMem(int NewSize);

public:
	CBase64Coder();
	~CBase64Coder();
	 char* encode(const string& message);
	 char* encode(const char* buffer, int buflen);
	 char* decode(const char* buffer, int Length);

};
