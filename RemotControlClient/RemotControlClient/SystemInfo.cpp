#include "SystemInfo.h"



SystemInfo::SystemInfo()
{
}


SystemInfo::~SystemInfo()
{
}

std::string SystemInfo::GetSystemVerson()
{

	SYSTEM_INFO info;        //��SYSTEM_INFO�ṹ�ж�64λAMD������ 
	GetSystemInfo(&info);    //����GetSystemInfo�������ṹ 
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	std::string osname = "unknown OperatingSystem.";

	if (GetVersionEx((OSVERSIONINFO *)&os))
	{
		//������ݰ汾��Ϣ�жϲ���ϵͳ���� 
		switch (os.dwMajorVersion)//�ж����汾��
		{
		case 4:
			switch (os.dwMinorVersion)//�жϴΰ汾�� 
			{
			case 0:
				if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
					osname = "Microsoft Windows NT 4.0"; //1996��7�·��� 
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
			switch (os.dwMinorVersion)	//�ٱȽ�dwMinorVersion��ֵ
			{
			case 0:
				osname = "Microsoft Windows 2000";//1999��12�·���
				break;

			case 1:
				osname = "Microsoft Windows XP";//2001��8�·���
				break;

			case 2:
				if (os.wProductType == VER_NT_WORKSTATION
					&& info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				{
					osname = "Microsoft Windows XP Professional x64 Edition";
				}
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					osname = "Microsoft Windows Server 2003";//2003��3�·��� 
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
					osname = "Microsoft Windows Server 2008";//�������汾 
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
					osname = "Microsoft Windows Server 2016 Technical Preview";//�������汾 
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
	GetUserNameAx yyCreateRemoteThread = (GetUserNameAx)GetProcAddress(LoadLibrary("advapi32.dll"), "GetUserNameA");		//��̬����GetUserNameA����	

	char strBuffer[256] = { 0 };
	DWORD dwSize = 256;
	yyCreateRemoteThread(strBuffer, &dwSize);
	return strBuffer;
}
