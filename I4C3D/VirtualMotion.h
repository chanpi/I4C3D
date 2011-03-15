#ifndef _VIRTUAL_MOTION_
#define _VIRTUAL_MOTION_

#include <Windows.h>

typedef enum {
	LButtonDrag,
	MButtonDrag,
	RButtonDrag,
} VMDragButton;

typedef struct {
	HWND hTargetWnd;
	VMDragButton dragButton;
	UINT uKeyState;
	POINT dragStartPos;
	POINT dragEndPos;
} VMMouseMessage;


#ifdef __cplusplus
extern "C" {
#endif

	// ���z�L�[������
	// �A���t�@�x�b�g�L�[������/�����ꍇ�́AuVirtualKey�ɑ啶�����w�肵�Ă��������B
	__declspec(dllexport) UINT VMVirtualKeyDown(HWND hTargetWnd, UINT uVirtualKey);
	__declspec(dllexport) UINT VMVirtualKeyUp(HWND hTargetWnd, UINT uVirtualKey);

	// �����񂩂牼�z�L�[���擾����
	__declspec(dllexport) UINT VMGetVirtualKey(LPCTSTR szKey);

	// ���z�L�[�ȊO������
	// �A���t�@�x�b�g�L�[������/�����ꍇ�́AuKey�ɑ啶�����w�肵�Ă��������B
	__declspec(dllexport) void VMKeyDown(HWND hTargetWnd, UINT uKey);
	__declspec(dllexport) void VMKeyUp(HWND hTargetWnd, UINT uKey);
	
	// �}�E�X�h���b�O
	__declspec(dllexport) BOOL VMMouseDrag(const VMMouseMessage* mouseMessage);

#ifdef __cplusplus
}
#endif

#endif /* _VIRTUAL_MOTION_ */