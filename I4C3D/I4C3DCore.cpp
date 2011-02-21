#include <process.h>

#include "StdAfx.h"
#include "I4C3D.h"
#include "I4C3DCore.h"
#include "I4C3DAccessor.h"
#include "I4C3DMisc.h"

static BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam);
static HWND g_targetWindow = NULL;
static TCHAR g_szFilePath[MAX_PATH] = {0};

static unsigned __stdcall I4C3DReceiveThreadProc(void* pParam);

I4C3DCore::I4C3DCore(void)
{
	m_started = FALSE;
}


I4C3DCore::~I4C3DCore(void)
{
}

BOOL I4C3DCore::Start(I4C3DContext* pContext) {
	USHORT uBridgePort = 0;
	TCHAR szError[I4C3D_BUFFER_SIZE];
	int nTimeoutMilisec = 3000;	// �K�v�H�H

	if (m_started) {
		return TRUE;
	}

	// ini�t�@�C���̐�΃p�X���擾
	I4C3DMisc::GetModuleFileWithExtension(g_szFilePath, sizeof(g_szFilePath)/sizeof(g_szFilePath[0]), _T("ini"));

	// �^�[�Q�b�g�E�B���h�E���擾
	if ((pContext->hMainWnd = GetTarget3DSoftwareWnd()) == NULL) {
		return FALSE;
	}

	// �ݒ�t�@�C�����Bridge Port���擾
	uBridgePort = (USHORT)GetPrivateProfileInt(_T("GLOBAL"), _T("BRIDGE_PORT"), 10001, g_szFilePath);

	// iPhone�҂��󂯃X�^�[�g
	I4C3DAccessor accessor;
	pContext->receiver = accessor.InitializeSocket(NULL, uBridgePort, nTimeoutMilisec, FALSE);
	if (pContext->receiver == INVALID_SOCKET) {
		OutputDebugString(_T("[ERROR] InitializeSocket"));
		return FALSE;
	}

	// 
	pContext->hStopEvent = WSACreateEvent();
	if (pContext->hStopEvent == WSA_INVALID_EVENT) {
		_stprintf(szError, _T("[ERROR] WSACreateEvent() : %d"), WSAGetLastError());
		I4C3DMisc::ReportError(szError);
		closesocket(pContext->receiver);
		return FALSE;
	}

	// �����X���b�h���쐬����Ƃ��́A�T�X�y���h��Ԃō쐬���A�Ō�Ɉ�C�Ƀ��W���[������Ƃ悢
	pContext->hThread = (HANDLE)_beginthreadex(NULL, 0, &I4C3DReceiveThreadProc, (void*)pContext, CREATE_SUSPENDED, &pContext->uThreadID);
	if (pContext->hThread == INVALID_HANDLE_VALUE) {
		_stprintf(szError, _T("[ERROR] _beginthreadex() : %d"), GetLastError());
		I4C3DMisc::ReportError(szError);
		WSACloseEvent(pContext->hStopEvent);
		closesocket(pContext->receiver);
		return FALSE;
	}

	ResumeThread(pContext->hThread);

	m_started = TRUE;
	return TRUE;
}

void I4C3DCore::Stop(I4C3DContext* pContext) {
	if (!m_started) {
		return;
	}

	// �I���C�x���g�ݒ�
	WSASetEvent(pContext->hStopEvent);
	WaitForSingleObject(pContext->hThread, INFINITE);
	WSACloseEvent(pContext->hStopEvent);
	pContext->hStopEvent = NULL;

	// �X���b�h�I��
	CloseHandle(pContext->hThread);
	pContext->hThread = NULL;

	// �\�P�b�g�N���[�Y
	closesocket(pContext->receiver);
	pContext->receiver = INVALID_SOCKET;

	m_started = FALSE;
}


HWND I4C3DCore::GetTarget3DSoftwareWnd(void) {
	TCHAR szTarget[MAX_PATH] = {0};
	if (GetTarget3DSoftwareName(szTarget, MAX_PATH)) {
		if (!EnumWindows(&EnumWindowProc, (LPARAM)szTarget)) {
			OutputDebugString(_T("not NULL\n"));
		} else {
			OutputDebugString(_T("NULL\n"));
		}
	}
	return g_targetWindow;
}

BOOL I4C3DCore::GetTarget3DSoftwareName(LPTSTR lpszTargetName, SIZE_T size) {
	GetPrivateProfileString(_T("GLOBAL"), _T("TARGET_SOFT"), NULL, lpszTargetName, size, g_szFilePath);
	
	OutputDebugString(lpszTargetName);
	OutputDebugString(_T("\n"));

	if (lstrcmp(lpszTargetName, _T("")) != 0) {
		GetPrivateProfileString(lpszTargetName, _T("WINDOW_NAME"), _T(""), lpszTargetName, size, g_szFilePath);
	} else {
		return FALSE;
	}

	return lstrlen(lpszTargetName);
}


//////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szTitle[MAX_PATH] = {0};
	GetWindowText(hWnd, szTitle, sizeof(szTitle)/sizeof(szTitle[0]));
	if (_tcsstr(szTitle, (LPCTSTR)lParam) != NULL) {
		g_targetWindow = hWnd;
		return FALSE;	// EnumWindow�𒆒f
	}
	return TRUE;		// EnumWindow���p��
}

// accept������񓯊��ōs��
unsigned __stdcall I4C3DReceiveThreadProc(void* pParam)
{
	TCHAR szError[I4C3D_BUFFER_SIZE];
	I4C3DContext* pContext = (I4C3DContext*)pParam;

	SOCKET newClient = NULL;
	SOCKADDR_IN address;
	int nLen = 0;
	HANDLE hChildThread = NULL;
	UINT uThreadID = 0;

	DWORD dwResult = 0;
	WSAEVENT hEvent = NULL;
	WSAEVENT hEventArray[2] = {0};
	WSANETWORKEVENTS events = {0};

	hEvent = WSACreateEvent();
	if (hEvent == WSA_INVALID_EVENT) {
		_stprintf(szError, _T("[ERROR] WSACreateEvent() : %d"), WSAGetLastError());
		I4C3DMisc::ReportError(szError);
		return FALSE;
	}

	WSAEventSelect(pContext->receiver, hEvent, FD_ACCEPT | FD_CLOSE);

	hEventArray[0] = hEvent;
	hEventArray[1] = pContext->hStopEvent;

	for (;;) {
		dwResult = WSAWaitForMultipleEvents(2, hEventArray, FALSE, WSA_INFINITE, FALSE);
		if (dwResult == WSA_WAIT_FAILED) {
			_stprintf(szError, _T("[ERROR] WSA_WAIT_FAILED : %d"), WSAGetLastError());
			I4C3DMisc::ReportError(szError);
			
		}
	}
	return TRUE;
}