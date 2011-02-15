#pragma once
#include <Windows.h>

// �R�}���h��ǉ�����ꍇ�́A�{�N���X���p�����Ă�������


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
	virtual ~I4C3DCommand(void);	// ����I4C3DCommand�N���X�͔h������邽�߁A���z�f�X�g���N�^�ɂ���

	virtual void Execute(void) = 0;

protected:
	HWND		m_hWnd;
	CommandSet	m_commandSet;
	INT			m_currentX;
	INT			m_currentY;
};

