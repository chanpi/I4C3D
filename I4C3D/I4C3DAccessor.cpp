#include "StdAfx.h"
#include "I4C3DAccessor.h"
#include "I4C3DMisc.h"

I4C3DAccessor::I4C3DAccessor(void)
{
}


I4C3DAccessor::~I4C3DAccessor(void)
{
}

SOCKET I4C3DAccessor::InitializeSocket(LPCSTR szAddress, USHORT uPort, int nTimeoutMilisec, BOOL bSend) {
	SOCKET socketHandler;
	SOCKADDR_IN address;
	TCHAR szError[I4C3D_BUFFER_SIZE];
	int nResult = 0;

	socketHandler = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketHandler == INVALID_SOCKET) {
		_stprintf(szError, _T("[ERROR] socket() : %d", WSAGetLastError()));
		I4C3DMisc::ReportError(szError);
		return socketHandler;
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(uPort);

	// 受信タイムアウト指定
	setsockopt(socketHandler, SOL_SOCKET, SO_RCVTIMEO, (const char*)&nTimeoutMilisec, sizeof(nTimeoutMilisec));

	if (bSend) {
		address.sin_addr.S_un.S_addr = inet_addr(szAddress);

		nResult = connect(socketHandler, (const SOCKADDR*)&address, sizeof(address));
		if (nResult == SOCKET_ERROR) {
			_stprintf(szError, _T("[ERROR] connect() : %d"), WSAGetLastError());
			I4C3DMisc::ReportError(szError);
			closesocket(socketHandler);
			return INVALID_SOCKET;
		}
		
	} else {
		address.sin_addr.S_un.S_addr = INADDR_ANY;

		nResult = bind(socketHandler, (const SOCKADDR*)&address, sizeof(address));
		if (nResult == SOCKET_ERROR) {
			_stprintf(szError, _T("[ERROR] bind() : %d"), WSAGetLastError());
			I4C3DMisc::ReportError(szError);
			closesocket(socketHandler);
			return INVALID_SOCKET;
		}

		nResult = listen(socketHandler, 10);	// 1端末につき、10接続要求まで受け付ける。それ以外はWSAECONNREFUSEDエラー。
		if (nResult == SOCKET_ERROR) {
			if (nResult == WSAECONNREFUSED) {
				I4C3DMisc::ReportError(_T("[ERROR] listen() : WSAECONNREFUSED"));
			} else {
				_stprintf(szError, _T("[ERROR] listen() : %d"), WSAGetLastError());
				I4C3DMisc::ReportError(szError);
			}
			closesocket(socketHandler);
			return INVALID_SOCKET;
		}
	}

	return socketHandler;
}

BOOL I4C3DAccessor::Send(LPCSTR lpszCommand) {
	// ただいま未実装
	return TRUE;
}

BOOL I4C3DAccessor::Recv(LPSTR lpszCommand, SIZE_T uSize) {
	int nBytes = 0;

	return TRUE;
}
