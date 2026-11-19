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

		int nCntColl = 0;		// 当たり判定が完了した数

		// 当たり判定
		do
		{
			CBall* pBall = m_vpBall.at(nCntColl)->Collision(m_vpBall.at(nCntColl)->GetPosition(), m_fRadius, m_vpBall.at(nCntColl));

			if (pBall != nullptr)
			{// 当たっていた
				// 反射する
				D3DXVECTOR3 rotColl = pBall->GetRotation();
				D3DXVECTOR3 vecNor = D3DXVECTOR3(sinf(rotColl.y), 0.0f, cosf(rotColl.y));
				D3DXVec3Normalize(&vecNor, &vecNor);
				D3DXVECTOR3 posOwn = m_vpBall.at(nCntColl)->GetPosition();
				D3DXVECTOR2 vecRef = D3DXVECTOR2(posOwn.x - pBall->GetPosition().x, posOwn.z - pBall->GetPosition().z);
				D3DXVec2Normalize(&vecRef, &vecRef);

				m_vpBall.at(nCntColl)->SetPosition(D3DXVECTOR3(posOwn.x + (m_fRadius * vecRef.x), posOwn.y, posOwn.z + (m_fRadius * vecRef.y)));

				nCntColl = 0;		// 最初から当たり判定しなおす

				continue;
			}

			nCntColl++;		// ぶつかってないならカウントアップ

		} while (nCntColl < m_nNumBall);

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
	float fLengthX = (fRadius * 2.0f);	// 設置できる最大距離（X方向）
	float fLengthZ = (fRadius * 2.0f);	// 設置できる最大距離（Z方向)
	int nLoopNum = 0;	// 試行回数

	do
	{// 必要数設置できるまで繰り返す
		// 位置を決める
		pos.x = center.x + (rand() % (int)(fLengthX * 100.0f)) / 100.0f - fRadius;
		pos.y = center.y;
		pos.z = center.z + (rand() % (int)(fLengthZ * 100.0f)) / 100.0f - fRadius;

		// 重なり確認
		if (CBall::Collision(pos, fRadius, nullptr) == nullptr)
		{// ぶつかる距離に球がない
			// 生成
			m_vpBall.push_back(CBall::Create(pos, m_fRadius, m_fSpeed, m_fSpeedInertia));
			m_nNumBall++;
		}

		nLoopNum++;		// 試行回数をカウント

		if (nLoopNum > ERROR_NUM)
		{// 試行回数が多くなりすぎた
			OutputDebugStringA("! ! ! ランダム生成が完了しませんでした ! ! !\n");

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