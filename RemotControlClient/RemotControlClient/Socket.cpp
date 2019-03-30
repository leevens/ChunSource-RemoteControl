#include "Socket.h"

Socket::Socket()
{
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "WSAStartup error" << endl;
	//	return 0;
	}
	this->isInit = true;
	this->isConnected = true;
	if (2 != LOBYTE(wsa.wVersion) || 2 != HIBYTE(wsa.wVersion))
	{
		WSACleanup();
		cout << "WSADATA version is not correct!" << endl;
	}
}


Socket::~Socket()
{
	int number = WSACleanup();
	cout << "socket exit number:" << number << "\n";
}

int Socket::connectToHost(char * ip, int port)
{
	 clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "clientSocket = socket(AF_INET, SOCK_STREAM, 0) execute failed!" << endl;
		return -1;
	}

	//初始化服务器端地址族变量
	SOCKADDR_IN srvAddr;
	srvAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(port);

	//连接服务器
	int iRet = connect(clientSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR));
	if (0 != iRet)
	{
		cout << "connect(clientSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR)) execute failed!" << endl;
		return -1;
	}
	this->Myip = ip;
	this->MyPort = port;
	return 1;
}

int Socket::write(const char * data)
{
	cout << "buf:" << data << "\n";
	int len = send(clientSocket, data, strlen(data) + 1, 0);
	
	return len;
}

int Socket::write(const char * data,int size)
{
	cout << "buf:" << data << "\n";
	int len = send(clientSocket, data, size, 0);

	return len;
}
std::string Socket::read(const char * buf)
{
//	CBase64Coder base64;

	int ret;
	char buff[1024] = { 0 };
		ret= recv(clientSocket, buff, 1024, 0);

		if ((ret == SOCKET_ERROR || ret == 0)) 
		{
			cout << "\nerror:" << ret;
			this->isConnected = false;
			return "";
		}

			std::cout << "buf:" << buff << "\n";
	string re = strtok(buff,  "&");
	buf = buff;
	return re;
}

int Socket::read(char* buffer,int size)
{
	int ret;
	ret = recv(clientSocket, buffer, size, 0);

	if ((ret == SOCKET_ERROR || ret == 0))
	{
		cout << "\nerror:" << ret;
		this->isConnected = false;
		return -1;
	}

	return ret;
}

wchar_t * Socket::char2wchar(const char* cchar)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

char * Socket::wchar2char(const wchar_t* wchar)
{
	char * m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}
