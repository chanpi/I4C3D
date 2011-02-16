#include "StdAfx.h"
#include "I4C3DCommandPool.h"
#include "I4C3DTumbleCommand.h"
#include "I4C3DTrackCommand.h"
#include "I4C3DDollyCommand.h"
#include "I4C3DMisc.h"
#include <map>

static std::map<LPCTSTR, I4C3DCommand*> g_hCommandMap;

I4C3DCommandPool::I4C3DCommandPool(void)
{
	g_hCommandMap.clear();
}

I4C3DCommandPool::I4C3DCommandPool(LPCTSTR lpszTarget3DSoftware)
{
	g_hCommandMap.clear();
	g_hCommandMap[_T("tumble")] = new I4C3DTumbleCommand();
	g_hCommandMap[_T("track")] = new I4C3DTrackCommand();
	g_hCommandMap[_T("dolly")] = new I4C3DDollyCommand();
}

I4C3DCommandPool::~I4C3DCommandPool(void)
{
	std::map<LPCTSTR, I4C3DCommand*>::iterator iterator;
	iterator = g_hCommandMap.begin();
	while (iterator != g_hCommandMap.end()) {
		I4C3DCommand* command = (*iterator).second;
		delete command;
		{
			TCHAR szBuffer[MAX_PATH];
			_stprintf(szBuffer, _T("delete %s \n"), (*iterator).first);
			OutputDebugString(szBuffer);
		}
		iterator++;
	}
	g_hCommandMap.clear();
}
