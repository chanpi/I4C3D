#pragma once

#include "resource.h"

class I4C3DControl;

typedef struct {
	HWND hMyWnd;
	HWND hTargetParentWnd;
	I4C3DControl* pController;

	HANDLE hThread;			// iPhone/iPod����̎�M�̓��[�J�[�X���b�h�ōs��
	UINT uThreadID;
	WSAEVENT hStopEvent;
	SOCKET receiver;
} I4C3DContext;