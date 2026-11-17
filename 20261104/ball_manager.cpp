//========================================================================
// 
// 球マネージャー [ ball_manager.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "ball_manager.h"

#include "renderer.h"
#include "MTrenderer.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "texture.h"
#include "XModel.h"
#include "debugproc.h"

#include "camera.h"
#include "ball.h"
#include "wind.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define ERROR_NUM			(9999)				// 試行回数エラー

//========================================================================
// 球マネージャークラスの生成処理
//========================================================================
CBallManager* CBallManager::Create(const float fRadius, const float fSpeed, const float fSpeedInertia)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CBallManager* pBallManager = nullptr;

	if (pBallManager == nullptr)
	{// NULLチェック
		// 球マネージャーの生成
		pBallManager = new CBallManager;
	}

	if (pBallManager != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pBallManager->Init(fRadius, fSpeed, fSpeedInertia)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! 球マネージャーの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// オブジェクトの種類を設定
		pBallManager->SetType(TYPE_BALLMANAGER);

		return pBallManager;
	}

	OutputDebugStringA("! ! ! 球マネージャーの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// 球マネージャークラスのコンストラクタ
//========================================================================
CBallManager::CBallManager(const int nPriority) :CObject(nPriority)
{
	// 球マネージャークラスの値をクリア
	m_vpBall.clear();
	m_nNumBall = 0;
	m_fRadius = 0.0f;
	m_fSpeed = 0.0f;
	m_fSpeedInertia = 0.0f;
	m_pos = DEFAULT_VECTOR3;
	m_pWindLeft = nullptr;
	m_pWindRight = nullptr;
}

//========================================================================
// 球マネージャークラスのデストラクタ
//========================================================================
CBallManager::~CBallManager()
{
}

//========================================================================
// 球マネージャークラスの初期化処理
//========================================================================
HRESULT CBallManager::Init(const float fRadius, const float fSpeed, const float fSpeedInertia)
{
	// 球マネージャークラスの値を初期化
	m_nNumBall = 0;
	m_pos = DEFAULT_VECTOR3;
	m_fRadius = fRadius;
	m_fSpeed = fSpeed;
	m_fSpeedInertia = fSpeedInertia;

	CManager::GetCamera()->SetFollow(this);

	return S_OK;
}

//========================================================================
// 球マネージャークラスの終了処理
//========================================================================
void CBallManager::Uninit(void)
{
	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// 球マネージャークラスの更新処理
//========================================================================
void CBallManager::Update(void)
{
	if (m_pWindLeft != nullptr && m_pWindRight != nullptr)
	{// NULLチェック
		// 風の移動
		m_pWindLeft->StickControll(CInputJoypad::JOYKEY_LEFTSTICK);
		m_pWindRight->StickControll(CInputJoypad::JOYKEY_RIGHTSTICK);

		for (int nCntBall = 0; nCntBall < m_nNumBall; nCntBall++)
		{
			// 全体の風の影響
			CManager::GetWind()->WindAffect(m_vpBall.at(nCntBall));

			// 左右の風の影響
			m_pWindLeft->WindAffect(m_vpBall.at(nCntBall));
			m_pWindRight->WindAffect(m_vpBall.at(nCntBall));
		}

		m_pos = (m_pWindLeft->GetPosition() + m_pWindRight->GetPosition()) * 0.5f;
	}
}

//========================================================================
// 球マネージャークラスの描画処理
//========================================================================
void CBallManager::Draw(void)
{
}

//========================================================================
// ランダム生成処理
//========================================================================
void CBallManager::Random(const int nValue, const D3DXVECTOR3 center, const float fRadius)
{
	if (nValue <= 0)
	{// 0以下なら抜ける
		return;
	}

	// ローカル変数
	D3DXVECTOR3 pos = DEFAULT_VECTOR3;		// 設置する位置
	D3DXVECTOR3 posOff = DEFAULT_VECTOR3;	// 設置する一個区画あたりの基準位置
	D3DXVECTOR2 block = DEFAULT_VECTOR2;	// 区画の分割数
	float fLengthX = (fRadius * 2.0f);	// 設置できる最大距離（X方向）
	float fLengthZ = (fRadius * 2.0f);	// 設置できる最大距離（Z方向)
	float fLengthAreaX = 0.0f;		// 一個区画あたりのX距離
	float fLengthAreaZ = 0.0f;		// 一個区画あたりのZ距離
	int nOneArea = 0;	// 一個区画あたりに設置する数
	int nLoopNum = 0;	// 試行回数

	// 区画の分割数を決める
	if (nValue >= 25)
	{// 最低数
		block.x = 5.0f;
		block.y = 5.0f;
	}
	else
	{// それより小さい
		block.x = sqrtf((float)nValue);
		block.y = sqrtf((float)nValue);
	}

	// 一個区画あたりの情報を決める
	nOneArea = (int)(((float)nValue + (block.x * block.y) - 1.0f) / (block.x * block.y));
	fLengthAreaX = fLengthX / block.x;
	fLengthAreaZ = fLengthZ / block.y;

	do
	{// 必要数設置できるまで繰り返す
		// 所属する区画を決める
		posOff.x = (((m_nNumBall / nOneArea) % (int)block.x) * fLengthAreaX) - fRadius;
		posOff.y = 0.0f;
		posOff.z = (((m_nNumBall / (nOneArea * (int)block.y)) % (int)block.y) * fLengthAreaZ) - fRadius;

		// 区画内での位置を決める
		pos.x = posOff.x + center.x + (rand() % (int)(fLengthAreaX * 100.0f)) / 100.0f;
		pos.y = posOff.y + center.y;
		pos.z = posOff.z + center.z + (rand() % (int)(fLengthAreaZ * 100.0f)) / 100.0f;

		// 重なり確認
		if (CBall::Collision(pos, fRadius, true) == nullptr)
		{// ぶつかる距離に風船がない
			// 生成
			m_vpBall.push_back(CBall::Create(pos, m_fRadius, m_fSpeed, m_fSpeedInertia));
			m_nNumBall++;
		}

		nLoopNum++;		// 試行回数をカウント

		if (nLoopNum > ERROR_NUM)
		{// 試行回数が多くなりすぎた
			OutputDebugStringA("! ! ! 風船のランダム生成が完了しませんでした ! ! !\n");

			break;
		}
	} while (m_nNumBall < nValue);

	// 風の生成
	if (m_pWindLeft == nullptr)
	{// NULLチェック
		m_pWindLeft = CWind::Create(D3DXVECTOR3(center.x - 200.0f, center.y, center.z), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 3.0f, 0.4f, true);
	}

	if (m_pWindRight == nullptr)
	{// NULLチェック
		m_pWindRight = CWind::Create(D3DXVECTOR3(center.x + 200.0f, center.y, center.z), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 3.0f, 0.4f, true);
	}
}