#pragma once

// 
class I4C3DCore
{
public:
	I4C3DCore(void);
	~I4C3DCore(void);

	BOOL Start(HWND* phWnd);
	void Stop(void);

	HWND GetTarget3DSoftwareWnd(void);

protected:
	BOOL GetTarget3DSoftwareName(LPTSTR lpszTargetName, SIZE_T size);

	BOOL	m_started;
	USHORT	m_bridgePort;
	UINT	m_cpuThreshold;
	UINT	m_cpuCoreTime;
};

