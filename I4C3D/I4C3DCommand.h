#pragma once
#include <Windows.h>

// コマンドを追加する場合は、本クラスを継承してください


typedef struct {
	BYTE control;
	BYTE shift;
	BYTE alt;
	UINT keyDownMessage;
	UINT mouseMessage;
	UINT keyUpMessage;
	INT deltaX;
	INT deltaY;
} CommandSet;

class I4C3DCommand
{
public:
	I4C3DCommand(void);
	virtual ~I4C3DCommand(void);	// このI4C3DCommandクラスは派生されるため、仮想デストラクタにする

	virtual void Execute(void) = 0;

protected:
	HWND		m_hWnd;
	CommandSet	m_commandSet;
	INT			m_currentX;
	INT			m_currentY;
};

