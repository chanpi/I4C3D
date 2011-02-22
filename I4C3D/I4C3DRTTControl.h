#pragma once
#include "i4c3dcontrol.h"

class I4C3DRTTControl :
	public I4C3DControl
{
public:
	I4C3DRTTControl(void);
	I4C3DRTTControl(HWND hWnd);
	~I4C3DRTTControl(void);

	void TumbleExecute(int deltaX, int deltaY);
	void TrackExecute(int deltaX, int deltaY);
	void DollyExecute(int deltaX, int deltaY);
};
