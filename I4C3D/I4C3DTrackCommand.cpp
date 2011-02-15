#include "StdAfx.h"
#include "I4C3DTrackCommand.h"

I4C3DTrackCommand::I4C3DTrackCommand(void)
{
	m_hWnd = NULL;
	ZeroMemory(&m_commandSet, sizeof(m_commandSet));
}

I4C3DTrackCommand::I4C3DTrackCommand(HWND hWnd, CommandSet commandSet)
{
	m_hWnd = hWnd;
	m_commandSet.alt = commandSet.alt;
	m_commandSet.control = commandSet.control;
	m_commandSet.shift = commandSet.shift;
	m_commandSet.keyDownMessage = commandSet.keyDownMessage;
	m_commandSet.keyUpMessage = commandSet.keyUpMessage;
	m_commandSet.mouseMessage = commandSet.mouseMessage;
	m_commandSet.deltaX = commandSet.deltaX;
	m_commandSet.deltaY = commandSet.deltaY;
}


I4C3DTrackCommand::~I4C3DTrackCommand(void)
{
}

void I4C3DTrackCommand::Execute(void) {
}
