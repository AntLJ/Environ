// Environ.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Environ.h"

#define MAX_LOADSTRING 100
#define ID_LIST 1
#define ID_TEXT 2

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ENVIRON, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ENVIRON));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

void FillListBox(HWND hwndList)
{
	int iLength;
	TCHAR *pTemp, *pVarBlock, *pVarBeg, *pVarEnd, *pVarName;
	pTemp = GetEnvironmentStrings(); // Get pointer to environment block
	pVarBlock = pTemp;
	while (*pVarBlock)
	{
		if (*pVarBlock != '=') // Skip variable names beginning with '='
		{
			pVarBeg = pVarBlock; // Beginning of variable name
			while (*pVarBlock++ != '='); // Scan until '='
			pVarEnd = pVarBlock - 1; // Points to '=' sign
			iLength = pVarEnd - pVarBeg; // Length of variable name
			// Allocate memory for the variable name and terminating
			// zero. Copy the variable name and append a zero.
			pVarName = (TCHAR *)calloc(iLength + 1, sizeof(TCHAR));
			CopyMemory(pVarName, pVarBeg, iLength * sizeof(TCHAR));
			pVarName[iLength] = '\0';
			// Put the variable name in the list box and free memory.
			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)pVarName);
			free(pVarName);
		}
		while (*pVarBlock++ != '\0'); // Scan until terminating zero
	}
	FreeEnvironmentStrings(pTemp);
	pTemp = NULL;
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ENVIRON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ENVIRON);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndList, hwndText;
	int iIndex, iLength, cxChar, cyChar;
	TCHAR * pVarName, *pVarValue;

    switch (message)
    {
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());
		// Create listbox and static text windows.
		hwndList = CreateWindow(TEXT("listbox"), NULL,
			WS_CHILD | WS_VISIBLE | LBS_STANDARD,
			cxChar, cyChar * 3,
			cxChar * 16 + GetSystemMetrics(SM_CXVSCROLL),
			cyChar * 5,
			hwnd, (HMENU)ID_LIST,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);

		hwndText = CreateWindow(TEXT("static"), NULL,
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			cxChar, cyChar,
			GetSystemMetrics(SM_CXSCREEN), cyChar,
			hwnd, (HMENU)ID_TEXT,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);
		FillListBox(hwndList);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndList);
		return 0;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_LIST && HIWORD(wParam) == LBN_SELCHANGE)
		{
			// Get current selection.
			iIndex = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			iLength = SendMessage(hwndList, LB_GETTEXTLEN, iIndex, 0) + 1;
			pVarName = (TCHAR *)calloc(iLength, sizeof(TCHAR));
			SendMessage(hwndList, LB_GETTEXT, iIndex, (LPARAM)pVarName);
			// Get environment string.
			iLength = GetEnvironmentVariable(pVarName, NULL, 0);
			pVarValue = (TCHAR *)calloc(iLength, sizeof(TCHAR));
			GetEnvironmentVariable(pVarName, pVarValue, iLength);
			// Show it in window.

			SetWindowText(hwndText, pVarValue);
			free(pVarName);
			free(pVarValue);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
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
