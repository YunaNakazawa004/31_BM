//========================================================================
// 
// C++版DirectX [ main.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "main.h"

#include "manager.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define CLASS_NAME			"WindowClass"			// ウィンドウクラスの名前
#define WINDOW_NAME			"バルーンディスコ [ F11 でウィンドウサイズを切り替え ]"		// ウィンドウの名前

//************************************************************************
// プロトタイプ宣言
//************************************************************************
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void FullScreenTogle(HWND hWnd);

//========================================================================
// メイン関数
//========================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR lpCmdLine, int nCmdShow)
{
	// ローカル変数宣言
	DWORD dwCurrentTime;		// 現在時刻
	DWORD dwExecLastTime;		// 最後に処理した時刻
	DWORD dwFrameCount;			// フレームカウント
	DWORD dwFPSLastTime;		// 最後にFPSを計測した時刻

	RECT rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };		// 画面サイズの構造体

	CManager* pManager = nullptr;

	// マネージャーの生成
	if (pManager == nullptr)
	{// NULLチェック
		pManager = new CManager;
	}

	if (pManager == nullptr)
	{// NULLチェック
		OutputDebugStringA("! ! ! マネージャーの生成に失敗しました ! ! !\n");

		return -1;
	}

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),					// WNDCLASSEXのメモリサイズ
		CS_CLASSDC,							// ウィンドウのスタイル
		WindowProc,							// ウィンドウプロシージャ
		0,									// 0にする(通常は使用しない)
		0,
		hInstance,							// インスタンスハンドル
		LoadIcon(nullptr, IDI_APPLICATION),	// タスクバーのアイコン
		LoadCursor(nullptr, IDC_ARROW),		// マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),			// クライアント領域の背景色
		nullptr,								// メニューバー
		CLASS_NAME,							// ウィンドウクラスの名前
		LoadIcon(nullptr, IDI_APPLICATION)		// ファイルのアイコン
	};

	HWND hWnd;			// ウィンドウハンドル(識別子)
	MSG msg;			// メッセージを格納する変数

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域を指定のサイズに調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウィンドウを生成
	hWnd = CreateWindowEx(0,
		CLASS_NAME,					// ウィンドウクラスの名前
		WINDOW_NAME,				// ウィンドウの名前
		WS_OVERLAPPEDWINDOW,		// ウィンドウスタイル
		CW_USEDEFAULT,				// ウィンドウの左上X座標
		CW_USEDEFAULT,				// ウィンドウの左上Y座標
		(rect.right - rect.left),	// ウィンドウの幅
		(rect.bottom - rect.top),	// ウィンドウの高さ
		nullptr,					// 親ウィンドウのハンドル
		nullptr,					// メニューハンドルまたは子ウィンドウID
		hInstance,					// インスタンスハンドル
		nullptr);					// ウィンドウ作成データ

#ifdef _DEBUG
	// 初期化処理
	if (FAILED(pManager->Init(hInstance, hWnd, TRUE)))
	{// 初期化処理が失敗した場合
		OutputDebugStringA("! ! ! マネージャーの初期化に失敗しました ! ! !\n");

		return -1;
	}

#else
	// 初期化処理
	if (FAILED(pManager->Init(hInstance, hWnd, TRUE)))
	{// 初期化処理が失敗した場合
		OutputDebugStringA("! ! ! マネージャーの初期化に失敗しました ! ! !\n");

		return -1;
	}

#endif

	// 分解能を設定
	timeBeginPeriod(1);
	dwCurrentTime = 0;					// 初期化する
	dwExecLastTime = timeGetTime();		// 現在時刻を取得(保存)

	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();

	// FPSを取得
	int nCountFPS = pManager->GetFPS();

	// ウィンドウの表示
	ShowWindow(hWnd, nCmdShow);		// ウィンドウの表示状態を設定
	UpdateWindow(hWnd);				// クライアント領域を更新

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
		{// Windowsの処理
			if (msg.message == WM_QUIT)
			{// WM_QUITメッセージを受け取ったらメッセージループを抜ける
				break;
			}

			else
			{// メッセージの設定
				TranslateMessage(&msg);		// 仮想キーメッセージを文字メッセージへ変換
				DispatchMessage(&msg);		// ウィンドウプロシージャへメッセージを送出
			}
		}

		else
		{// DirectXの処理
			dwCurrentTime = timeGetTime();			// 現在時刻を取得

			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{// 0.5秒経過
				// FPSを計測
				nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);
				pManager->SetFPS(nCountFPS);

				dwFPSLastTime = dwCurrentTime;			// FPSを測定した時刻を保存
				dwFrameCount = 0;						// フレームカウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{// 60分の1秒経過
				dwExecLastTime = dwCurrentTime;		// 処理開始の時刻を保存

				// 更新処理
				pManager->Update();

				// 描画処理
				pManager->Draw();

				dwFrameCount++;
			}
		}
	}

	// マネージャーの破棄
	if (pManager != nullptr)
	{// NULLチェック
		// 終了処理
		pManager->Uninit();

		delete pManager;
		pManager = nullptr;
	}

	// 分解能を戻す
	timeEndPeriod(1);

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	return (int)msg.wParam;
}

//========================================================================
// ウィンドウプロシージャ
//========================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID;		// 返り値を格納

	switch (uMsg)
	{
	case WM_DESTROY:		// ウィンドウ破棄のメッセージ
		// WM_QUITメッセージを送る
		PostQuitMessage(0);

		break;

	case WM_KEYDOWN:		// キー押下のメッセージ
		switch (wParam)
		{
		case VK_ESCAPE:		// [ ESC ]キーが押された
			nID = MessageBox(hWnd, "終了しますか？", "終了確認", MB_YESNO | MB_ICONQUESTION);

			if (nID == IDYES)
			{// 終了する場合
				// ウィンドウ破棄する(WM_DESTROYメッセージを送る)
				DestroyWindow(hWnd);
			}

			break;

		case VK_F11:		// [ F11 ]キーが押された
			// フルスクリーンを切り替え
			FullScreenTogle(hWnd);

			break;
		}

		break;

	case WM_CLOSE:			// 閉じるボタン押下のメッセージ
		nID = MessageBox(hWnd, "終了しますか？", "終了確認", MB_YESNO | MB_ICONQUESTION);

		if (nID == IDYES)
		{// 終了する場合
			// ウィンドウ破棄する(WM_DESTROYメッセージを送る)
			DestroyWindow(hWnd);
		}

		else
		{// 終了しない場合
			return 0;		// 0を返さないと終了してしまう
		}

		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);		// 既定の処理を返す
}

//========================================================================
// スクリーンモード切替
//========================================================================
void FullScreenTogle(HWND hWnd)
{
	// 現在のスクリーンモードを取得
	LONG style = GetWindowLong(hWnd, GWL_STYLE);

	if ((style & WS_CAPTION) || (style & WS_THICKFRAME))
	{// フルスクリーンではない
		// フルスクリーンにする
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
		SetWindowPos(hWnd, NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowWindow(hWnd, SW_SHOW);
	}
	else
	{// フルスクリーン
		// 元のサイズに戻す
		RECT rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };		// 画面サイズの構造体

		// クライアント領域を指定のサイズに調整
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowWindow(hWnd, SW_NORMAL);
	}
}