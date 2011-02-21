#include "StdAfx.h"
#include "I4C3DRTTControl.h"


I4C3DRTTControl::I4C3DRTTControl(void)
{
}


I4C3DRTTControl::I4C3DRTTControl(HWND hWnd)
{
	m_hWnd = hWnd;
	m_posX = 0;
	m_posY = 0;
}


I4C3DRTTControl::~I4C3DRTTControl(void)
{
}

// ctrl + マウス左ドラッグ
void I4C3DRTTControl::TumbleExecute(int deltaX, int deltaY)
{
	PostMessage(m_hWnd, WM_LBUTTONDOWN, MK_CONTROL | MK_LBUTTON, 0);
	PostMessage(m_hWnd, WM_MOUSEMOVE, MK_CONTROL | MK_LBUTTON, MAKELPARAM(deltaX + m_posX, deltaY + m_posY));
	PostMessage(m_hWnd, WM_LBUTTONUP, 0, 0);
}

// ctrl + マウス中ドラッグ
void I4C3DRTTControl::TrackExecute(int deltaX, int deltaY)
{
	PostMessage(m_hWnd, WM_MBUTTONDOWN, MK_CONTROL | MK_MBUTTON, 0);
	PostMessage(m_hWnd, WM_MOUSEMOVE, MK_CONTROL | MK_MBUTTON, MAKELPARAM(deltaX + m_posX, deltaY + m_posY));
	PostMessage(m_hWnd, WM_MBUTTONUP, 0, 0);
}

// ctrl + マウス右ドラッグ
void I4C3DRTTControl::DollyExecute(int deltaX, int deltaY)
{
	PostMessage(m_hWnd, WM_RBUTTONDOWN, MK_CONTROL | MK_RBUTTON, 0);
	PostMessage(m_hWnd, WM_MOUSEMOVE, MK_CONTROL | MK_RBUTTON, MAKELPARAM(deltaX + m_posX, deltaY + m_posY));
	PostMessage(m_hWnd, WM_RBUTTONUP, 0, 0);
}