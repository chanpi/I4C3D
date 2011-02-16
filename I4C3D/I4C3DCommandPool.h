#pragma once

#include "I4C3DCommand.h"

class I4C3DCommandPool
{
public:
	I4C3DCommandPool(void);
	I4C3DCommandPool(LPCTSTR lpszTarget3DSoftware);
	~I4C3DCommandPool(void);

	I4C3DCommand* GetCommand(LPCTSTR lpszCommandName);
};

