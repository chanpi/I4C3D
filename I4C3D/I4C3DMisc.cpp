#include "StdAfx.h"
#include "I4C3DMisc.h"

extern TCHAR *szTitle;
static HANDLE g_hMutex = NULL;

I4C3DMisc::I4C3DMisc(void)
{
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