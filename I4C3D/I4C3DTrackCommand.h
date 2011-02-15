#pragma once
#include "i4c3dcommand.h"

class I4C3DTrackCommand :
	public I4C3DCommand
{
public:
	I4C3DTrackCommand(void);
	I4C3DTrackCommand(HWND hWnd, CommandSet commandSet);
	~I4C3DTrackCommand(void);

	void Execute(void);
};

