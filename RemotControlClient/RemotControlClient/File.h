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

struct listFileName		//�ļ��������ڴ���Ľṹ�岢��#�ָ�
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
	bool	CurrentStatus = false;	//�ж��Ƿ��ļ�����״̬
	string DataFileName;	//��ʱ�������ڼ�¼�ļ���
	int	   DataFileSize;	//ͬ�ϣ����ڼ�¼�ļ���С

	//����
	char* getAllDriver();	//����ϵͳ�̷�
	int	   getFileSize(string name);
	char* listFiles(const char* dir);		//�г�Ŀ¼�������ļ�����Ϣ
	char *join1(char *a, char *b);		//ƴ���ַ���
	char *result(char* data);


};

