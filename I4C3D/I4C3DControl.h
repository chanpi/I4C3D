#pragma once
class I4C3DControl
{
public:
	I4C3DControl(void);
	I4C3DControl(HWND hWnd);
	virtual ~I4C3DControl(void) = 0;

	virtual void TumbleExecute(int deltaX, int deltaY);
	virtual void TrackExecute(int deltaX, int deltaY);
	virtual void DollyExecute(int deltaX, int deltaY);

protected:
	HWND m_hWnd;
	int m_posX;
	int m_posY;
};
