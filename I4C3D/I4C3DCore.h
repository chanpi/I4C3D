#pragma once

// 
class I4C3DCore
{
public:
	I4C3DCore(void);
	~I4C3DCore(void);

	BOOL Start(void);
	void Stop(void);

private:
	BOOL	m_started;

	USHORT	m_bridgePort;
	UINT	m_cpuThreshold;
	UINT	m_cpuCoreTime;
};

