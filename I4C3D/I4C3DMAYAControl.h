#pragma once
#include "i4c3dcontrol.h"
class I4C3DMAYAControl :
	public I4C3DControl
{
public:
	I4C3DMAYAControl(void);
	I4C3DMAYAControl(I4C3DContext* pContext);
	~I4C3DMAYAControl(void);

	void TumbleExecute(int deltaX, int deltaY);
	void TrackExecute(int deltaX, int deltaY);
	void DollyExecute(int deltaX, int deltaY);
	void HotkeyExecute(LPCTSTR szCommand);
};

