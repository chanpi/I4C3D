#include "StdAfx.h"
#include "I4C3D.h"
#include "I4C3DCore.h"
#include "I4C3DAccessor.h"
#include "I4C3DMisc.h"

#include <process.h>

extern TCHAR g_szFilePath[MAX_PATH];

static BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam);
static HWND g_targetWindow = NULL;

static unsigned __stdcall I4C3DReceiveThreadProc(void* pParam);
static unsigned __stdcall I4C3DAcceptedThreadProc(void* pParam);
static int AnalyzeMessage(LPCSTR lpszMessage, LPSTR lpszCommand, SIZE_T size, PPOINT pDelta);

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
	int nTimeoutMilisec = 3000;	// 必要？？

	if (m_started) {
		return TRUE;
	}

	// ターゲットウィンドウを取得
	//if ((pContext->hTargetParentWnd = GetTarget3DSoftwareWnd()) == NULL) {
	//	return FALSE;
	//}

	// 設定ファイルよりBridge Portを取得
	uBridgePort = (USHORT)GetPrivateProfileInt(_T("GLOBAL"), _T("BRIDGE_PORT"), 10001, g_szFilePath);

	// iPhone待ち受けスタート
	I4C3DAccessor accessor;
	pContext->receiver = accessor.InitializeSocket(NULL, uBridgePort, nTimeoutMilisec, FALSE);
	if (pContext->receiver == INVALID_SOCKET) {
		OutputDebugString(_T("[ERROR] InitializeSocket"));
		return FALSE;
	}

	// 
	pContext->hStopEvent = WSACreateEvent();
	if (pContext->hStopEvent == WSA_INVALID_EVENT) {
		_stprintf_s(szError, sizeof(szError)/sizeof(szError), _T("[ERROR] WSACreateEvent() : %d"), WSAGetLastError());
		I4C3DMisc::ReportError(szError);
		closesocket(pContext->receiver);
		return FALSE;
	}

	// 複数スレッドを作成するときは、サスペンド状態で作成し、最後に一気にレジュームするとよい
	pContext->hThread = (HANDLE)_beginthreadex(NULL, 0, &I4C3DReceiveThreadProc, (void*)pContext, CREATE_SUSPENDED, &pContext->uThreadID);
	if (pContext->hThread == INVALID_HANDLE_VALUE) {
		_stprintf_s(szError, sizeof(szError)/sizeof(szError), _T("[ERROR] _beginthreadex() : %d"), GetLastError());
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

	// 終了イベント設定
	WSASetEvent(pContext->hStopEvent);
	WaitForSingleObject(pContext->hThread, INFINITE);
	WSACloseEvent(pContext->hStopEvent);
	pContext->hStopEvent = NULL;

	// スレッド終了
	CloseHandle(pContext->hThread);
	pContext->hThread = NULL;

	// ソケットクローズ
	closesocket(pContext->receiver);
	pContext->receiver = INVALID_SOCKET;

	m_started = FALSE;
}


//HWND I4C3DCore::GetTarget3DSoftwareWnd(void) {
//	TCHAR szTarget[MAX_PATH] = {0};
//	if (GetTarget3DSoftwareName(szTarget, MAX_PATH)) {
//		if (!EnumWindows(&EnumWindowProc, (LPARAM)szTarget)) {
//			OutputDebugString(_T("not NULL\n"));
//		} else {
//			OutputDebugString(_T("NULL\n"));
//		}
//	}
//	return g_targetWindow;
//}

BOOL I4C3DCore::GetTarget3DSoftwareName(LPTSTR lpszTargetName, SIZE_T size) {
	GetPrivateProfileString(_T("GLOBAL"), _T("TARGET_SOFT"), NULL, lpszTargetName, size, g_szFilePath);
	
	OutputDebugString(lpszTargetName);
	OutputDebugString(_T("\n"));

	//if (lstrcmp(lpszTargetName, _T("")) != 0) {
	//	GetPrivateProfileString(lpszTargetName, _T("WINDOW_NAME"), _T(""), lpszTargetName, size, g_szFilePath);
	//} else {
	//	return FALSE;
	//}

	return lstrlen(lpszTargetName);
}


//////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szTitle[MAX_PATH] = {0};
	GetWindowText(hWnd, szTitle, sizeof(szTitle)/sizeof(szTitle[0]));
	if (_tcsstr(szTitle, (LPCTSTR)lParam) != NULL) {
		g_targetWindow = hWnd;
		return FALSE;	// EnumWindowを中断
	}
	return TRUE;		// EnumWindowを継続
}

// accept処理を非同期で行う
unsigned __stdcall I4C3DReceiveThreadProc(void* pParam)
{
	I4C3DMisc::LogDebugMessage(_T("--- in I4C3DReceiveThreadProc ---"));

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
		_stprintf_s(szError, sizeof(szError)/sizeof(szError), _T("[ERROR] WSACreateEvent() : %d"), WSAGetLastError());
		I4C3DMisc::ReportError(szError);
		return FALSE;
	}

	WSAEventSelect(pContext->receiver, hEvent, FD_ACCEPT | FD_CLOSE);

	hEventArray[0] = hEvent;
	hEventArray[1] = pContext->hStopEvent;

	for (;;) {
		dwResult = WSAWaitForMultipleEvents(2, hEventArray, FALSE, WSA_INFINITE, FALSE);
		if (dwResult == WSA_WAIT_FAILED) {
			_stprintf_s(szError, sizeof(szError)/sizeof(szError), _T("[ERROR] WSA_WAIT_FAILED : %d"), WSAGetLastError());
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
					_stprintf_s(szError, sizeof(szError)/sizeof(szError), _T("[ERROR] accept : %d"), WSAGetLastError());
					I4C3DMisc::ReportError(szError);
					break;
				}

				// Create child thread.
				pChildContext = (I4C3DChildContext*)calloc(1, sizeof(I4C3DChildContext));
				if (pChildContext == NULL) {
					_stprintf_s(szError, sizeof(szError)/sizeof(szError), _T("[ERROR] Create child thread. calloc : %d"), GetLastError());
					I4C3DMisc::ReportError(szError);
					break;
				}

				// 設定ファイルから終端文字を取得
				TCHAR szTermination[3] = {0};
				GetPrivateProfileString(_T("GLOBAL"), _T("TERMINATION"), NULL, szTermination, sizeof(szTermination)/sizeof(szTermination[0]), g_szFilePath);
				if (szTermination[0] != 0) {
					char cszTermination[3] = {0};
					WideCharToMultiByte(CP_ACP, 0, szTermination, lstrlen(szTermination), cszTermination, sizeof(cszTermination), NULL, NULL);
					pChildContext->cTermination = cszTermination[0];
					OutputDebugStringA(cszTermination);
					OutputDebugString(_T("\n"));
					I4C3DMisc::LogDebugMessage(szTermination);

				} else {
					pChildContext->cTermination = _T('\0');

				}
				pChildContext->pContext = pContext;
				pChildContext->clientSocket = newClient;
				hChildThread = (HANDLE)_beginthreadex(NULL, 0, I4C3DAcceptedThreadProc, pChildContext, 0, &uThreadID);
				if (hChildThread == INVALID_HANDLE_VALUE) {
					_stprintf_s(szError, sizeof(szError)/sizeof(szError), _T("[ERROR] Create child thread. : %d"), GetLastError());
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
	I4C3DMisc::LogDebugMessage(_T("--- out I4C3DReceiveThreadProc ---"));
	return TRUE;
}

unsigned __stdcall I4C3DAcceptedThreadProc(void* pParam)
{
	I4C3DMisc::LogDebugMessage(_T("--- in I4C3DAcceptedThreadProc ---"));

	I4C3DChildContext* pChildContext = (I4C3DChildContext*)pParam;

	TCHAR szError[I4C3D_BUFFER_SIZE];
	char recvBuffer[I4C3D_RECEIVE_LENGTH];
	SIZE_T totalRecvBytes = 0;
	int nBytes = 0;
	BOOL bBreak = FALSE;

	char szCommand[I4C3D_BUFFER_SIZE] = {0};
	POINT delta;

	DWORD dwResult = 0;
	WSAEVENT hEvent = NULL;
	WSAEVENT hEventArray[2];
	WSANETWORKEVENTS events = {0};

	hEvent = WSACreateEvent();
	WSAEventSelect(pChildContext->clientSocket, hEvent, FD_READ | FD_CLOSE);

	hEventArray[0] = hEvent;
	hEventArray[1] = pChildContext->pContext->hStopEvent;

	FillMemory(recvBuffer, sizeof(recvBuffer), 0xFF);

	while (!bBreak) {
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
				_stprintf_s(szError, sizeof(szError)/sizeof(szError), _T("[ERROR] WSAEnumNetworkEvents() : %d"), WSAGetLastError());
				I4C3DMisc::ReportError(szError);
				break;
			}

			if (events.lNetworkEvents & FD_CLOSE) {
				break;
			} else if (events.lNetworkEvents & FD_READ) {
				nBytes = recv(pChildContext->clientSocket, recvBuffer, sizeof(recvBuffer) - totalRecvBytes, 0);

				if (nBytes == SOCKET_ERROR) {
					_stprintf_s(szError, sizeof(szError)/sizeof(szError), _T("[ERROR] recv : %d"), WSAGetLastError());
					I4C3DMisc::ReportError(szError);
					break;

				} else if (nBytes > 0) {
					LPCSTR pTermination = (LPCSTR)memchr(recvBuffer+totalRecvBytes, pChildContext->cTermination, nBytes);
					totalRecvBytes += nBytes;

					// 終端文字が見つからない場合、バッファをクリア
					if (pTermination == NULL) {
						if (totalRecvBytes >= sizeof(recvBuffer)) {
							FillMemory(recvBuffer, sizeof(recvBuffer), 0xFF);
							totalRecvBytes = 0;
						}
						continue;
					}

					do {
						// 電文解析
						delta.x = delta.y = 0;
						if (AnalyzeMessage(recvBuffer, szCommand, sizeof(szCommand), &delta) != 3) {
							I4C3DMisc::ReportError(_T("[ERROR] 電文解析に失敗しています。"));
							break;

						} else {
							// コマンド送信のためSendMessage（deltaX, deltaY, szCommandが解放される前に処理を行う）
							{
								TCHAR szError[I4C3D_BUFFER_SIZE];
								_stprintf_s(szError, sizeof(szError)/sizeof(szError[0]), _T("Core: deltaX:%d deltaY:%d"), delta.x, delta.y);
								I4C3DMisc::LogDebugMessage(szError);
							}
							SendMessage(pChildContext->pContext->hMyWnd, WM_BRIDGEMESSAGE, (WPARAM)&delta, (LPARAM)szCommand);
						}

						if (pTermination == recvBuffer + totalRecvBytes) {
							FillMemory(recvBuffer, sizeof(recvBuffer), 0xFF);
							totalRecvBytes = 0;

						} else if (pTermination < recvBuffer + totalRecvBytes) {
							int nCopySize = sizeof(recvBuffer)/sizeof(recvBuffer[0]) - (pTermination - recvBuffer + 1);
							totalRecvBytes -= (pTermination - recvBuffer);
							MoveMemory(recvBuffer, pTermination+1, nCopySize);
							FillMemory(recvBuffer + nCopySize, sizeof(recvBuffer)/sizeof(recvBuffer[0]) - nCopySize, 0xFF);

						} else {
							bBreak = TRUE;
							I4C3DMisc::ReportError(_T("[ERROR] 受信メッセージの解析に失敗しています。"));
							break;
						}
					} while ((pTermination = (LPCSTR)memchr(recvBuffer+totalRecvBytes, pChildContext->cTermination, nBytes)) != NULL);

				}
			}

			ResetEvent(hEvent);
		} else if (dwResult - WSA_WAIT_EVENT_0 == 1) {
			// pChildContext->pContext->hStopEvent に終了イベントがセットされた
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

	I4C3DMisc::LogDebugMessage(_T("--- out I4C3DAcceptedThreadProc ---"));

	return TRUE;
}

int AnalyzeMessage(LPCSTR lpszMessage, LPSTR lpszCommand, SIZE_T size, PPOINT pDelta)
{
	return sscanf_s(lpszMessage, "%s %d %d?", lpszCommand, size, &pDelta->x, &pDelta->y);
}