#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <tchar.h>
#include "Socket.h"
#include <windows.h>
#pragma comment(lib, "legacy_stdio_definitions.lib ")
#pragma comment(lib,"LIBCP.lib")
#pragma comment(lib,"libjpeg.lib")
extern "C" {
#include "jpeg/jpeglib.h"
#include "jpeg/jmorecfg.h"
#include "jpeg/jconfig.h"
}

/*
typedef JSAMPLE FAR *JSAMPROW;	
typedef JSAMPROW *JSAMPARRAY;	
typedef JSAMPARRAY *JSAMPIMAGE;
*/
class Screen
{
public:
	struct Info
	{
		char* host;
		int  port;
	};
	Screen(char*ip, int port);
	~Screen();

private:
	typedef struct {
		unsigned int len;    // jpg文件大小
	} ScreenSpyHeader;

	Socket socket;

	bool sendScreenData(Socket *sock, unsigned int len);
	unsigned int convertToJpgData(const std::vector<unsigned char> &bmpData);
	bool captureScreen(std::vector<unsigned char> &bmpData);
	static DWORD WINAPI readThread(LPVOID pM);
};

