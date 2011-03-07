#ifndef _VIRTUAL_MOTION_
#define _VIRTUAL_MOTION_

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

	// 仮想キーを押す
	__declspec(dllexport) UINT VMVirtualKeyDown(HWND hTargetWnd, UINT uVirtualKey);
	__declspec(dllexport) UINT VMVirtualKeyUp(HWND hTargetWnd, UINT uVirtualKey);

	// 文字列から仮想キーを取得する
	__declspec(dllexport) UINT VMGetVirtualKey(LPCTSTR szKey);

	// 仮想キー以外を押す
	__declspec(dllexport) void VMKeyDown(HWND hTargetWnd, UINT uKey);
	__declspec(dllexport) void VMKeyUp(HWND hTargetWnd, UINT uKey);
	
	// マウスドラッグ
	__declspec(dllexport) BOOL VMMouseDrag(const VMMouseMessage* mouseMessage);

#ifdef __cplusplus
}
#endif

#endif /* _VIRTUAL_MOTION_ */