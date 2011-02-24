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

#define BUTTON_WIDTH		80
#define BUTTON_HEIGHT		32
#define ID_SELECT_BUTTON	501
#define ID_UPDATE_BUTTON	502

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��
TCHAR g_szFilePath[MAX_PATH] = {0};

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static void CreateListBox(HWND hMainWnd);
static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
static HWND g_listBox = NULL;

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
      CW_USEDEFAULT, 0, 300, 200, NULL, NULL, hInstance, NULL);

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
	RECT rect;

	static I4C3DContext context = {0};
	static I4C3DCore core;
	static I4C3DControl* controller = NULL;

	static int currentPosX = 0;
	static int currentPosY = 0;

	switch (message)
	{
	case WM_CREATE:
		context.hMyWnd = hWnd;

		if (!core.Start(&context)) {
			I4C3DMisc::ReportError(_T("[ERROR] �v���O�����̏������Ɏ��s���܂����B\n�I�����܂��B"));
			DestroyWindow(hWnd);
		} else {
			// ���j���[���ځ@�L���ɂ���

			// ���X�g�{�b�N�X�\��
			CreateListBox(hWnd);
		}
		break;

	case WM_BRIDGEMESSAGE:
		// WPARAM: HIWORD(wParam)	-> y���W  LOWORD(wParam) -> x���W
		// LPARAM: (LPCTSTR)lParam	-> �R�}���h��(tumble, dolly�Ȃ�)
		if (controller == NULL) {
			I4C3DMisc::ReportError(_T("[ERROR] �^�[�Q�b�g�\�t�g��I�����Ă��������B"));
			break;
		}

		GetWindowRect(context.hTargetParentWnd, &rect);
		SetWindowPos(context.hTargetParentWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetWindowPos(context.hTargetParentWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		SetFocus(context.hTargetParentWnd);
		SetForegroundWindow(context.hTargetParentWnd);
		//if (!currentPosX && !currentPosY) {
		//	currentPosX = 
		//}

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

		case ID_SELECT_BUTTON:
			{
				TCHAR szTargetTitle[MAX_PATH];
				int index = SendMessage(g_listBox, LB_GETCURSEL, 0, 0);
				if (index != LB_ERR) {
					SendMessage(g_listBox, LB_GETTEXT, index, (LPARAM)szTargetTitle);

					context.hTargetParentWnd = NULL;
					context.hTargetParentWnd = FindWindow(NULL, szTargetTitle);
					if (context.hTargetParentWnd != NULL) {
						// �擾����

						// �X�^�[�g���W���擾
						GetWindowRect(context.hTargetParentWnd, &rect);
						currentPosX = rect.left + (rect.right - rect.left)/2;
						currentPosY = rect.top + (rect.bottom - rect.top)/2;

						// �R���g���[���𐶐�
						TCHAR szSoftwareName[MAX_PATH];
						core.GetTarget3DSoftwareName(szSoftwareName, sizeof(szSoftwareName)/sizeof(szSoftwareName[0]));
						if (controller != NULL) {
							delete controller;
							controller = NULL;
						}
						if (lstrcmpi(szSoftwareName, _T("RTT")) == 0) {
							controller = new I4C3DRTTControl(&context);
						} else if (lstrcmpi(szSoftwareName, _T("MAYA")) == 0) {
							controller = new I4C3DMAYAControl(&context);
						} else if (lstrcmpi(szSoftwareName, _T("Alias")) == 0) {
							controller = new I4C3DAliasControl(&context);
						} else {
							I4C3DMisc::ReportError(_T("[ERROR] �^�[�Q�b�g�R���g���[���̍쐬�Ɏ��s���܂����B\n�I�����܂��B"));
							DestroyWindow(hWnd);
						}

						break;
					}
				}
				I4C3DMisc::ReportError(_T("[ERROR] �^�[�Q�b�g�\�t�g���擾�ł��܂���B\n�X�V�{�^���������Ă�����x�I�����Ă��������B"));
			}
			break;

		case ID_UPDATE_BUTTON:
			SendMessage(g_listBox, LB_RESETCONTENT, 0, 0);
			EnumWindows(EnumWindowsProc, NULL);
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

	case WM_SIZE:
		CreateListBox(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_DESTROY:
		core.Stop(&context);
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

//////////////////////////////////////////////////////////

void CreateListBox(HWND hMainWnd)
{
	RECT rect;
	static HWND hSelectButton = NULL;
	static HWND hUpdateButton = NULL;

	// ���X�g�{�b�N�X�쐬
	GetClientRect(hMainWnd, &rect);
	if (g_listBox != NULL) {
		DestroyWindow(g_listBox);
		DestroyWindow(hSelectButton);
		DestroyWindow(hUpdateButton);
	}
	g_listBox = CreateWindow(_T("LISTBOX"), NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
		0, 0, rect.right - rect.left, rect.bottom - rect.top - BUTTON_HEIGHT - 15,
		hMainWnd, (HMENU)1, hInst, NULL);

	if (g_listBox == NULL) {
		I4C3DMisc::ReportError(_T("[ERROR] CreateWindow : List Box"));
		DestroyWindow(hMainWnd);
		return;
	}
	SendMessage(g_listBox, LB_RESETCONTENT, 0, 0);
	EnumWindows(EnumWindowsProc, NULL);

	// �I���{�^��
	hSelectButton = CreateWindowEx(0, _T("BUTTON"), _T("Select"), WS_CHILD | WS_VISIBLE,
		(rect.right - rect.left)/2 - BUTTON_WIDTH - 2, 
		rect.bottom - BUTTON_HEIGHT - 10,
		BUTTON_WIDTH, BUTTON_HEIGHT, hMainWnd, (HMENU)ID_SELECT_BUTTON,
		hInst, NULL);

	// �X�V�{�^��
	hUpdateButton = CreateWindowEx(0, _T("BUTTON"), _T("Update"), WS_CHILD | WS_VISIBLE,
		(rect.right - rect.left)/2 + 2, 
		rect.bottom - BUTTON_HEIGHT - 10,
		BUTTON_WIDTH, BUTTON_HEIGHT, hMainWnd, (HMENU)ID_UPDATE_BUTTON,
		hInst, NULL);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szWndTitle[MAX_PATH] = {0};
	GetWindowText(hWnd, szWndTitle, MAX_PATH);
	if (!lstrcmp(szWndTitle, _T("�X�^�[�g")) || !lstrcmpi(szWndTitle, _T("start")) || !lstrcmpi(szWndTitle, _T("Program Manager"))) {
		return TRUE;
	}
	if (!IsWindowVisible(hWnd)) {
		return TRUE;
	}
	if (szWndTitle[0] == _T('\0')) {
		return TRUE;
	}
	SendMessage(g_listBox, LB_ADDSTRING, 0, (LPARAM)szWndTitle);
	return TRUE;
}