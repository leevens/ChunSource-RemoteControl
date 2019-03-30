#pragma once
#include "Socket.h"
#include <fstream>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
//using namespace std;
/************************
SEND_FILE
FILE_PATH
FILE_SIZE
*************************/

/************************
SEND_FILELIST
FILENAME
.....
*************************/

struct listFileName		//文件遍历用于传输的结构体并用#分割
{
	/*
	string fileName;
	string fileSize;
	string fileProperty;
	*/
	char fileName[4096] = { 0 };
	char fileSize[4096] = { 0 };
	char fileProperty[4096] = { 0 };
};

class FileConTrol
{
public:
	FileConTrol(char*ip,int port);
	void run();
	~FileConTrol();

private:
	Socket socket;
	bool	CurrentStatus = false;	//判断是否文件传输状态
	string DataFileName;	//临时变量用于记录文件名
	int	   DataFileSize;	//同上，用于记录文件大小

	//函数
	char* getAllDriver();	//遍历系统盘符
	int	   getFileSize(string name);
	char* listFiles(const char* dir);		//列出目录的所有文件和信息
	char *join1(char *a, char *b);		//拼接字符串
	char *result(char* data);


};

