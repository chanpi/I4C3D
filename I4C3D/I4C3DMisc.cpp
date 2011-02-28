#include "StdAfx.h"
#include "I4C3D.h"
#include "I4C3DMisc.h"

extern TCHAR szTitle[MAX_LOADSTRING];
static HANDLE g_hMutex = NULL;
static TCHAR g_szLogFileName[MAX_PATH] = {0};

I4C3DMisc::I4C3DMisc(void)
{
	GetModuleFileWithExtension(g_szLogFileName, sizeof(g_szLogFileName)/sizeof(g_szLogFileName[0]), _T("log"));
}


I4C3DMisc::~I4C3DMisc(void)
{
}

BOOL I4C3DMisc::ExecuteOnce(void) {
	g_hMutex = CreateMutex(NULL, TRUE, szTitle);
	if (g_hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
		return FALSE;
	}
	return TRUE;
}

void I4C3DMisc::Cleanup(void) {
	if (g_hMutex != NULL) {
		CloseHandle(g_hMutex);
		g_hMutex = NULL;
	}
}

void I4C3DMisc::GetModuleFileWithExtension(LPTSTR lpszFilePath, SIZE_T size, LPCTSTR lpszExtension) {
	TCHAR* ptr = NULL;
	if (!GetModuleFileName(NULL, lpszFilePath, size)) {
		ReportError(_T("[ERROR] ファイル名取得に失敗しました。"));
	}
	ptr = wcsrchr(lpszFilePath, _T('.'));
	if (ptr != NULL) {
		lstrcpy(ptr+1, lpszExtension);
	}
}

void I4C3DMisc::ReportError(LPCTSTR lpszMessage) {
	MessageBox(NULL, lpszMessage, szTitle, MB_OK | MB_ICONERROR);
}

void I4C3DMisc::LogDebugMessage(LPCTSTR lpszMessage)
{
	HANDLE hLogFile;
	unsigned char szBOM[] = { 0xFF, 0xFE };
	DWORD dwNumberOfBytesWritten = 0;

	if (g_szLogFileName[0] == _T('\0')) {
		GetModuleFileWithExtension(g_szLogFileName, sizeof(g_szLogFileName)/sizeof(g_szLogFileName[0]), _T("log"));
	}

	hLogFile = CreateFile(g_szLogFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hLogFile == INVALID_HANDLE_VALUE) {
		TCHAR szError[I4C3D_BUFFER_SIZE];
		_stprintf_s(szError, sizeof(szError)/sizeof(szError[0]), _T("[ERROR] ログファイルのオープンに失敗しています。: %d"), GetLastError());
		ReportError(szError);
		return;
	}

	WriteFile(hLogFile, szBOM, 2, &dwNumberOfBytesWritten, NULL);
	SetFilePointer(hLogFile, 0, NULL, FILE_END);
	WriteFile(hLogFile, lpszMessage, lstrlen(lpszMessage) * sizeof(TCHAR), &dwNumberOfBytesWritten, NULL);
	CloseHandle(hLogFile);
}