#pragma once

// iPhoneÇ©ÇÁìdï∂ÇéÛêMÇ∑ÇÈ
class I4C3DAccessor
{
public:
	I4C3DAccessor(void);
	~I4C3DAccessor(void);
	
	SOCKET InitializeSocket(LPCSTR szAddress, USHORT uPort, int nTimeoutMilisec, BOOL bSend);
	BOOL Send(LPCSTR lpszCommand);
	BOOL Recv(LPSTR lpszCommand, SIZE_T uSize);
};

