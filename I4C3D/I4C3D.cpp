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
#include "I4C3DShowcaseControl.h"

//#define	I4C3D_LOG_DEBUG

#define BUTTON_WIDTH		80
#define BUTTON_HEIGHT		32
#define ID_SELECT_BUTTON	501
#define ID_UPDATE_BUTTON	502

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名
TCHAR g_szIniFilePath[MAX_PATH] = {0};

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static BOOL BridgeMessage(I4C3DContext* pContext, int deltaX, int deltaY, LPCSTR lpszCommand);
static BOOL SelectTarget(I4C3DContext* pContext, I4C3DCore* pCore);
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

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_I4C3D, szWindowClass, MAX_LOADSTRING);

	// iniファイルの絶対パスを取得
	if (g_szIniFilePath[0] == _T('\0')) {
		I4C3DMisc::GetModuleFileWithExtension(g_szIniFilePath, sizeof(g_szIniFilePath)/sizeof(g_szIniFilePath[0]), _T("ini"));
	}

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

	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		WSACleanup();
		misc.Cleanup();
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_I4C3D));

	//{
	//	TCHAR szError[256];
	//	_stprintf_s(szError, 256, _T("%u %d %x"), _T('U'), _T('U'), _T('u'));
	//	I4C3DMisc::ReportError(szError);
	//}

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

	static I4C3DContext context = {0};
	static I4C3DCore core;

	static int currentPosX = 0;
	static int currentPosY = 0;

	switch (message)
	{
	case WM_CREATE:
		I4C3DMisc::LogDebugMessage(_T("--- start ---"));
		context.hMyWnd = hWnd;

		if (!core.Start(&context)) {
			I4C3DMisc::ReportError(_T("[ERROR] プログラムの初期化に失敗しました。\n終了します。"));
			DestroyWindow(hWnd);
		} else {
			// メニュー項目　有効にする

			// リストボックス表示
			CreateListBox(hWnd);
		}
		break;

	case WM_BRIDGEMESSAGE:
		// WPARAM: PPOINT
		// LPARAM: (LPCSTR)lParam	-> コマンド名(tumble, dollyなど)。TCHAR*ではなくchar*
		BridgeMessage(&context, ((PPOINT)wParam)->x, ((PPOINT)wParam)->y, (LPCSTR)lParam);
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

		case ID_SELECT_BUTTON:
			if (!SelectTarget(&context, &core)) {
				DestroyWindow(hWnd);
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
		// TODO: 描画コードをここに追加してください...
		EndPaint(hWnd, &ps);
		break;

	case WM_SIZE:
		CreateListBox(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_DESTROY:
		core.Stop(&context);
		if (context.pController != NULL) {
			delete context.pController;
			context.pController = NULL;
		}
		I4C3DMisc::LogDebugMessage(_T("--- end ---"));
		
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

//////////////////////////////////////////////////////////

BOOL BridgeMessage(I4C3DContext* pContext, int deltaX, int deltaY, LPCSTR lpszCommand)
{
	if (pContext->pController == NULL) {
		I4C3DMisc::ReportError(_T("[ERROR] ターゲットソフトを選択してください。"));
		return FALSE;
	}
	
	if (_strcmpi(lpszCommand, "tumble") == 0) {
		pContext->pController->TumbleExecute(deltaX, deltaY);

	} else if (_strcmpi(lpszCommand, "track") == 0) {
		pContext->pController->TrackExecute(deltaX, deltaY);

	} else if (_strcmpi(lpszCommand, "dolly") == 0) {
		pContext->pController->DollyExecute(deltaX, deltaY);

	} else {
#ifdef _UNICODE
		TCHAR wszCommand[I4C3D_BUFFER_SIZE];
		MultiByteToWideChar(CP_ACP, 0, lpszCommand, -1, wszCommand, sizeof(wszCommand)/sizeof(wszCommand[0]));
		pContext->pController->HotkeyExecute(wszCommand);
#else
		pContext->pController->HotkeyExecute(lpszCommand);
#endif

	}
	return TRUE;
}

BOOL SelectTarget(I4C3DContext* pContext, I4C3DCore* pCore)
{
	TCHAR szTargetTitle[MAX_PATH];
	int index = SendMessage(g_listBox, LB_GETCURSEL, 0, 0);
	if (index != LB_ERR) {
		SendMessage(g_listBox, LB_GETTEXT, index, (LPARAM)szTargetTitle);

		pContext->hTargetParentWnd = NULL;
		pContext->hTargetParentWnd = FindWindow(NULL, szTargetTitle);
		if (pContext->hTargetParentWnd != NULL) {
			// 取得成功

			// コントローラを生成
			if (pContext->pController != NULL) {
				delete pContext->pController;
				pContext->pController = NULL;
			}
			TCHAR szSoftwareName[MAX_PATH];
			pCore->GetTarget3DSoftwareName(szSoftwareName, sizeof(szSoftwareName)/sizeof(szSoftwareName[0]));
			if (lstrcmpi(szSoftwareName, _T("RTT")) == 0) {
				pContext->pController = new I4C3DRTTControl(pContext);

			} else if (lstrcmpi(szSoftwareName, _T("MAYA")) == 0) {
				pContext->pController = new I4C3DMAYAControl(pContext);

			} else if (lstrcmpi(szSoftwareName, _T("Alias")) == 0) {
				pContext->pController = new I4C3DAliasControl(pContext);

			} else if (lstrcmpi(szSoftwareName, _T("SHOWCASE")) == 0) {
				pContext->pController = new I4C3DShowcaseControl(pContext);

			} else {
				I4C3DMisc::ReportError(_T("[ERROR] ターゲットコントローラの作成に失敗しました。\n設定ファイルを確認してください。"));
				return FALSE;

			}

			return TRUE;
		}
	}
	I4C3DMisc::ReportError(_T("[ERROR] ターゲットソフトが取得できません。\n更新ボタンを押してもう一度選択してください。"));
	return TRUE;
}

void CreateListBox(HWND hMainWnd)
{
	RECT rect;
	static HWND hSelectButton = NULL;
	static HWND hUpdateButton = NULL;

	// リストボックス作成
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

	// 選択ボタン
	hSelectButton = CreateWindowEx(0, _T("BUTTON"), _T("Select"), WS_CHILD | WS_VISIBLE,
		(rect.right - rect.left)/2 - BUTTON_WIDTH - 2, 
		rect.bottom - BUTTON_HEIGHT - 10,
		BUTTON_WIDTH, BUTTON_HEIGHT, hMainWnd, (HMENU)ID_SELECT_BUTTON,
		hInst, NULL);

	// 更新ボタン
	hUpdateButton = CreateWindowEx(0, _T("BUTTON"), _T("Update"), WS_CHILD | WS_VISIBLE,
		(rect.right - rect.left)/2 + 2, 
		rect.bottom - BUTTON_HEIGHT - 10,
		BUTTON_WIDTH, BUTTON_HEIGHT, hMainWnd, (HMENU)ID_UPDATE_BUTTON,
		hInst, NULL);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM /*lParam*/)
{
	TCHAR szWndTitle[MAX_PATH] = {0};
	GetWindowText(hWnd, szWndTitle, MAX_PATH);
	if (!lstrcmp(szWndTitle, _T("スタート")) || !lstrcmpi(szWndTitle, _T("start")) || !lstrcmpi(szWndTitle, _T("Program Manager"))) {
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