//========================================================================
// 
// マネージャー [ manager.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "manager.h"
#include "file.h"

#include "renderer.h"
#include "MTrenderer.h"
#include "input.h"
#include "sound.h"
#include "debugproc.h"
#include "texture.h"
#include "Xmodel.h"
#include "camera.h"
#include "light.h"
#include "object.h"
#include "object2D.h"
#include "object2D_Multi.h"
#include "object3D.h"
#include "object3D_Multi.h"
#include "objectX.h"
#include "objectBillboard.h"
#include "number.h"
#include "player.h"
#include "wind.h"
#include "meshwallM.h"
#include "wallMM.h"
#include "tcp_client.h"
#include "protocol_ranking.h"

#include "effect2D.h"
#include "effect3D.h"
#include "effect_batching.h"
#include "effectX.h"

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
CRenderer* CManager::m_pRenderer = nullptr;				// レンダラーのインスタンス
CMTRenderer* CManager::m_pMTRenderer = nullptr;			// MTレンダラーのインスタンス
CInputKeyboard* CManager::m_pInputKeyboard = nullptr;	// キーボード入力のインスタンス
CInputMouse* CManager::m_pInputMouse = nullptr;			// マウス入力のインスタンス
CInputJoypad* CManager::m_pInputJoypad = nullptr;		// ジョイパッド入力のインスタンス
CSound* CManager::m_pSound = nullptr;					// サウンドのインスタンス
CDebugProc* CManager::m_pDebugProc = nullptr;			// デバッグ表示のインスタンス
CCamera* CManager::m_pCamera = nullptr;					// カメラのインスタンス
CLight* CManager::m_pLight = nullptr;					// ライトのインスタンス
CTexture* CManager::m_pTexture = nullptr;				// テクスチャのインスタンス
CXModel* CManager::m_pXModel = nullptr;					// モデル管理のインスタンス
CEffectBatching* CManager::m_pEffectBatching = nullptr;	// バッチ描画エフェクトのインスタンス
CPlayer* CManager::m_pPlayer = nullptr;					// プレイヤーのインスタンス
CWind* CManager::m_pWind = nullptr;						// 風のインスタンス
CTcpClient* CManager::m_pTcpClient = nullptr;			// TCP送受信のインスタンス
int CManager::m_nCountFPS = 0;							// FPSカウンター
bool CManager::m_bPause = false;						// ポーズするかしないか
bool CManager::m_bResetCall = false;					// リセットしたいかどうか
HWND CManager::m_hWnd = nullptr;						// 保存用
D3DXVECTOR2 CManager::m_limit = DEFAULT_VECTOR2;		// 移動限界

//========================================================================
// マネージャークラスのコンストラクタ
//========================================================================
CManager::CManager()
{
	// マネージャークラスの値をクリア
	m_pRenderer = nullptr;
	m_pMTRenderer = nullptr;
	m_pInputKeyboard = nullptr;
	m_pInputMouse = nullptr;
	m_pInputJoypad = nullptr;
	m_pSound = nullptr;
	m_pDebugProc = nullptr;
	m_pCamera = nullptr;
	m_pLight = nullptr;
	m_pTexture = nullptr;
	m_pXModel = nullptr;
	m_pEffectBatching = nullptr;
	m_pPlayer = nullptr;
	m_pWind = nullptr;
	m_pTcpClient = nullptr;
	m_nCountFPS = 0;
	m_bPause = false;
	m_bResetCall = false;
	m_hWnd = nullptr;
	m_limit = DEFAULT_VECTOR2;
}

//========================================================================
// マネージャークラスのデストラクタ
//========================================================================
CManager::~CManager()
{
}

//========================================================================
// マネージャークラスの初期化処理
//========================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// レンダラーの生成
	if (SUCCEEDED(CreateInstance(&m_pRenderer)))
	{// レンダラーの生成に成功
		// 初期化処理
		if (FAILED(m_pRenderer->Init(hWnd, bWindow)))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! レンダラーの初期化に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// MTレンダラーの生成
	if (SUCCEEDED(CreateInstance(&m_pMTRenderer)))
	{// MTレンダラーの生成に成功
		// 初期化処理
		if (FAILED(m_pMTRenderer->Init()))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! MTレンダラーの初期化に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// キーボード入力の生成
	if (SUCCEEDED(CreateInstance(&m_pInputKeyboard)))
	{// キーボード入力の生成に成功
		// 初期化処理
		if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd)))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! キーボードの初期化に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// マウス入力の生成
	if (SUCCEEDED(CreateInstance(&m_pInputMouse)))
	{// マウス入力の生成に成功
		// 初期化処理
		if (FAILED(m_pInputMouse->Init(hInstance, hWnd)))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! マウスの初期化に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// ジョイパッド入力の生成
	if (SUCCEEDED(CreateInstance(&m_pInputJoypad)))
	{// ジョイパッド入力の生成に成功
		// 初期化処理
		if (FAILED(m_pInputJoypad->Init()))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! ジョイパッドの初期化に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// サウンドの生成
	if (SUCCEEDED(CreateInstance(&m_pSound)))
	{// サウンドの生成に成功
		// 初期化処理
		if (FAILED(m_pSound->Init(hWnd)))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! サウンドの初期化に失敗しました ! ! !\n");

			return E_FAIL;
		}

		if (m_pSound != nullptr)
		{// NULLチェック
			m_pSound->PlaySound(CSound::BGM_GAME);
		}
	}

	// カメラの生成
	if (SUCCEEDED(CreateInstance(&m_pCamera)))
	{// カメラの生成に成功
		// 初期化処理
		if (FAILED(m_pCamera->Init()))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! カメラの初期化に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// ライトの生成
	if (SUCCEEDED(CreateInstance(&m_pLight)))
	{// ライトの生成に成功
		// 初期化処理
		if (FAILED(m_pLight->Init()))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! ライトの初期化に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// デバッグ表示の生成
	if (SUCCEEDED(CreateInstance(&m_pDebugProc)))
	{// デバッグ表示の生成に成功
		// 初期化処理
		if (FAILED(m_pDebugProc->Init()))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! デバッグ表示の初期化に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// テクスチャの生成
	if (SUCCEEDED(CreateInstance(&m_pTexture)))
	{// テクスチャの生成に成功
		// 読み込み処理
		if (FAILED(m_pTexture->Load()))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! 規定テクスチャの読み込みに失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// モデル管理の生成
	if (SUCCEEDED(CreateInstance(&m_pXModel)))
	{// テクスチャの生成に成功
		// 読み込み処理
		if (FAILED(m_pXModel->Load()))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! 規定モデルの読み込みに失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// WinSockの初期化
	WSADATA wsaData;
	int nErr = WSAStartup(WINSOCK_VERSION, &wsaData);

	if (nErr != 0)
	{// 初期化に失敗した場合
		OutputDebugStringA("[Winsockの初期化に失敗しました]\n");

		return E_FAIL;
	}

	// オブジェクトのテクスチャを読み込み
	CEffect2D::Load();
	CEffect3D::Load();
	CEffectBatching::Load();
	CNumber::Load();

	// バッチ描画エフェクトの生成
	if (m_pEffectBatching == nullptr)
	{// NULLチェック
		m_pEffectBatching = CEffectBatching::Create(100000);

		if (m_pEffectBatching == nullptr)
		{// NULLチェック
			OutputDebugStringA("! ! ! バッチ描画エフェクトの生成に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// フィールドの生成
	CFile::SetObject("data\\object.txt", CObject::TYPE_FIELD);

	// 風の生成
	if (m_pWind == nullptr)
	{// NULLチェック
		m_pWind = dynamic_cast<CWind*>(CFile::SetObject("data\\object.txt", CObject::TYPE_WIND));

		if (m_pWind == nullptr)
		{// NULLチェック
			OutputDebugStringA("! ! ! 風の生成に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// プレイヤーの生成
	if (m_pPlayer == nullptr)
	{// NULLチェック
		m_pPlayer = dynamic_cast<CPlayer*>(CFile::SetObject("data\\object.txt", CObject::TYPE_PLAYER));

		if (m_pPlayer == nullptr)
		{// NULLチェック
			OutputDebugStringA("! ! ! プレイヤーの生成に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// FPSカウンタを初期化
	m_nCountFPS = 0;
	m_hWnd = hWnd;

	return S_OK;
}

//========================================================================
// マネージャークラスの終了処理
//========================================================================
void CManager::Uninit(void)
{
	// 全てのオブジェクトを破棄
	CObject::ReleaseAll();

	// オブジェクトのモデルを破棄
	CEffectX::Unload();
	CWind::Unload();
	CPlayer::Unload();

	// オブジェクトのテクスチャを破棄
	CWallMM::Unload();
	CNumber::Unload();
	CEffectBatching::Unload();
	CEffect3D::Unload();
	CEffect2D::Unload();

	// 風の破棄
	if (m_pWind != nullptr)
	{// NULLチェック
		m_pWind = nullptr;
	}
	
	// プレイヤーの破棄
	if (m_pPlayer != nullptr)
	{// NULLチェック
		m_pPlayer = nullptr;
	}

	// バッチ描画エフェクトの破棄
	if (m_pEffectBatching != nullptr)
	{// NULLチェック
		m_pEffectBatching = nullptr;
	}

	// Winsock終了処理
	WSACleanup();

	// モデル管理の破棄
	if (m_pXModel != nullptr)
	{// NULLチェック
		// 破棄
		m_pXModel->Unload();

		delete m_pXModel;
		m_pXModel = nullptr;
	}

	// テクスチャの破棄
	if (m_pTexture != nullptr)
	{// NULLチェック
		// 破棄
		m_pTexture->Unload();

		delete m_pTexture;
		m_pTexture = nullptr;
	}

	// デバッグ表示の破棄
	if (m_pDebugProc != nullptr)
	{// NULLチェック
		// 終了処理
		m_pDebugProc->Uninit();

		delete m_pDebugProc;
		m_pDebugProc = nullptr;
	}

	// ライトの破棄
	if (m_pLight != nullptr)
	{// NULLチェック
		// 終了処理
		m_pLight->Uninit();

		delete m_pLight;
		m_pLight = nullptr;
	}

	// カメラの破棄
	if (m_pCamera != nullptr)
	{// NULLチェック
		// 終了処理
		m_pCamera->Uninit();

		delete m_pCamera;
		m_pCamera = nullptr;
	}

	// サウンドの破棄
	if (m_pSound != nullptr)
	{// NULLチェック
		// サウンドを止める
		m_pSound->StopSound();

		// 終了処理
		m_pSound->Uninit();

		delete m_pSound;
		m_pSound = nullptr;
	}

	// ジョイパッド入力の破棄
	if (m_pInputJoypad != nullptr)
	{// NULLチェック
		// 終了処理
		m_pInputJoypad->Uninit();

		delete m_pInputJoypad;
		m_pInputJoypad = nullptr;
	}

	// マウス入力の破棄
	if (m_pInputMouse != nullptr)
	{// NULLチェック
		// 終了処理
		m_pInputMouse->Uninit();

		delete m_pInputMouse;
		m_pInputMouse = nullptr;
	}

	// キーボード入力の破棄
	if (m_pInputKeyboard != nullptr)
	{// NULLチェック
		// 終了処理
		m_pInputKeyboard->Uninit();

		delete m_pInputKeyboard;
		m_pInputKeyboard = nullptr;
	}

	// MTレンダラーの破棄
	if (m_pMTRenderer != nullptr)
	{// NULLチェック
		// 終了処理
		m_pMTRenderer->Uninit();

		delete m_pMTRenderer;
		m_pMTRenderer = nullptr;
	}

	// レンダラーの破棄
	if (m_pRenderer != nullptr)
	{// NULLチェック
		// 終了処理
		m_pRenderer->Uninit();

		delete m_pRenderer;
		m_pRenderer = nullptr;
	}
}

//========================================================================
// マネージャークラスの更新処理
//========================================================================
void CManager::Update(void)
{
	if (m_pDebugProc != nullptr)
	{// NULLチェック
		// FPS を表示
		m_pDebugProc->Print("FPS : %d\n", m_nCountFPS);

		// オブジェクトの総数を表示
		m_pDebugProc->Print("オブジェクトの総数 : %d\n", CObject::GetNumAll());

		m_pDebugProc->Print("リセット [ BackSpace / BACK ]\n");
	}

	if (m_bPause == false)
	{// ポーズしていないときだけ
		if (m_pRenderer != nullptr)
		{// NULLチェック
			// レンダラーの更新
			m_pRenderer->Update();
		}

		if (m_pEffectBatching != nullptr)
		{// NULLチェック
			// バッチ描画エフェクトの更新
			m_pEffectBatching->Update();
		}

		if (m_bResetCall == true)
		{// リセットコールが呼ばれていた
			// リセット
			Reset();

			m_bResetCall = false;
		}
	}

	if (m_pMTRenderer != nullptr)
	{// NULLチェック
		// MTレンダラーの更新
		m_pMTRenderer->Update();
	}

	if (m_pCamera != nullptr)
	{// NULLチェック
		// カメラの更新
		m_pCamera->Update();
	}

	if (m_pLight != nullptr)
	{// NULLチェック
		// ライトの更新
		m_pLight->Update();
	}

	if (m_pInputKeyboard != nullptr)
	{// NULLチェック
		// キーボード入力の更新
		m_pInputKeyboard->Update();

		if (m_pInputKeyboard->GetTrigger(DIK_P) == true)
		{// ポーズ切り替え
			m_bPause = m_bPause ? false : true;

			m_pSound->PlaySound(CSound::SE_PAUSE);
		}

		// マルチターゲットレンダリング
		if (m_pInputKeyboard->GetPress(DIK_NUMPAD3) == true)
		{
			CManager::GetMTRenderer()->SetMultiRendering(true);
		}
		else if (m_pInputKeyboard->GetRelease(DIK_NUMPAD3) == true)
		{
			CManager::GetMTRenderer()->SetMultiRendering(false);
		}

		if (m_pInputKeyboard->GetTrigger(DIK_F8) == true)
		{// ワイヤーフレーム切り替え
			if (m_pRenderer != nullptr)
			{// NULLチェック
				m_pRenderer->SwapWireFrame();
			}
		}

		if (m_pInputKeyboard->GetTrigger(DIK_BACKSPACE) == true)
		{// リセット
			Reset();
		}
	}

	if (m_pInputMouse != nullptr)
	{// NULLチェック
		// マウス入力の更新
		m_pInputMouse->Update();
	}

	if (m_pInputJoypad != nullptr)
	{// NULLチェック
		// ジョイパッド入力の更新
		m_pInputJoypad->Update();
	}

	if (m_pDebugProc != nullptr)
	{// NULLチェック
		// デバッグ表示の更新
		m_pDebugProc->Update();
	}
}

//========================================================================
// マネージャークラスの描画処理
//========================================================================
void CManager::Draw(void)
{
	if (m_pRenderer != nullptr)
	{// NULLチェック
		// レンダラーの描画
		m_pRenderer->Draw();
	}
}

//========================================================================
// リセット処理
//========================================================================
void CManager::Reset(void)
{
	// ゲームオブジェクトの破棄
	CObject::ReleaseAll();
	m_pEffectBatching->Reset();

	m_pPlayer = nullptr;
	m_pWind = nullptr;

	// ポーズを解除
	m_bPause = false;

	// フィールドの生成
	CFile::SetObject("data\\object.txt", CObject::TYPE_FIELD);

	// 風の生成
	if (m_pWind == nullptr)
	{// NULLチェック
		m_pWind = dynamic_cast<CWind*>(CFile::SetObject("data\\object.txt", CObject::TYPE_WIND));

		if (m_pWind == nullptr)
		{// NULLチェック
			OutputDebugStringA("! ! ! 風の生成に失敗しました ! ! !\n");
		}
	}

	// プレイヤーの生成
	if (m_pPlayer == nullptr)
	{// NULLチェック
		m_pPlayer = dynamic_cast<CPlayer*>(CFile::SetObject("data\\object.txt", CObject::TYPE_PLAYER));

		if (m_pPlayer == nullptr)
		{// NULLチェック
			OutputDebugStringA("! ! ! プレイヤーの生成に失敗しました ! ! !\n");
		}
	}
}

//========================================================================
// ウィンドウ破棄
//========================================================================
void CManager::Quit(void)
{
	DestroyWindow(m_hWnd);
}

//========================================================================
// ランキング設定
//========================================================================
int CManager::SetRanking(int nScore)
{
	// TCP送受信の生成
	if (SUCCEEDED(CreateInstance(&m_pTcpClient)))
	{// テクスチャの生成に成功
		// 読み込み処理
		if (FAILED(m_pTcpClient->Init((char*)"127.0.0.1", 22333)))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! TCP送受信の初期化に失敗しました ! ! !\n");
		}
	}

	unsigned char aSendData[256] = {};
	unsigned char aRecvData[256] = {};

	nScore = htonl(nScore);

	aSendData[0] = COMMAND_TYPE_SET_RANKING;
	memcpy(&aSendData[1], &nScore, sizeof nScore);

	if (FAILED(m_pTcpClient->Send(&aSendData[0], sizeof aSendData)))
	{// 失敗
		OutputDebugStringA("! ! ! ランキングの設定に失敗しました ! ! !\n");
	}

	if (FAILED(m_pTcpClient->Recv(&aRecvData[0], sizeof aRecvData)))
	{// 失敗
		OutputDebugStringA("! ! ! ランキングの順位取得に失敗しました ! ! !\n");
	}

	int Rank = *aRecvData;

	// TCP送受信の破棄
	if (m_pTcpClient != nullptr)
	{// NULLチェック
		// 終了処理
		m_pTcpClient->Uninit();

		delete m_pTcpClient;
		m_pTcpClient = nullptr;
	}

	return Rank;
}

//========================================================================
// ランキング取得
//========================================================================
int* CManager::GetRanking(HRESULT* hr)
{
	// TCP送受信の生成
	if (SUCCEEDED(CreateInstance(&m_pTcpClient)))
	{// テクスチャの生成に成功
		// 読み込み処理
		if (FAILED(m_pTcpClient->Init((char*)"127.0.0.1", 22333)))
		{// 初期化処理が失敗した場合
			OutputDebugStringA("! ! ! TCP送受信の初期化に失敗しました ! ! !\n");

			*hr = E_FAIL;
		}
	}

	unsigned char nData = {};
	static int aRecvData[MAX_RANKING + 1] = {};

	nData = COMMAND_TYPE_GET_RANKING;

	if (FAILED(m_pTcpClient->Send(&nData, sizeof nData)))
	{// 失敗
		OutputDebugStringA("! ! ! ランキングの取得に失敗しました ! ! !\n");

		*hr = E_FAIL;
	}

	if (FAILED(m_pTcpClient->Recv((unsigned char*)&aRecvData[0], sizeof aRecvData)))
	{// 失敗
		OutputDebugStringA("! ! ! ランキングの取得に失敗しました ! ! !\n");

		*hr = E_FAIL;
	}

	for (int nCount = 0; nCount < MAX_RANKING + 1; nCount++)
	{
		aRecvData[nCount] = ntohl(aRecvData[nCount]);
	}

	// TCP送受信の破棄
	if (m_pTcpClient != nullptr)
	{// NULLチェック
		// 終了処理
		m_pTcpClient->Uninit();

		delete m_pTcpClient;
		m_pTcpClient = nullptr;
	}

	return &aRecvData[0];
}