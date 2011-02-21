#include "StdAfx.h"
#include "I4C3D.h"
#include "I4C3DControl.h"

extern TCHAR szTitle[MAX_LOADSTRING];

I4C3DControl::I4C3DControl(void)
{
	m_hWnd = NULL;
	m_posX = 0;
	m_posY = 0;
}


I4C3DControl::I4C3DControl(HWND hWnd)
{
	m_hWnd = hWnd;
	m_posX = 0;
	m_posY = 0;
}

I4C3DControl::~I4C3DControl(void)
{
}

void I4C3DControl::TumbleExecute(int deltaX, int deltaY)
{
	MessageBox(m_hWnd, _T("Please Implement TumbleExecute!"), szTitle, MB_OK);
}

void I4C3DControl::TrackExecute(int deltaX, int deltaY)
{
	MessageBox(m_hWnd, _T("Please Implement TrackExecute!"), szTitle, MB_OK);
}

void I4C3DControl::DollyExecute(int deltaX, int deltaY)
{
	MessageBox(m_hWnd, _T("Please Implement DollyExecute!"), szTitle, MB_OK);
}