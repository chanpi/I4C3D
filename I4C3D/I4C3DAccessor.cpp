#include "StdAfx.h"
#include "I4C3DAccessor.h"


I4C3DAccessor::I4C3DAccessor(void)
{
}


I4C3DAccessor::~I4C3DAccessor(void)
{
}

SOCKET I4C3DAccessor::InitializeSocket(LPCTSTR szAddress, USHORT uPort, BOOL bSend) {
	SOCKET sock;
	return sock;
}

BOOL I4C3DAccessor::Send(LPCSTR lpszCommand) {
	return TRUE;
}

BOOL I4C3DAccessor::Recv(LPSTR lpszCommand, SIZE_T uSize) {
	return TRUE;
}
