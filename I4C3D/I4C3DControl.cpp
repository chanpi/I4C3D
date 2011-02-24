#include "StdAfx.h"
#include "I4C3DControl.h"

extern TCHAR szTitle[MAX_LOADSTRING];

I4C3DControl::I4C3DControl(void)
{
	m_hTargetParentWnd = NULL;
	m_hTargetChildWnd = NULL;
	m_posX = 0;
	m_posY = 0;
	m_ctrl = m_alt = m_shift = FALSE;
}


I4C3DControl::I4C3DControl(I4C3DContext* pContext)
{
	m_hTargetParentWnd = pContext->hTargetParentWnd;
	m_hTargetChildWnd = NULL;			// �q�E�B���h�E���K�v�ł���Δh���N���X�Ŏ擾���Ă��������B
	m_posX = 0;
	m_posY = 0;
	m_ctrl = m_alt = m_shift = FALSE;	// �e�V�X�e���L�[�͔h���N���X�Ŏ擾���Ă��������B
}

I4C3DControl::~I4C3DControl(void)
{
}

void I4C3DControl::TumbleExecute(int deltaX, int deltaY)
{
	if (m_hTargetChildWnd != NULL) {
		PostMessage(m_hTargetChildWnd, WM_LBUTTONDOWN, MK_SHIFT | MK_LBUTTON, MAKELPARAM(m_posX, m_posX));
		PostMessage(m_hTargetChildWnd, WM_MOUSEMOVE, MK_SHIFT | MK_LBUTTON, MAKELPARAM(m_posX+=deltaX, m_posY+=deltaY));
		PostMessage(m_hTargetChildWnd, WM_LBUTTONUP, MK_SHIFT, MAKELPARAM(m_posX, m_posY));
	} else {
		I4C3DMisc::ReportError(_T("�q�E�B���h�E���擾���Ă��������B"));
	}
}

void I4C3DControl::TrackExecute(int deltaX, int deltaY)
{
	if (m_hTargetChildWnd != NULL) {
		PostMessage(m_hTargetChildWnd, WM_MBUTTONDOWN, MK_SHIFT | MK_MBUTTON, MAKELPARAM(m_posX, m_posX));
		PostMessage(m_hTargetChildWnd, WM_MOUSEMOVE, MK_SHIFT | MK_MBUTTON, MAKELPARAM(m_posX+=deltaX, m_posY+=deltaY));
		PostMessage(m_hTargetChildWnd, WM_MBUTTONUP, MK_SHIFT, MAKELPARAM(m_posX, m_posY));
	} else {
		I4C3DMisc::ReportError(_T("�q�E�B���h�E���擾���Ă��������B"));
	}
}

void I4C3DControl::DollyExecute(int deltaX, int deltaY)
{
	if (m_hTargetChildWnd != NULL) {
		PostMessage(m_hTargetChildWnd, WM_RBUTTONDOWN, MK_SHIFT | MK_RBUTTON, MAKELPARAM(m_posX, m_posX));
		PostMessage(m_hTargetChildWnd, WM_MOUSEMOVE, MK_SHIFT | MK_RBUTTON, MAKELPARAM(m_posX+=deltaX, m_posY+=deltaY));
		PostMessage(m_hTargetChildWnd, WM_RBUTTONUP, MK_SHIFT, MAKELPARAM(m_posX, m_posY));
	} else {
		I4C3DMisc::ReportError(_T("�q�E�B���h�E���擾���Ă��������B"));
	}
}