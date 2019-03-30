#pragma once
#include <Windows.h>
#include <iostream>
class SystemInfo
{
public:
	SystemInfo();
	~SystemInfo();
	static std::string GetSystemVerson();
	static std::string GetSystemUserName();

};

