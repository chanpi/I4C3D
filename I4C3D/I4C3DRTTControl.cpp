#include "StdAfx.h"
#include "I4C3DRTTControl.h"

extern TCHAR g_szIniFilePath[MAX_PATH];
static BOOL CALLBACK EnumChildProcForKeyInput(HWND hWnd, LPARAM lParam);
static BOOL CALLBACK EnumChildProcForMouseInput(HWND hWnd, LPARAM lParam);

I4C3DRTTControl::I4C3DRTTControl(void)
{
	m_hTargetParentWnd	= NULL;
	m_hTargetChildWnd	= NULL;
	m_basePos.x			= 0;
	m_basePos.y			= 0;
	m_currentPos.x		= 0;
	m_currentPos.y		= 0;
	m_ctrl = m_alt = m_shift = m_bSyskeyDown = FALSE;

	m_hKeyInputWnd		= NULL;
}


I4C3DRTTControl::I4C3DRTTControl(I4C3DContext* pContext)
{
	m_hTargetParentWnd	= pContext->hTargetParentWnd;
	m_hTargetChildWnd	= NULL;
	m_basePos.x			= 0;
	m_basePos.y			= 0;
	m_currentPos.x		= 0;
	m_currentPos.y		= 0;
	m_hKeyInputWnd		= NULL;

	// キー入力ウィンドウを取得
	EnumChildWindows(m_hTargetParentWnd, EnumChildProcForKeyInput, (LPARAM)&m_hKeyInputWnd);
	if (m_hKeyInputWnd == NULL) {
		I4C3DMisc::ReportError(_T("[ERROR] キー入力ウィンドウが取得できません。\n設定ファイルを確認してください。"));
		DestroyWindow(pContext->hMyWnd);
		return;
	}

	// マウス入力ウィンドウを取得
	EnumChildWindows(m_hKeyInputWnd, EnumChildProcForMouseInput, (LPARAM)&m_hTargetChildWnd);
	if (m_hTargetChildWnd == NULL) {
		I4C3DMisc::ReportError(_T("[ERROR] マウス入力ウィンドウが取得できません。\n設定ファイルを確認してください。"));
		DestroyWindow(pContext->hMyWnd);
		return;
	}

	m_ctrl = m_alt = m_shift = m_bSyskeyDown = FALSE;

	TCHAR tempBuffer[I4C3D_BUFFER_SIZE] = {0};
	GetPrivateProfileString(_T("RTT"), _T("MODIFIER_KEY"), _T("Ctrl"), tempBuffer, sizeof(tempBuffer)/sizeof(tempBuffer[0]), g_szIniFilePath);

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

	CreateSettingMap(_T("RTT"));
}


I4C3DRTTControl::~I4C3DRTTControl(void)
{
	SendSystemKeys(m_hKeyInputWnd, FALSE);
}

// ctrl + マウス左ドラッグ
void I4C3DRTTControl::TumbleExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hKeyInputWnd, TRUE);
	I4C3DControl::TumbleExecute(deltaX, deltaY);
	SendSystemKeys(m_hKeyInputWnd, FALSE);
}

// ctrl + マウス中ドラッグ
void I4C3DRTTControl::TrackExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hKeyInputWnd, TRUE);
	I4C3DControl::TrackExecute(deltaX, deltaY);
	SendSystemKeys(m_hKeyInputWnd, FALSE);
}

// ctrl + マウス右ドラッグ
void I4C3DRTTControl::DollyExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hKeyInputWnd, TRUE);
	I4C3DControl::DollyExecute(deltaX, deltaY);
	SendSystemKeys(m_hKeyInputWnd, FALSE);
}

void I4C3DRTTControl::HotkeyExecute(LPCTSTR szCommand)
{
	I4C3DControl::HotkeyExecute(m_hKeyInputWnd, szCommand);
}

BOOL CALLBACK EnumChildProcForKeyInput(HWND hWnd, LPARAM lParam)
{
	TCHAR szWindowTitle[I4C3D_BUFFER_SIZE];
	GetWindowText(hWnd, szWindowTitle, sizeof(szWindowTitle)/sizeof(szWindowTitle[0]));

	//{
	//	TCHAR szError[I4C3D_BUFFER_SIZE];
	//	_stprintf_s(szError, sizeof(szError)/sizeof(szError[0]), _T("%s [%X]"), szWindowTitle, hWnd);
	//	I4C3DMisc::LogDebugMessage(szError);
	//}

	if (!lstrcmpi(_T("untitled"), szWindowTitle) /*&& !lstrcmpi(_T("QWidget"), szClassTitle)*/) {
		*(HWND*)lParam = hWnd;
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK EnumChildProcForMouseInput(HWND hWnd, LPARAM lParam)
{
	*(HWND*)lParam = hWnd;
	return TRUE;
}
