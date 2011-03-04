#include "StdAfx.h"
#include "I4C3DControl.h"

extern TCHAR szTitle[MAX_LOADSTRING];
extern TCHAR g_szIniFilePath[MAX_PATH];
static void CreateSettingMap();

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
}

void I4C3DControl::TumbleExecute(int deltaX, int deltaY)
{
	VMMouseMessage mouseMessage;

	if (!CheckTargetState()) {
		return;
	}

	mouseMessage.hTargetWnd		= m_hTargetChildWnd;
	mouseMessage.dragButton		= LButtonDrag;
	mouseMessage.dragStartPos	= m_currentPos;
	m_currentPos.x				+= deltaX;
	m_currentPos.y				+= deltaY;
	mouseMessage.dragEndPos		= m_currentPos;
	mouseMessage.uKeyState		= MK_SHIFT;
	VMMouseDrag(&mouseMessage);
}

void I4C3DControl::TrackExecute(int deltaX, int deltaY)
{
	VMMouseMessage mouseMessage;

	if (!CheckTargetState()) {
		return;
	}

	mouseMessage.hTargetWnd		= m_hTargetChildWnd;
	mouseMessage.dragButton		= MButtonDrag;
	mouseMessage.dragStartPos	= m_currentPos;
	m_currentPos.x				+= deltaX;
	m_currentPos.y				+= deltaY;
	mouseMessage.dragEndPos		= m_currentPos;
	mouseMessage.uKeyState		= MK_SHIFT;
	VMMouseDrag(&mouseMessage);
}

void I4C3DControl::DollyExecute(int deltaX, int deltaY)
{
	VMMouseMessage mouseMessage;

	if (!CheckTargetState()) {
		return;
	}

	mouseMessage.hTargetWnd		= m_hTargetChildWnd;
	mouseMessage.dragButton		= RButtonDrag;
	mouseMessage.dragStartPos	= m_currentPos;
	m_currentPos.x				+= deltaX;
	m_currentPos.y				+= deltaY;
	mouseMessage.dragEndPos		= m_currentPos;
	mouseMessage.uKeyState		= MK_SHIFT;
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
	TCHAR szReturnedString[I4C3D_BUFFER_SIZE];
	TCHAR szKey[I4C3D_BUFFER_SIZE];
	TCHAR szValue[I4C3D_BUFFER_SIZE];

	TCHAR* ptr = szReturnedString;
	int ret;

	if (g_szIniFilePath[0] == _T('\0')) {
		I4C3DMisc::GetModuleFileWithExtension(g_szIniFilePath, sizeof(g_szIniFilePath)/sizeof(g_szIniFilePath[0]), _T("ini"));
	}
	GetPrivateProfileSection(szSectionName, szReturnedString, sizeof(szReturnedString)/sizeof(szReturnedString[0]), g_szIniFilePath);

	// 解析
	do {
		I4C3DMisc::ReportError(ptr);
		ret = _stscanf_s(ptr, _T("%s=%s"), szKey, sizeof(szKey)/sizeof(szKey[0]), 
			szValue, sizeof(szValue)/sizeof(szValue[0]));
		{
			TCHAR szBuffer[5];
			_stprintf_s(szBuffer, _T("%d"), ret);
			I4C3DMisc::ReportError(szBuffer);
		}
		if (ret != 2) {
			I4C3DMisc::ReportError(szKey);
			I4C3DMisc::ReportError(szValue);
			break;
		}
		I4C3DMisc::LogDebugMessage(szKey);
		I4C3DMisc::LogDebugMessage(szValue);
		if (_tcsncicmp(szKey, _T("HOTKEY_"), 7) == 0) {
			//TCHAR* pKey = (LPTSTR)calloc(lstrlen( sizeof(TCHAR)));
			m_settingsMap.insert(std::map<LPCTSTR, LPCTSTR>::value_type(szKey, szValue));
		}
		ptr = ptr + lstrlen(ptr);
	} while (ret == 2);
}