#pragma once
#include "i4c3dcommand.h"
class I4C3DDollyCommand :
	public I4C3DCommand
{
public:
	I4C3DDollyCommand(void);
	I4C3DDollyCommand(HWND hWnd, CommandSet commandSet);
	~I4C3DDollyCommand(void);

	void Execute(void);
};

