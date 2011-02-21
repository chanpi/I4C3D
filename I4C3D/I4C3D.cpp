// I4C3D.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "I4C3D.h"
#include "I4C3DMisc.h"
#include "I4C3DCore.h"
#include "I4C3DControl.h"
#include "I4C3DRTTControl.h"
#include "I4C3DMAYAControl.h"
#include "I4C3DAliasControl.h"

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
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

	// 二重起動防止
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
		I4C3DMisc::ReportError(TEXT("[ERROR] Winsock バージョン."));
		WSACleanup();
		misc.Cleanup();
		return EXIT_FAILURE;
	}

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_I4C3D, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		WSACleanup();
		misc.Cleanup();
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_I4C3D));

	// メイン メッセージ ループ:
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
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
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
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

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
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
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
			// メニュー項目　有効にする

			// コントローラを生成
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
		// WPARAM: HIWORD(wParam)	-> y座標  LOWORD(wParam) -> x座標
		// LPARAM: (LPCTSTR)lParam	-> コマンド名(tumble, dollyなど)
		if (controller == NULL) {
			I4C3DMisc::ReportError(_T("[ERROR] Controllerが生成されていません。"));
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
		// 選択されたメニューの解析:
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
		// TODO: 描画コードをここに追加してください...
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

// バージョン情報ボックスのメッセージ ハンドラーです。
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
