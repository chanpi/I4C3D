#include "StdAfx.h"
#include "I4C3D.h"
#include "I4C3DMisc.h"

extern TCHAR szTitle[MAX_LOADSTRING];
extern TCHAR g_szIniFilePath[MAX_PATH];

static HANDLE g_hMutex = NULL;
static TCHAR g_szLogFileName[MAX_PATH] = {0};
static BOOL g_bDebugOn = FALSE;

I4C3DMisc::I4C3DMisc(void)
{
	TCHAR szDebugOn[5];
	if (g_szIniFilePath[0] == _T('\0')) {
		GetModuleFileWithExtension(g_szIniFilePath, sizeof(g_szIniFilePath)/sizeof(g_szIniFilePath[0]), _T("ini"));
	}
	GetPrivateProfileString(_T("GLOBAL"), _T("DEBUG"), NULL, szDebugOn, sizeof(szDebugOn)/sizeof(szDebugOn[0]), g_szIniFilePath);

	if (szDebugOn != NULL && !lstrcmpi(szDebugOn, _T("ON"))) {
		g_bDebugOn = TRUE;
		GetModuleFileWithExtension(g_szLogFileName, sizeof(g_szLogFileName)/sizeof(g_szLogFileName[0]), _T("log"));
	}
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
	if (g_bDebugOn) {
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
			ReportError(lpszMessage);
			return;
		}

		WriteFile(hLogFile, szBOM, 2, &dwNumberOfBytesWritten, NULL);
		SetFilePointer(hLogFile, 0, NULL, FILE_END);
		WriteFile(hLogFile, lpszMessage, lstrlen(lpszMessage) * sizeof(TCHAR), &dwNumberOfBytesWritten, NULL);
		WriteFile(hLogFile, _T("\r\n"), 2 * sizeof(TCHAR), &dwNumberOfBytesWritten, NULL);
		CloseHandle(hLogFile);
	}
}

void I4C3DMisc::LogDebugMessageA(LPCSTR lpszMessage)
{
	static TCHAR szLogFileNameA[MAX_PATH];

	if (g_bDebugOn) {
		HANDLE hLogFile;
		DWORD dwNumberOfBytesWritten = 0;

		if (szLogFileNameA[0] == _T('\0')) {
			GetModuleFileWithExtension(szLogFileNameA, sizeof(szLogFileNameA)/sizeof(szLogFileNameA[0]), _T("txt"));
		}

		hLogFile = CreateFile(szLogFileNameA, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hLogFile == INVALID_HANDLE_VALUE) {
			TCHAR szError[I4C3D_BUFFER_SIZE];
			_stprintf_s(szError, sizeof(szError)/sizeof(szError[0]), _T("[ERROR] ログファイルのオープンに失敗しています。: %d"), GetLastError());
			ReportError(szError);
			return;
		}

		SetFilePointer(hLogFile, 0, NULL, FILE_END);
		WriteFile(hLogFile, lpszMessage, strlen(lpszMessage), &dwNumberOfBytesWritten, NULL);
		WriteFile(hLogFile, "\r\n", 2, &dwNumberOfBytesWritten, NULL);
		CloseHandle(hLogFile);
	}
}

void I4C3DMisc::RemoveWhiteSpace(LPTSTR szBuffer)
{
	TCHAR* pStart = szBuffer;
	TCHAR* pEnd = szBuffer + lstrlen(szBuffer) - 1;
	WORD wCharType = 0;

	while (pStart < pEnd && GetStringTypeEx(LOCALE_USER_DEFAULT, CT_CTYPE1, pStart, 1, &wCharType)) {
		if (wCharType & C1_SPACE) {
			pStart++;
		} else {
			break;
		}
	}

	while (pStart < pEnd && GetStringTypeEx(LOCALE_USER_DEFAULT, CT_CTYPE1, pEnd, 1, &wCharType)) {
		if (wCharType & C1_SPACE) {
			pEnd--;
		} else {
			break;
		}
	}

	MoveMemory(szBuffer, pStart, pEnd - pStart + 1);
	*(szBuffer + (pEnd - pStart + 1)) = _T('\0');
}