#include "StdAfx.h"
#include "I4C3DCore.h"
#include "I4C3DMisc.h"


I4C3DCore::I4C3DCore(void)
{
	m_started = FALSE;
}


I4C3DCore::~I4C3DCore(void)
{
}

BOOL I4C3DCore::Start(HWND* phWnd) {
	if (m_started) {
		return TRUE;
	}
	if ((*phWnd = GetTarget3DSoftwareWnd()) == NULL) {
		return FALSE;
	}

	// iPhone待ち受けスタート

	m_started = TRUE;
	return TRUE;
}

void I4C3DCore::Stop(void) {
	if (!m_started) {
		return;
	}

	m_started = FALSE;
}


HWND I4C3DCore::GetTarget3DSoftwareWnd(void) {
	HWND hTargetWnd = NULL;
	TCHAR szTarget[MAX_PATH] = {0};
	if (GetTarget3DSoftwareName(szTarget, MAX_PATH)) {
		hTargetWnd = FindWindow(NULL, szTarget);
	}
	return hTargetWnd;
}

//////////////////////////////////////////////////////////////////////////////

BOOL I4C3DCore::GetTarget3DSoftwareName(LPTSTR lpszTargetName, SIZE_T size) {
	TCHAR szFilePath[MAX_PATH] = {0};
	I4C3DMisc::GetModuleFileWithExtension(szFilePath, sizeof(szFilePath)/sizeof(szFilePath[0]), _T("ini"));
	GetPrivateProfileString(_T("GLOBAL"), _T("TARGET_SOFT"), NULL, lpszTargetName, size, szFilePath);
	
	OutputDebugString(lpszTargetName);
	OutputDebugString(_T("\n"));

	return lstrlen(lpszTargetName);
}

