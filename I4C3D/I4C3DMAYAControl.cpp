#include "StdAfx.h"
#include "I4C3DMAYAControl.h"

extern TCHAR g_szIniFilePath[MAX_PATH];

I4C3DMAYAControl::I4C3DMAYAControl(void)
{
	m_hTargetParentWnd	= NULL;
	m_hTargetChildWnd	= NULL;
	m_basePos.x			= 0;
	m_basePos.y			= 0;
	m_currentPos.x		= 0;
	m_currentPos.y		= 0;
	m_ctrl = m_alt = m_shift = m_bSyskeyDown = FALSE;
}


I4C3DMAYAControl::I4C3DMAYAControl(I4C3DContext* pContext) {
	m_hTargetParentWnd	= pContext->hTargetParentWnd;
	m_hTargetChildWnd	= NULL;	// TODO!!!!
	m_basePos.x			= 0;
	m_basePos.y			= 0;
	m_currentPos.x		= 0;
	m_currentPos.y		= 0;

	m_ctrl = m_alt = m_shift = m_bSyskeyDown = FALSE;

	TCHAR tempBuffer[I4C3D_BUFFER_SIZE] = {0};
	GetPrivateProfileString(_T("MAYA"), _T("MODIFIER_KEY"), _T("Alt"), tempBuffer, sizeof(tempBuffer)/sizeof(tempBuffer[0]), g_szIniFilePath);

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

	CreateSettingMap(_T("MAYA"));
}


I4C3DMAYAControl::~I4C3DMAYAControl(void)
{
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}

void I4C3DMAYAControl::TumbleExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetParentWnd, TRUE);
	I4C3DControl::TumbleExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}

void I4C3DMAYAControl::TrackExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetParentWnd, TRUE);
	I4C3DControl::TrackExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}

void I4C3DMAYAControl::DollyExecute(int deltaX, int deltaY)
{
	SendSystemKeys(m_hTargetParentWnd, TRUE);
	I4C3DControl::DollyExecute(deltaX, deltaY);
	SendSystemKeys(m_hTargetParentWnd, FALSE);
}

void I4C3DMAYAControl::HotkeyExecute(LPCTSTR szCommand)
{
	I4C3DControl::HotkeyExecute(m_hTargetParentWnd, szCommand);
}
