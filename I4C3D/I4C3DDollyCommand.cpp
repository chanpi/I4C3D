#include "StdAfx.h"
#include "I4C3DDollyCommand.h"

I4C3DDollyCommand::I4C3DDollyCommand(void)
{
	m_hWnd = NULL;
	ZeroMemory(&m_commandSet, sizeof(m_commandSet));
}

I4C3DDollyCommand::I4C3DDollyCommand(HWND hWnd, CommandSet commandSet)
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


I4C3DDollyCommand::~I4C3DDollyCommand(void)
{
}

void I4C3DDollyCommand::Execute(void)
{
	// システムキーDown
	if (m_commandSet.control) {
		PostMessage(m_hWnd, WM_SYSKEYDOWN, VK_CONTROL, 0);
	}
	if (m_commandSet.alt) {
		PostMessage(m_hWnd, WM_SYSKEYDOWN, VK_MENU, 0);
	}
	if (m_commandSet.shift) {
		PostMessage(m_hWnd, WM_SYSKEYDOWN, VK_SHIFT, 0);
	}

	// マウス
	//if (m_commandSet.keyDownMessage) {
	//	PostMessage(m_hWnd, m_commandSet.keyDownMessage, );
	//}

	// システムキーUp
	if (m_commandSet.shift) {
		PostMessage(m_hWnd, WM_SYSKEYUP, VK_SHIFT, 0);
	}
	if (m_commandSet.alt) {
		PostMessage(m_hWnd, WM_SYSKEYUP, VK_MENU, 0);
	}
	if (m_commandSet.control) {
		PostMessage(m_hWnd, WM_SYSKEYUP, VK_CONTROL, 0);
	}

}