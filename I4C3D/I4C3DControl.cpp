#include "StdAfx.h"
#include "I4C3DControl.h"

extern TCHAR szTitle[MAX_LOADSTRING];
extern TCHAR g_szIniFilePath[MAX_PATH];
static void VKPush(HWND hTargetWnd, LPCTSTR szKeyTypes);

I4C3DControl::I4C3DControl(void)
{
	m_hTargetParentWnd	= NULL;
	m_hTargetChildWnd	= NULL;
	m_basePos.x			= 0;
	m_basePos.y			= 0;
	m_currentPos.x		= 0;
	m_currentPos.y		= 0;
	m_ctrl = m_alt = m_shift = FALSE;
}


I4C3DControl::I4C3DControl(I4C3DContext* pContext)
{
	m_hTargetParentWnd	= pContext->hTargetParentWnd;
	m_hTargetChildWnd	= NULL;			// 子ウィンドウが必要であれば派生クラスで取得してください。
	m_basePos.x			= 0;
	m_basePos.y			= 0;
	m_currentPos.x		= 0;
	m_currentPos.y		= 0;
	m_ctrl = m_alt = m_shift = FALSE;	// 各システムキーは派生クラスで取得してください。
}

I4C3DControl::~I4C3DControl(void)
{
	std::map<LPCTSTR, LPCTSTR>::iterator it = m_settingsMap.begin();
	while (it != m_settingsMap.end()) {
		free((void*)it->first);
		free((void*)it->second);
		it++;
	}

	m_settingsMap.clear();
}

void I4C3DControl::TumbleExecute(int deltaX, int deltaY)
{
	VMMouseMessage mouseMessage = {0};

	if (!CheckTargetState()) {
		return;
	}

	mouseMessage.hTargetWnd		= m_hTargetChildWnd;
	mouseMessage.dragButton		= LButtonDrag;
	mouseMessage.dragStartPos	= m_currentPos;
	m_currentPos.x				+= deltaX;
	m_currentPos.y				+= deltaY;
	mouseMessage.dragEndPos		= m_currentPos;
	if (m_ctrl) {
		mouseMessage.uKeyState = MK_CONTROL;
	}
	if (m_shift) {
		mouseMessage.uKeyState |= MK_SHIFT;
	}
	VMMouseDrag(&mouseMessage);
}

void I4C3DControl::TrackExecute(int deltaX, int deltaY)
{
	VMMouseMessage mouseMessage = {0};

	if (!CheckTargetState()) {
		return;
	}

	mouseMessage.hTargetWnd		= m_hTargetChildWnd;
	mouseMessage.dragButton		= MButtonDrag;
	mouseMessage.dragStartPos	= m_currentPos;
	m_currentPos.x				+= deltaX;
	m_currentPos.y				+= deltaY;
	mouseMessage.dragEndPos		= m_currentPos;
	if (m_ctrl) {
		mouseMessage.uKeyState = MK_CONTROL;
	}
	if (m_shift) {
		mouseMessage.uKeyState |= MK_SHIFT;
	}
	VMMouseDrag(&mouseMessage);
}

void I4C3DControl::DollyExecute(int deltaX, int deltaY)
{
	VMMouseMessage mouseMessage = {0};

	if (!CheckTargetState()) {
		return;
	}

	mouseMessage.hTargetWnd		= m_hTargetChildWnd;
	mouseMessage.dragButton		= RButtonDrag;
	mouseMessage.dragStartPos	= m_currentPos;
	m_currentPos.x				+= deltaX;
	m_currentPos.y				+= deltaY;
	mouseMessage.dragEndPos		= m_currentPos;
	if (m_ctrl) {
		mouseMessage.uKeyState = MK_CONTROL;
	}
	if (m_shift) {
		mouseMessage.uKeyState |= MK_SHIFT;
	}
	VMMouseDrag(&mouseMessage);
}

void I4C3DControl::SendSystemKeys(HWND hTargetWnd, BOOL bDown)
{
	if (bDown && !m_bSyskeyDown) {
		if (m_ctrl) {
			I4C3DMisc::LogDebugMessage(_T("[Ctrl] down"));
			VMVirtualKeyDown(hTargetWnd, VK_CONTROL);
		}
		if (m_alt) {
			I4C3DMisc::LogDebugMessage(_T("[Alt] down"));
			VMVirtualKeyDown(hTargetWnd, VK_MENU);
		}
		if (m_shift) {
			I4C3DMisc::LogDebugMessage(_T("[Shift] down"));
			VMVirtualKeyDown(hTargetWnd, VK_SHIFT);
		}
		m_bSyskeyDown = TRUE;

	} else if (!bDown && m_bSyskeyDown) {
		if (m_shift) {
			I4C3DMisc::LogDebugMessage(_T("[Shift] up"));
			VMVirtualKeyUp(hTargetWnd, VK_SHIFT);
		}
		if (m_alt) {
			I4C3DMisc::LogDebugMessage(_T("[Alt] up"));
			VMVirtualKeyUp(hTargetWnd, VK_MENU);
		}
		if (m_ctrl) {
			I4C3DMisc::LogDebugMessage(_T("[Ctrl] up"));
			VMVirtualKeyUp(hTargetWnd, VK_CONTROL);
		}
		m_bSyskeyDown = FALSE;

	}
}

void I4C3DControl::HotkeyExecute(LPCTSTR szCommand)
{
	HotkeyExecute(m_hTargetParentWnd, szCommand);
}

void I4C3DControl::HotkeyExecute(HWND hTargetWnd, LPCTSTR szCommand)
{
	std::map<LPCTSTR, LPCTSTR>::iterator it = m_settingsMap.begin();
	while (it != m_settingsMap.end()) {
		if (lstrcmpi(it->first, szCommand) == 0) {
			SetWindowPos(m_hTargetParentWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetWindowPos(m_hTargetParentWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

			VKPush(hTargetWnd, it->second);
			break;
		}
		it++;
	}
}

/////////////////////////////////////////////////////////////////////////

BOOL I4C3DControl::CheckTargetState(void)
{
	RECT rect;

	if (m_hTargetParentWnd == NULL) {
		I4C3DMisc::ReportError(_T("ターゲットウィンドウを取得してください。"));

	} else if (m_hTargetChildWnd == NULL) {
		I4C3DMisc::ReportError(_T("子ウィンドウを取得してください。"));

	} else {
		// ターゲットウィンドウの位置のチェック
		GetWindowRect(m_hTargetParentWnd, &rect);

		{
			TCHAR szError[I4C3D_BUFFER_SIZE];
			_stprintf_s(szError, sizeof(szError)/sizeof(szError[0]), _T("top:%d bottom:%d left:%d right:%d Base(%d, %d) Current(%d, %d)"),
				rect.top, rect.bottom, rect.left, rect.right, m_basePos.x, m_basePos.y, m_currentPos.x, m_currentPos.y);
			I4C3DMisc::LogDebugMessage(szError);
		}
		m_basePos.x = m_currentPos.x = rect.left + (rect.right - rect.left) / 2;
		m_basePos.y = m_currentPos.y = rect.top + (rect.bottom - rect.top) / 2;

		//// currentPosのはみ出しチェック
		//if (m_currentPos.x < rect.left || rect.right < m_currentPos.x
		//	|| m_currentPos.y < rect.top || rect.bottom < m_currentPos.y) {
		//		m_currentPos.x = m_basePos.x;
		//		m_currentPos.y = m_basePos.y;
		//}
		return TRUE;
	}

	return FALSE;
}

void I4C3DControl::CreateSettingMap(LPCTSTR szSectionName) {
	TCHAR* szReturnedString = (TCHAR*)calloc(I4C3D_BUFFER_SIZE, sizeof(TCHAR));
	if (szReturnedString == NULL) {
		I4C3DMisc::ReportError(_T("[ERROR] メモリが確保できません。終了します。"));
		exit(1);
	}

	TCHAR szKey[I4C3D_BUFFER_SIZE];
	TCHAR szValue[I4C3D_BUFFER_SIZE];

	TCHAR *pSrc, *pKey, *pValue;
	DWORD ret;
	DWORD nBufferSize = I4C3D_BUFFER_SIZE;

	if (g_szIniFilePath[0] == _T('\0')) {
		I4C3DMisc::GetModuleFileWithExtension(g_szIniFilePath, sizeof(g_szIniFilePath)/sizeof(g_szIniFilePath[0]), _T("ini"));
	}

	for (;;) {
		ret = GetPrivateProfileSection(szSectionName, szReturnedString, nBufferSize, g_szIniFilePath);

		if (ret == nBufferSize-2) {	// メモリが足りない
			szReturnedString = (TCHAR*)realloc(szReturnedString, nBufferSize*2);
			if (szReturnedString == NULL) {
				I4C3DMisc::ReportError(_T("[ERROR] メモリが確保できません。終了します。"));
				exit(1);
			}

		} else {
			break;
		}
		nBufferSize *= 2;
	}

	pSrc = szReturnedString;
	while (*pSrc != _T('\0')) {

		pKey = szKey;
		pValue = szValue;
		while (*pSrc != _T('=')) {
			*pKey++ = *pSrc++;
		}
		*pKey = _T('\0');
		pSrc++;	// =を読み飛ばし

		while (*pSrc != _T('\0')) {
			*pValue++ = *pSrc++;
		}
		*pValue = _T('\0');
		pSrc++;	// '\0'読み飛ばし

		I4C3DMisc::RemoveWhiteSpace(szKey);
		I4C3DMisc::RemoveWhiteSpace(szValue);
		if (_tcsncicmp(szKey, _T("HOTKEY_"), 7) == 0) {
			TCHAR *pszKey	= (TCHAR*)calloc(lstrlen(szKey)-7, sizeof(TCHAR));
			TCHAR *pszValue	= (TCHAR*)calloc(lstrlen(szValue), sizeof(TCHAR));
			if (pszKey == NULL || pszValue == NULL) {
				I4C3DMisc::ReportError(_T("[ERROR] メモリが確保できません。終了します。"));
				exit(1);
			}
			lstrcpy(pszKey, szKey+7);
			lstrcpy(pszValue, szValue);
			m_settingsMap.insert(std::map<LPCTSTR, LPCTSTR>::value_type(pszKey, pszValue));
		}
	}

	free(szReturnedString);
}

void VKPush(HWND hTargetWnd, LPCTSTR szKeyTypes) {
	LPCTSTR pType = _tcschr(szKeyTypes, _T('+'));
	TCHAR szKey[I4C3D_BUFFER_SIZE] = {0};
	if (pType != NULL) {
		lstrcpyn(szKey, szKeyTypes, pType-szKeyTypes+1);
	} else {
		lstrcpy(szKey, szKeyTypes);
	}
	I4C3DMisc::RemoveWhiteSpace(szKey);
	UINT vKey = VMGetVirtualKey(szKey);

	switch (vKey) {
	case 0:
		{
			TCHAR szError[I4C3D_BUFFER_SIZE];
			_stprintf_s(szError, sizeof(szError)/sizeof(szError[0]), _T("Ctrlが%s [%s]"), GetKeyState(VK_CONTROL) < 0 ? _T("DOWN"):_T("UP"), szKey);
			I4C3DMisc::LogDebugMessage(szError);
		}
		VMKeyDown(hTargetWnd, szKey[0]);
		//VMVirtualKeyDown(hTargetWnd, szKey[0]);
		break;

	case VK_CONTROL:
	case VK_MENU:
	case VK_SHIFT:
		VMVirtualKeyDown(hTargetWnd, vKey);
		break;

	default:
		//VMVirtualKeyDown(hTargetWnd, vKey);
		VMKeyDown(hTargetWnd, vKey);
	}

	// 再帰的に次のキー入力
	if (pType != NULL) {
		VKPush(hTargetWnd, pType+1);
	}

	switch (vKey) {
	case 0:
		VMKeyUp(hTargetWnd, szKey[0]);
		//VMVirtualKeyUp(hTargetWnd, szKey[0]);
		break;

	case VK_CONTROL:
	case VK_MENU:
	case VK_SHIFT:
		VMVirtualKeyUp(hTargetWnd, vKey);
		break;

	default:
		//VMVirtualKeyUp(hTargetWnd, vKey);
		VMKeyUp(hTargetWnd, vKey);
	}
}

