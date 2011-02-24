#pragma once
#include "I4C3D.h"
#include "I4C3DMisc.h"

class I4C3DControl
{
public:
	I4C3DControl(void);
	I4C3DControl(I4C3DContext* pContext);
	virtual ~I4C3DControl(void) = 0;

	virtual void TumbleExecute(int deltaX, int deltaY);
	virtual void TrackExecute(int deltaX, int deltaY);
	virtual void DollyExecute(int deltaX, int deltaY);

protected:
	HWND m_hTargetParentWnd;
	HWND m_hTargetChildWnd;
	int m_posX;
	int m_posY;
	BOOL m_ctrl;
	BOOL m_alt;
	BOOL m_shift;
};
