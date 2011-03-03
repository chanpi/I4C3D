#include "StdAfx.h"
#include "I4C3DRTTControl.h"

extern TCHAR g_szFilePath[MAX_PATH];

I4C3DRTTControl::I4C3DRTTControl(void)
{
	m_hTargetParentWnd	= NULL;
	m_hTargetChildWnd	= NULL;
	m_basePos.x			= 0;
	m_basePos.y			= 0;
	m_currentPos.x		= 0;
	m_currentPos.y		= 0;
	m_ctrl = m_alt = m_shift = m_bSyskeyDown = FALSE;
}


I4C3DRTTControl::I4C3DRTTControl(I4C3DContext* pContext)
{
	m_hTargetParentWnd	= pContext->hTargetParentWnd;
	m_hTargetChildWnd	= NULL;	// TODO!!!!
	m_basePos.x			= 0;
	m_basePos.y			= 0;
	m_currentPos.x		= 0;
	m_currentPos.y		= 0;

	m_ctrl = m_alt = m_shift = m_bSyskeyDown = FALSE;

	TCHAR tempBuffer[5] = {0};
	GetPrivateProfileString(_T("RTT"), _T("MODIFIER_KEY"), _T("C"), tempBuffer, sizeof(tempBuffer)/sizeof(tempBuffer[0]), g_szFilePath);
	int count = lstrlen(tempBuffer);
	for (int i = 0; i < count; i++) {
		switch (tempBuffer[i]) {
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
	}
}


I4C3DRTTControl::~I4C3DRTTControl(void)
{
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}

// ctrl + マウス左ドラッグ
void I4C3DRTTControl::TumbleExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetParentWnd, TRUE);
	I4C3DControl::TumbleExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}

// ctrl + マウス中ドラッグ
void I4C3DRTTControl::TrackExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetParentWnd, TRUE);
	I4C3DControl::TrackExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}

// ctrl + マウス右ドラッグ
void I4C3DRTTControl::DollyExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetParentWnd, TRUE);
	I4C3DControl::DollyExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}