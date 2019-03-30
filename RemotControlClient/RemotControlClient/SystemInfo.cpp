#include "SystemInfo.h"



SystemInfo::SystemInfo()
{
}


SystemInfo::~SystemInfo()
{
}

std::string SystemInfo::GetSystemVerson()
{

	SYSTEM_INFO info;        //用SYSTEM_INFO结构判断64位AMD处理器 
	GetSystemInfo(&info);    //调用GetSystemInfo函数填充结构 
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	std::string osname = "unknown OperatingSystem.";

	if (GetVersionEx((OSVERSIONINFO *)&os))
	{
		//下面根据版本信息判断操作系统名称 
		switch (os.dwMajorVersion)//判断主版本号
		{
		case 4:
			switch (os.dwMinorVersion)//判断次版本号 
			{
			case 0:
				if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
					osname = "Microsoft Windows NT 4.0"; //1996年7月发布 
				else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
					osname = "Microsoft Windows 95";
				break;
			case 10:
				osname = "Microsoft Windows 98";
				break;
			case 90:
				osname = "Microsoft Windows Me";
				break;
			}
			break;

		case 5:
			switch (os.dwMinorVersion)	//再比较dwMinorVersion的值
			{
			case 0:
				osname = "Microsoft Windows 2000";//1999年12月发布
				break;

			case 1:
				osname = "Microsoft Windows XP";//2001年8月发布
				break;

			case 2:
				if (os.wProductType == VER_NT_WORKSTATION
					&& info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				{
					osname = "Microsoft Windows XP Professional x64 Edition";
				}
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					osname = "Microsoft Windows Server 2003";//2003年3月发布 
				else if (GetSystemMetrics(SM_SERVERR2) != 0)
					osname = "Microsoft Windows Server 2003 R2";
				break;
			}
			break;

		case 6:
			switch (os.dwMinorVersion)
			{
			case 0:
				if (os.wProductType == VER_NT_WORKSTATION)
					osname = "Microsoft Windows Vista";
				else
					osname = "Microsoft Windows Server 2008";//服务器版本 
				break;
			case 1:
				if (os.wProductType == VER_NT_WORKSTATION)
					osname = "Microsoft Windows 7";
				else
					osname = "Microsoft Windows Server 2008 R2";
				break;
			case 2:
				if (os.wProductType == VER_NT_WORKSTATION)
					osname = "Microsoft Windows 8";
				else
					osname = "Microsoft Windows Server 2012";
				break;
			case 3:
				if (os.wProductType == VER_NT_WORKSTATION)
					osname = "Microsoft Windows 8.1";
				else
					osname = "Microsoft Windows Server 2012 R2";
				break;
			}
			break;

		case 10:
			switch (os.dwMinorVersion)
			{
			case 0:
				if (os.wProductType == VER_NT_WORKSTATION)
					osname = "Microsoft Windows 10";
				else
					osname = "Microsoft Windows Server 2016 Technical Preview";//服务器版本 
				break;
			}
			break;
		}
	}
	return osname;
}

std::string SystemInfo::GetSystemUserName()
{
	/*
	BOOL
		WINAPI
		GetUserNameA(
			_Out_writes_to_opt_(*pcbBuffer, *pcbBuffer) LPSTR lpBuffer,
			_Inout_ LPDWORD pcbBuffer
		);
	*/

	typedef HANDLE(WINAPI *GetUserNameAx)
		(
			_Out_writes_to_opt_(*pcbBuffer, *pcbBuffer) LPSTR lpBuffer,
			_Inout_ LPDWORD pcbBuffer
			);
	GetUserNameAx yyCreateRemoteThread = (GetUserNameAx)GetProcAddress(LoadLibrary("advapi32.dll"), "GetUserNameA");		//动态调用GetUserNameA函数	

	char strBuffer[256] = { 0 };
	DWORD dwSize = 256;
	yyCreateRemoteThread(strBuffer, &dwSize);
	return strBuffer;
}
