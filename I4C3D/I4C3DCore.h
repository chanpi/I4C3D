#pragma once

//

#include "I4C3D.h"

class I4C3DCore
{
public:
	I4C3DCore(void);
	~I4C3DCore(void);

	BOOL Start(I4C3DContext* pContext);
	void Stop(I4C3DContext* pContext);

	//HWND GetTarget3DSoftwareWnd(void);
	BOOL GetTarget3DSoftwareName(LPTSTR lpszTargetName, SIZE_T size);

protected:

	BOOL	m_started;
	USHORT	m_bridgePort;
	UINT	m_cpuThreshold;
	UINT	m_cpuCoreTime;
};

