#include "StdAfx.h"
#include "I4C3DAliasControl.h"

extern TCHAR g_szIniFilePath[MAX_PATH];
static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);

I4C3DAliasControl::I4C3DAliasControl(void)
{
	m_hTargetParentWnd	= NULL;
	m_hTargetChildWnd	= NULL;
	m_basePos.x			= 0;
	m_basePos.y			= 0;
	m_currentPos.x		= 0;
	m_currentPos.y		= 0;
	m_ctrl = m_alt = m_shift = m_bSyskeyDown = FALSE;
}

I4C3DAliasControl::I4C3DAliasControl(I4C3DContext* pContext)
{
	m_hTargetParentWnd = pContext->hTargetParentWnd;
	if (m_hTargetParentWnd == NULL) {
		I4C3DMisc::ReportError(_T("[ERROR] ターゲットウィンドウが取得できません。"));
		DestroyWindow(pContext->hMyWnd);
	}
	m_hTargetChildWnd	= NULL;
	m_basePos.x			= 0;
	m_basePos.y			= 0;
	m_currentPos.x		= 0;
	m_currentPos.y		= 0;

	EnumChildWindows(m_hTargetParentWnd, EnumChildProc, (LPARAM)&m_hTargetChildWnd);
	if (m_hTargetChildWnd == NULL) {
		I4C3DMisc::ReportError(_T("[ERROR] 子ウィンドウが取得できません。\n設定ファイルを確認してください。"));
		DestroyWindow(pContext->hMyWnd);
	}

	m_ctrl = m_alt = m_shift = m_bSyskeyDown = FALSE;

	TCHAR tempBuffer[5] = {0};
	GetPrivateProfileString(_T("Alias"), _T("MODIFIER_KEY"), _T("AS"), tempBuffer, sizeof(tempBuffer)/sizeof(tempBuffer[0]), g_szIniFilePath);
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

	CreateSettingMap(_T("Alias"));
	//std::map<LPCTSTR, LPCTSTR>::iterator it = m_settingsMap.begin();
	//while (it != m_settingsMap.end()) {
	//	I4C3DMisc::LogDebugMessage(it->first);
	//	I4C3DMisc::LogDebugMessage(it->second);
	//	it++;
	//}
}

I4C3DAliasControl::~I4C3DAliasControl(void)
{
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}


void I4C3DAliasControl::TumbleExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetParentWnd, TRUE);
	I4C3DControl::TumbleExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}

void I4C3DAliasControl::TrackExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetParentWnd, TRUE);
	I4C3DControl::TrackExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}

void I4C3DAliasControl::DollyExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetParentWnd, TRUE);
	I4C3DControl::DollyExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szWindowTitle[I4C3D_BUFFER_SIZE];
	GetWindowText(hWnd, szWindowTitle, sizeof(szWindowTitle)/sizeof(szWindowTitle[0]));
	if (!lstrcmpi(_T("Alias.glw"), szWindowTitle)) {
		*(HWND*)lParam = hWnd;
		return FALSE;
	}
	return TRUE;
}
