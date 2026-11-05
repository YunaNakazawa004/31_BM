//========================================================================
// 
// マネージャーヘッダー [ manager.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _MANAGER_H_		// このマクロ定義がされていなかったら
#define _MANAGER_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define AUTOLOOP_ENABLE			// 自動ループ
#undef AUTOLOOP_ENABLE

#define ONE_GAME			(5)				// 1ゲームのフェーズ回数
#define MAX_RANKING			(5)				// ランキングの最大数

//************************************************************************
// 前方宣言
//************************************************************************
class CRenderer;
class CMTRenderer;
class CInputKeyboard;
class CInputMouse;
class CInputJoypad;
class CSound;
class CDebugProc;
class CCamera;
class CLight;
class CTexture;
class CXModel;
class CEffectBatching;
class CPlayer;
class CWind;
class CTcpClient;

//************************************************************************
// マネージャークラス
//************************************************************************
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CRenderer* GetRenderer(void) { return m_pRenderer; }
	static CMTRenderer* GetMTRenderer(void) { return m_pMTRenderer; }
	static CInputKeyboard* GetInputKeyboard(void) { return m_pInputKeyboard; }
	static CInputMouse* GetInputMouse(void) { return m_pInputMouse; }
	static CInputJoypad* GetInputJoypad(void) { return m_pInputJoypad; }
	static CSound* GetSound(void) { return m_pSound; }
	static CDebugProc* GetDebugProc(void) { return m_pDebugProc; }
	static CCamera* GetCamera(void) { return m_pCamera; }
	static CLight* GetLight(void) { return m_pLight; }
	static CTexture* GetTexture(void) { return m_pTexture; }
	static CXModel* GetXModel(void) { return m_pXModel; }
	static CEffectBatching* GetEffectBatching(void) { return m_pEffectBatching; }
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CWind* GetWind(void) { return m_pWind; }
	static CTcpClient* GetTcpClient(void) { return m_pTcpClient; }

	static int GetFPS(void) { return m_nCountFPS; }
	static void SetFPS(const int nCountFPS) { m_nCountFPS = nCountFPS; };
	static bool GetPause(void) { return m_bPause; }
	static void SetPause(const bool bPause) { m_bPause = bPause; }
	static void SetLimit(const D3DXVECTOR2 limit) { m_limit.x = limit.x; m_limit.y = limit.y; }
	static D3DXVECTOR2 GetLimit(void) { return m_limit; }

	template <class T> static HRESULT CreateInstance(T** instance);
	static void Reset(void);
	static void SetResetCall(void) { m_bResetCall = true; }
	static void Quit(void);

	static int SetRanking(int nScore);
	static int* GetRanking(HRESULT* hr);

private:
	static CRenderer* m_pRenderer;					// レンダラーのインスタンス
	static CMTRenderer* m_pMTRenderer;				// MTレンダラーのインスタンス
	static CInputKeyboard* m_pInputKeyboard;		// キーボード入力のインスタンス
	static CInputMouse* m_pInputMouse;				// マウス入力のインスタンス
	static CInputJoypad* m_pInputJoypad;			// ジョイパッド入力のインスタンス
	static CSound* m_pSound;						// サウンドのインスタンス
	static CDebugProc* m_pDebugProc;				// デバッグ表示のインスタンス
	static CCamera* m_pCamera;						// カメラのインスタンス
	static CLight* m_pLight;						// ライトのインスタンス
	static CTexture* m_pTexture;					// テクスチャのインスタンス
	static CXModel* m_pXModel;						// モデル管理のインスタンス
	static CEffectBatching* m_pEffectBatching;		// バッチ描画エフェクトのインスタンス
	static CPlayer* m_pPlayer;						// プレイヤーのインスタンス
	static CWind* m_pWind;							// 風のインスタンス
	static CTcpClient* m_pTcpClient;				// TCP送受信のインスタンス

	static int m_nCountFPS;			// FPSカウンター
	static bool m_bPause;			// ポーズするかしないか
	static bool m_bResetCall;		// リセットしたいかどうか
	static HWND m_hWnd;				// 保存用
	static D3DXVECTOR2 m_limit;		// 移動限界
};

//************************************************************************
// インスタンス生成のテンプレート関数
//************************************************************************
template <class T> HRESULT CManager::CreateInstance(T** instance)
{
	if (*instance == nullptr)
	{// NULLチェック
		*instance = new T;

		if (*instance == nullptr)
		{// NULLチェック
			OutputDebugStringA("! ! ! インスタンスの生成に失敗しました ! ! !\n");

			return E_FAIL;
		}

		return S_OK;
	}

	return E_FAIL;
}

#endif