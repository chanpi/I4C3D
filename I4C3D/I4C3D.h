#pragma once

#include "resource.h"

typedef struct {
	HWND hMainWnd;

	HANDLE hThread;			// iPhone/iPod����̎�M�̓��[�J�[�X���b�h�ōs��
	UINT uThreadID;
	WSAEVENT hStopEvent;
	SOCKET receiver;
} I4C3DContext;