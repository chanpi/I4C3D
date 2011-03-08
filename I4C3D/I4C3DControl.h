#pragma once
#include <map>
#include "I4C3D.h"
#include "I4C3DMisc.h"
#include "VirtualMotion.h"

class I4C3DControl
{
public:
	I4C3DControl(void);
	I4C3DControl(I4C3DContext* pContext);
	virtual ~I4C3DControl(void) = 0;

	virtual void TumbleExecute(int deltaX, int deltaY);
	virtual void TrackExecute(int deltaX, int deltaY);
	virtual void DollyExecute(int deltaX, int deltaY);
	virtual void HotkeyExecute(LPCTSTR szCommand);

protected:
	void HotkeyExecute(HWND hTargetWnd, LPCTSTR szCommand);
	void SendSystemKeys(HWND hTargetWnd, BOOL bDown);
	void CreateSettingMap(LPCTSTR szSectionName);
	HWND m_hTargetParentWnd;
	HWND m_hTargetChildWnd;
	POINT m_basePos;
	POINT m_currentPos;
	BOOL m_ctrl;
	BOOL m_alt;
	BOOL m_shift;
	BOOL m_bSyskeyDown;

	std::map<LPCTSTR, LPCTSTR> m_settingsMap;
	
private:
	BOOL CheckTargetState(void);
};
