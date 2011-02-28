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

	//PostMessage(m_hTargetChildWnd, WM_LBUTTONDOWN, MK_SHIFT | MK_LBUTTON, MAKELPARAM(m_currentPos.x, m_currentPos.y));
	//PostMessage(m_hTargetChildWnd, WM_MOUSEMOVE, MK_SHIFT | MK_LBUTTON, MAKELPARAM(m_currentPos.x+=deltaX, m_currentPos.y+=deltaY));
	//PostMessage(m_hTargetChildWnd, WM_LBUTTONUP, MK_SHIFT, MAKELPARAM(m_currentPos.x, m_currentPos.y));
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

	//PostMessage(m_hTargetChildWnd, WM_MBUTTONDOWN, MK_SHIFT | MK_MBUTTON, MAKELPARAM(m_currentPos.x, m_currentPos.y));
	//PostMessage(m_hTargetChildWnd, WM_MOUSEMOVE, MK_SHIFT | MK_MBUTTON, MAKELPARAM(m_currentPos.x+=deltaX, m_currentPos.y+=deltaY));
	//PostMessage(m_hTargetChildWnd, WM_MBUTTONUP, MK_SHIFT, MAKELPARAM(m_currentPos.x, m_currentPos.y));
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

	//PostMessage(m_hTargetChildWnd, WM_RBUTTONDOWN, MK_SHIFT | MK_RBUTTON, MAKELPARAM(m_currentPos.x, m_currentPos.y));
	//PostMessage(m_hTargetChildWnd, WM_MOUSEMOVE, MK_SHIFT | MK_RBUTTON, MAKELPARAM(m_currentPos.x+=deltaX, m_currentPos.y+=deltaY));
	//PostMessage(m_hTargetChildWnd, WM_RBUTTONUP, MK_SHIFT, MAKELPARAM(m_currentPos.x, m_currentPos.y));
}

void I4C3DControl::SendSystemKeys(HWND hTargetWnd, BOOL bDown)
{
	if (bDown && !m_bSyskeyDown) {
		if (m_ctrl) {
			VMVirtualKeyDown(hTargetWnd, VK_CONTROL);
		}
		if (m_alt) {
			VMVirtualKeyDown(hTargetWnd, VK_MENU);
		}
		if (m_shift) {
			VMVirtualKeyDown(hTargetWnd, VK_SHIFT);
		}

	} else if (!bDown && m_bSyskeyDown) {
		if (m_shift) {
			VMVirtualKeyUp(hTargetWnd, VK_SHIFT);
		}
		if (m_alt) {
			VMVirtualKeyUp(hTargetWnd, VK_MENU);
		}
		if (m_ctrl) {
			VMVirtualKeyUp(hTargetWnd, VK_CONTROL);
		}

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
		m_basePos.x = rect.left + (rect.right - rect.left) / 2;
		m_basePos.y = rect.top + (rect.bottom - rect.right) / 2;

		// currentPosのはみ出しチェック
		GetClientRect(m_hTargetParentWnd, &rect);
		if (m_currentPos.x < rect.left || rect.right < m_currentPos.x
			|| m_currentPos.y < rect.top || rect.bottom < m_currentPos.y) {
				m_currentPos.x = m_basePos.x;
				m_currentPos.y = m_basePos.y;
		}

		return TRUE;
	}

	return FALSE;
}