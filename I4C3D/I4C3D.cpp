// I4C3D.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "I4C3D.h"
#include "I4C3DMisc.h"
#include "I4C3DCore.h"
#include "I4C3DControl.h"
#include "I4C3DRTTControl.h"
#include "I4C3DMAYAControl.h"
#include "I4C3DAliasControl.h"

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	I4C3DMisc misc;
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2,2);
	int nResult;
	MSG msg;
	HACCEL hAccelTable;

	// ��d�N���h�~
	if (!misc.ExecuteOnce()) {
		misc.Cleanup();
		return EXIT_FAILURE;
	}

	nResult = WSAStartup(wVersion, &wsaData);
	if (nResult != 0) {
		I4C3DMisc::ReportError(_T("[ERROR] Initialize Winsock."));
		misc.Cleanup();
		return EXIT_FAILURE;
	}
	if (wsaData.wVersion != wVersion) {
		I4C3DMisc::ReportError(TEXT("[ERROR] Winsock �o�[�W����."));
		WSACleanup();
		misc.Cleanup();
		return EXIT_FAILURE;
	}

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_I4C3D, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance (hInstance, nCmdShow))
	{
		WSACleanup();
		misc.Cleanup();
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_I4C3D));

	// ���C�� ���b�Z�[�W ���[�v:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	WSACleanup();
	misc.Cleanup();

	return (int) msg.wParam;
}


//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
//  �R�����g:
//
//    ���̊֐�����юg�����́A'RegisterClassEx' �֐����ǉ����ꂽ
//    Windows 95 ���O�� Win32 �V�X�e���ƌ݊�������ꍇ�ɂ̂ݕK�v�ł��B
//    �A�v���P�[�V�������A�֘A�t����ꂽ
//    �������`���̏������A�C�R�����擾�ł���悤�ɂ���ɂ́A
//    ���̊֐����Ăяo���Ă��������B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_I4C3D));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_I4C3D);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HWND hWnd3DSoftware;

	static I4C3DContext context = {0};
	static I4C3DCore core;
	static I4C3DControl* controller = NULL;

	switch (message)
	{
	case WM_CREATE:
		if (!core.Start(&hWnd3DSoftware)) {
		} else {
			// ���j���[���ځ@�L���ɂ���

			// �R���g���[���𐶐�
			TCHAR szSoftwareName[MAX_PATH];
			core.GetTarget3DSoftwareName(szSoftwareName, sizeof(szSoftwareName)/sizeof(szSoftwareName[0]));
			if (lstrcmpi(szSoftwareName, _T("RTT")) == 0) {
				controller = new I4C3DRTTControl(hWnd3DSoftware);
			} else if (lstrcmpi(szSoftwareName, _T("MAYA")) == 0) {
				controller = new I4C3DMAYAControl(hWnd3DSoftware);
			} else if (lstrcmpi(szSoftwareName, _T("Alias")) == 0) {
				controller = new I4C3DAliasControl(hWnd3DSoftware);
			}
		}
		break;

	case WM_BRIDGEMESSAGE:
		// WPARAM: HIWORD(wParam)	-> y���W  LOWORD(wParam) -> x���W
		// LPARAM: (LPCTSTR)lParam	-> �R�}���h��(tumble, dolly�Ȃ�)
		if (controller == NULL) {
			I4C3DMisc::ReportError(_T("[ERROR] Controller����������Ă��܂���B"));
			break;
		}

		if (lstrcmpi((LPCTSTR)lParam, _T("tumble")) == 0) {
			controller->TumbleExecute(LOWORD(wParam), HIWORD(wParam));

		} else if (lstrcmpi((LPCTSTR)lParam, _T("track")) == 0) {
			controller->TrackExecute(LOWORD(wParam), HIWORD(wParam));

		} else if (lstrcmpi((LPCTSTR)lParam, _T("dolly")) == 0) {
			controller->DollyExecute(LOWORD(wParam), HIWORD(wParam));

		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �`��R�[�h�������ɒǉ����Ă�������...
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		core.Stop();
		if (controller != NULL) {
			delete controller;
			controller = NULL;
		}
		
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
