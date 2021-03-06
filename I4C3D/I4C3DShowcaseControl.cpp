#include "StdAfx.h"
#include "I4C3DShowcaseControl.h"

extern TCHAR g_szIniFilePath[MAX_PATH];
static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);

I4C3DShowcaseControl::I4C3DShowcaseControl(void)
{
	m_hTargetParentWnd	= NULL;
	m_hTargetChildWnd	= NULL;
	m_basePos.x			= 0;
	m_basePos.y			= 0;
	m_currentPos.x		= 0;
	m_currentPos.y		= 0;
	m_ctrl = m_alt = m_shift = m_bSyskeyDown = FALSE;
}

I4C3DShowcaseControl::I4C3DShowcaseControl(I4C3DContext* pContext)
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

	TCHAR tempBuffer[I4C3D_BUFFER_SIZE] = {0};
	GetPrivateProfileString(_T("SHOWCASE"), _T("MODIFIER_KEY"), _T("Alt"), tempBuffer, sizeof(tempBuffer)/sizeof(tempBuffer[0]), g_szIniFilePath);

	LPCTSTR pType = NULL;
	LPCTSTR pTempBuffer = tempBuffer;
	do {
		TCHAR szKey[I4C3D_BUFFER_SIZE] = {0};
		pType = _tcschr(pTempBuffer, _T('+'));
		if (pType != NULL) {
			lstrcpyn(szKey, pTempBuffer, pType-pTempBuffer+1);
			pTempBuffer = pType+1;
		} else {
			lstrcpy(szKey, pTempBuffer);
		}
		I4C3DMisc::RemoveWhiteSpace(szKey);
		switch (szKey[0]) {
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
	} while (pType != NULL);

	CreateSettingMap(_T("SHOWCASE"));
}

I4C3DShowcaseControl::~I4C3DShowcaseControl(void)
{
	SendSystemKeys(m_hTargetChildWnd, FALSE);
}

void I4C3DShowcaseControl::TumbleExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetChildWnd, TRUE);
	I4C3DControl::TumbleExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetChildWnd, FALSE);
}

void I4C3DShowcaseControl::TrackExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetChildWnd, TRUE);
	I4C3DControl::TrackExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetChildWnd, FALSE);
}

void I4C3DShowcaseControl::DollyExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetChildWnd, TRUE);
	I4C3DControl::DollyExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetChildWnd, FALSE);
}

void I4C3DShowcaseControl::HotkeyExecute(LPCTSTR szCommand)
{
	I4C3DControl::HotkeyExecute(m_hTargetParentWnd, szCommand);
}

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szClassName[I4C3D_BUFFER_SIZE];
	GetClassName(hWnd, szClassName, sizeof(szClassName)/sizeof(szClassName[0]));
	if (!lstrcmpi(_T("AW_VIEWER"), szClassName)) {
		{
			TCHAR szError[I4C3D_BUFFER_SIZE];
			_stprintf_s(szError, sizeof(szError)/sizeof(szError[0]), _T("Child: %X"), hWnd);
			I4C3DMisc::LogDebugMessage(szError);
		}
		*(HWND*)lParam = hWnd;
		return FALSE;
	}
	return TRUE;
}
