#pragma once

#include <WinSock2.h>
#include <Windows.h>

// iPhoneÇ©ÇÁìdï∂ÇéÛêMÇ∑ÇÈ
class I4C3DAccessor
{
public:
	I4C3DAccessor(void);
	~I4C3DAccessor(void);
	
	SOCKET InitializeSocket(LPCTSTR szAddress, USHORT uPort, BOOL bSend);
	BOOL Send(LPCSTR lpszCommand);
	BOOL Recv(LPSTR lpszCommand, SIZE_T uSize);
};

