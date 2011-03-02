#include "StdAfx.h"
#include "I4C3DControl.h"

extern TCHAR szTitle[MAX_LOADSTRING];

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
	Sleep(5);
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