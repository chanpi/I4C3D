#include "StdAfx.h"
#include "I4C3DCore.h"


I4C3DCore::I4C3DCore(void)
{
	m_started = FALSE;

	
}


I4C3DCore::~I4C3DCore(void)
{
}

BOOL I4C3DCore::Start(void) {
	if (m_started) {
		return TRUE;
	}

	// iPhone待ち受けスタート

	m_started = TRUE;
}

void I4C3DCore::Stop(void) {
	if (!m_started) {
		return;
	}

	m_started = FALSE;
}
