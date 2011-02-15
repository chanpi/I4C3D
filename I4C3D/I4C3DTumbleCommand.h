#pragma once
#include "i4c3dcommand.h"

class I4C3DTumbleCommand :
	public I4C3DCommand
{
public:
	I4C3DTumbleCommand(void);
	I4C3DTumbleCommand(HWND hWnd, CommandSet commandSet);
	~I4C3DTumbleCommand(void);

	void Execute(void);
};

