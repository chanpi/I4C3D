#include "StdAfx.h"
#include "I4C3DMAYAControl.h"


I4C3DMAYAControl::I4C3DMAYAControl(void)
{
}


I4C3DMAYAControl::I4C3DMAYAControl(HWND hWnd) {
	m_hWnd = hWnd;
	m_posX = 0;
	m_posY = 0;
}


I4C3DMAYAControl::~I4C3DMAYAControl(void)
{
}

void I4C3DMAYAControl::TumbleExecute(int deltaX, int deltaY)
{
	PostMessage(m_hWnd, WM_SYSKEYDOWN, VK_MENU, 0);
	PostMessage(m_hWnd, WM_LBUTTONDOWN, MK_LBUTTON, 0);
	PostMessage(m_hWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(deltaX + m_posX, deltaY + m_posY));
	PostMessage(m_hWnd, WM_LBUTTONUP, 0, 0);
	PostMessage(m_hWnd, WM_SYSKEYUP, VK_MENU, 0);
}

void I4C3DMAYAControl::TrackExecute(int deltaX, int deltaY)
{
	PostMessage(m_hWnd, WM_SYSKEYDOWN, VK_MENU, 0);
	PostMessage(m_hWnd, WM_MBUTTONDOWN, MK_MBUTTON, 0);
	PostMessage(m_hWnd, WM_MOUSEMOVE, MK_MBUTTON, MAKELPARAM(deltaX + m_posX, deltaY + m_posY));
	PostMessage(m_hWnd, WM_MBUTTONUP, 0, 0);
	PostMessage(m_hWnd, WM_SYSKEYUP, VK_MENU, 0);
}

void I4C3DMAYAControl::DollyExecute(int deltaX, int deltaY)
{
	PostMessage(m_hWnd, WM_SYSKEYDOWN, VK_MENU, 0);
	PostMessage(m_hWnd, WM_RBUTTONDOWN, MK_RBUTTON, 0);
	PostMessage(m_hWnd, WM_MOUSEMOVE, MK_RBUTTON, MAKELPARAM(deltaX + m_posX, deltaY + m_posY));
	PostMessage(m_hWnd, WM_RBUTTONUP, 0, 0);
	PostMessage(m_hWnd, WM_SYSKEYUP, VK_MENU, 0);
}
