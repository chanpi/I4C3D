#pragma once
#include "i4c3dcontrol.h"
class I4C3DShowcaseControl :
	public I4C3DControl
{
public:
	I4C3DShowcaseControl(void);
	I4C3DShowcaseControl(I4C3DContext* pContext);
	~I4C3DShowcaseControl(void);

	void TumbleExecute(int deltaX, int deltaY);
	void TrackExecute(int deltaX, int deltaY);
	void DollyExecute(int deltaX, int deltaY);
};

