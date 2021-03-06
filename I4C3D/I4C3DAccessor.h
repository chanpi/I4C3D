#pragma once

// iPhoneから電文を受信する
class I4C3DAccessor
{
public:
	I4C3DAccessor(void);
	~I4C3DAccessor(void);
	
	SOCKET InitializeSocket(LPCSTR szAddress, USHORT uPort, int nTimeoutMilisec, BOOL bSend);
	BOOL Send(LPCSTR lpszCommand);
	BOOL Recv(LPSTR lpszCommand, SIZE_T uSize);
};

