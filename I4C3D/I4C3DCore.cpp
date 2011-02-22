#include "StdAfx.h"
#include "I4C3D.h"
#include "I4C3DCore.h"
#include "I4C3DAccessor.h"
#include "I4C3DMisc.h"

#include <process.h>

static BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam);
static HWND g_targetWindow = NULL;
static TCHAR g_szFilePath[MAX_PATH] = {0};

static unsigned __stdcall I4C3DReceiveThreadProc(void* pParam);
static unsigned __stdcall I4C3DAcceptedThreadProc(void* pParam);
static LPCSTR AnalyzeMessage(LPCSTR lpszMessage, LPSTR lpszCommand, SIZE_T size, int* pDeltaX, int* pDeltaY);

typedef struct {
	I4C3DContext* pContext;
	SOCKET clientSocket;
	char cTermination;
} I4C3DChildContext;

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
	CloseHandle(pContext->hThread);

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
	I4C3DChildContext* pChildContext = NULL;

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
			break;
		}

		if (dwResult - WSA_WAIT_EVENT_0 == 0) {
			WSAEnumNetworkEvents(pContext->receiver, hEvent, &events);
			if (events.lNetworkEvents & FD_CLOSE) {
				break;
			} else if (events.lNetworkEvents & FD_ACCEPT) {
				// accept
				nLen = sizeof(address);
				newClient = accept(pContext->receiver, (SOCKADDR*)&address, &nLen);
				if (newClient == INVALID_SOCKET) {
					_stprintf(szError, _T("[ERROR] accept : %d"), WSAGetLastError());
					I4C3DMisc::ReportError(szError);
					break;
				}

				// Create child thread.
				pChildContext = (I4C3DChildContext*)calloc(1, sizeof(I4C3DChildContext));
				if (pChildContext == NULL) {
					_stprintf(szError, _T("[ERROR] Create child thread. calloc : %d"), GetLastError());
					I4C3DMisc::ReportError(szError);
					break;
				}
				pChildContext->pContext = pContext;
				pChildContext->clientSocket = newClient;
				pChildContext->cTermination = '\0';		// TODO �ݒ�t�@�C������ǂݍ��݁H
				hChildThread = (HANDLE)_beginthreadex(NULL, 0, I4C3DAcceptedThreadProc, pChildContext, 0, &uThreadID);
				if (hChildThread == INVALID_HANDLE_VALUE) {
					_stprintf(szError, _T("[ERROR] Create child thread. : %d"), GetLastError());
					I4C3DMisc::ReportError(szError);
					break;
				} else {
					CloseHandle(hChildThread);
				}
			}
			WSAResetEvent(hEvent);

		} else if (dwResult - WSA_WAIT_EVENT_0 == 1) {
			break;
		}
	}
	WSACloseEvent(hEvent);

	shutdown(pContext->receiver, SD_BOTH);
	closesocket(pContext->receiver);
	return TRUE;
}

unsigned __stdcall I4C3DAcceptedThreadProc(void* pParam)
{
	I4C3DChildContext* pChildContext = (I4C3DChildContext*)pParam;

	TCHAR szError[I4C3D_BUFFER_SIZE];
	char recvBuffer[I4C3D_RECEIVE_LENGTH];
	SIZE_T totalRecvBytes = 0;
	int nBytes = 0;

	char szCommand[I4C3D_BUFFER_SIZE] = {0};
	int deltaX;
	int deltaY;

	DWORD dwResult = 0;
	WSAEVENT hEvent = NULL;
	WSAEVENT hEventArray[2];
	WSANETWORKEVENTS events = {0};

	hEvent = WSACreateEvent();
	WSAEventSelect(pChildContext->clientSocket, hEvent, FD_READ | FD_CLOSE);

	hEventArray[0] = hEvent;
	hEventArray[1] = pChildContext->pContext->hStopEvent;

	FillMemory(recvBuffer, sizeof(recvBuffer), 0xFF);

	for (;;) {
		if (events.iErrorCode[FD_ACCEPT_BIT] != 0) {
			I4C3DMisc::ReportError(TEXT("FD_ACCEPT_BIT"));
			break;
		} else if (events.iErrorCode[FD_CLOSE_BIT] != 0) {
			I4C3DMisc::ReportError(TEXT("FD_CLOSE_BIT"));
			break;
		} else if (events.iErrorCode[FD_CONNECT_BIT] != 0) {
			I4C3DMisc::ReportError(TEXT("FD_CONNECT_BIT"));
			break;
		} else if (events.iErrorCode[FD_OOB_BIT] != 0) {
			I4C3DMisc::ReportError(TEXT("FD_OOB_BIT"));
			break;
		} else if (events.iErrorCode[FD_QOS_BIT] != 0) {
			I4C3DMisc::ReportError(TEXT("FD_QOS_BIT"));
			break;
		} else if (events.iErrorCode[FD_READ_BIT] != 0) {
			I4C3DMisc::ReportError(TEXT("FD_READ_BIT"));
			break;
		} else if (events.iErrorCode[FD_WRITE_BIT] != 0) {
			I4C3DMisc::ReportError(TEXT("FD_WRITE_BIT"));
			break;
		}

		dwResult = WSAWaitForMultipleEvents(2, hEventArray, FALSE, WSA_INFINITE, FALSE);
		if (dwResult == WSA_WAIT_FAILED) {
			break;
		}

		if (dwResult - WSA_WAIT_EVENT_0 == 0) {
			if (WSAEnumNetworkEvents(pChildContext->clientSocket, hEvent, &events) != 0) {
				_stprintf(szError, _T("[ERROR] WSAEnumNetworkEvents() : %d"), WSAGetLastError());
				I4C3DMisc::ReportError(szError);
				break;
			}

			if (events.lNetworkEvents & FD_CLOSE) {
				break;
			} else if (events.lNetworkEvents & FD_READ) {
				nBytes = recv(pChildContext->clientSocket, recvBuffer, sizeof(recvBuffer) - totalRecvBytes, 0);

				if (nBytes == SOCKET_ERROR) {
					_stprintf(szError, _T("[ERROR] recv : %d"), WSAGetLastError());
					I4C3DMisc::ReportError(szError);
					break;
				} else if (nBytes > 0) {
					totalRecvBytes += nBytes;

					// �I�[������������Ȃ�����
					if (totalRecvBytes >= sizeof(recvBuffer) && memchr(recvBuffer+totalRecvBytes-nBytes, pChildContext->cTermination, nBytes) == NULL) {
						FillMemory(recvBuffer, sizeof(recvBuffer), 0xFF);
						totalRecvBytes = 0;
						continue;
					}

					// �d�����
					deltaX = deltaY = 0;
					LPCSTR ptr = AnalyzeMessage(recvBuffer, szCommand, sizeof(szCommand), &deltaX, &deltaY);
					_stprintf(szError, _T("ptr : %p , recvBuffer + totalRecvBytes : %p\n"), ptr, recvBuffer + totalRecvBytes);
					OutputDebugString(szError);

					// �R�}���h���M�̂���SendMessage�ideltaX, deltaY, szCommand����������O�ɏ������s���j
					SendMessage(pChildContext->pContext->hMainWnd, WM_BRIDGEMESSAGE, MAKEWPARAM(deltaX, deltaY), (LPARAM)szCommand);

					if (ptr == recvBuffer + totalRecvBytes) {
						FillMemory(recvBuffer, sizeof(recvBuffer), 0xFF);
						totalRecvBytes = 0;
					}
				}
			}

			ResetEvent(hEvent);
		} else if (dwResult - WSA_WAIT_EVENT_0 == 1) {
			// pChildContext->pContext->hStopEvent �ɏI���C�x���g���Z�b�g���ꂽ
			break;
		}
	}
	WSACloseEvent(hEvent);

	// closesocket
	shutdown(pChildContext->clientSocket, SD_SEND);
	recv(pChildContext->clientSocket, recvBuffer, sizeof(recvBuffer), 0);
	shutdown(pChildContext->clientSocket, SD_BOTH);
	closesocket(pChildContext->clientSocket);

	free(pChildContext);
	return TRUE;
}

LPCSTR AnalyzeMessage(LPCSTR lpszMessage, LPSTR lpszCommand, SIZE_T size, int* pDeltaX, int* pDeltaY)
{
	char tempBuffer[I4C3D_BUFFER_SIZE];
	OutputDebugString(_T("Analyze!!!!!!!!!!!!!!!!!!!!!!!!\n"));

	sscanf(lpszMessage, "%s %d %d?", lpszCommand, pDeltaX, pDeltaY);

	OutputDebugStringA(lpszCommand);
	OutputDebugString(_T("\n"));

	return NULL;
	/*
	int index = 0;
	char tempBuffer[I4C3D_BUFFER_SIZE] = {0};
	const char* ptr = lpszMessage;
	ZeroMemory(lpszCommand, size);

	// �R�}���h�ǂݎ��
	while (isspace(*ptr)) {
		ptr++;
	}
	while (isalpha(*ptr)) {
		*lpszCommand++ = *ptr++;
	}

	// X�ړ��ʓǂݎ��
	while (isspace(*ptr)) {
		ptr++;
	}
	while (!isspace(*ptr)) {
		tempBuffer[index++] = *ptr++;
	}
	*pDeltaX = atoi(tempBuffer);

	// Y�ړ��ʓǂݎ��
	ZeroMemory(tempBuffer, sizeof(tempBuffer));
	index = 0;
	while (isspace(*ptr)) {
		ptr++;
	}
	while (!isspace(*ptr)) {
		tempBuffer[index++] = *ptr++;
	}
	*pDeltaY = atoi(tempBuffer);

	return ptr-1;	// 1�����ǂݖ߂�
	*/
}