#include "StdAfx.h"
#include "I4C3DAliasControl.h"

extern TCHAR g_szFilePath[MAX_PATH];
static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);

I4C3DAliasControl::I4C3DAliasControl(void)
{
	m_hTargetParentWnd = NULL;
	m_hTargetChildWnd = NULL;
	m_posX = 0;
	m_posY = 0;
	m_ctrl = m_alt = m_shift = FALSE;
}

I4C3DAliasControl::I4C3DAliasControl(I4C3DContext* pContext)
{
	m_hTargetParentWnd = pContext->hTargetParentWnd;
	if (m_hTargetParentWnd == NULL) {
		I4C3DMisc::ReportError(_T("[ERROR] ターゲットウィンドウが取得できません。"));
		DestroyWindow(pContext->hMyWnd);
	}
	m_hTargetChildWnd = NULL;
	m_posX = 0;
	m_posY = 0;

	EnumChildWindows(m_hTargetParentWnd, EnumChildProc, (LPARAM)&m_hTargetChildWnd);
	if (m_hTargetChildWnd == NULL) {
		I4C3DMisc::ReportError(_T("[ERROR] 子ウィンドウが取得できません。"));
		DestroyWindow(pContext->hMyWnd);
	}

	m_ctrl = m_alt = m_shift = FALSE;

	TCHAR tempBuffer[5] = {0};
	GetPrivateProfileString(_T("Alias"), _T("MODIFIER_KEY"), _T("AS"), tempBuffer, sizeof(tempBuffer)/sizeof(tempBuffer[0]), g_szFilePath);
	int count = lstrlen(tempBuffer);
	for (int i = 0; i < count; i++) {
		switch (tempBuffer[i]) {
		case _T('C'):
		case _T('c'):
			m_ctrl = TRUE;
			break;

		case _T('S'):
		case _T('s'):
			m_shift = TRUE;
			break;

		case _T('A'):
		case _T('a'):
			m_alt = TRUE;
			break;
		}
	}
}

I4C3DAliasControl::~I4C3DAliasControl(void)
{
}


void I4C3DAliasControl::TumbleExecute(int deltaX, int deltaY)
{
	I4C3DControl::TumbleExecute(deltaX, deltaY);
}

void I4C3DAliasControl::TrackExecute(int deltaX, int deltaY)
{
	I4C3DControl::TrackExecute(deltaX, deltaY);
}

void I4C3DAliasControl::DollyExecute(int deltaX, int deltaY)
{
	I4C3DControl::DollyExecute(deltaX, deltaY);
}

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szWindowTitle[I4C3D_BUFFER_SIZE];
	GetWindowText(hWnd, szWindowTitle, sizeof(szWindowTitle)/sizeof(szWindowTitle[0]));
	//lstrcat(szBuffer, szWindowTitle);
	//lstrcat(szBuffer, _T("\n"));
	if (!lstrcmpi(_T("Alias.glw"), szWindowTitle)) {
		*(HWND*)lParam = hWnd;
		return FALSE;
	}
	return TRUE;
}
