#pragma once
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>

#pragma comment(lib,"Ws2_32.lib ")
using namespace std;
class Socket
{
public:
	Socket();
	~Socket();
	char *Myip;
	int MyPort;
	SOCKET clientSocket;
	bool isConnected = false;

	int connectToHost(char* ip,int port);	//连接服务端函数
	int write(const char *data);
	int write(const char * data, int size);

	std::string read(const char* buf);
	int read(char * buffer, int size);
	wchar_t * char2wchar(const char * cchar);	//ascill to unicode
	char * wchar2char(const wchar_t * wchar);	//unicode to ascill
//	DWORD WINAPI recvThread(void* param);	//接受数据线程
private:
	WSAData wsa;
	
	bool isInit = false;		//判断是否初始化成功
	
};

