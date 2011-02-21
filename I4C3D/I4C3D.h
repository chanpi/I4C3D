#pragma once

#include "resource.h"

typedef struct {
	HWND hMainWnd;

	HANDLE hThread;			// iPhone/iPodからの受信はワーカースレッドで行う
	UINT uThreadID;
	WSAEVENT hStopEvent;
	SOCKET receiver;
} I4C3DContext;