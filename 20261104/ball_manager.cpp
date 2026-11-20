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
#include "map_object.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define ERROR_NUM			(9999)				// 試行回数エラー
#define FIRST_DIST			(400.0f)			// はじめの距離

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
		CCamera* pCamera = CManager::GetCamera();
		CDebugProc* pDebug = CManager::GetDebugProc();
		D3DXVECTOR3 vecU = pCamera->GetVecU();

		// 風の移動
		m_pWindLeft->StickControll(CInputJoypad::JOYKEY_LEFTSTICK);
		m_pWindRight->StickControll(CInputJoypad::JOYKEY_RIGHTSTICK);

		// 距離/向きの計算
		D3DXVECTOR3 dist = m_pWindLeft->GetPosition() - m_pWindRight->GetPosition();
		float fDist = D3DXVec3Length(&dist);
		float rotLeft = m_pWindLeft->GetRotation().y;
		float rotRight = m_pWindRight->GetRotation().y;
		float rotSum = rotLeft + rotRight;
		vecU.x = rotSum * 0.05f;

		// カメラ設定
		pCamera->SetVecU(vecU);
		pCamera->SetViewAngle(45.0f + ((FIRST_DIST - fDist) * -0.01f));
		pCamera->SetAngle(0.5f + ((FIRST_DIST - fDist) * 0.0007f));

		for (int nCntBall = 0; nCntBall < m_nNumBall; nCntBall++)
		{
			if (m_vpBall.at(nCntBall)->GetState() == CBall::STATE_REFLECT)
			{// 反射状態ならスキップ
				continue;
			}

			// 全体の風の影響
			CManager::GetWind()->WindAffect(m_vpBall.at(nCntBall));

			// 左右の風の影響
			m_pWindLeft->WindAffect(m_vpBall.at(nCntBall));
			m_pWindRight->WindAffect(m_vpBall.at(nCntBall));
		}

		int nCntColl = 0;		// 当たり判定が完了した数
		int nTry = 0;			// 試行回数

		// 当たり判定
		do
		{
			nTry++;

			if (nTry > ERROR_NUM)
			{// 試行回数が多すぎる
				OutputDebugStringA("! ! ! 当たり判定が完了しませんでした ! ! !\n");

				break;
			}

			D3DXVECTOR3 pos = m_vpBall.at(nCntColl)->GetPosition();
			D3DXVECTOR3 posOld = m_vpBall.at(nCntColl)->GetPosOld();
			D3DXVECTOR3 rot = m_vpBall.at(nCntColl)->GetRotation();
			D3DXVECTOR3 rotDest = m_vpBall.at(nCntColl)->GetRotDest();
			D3DXVECTOR3 move = m_vpBall.at(nCntColl)->GetVelocity();
			D3DXVECTOR3 vecAxis = m_vpBall.at(nCntColl)->GetVecAxis();
			float fValueRot = m_vpBall.at(nCntColl)->GetValueRot();

			if (CManager::GetMapObject()->Collision(&pos, &posOld, &move, &rotDest, &vecAxis, &fValueRot, m_fRadius) == true)
			{// 配置オブジェクトとの当たり判定
				// 値を設定
				m_vpBall.at(nCntColl)->SetPosition(pos);
				m_vpBall.at(nCntColl)->SetMove(move);
				m_vpBall.at(nCntColl)->SetRotDest(rotDest);
				m_vpBall.at(nCntColl)->SetVecAxis(vecAxis);
				m_vpBall.at(nCntColl)->SetValueRot(fValueRot);

				// 状態を設定
				m_vpBall.at(nCntColl)->SetState(CBall::STATE_REFLECT);
			}

			CBall* pBall = m_vpBall.at(nCntColl)->Collision(m_vpBall.at(nCntColl)->GetPosition(), m_fRadius, m_vpBall.at(nCntColl));

			if (pBall != nullptr)
			{// 当たっていた
				// 反射する
				D3DXVECTOR3 posColl = pBall->GetPosition();
				D3DXVECTOR3 rotColl = pBall->GetRotation();

				// 境界線ベクトル（当たられた球の移動ベクトル）
				D3DXVECTOR3 vecLine = DEFAULT_VECTOR3;
				vecLine.x = (posColl.x + (sinf(rotColl.y + D3DX_PI) * m_fRadius)) - (posColl.x + (sinf(rotColl.y) * m_fRadius));
				vecLine.z = (posColl.z + (sinf(rotColl.y + D3DX_PI) * m_fRadius)) - (posColl.z + (sinf(rotColl.y) * m_fRadius));

				// 始点から現在位置へのベクトル
				D3DXVECTOR3 vecToPos = DEFAULT_VECTOR3;
				vecToPos.x = pos.x - (posColl.x + (sinf(rotColl.y) * m_fRadius));
				vecToPos.y = 0.0f;
				vecToPos.z = pos.z - (posColl.z + (sinf(rotColl.y) * m_fRadius));

				// 現在の外積
				float fPosLine = ((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z));

				// 移動平面に対する法線
				D3DXVECTOR3 vecNor = DEFAULT_VECTOR3;

				// 境界線ベクトルの右にいるか左にいるか
				if (fPosLine < 0.0f)
				{// 左にいる
					vecNor.x = sinf(rotColl.y + D3DX_PI * 0.5f);
					vecNor.z = cosf(rotColl.y + D3DX_PI * 0.5f);
				}
				else
				{// 右にいる
					vecNor.x = sinf(rotColl.y - D3DX_PI * 0.5f);
					vecNor.z = cosf(rotColl.y - D3DX_PI * 0.5f);
				}

				// 正規化
				D3DXVec3Normalize(&vecNor, &vecNor);

				// 移動ベクトル
				D3DXVECTOR3 vecMove = DEFAULT_VECTOR3;
				vecMove.x = (pos.x) - (posOld.x);
				vecMove.z = (pos.z) - (posOld.z);

				// 内積
				float fDot = (-vecMove.x * vecNor.x) + (-vecMove.z * vecNor.z);

				// 反射後の移動ベクトル
				D3DXVECTOR3 vecMoveRef = DEFAULT_VECTOR3;
				vecMoveRef.x = vecMove.x + ((vecNor.x * fDot) * 2.0f);
				vecMoveRef.z = vecMove.z + ((vecNor.z * fDot) * 2.0f);

				// 反射後の回転軸
				D3DXVECTOR3 vecAxis = D3DXVECTOR3(vecMoveRef.z * 1.5f, 0.0f, -(vecMoveRef.x * 1.5f));
				D3DXVec3Normalize(&vecAxis, &vecAxis);

				m_vpBall.at(nCntColl)->SetMove(D3DXVECTOR3(vecMoveRef.x * 1.5f, 0.0f, vecMoveRef.z * 1.5f));
				m_vpBall.at(nCntColl)->SetRotDest(D3DXVECTOR3(0.0f, atan2f(vecMoveRef.x, vecMoveRef.z) + D3DX_PI, 0.0f));
				m_vpBall.at(nCntColl)->SetVecAxis(vecAxis);

				D3DXVECTOR2 vecRef = D3DXVECTOR2(pos.x - pBall->GetPosition().x, pos.z - pBall->GetPosition().z);
				D3DXVec2Normalize(&vecRef, &vecRef);

				m_vpBall.at(nCntColl)->SetPosition(D3DXVECTOR3(posColl.x + ((m_fRadius * 2.0f) * vecRef.x), pos.y, posColl.z + ((m_fRadius * 2.0f) * vecRef.y * 1.1f)));

				// 状態を設定
				m_vpBall.at(nCntColl)->SetState(CBall::STATE_REFLECT);

				nCntColl = 0;		// 最初から当たり判定しなおす

				continue;
			}

			nCntColl++;		// ぶつかってないならカウントアップ

		} while (nCntColl < m_nNumBall);

		// 2つの風の中心座標
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
	D3DXVECTOR3 pos = DEFAULT_VECTOR3;	// 設置する位置
	float fLengthX = (fRadius * 2.0f);	// 設置できる最大距離（X方向）
	float fLengthZ = (fRadius * 2.0f);	// 設置できる最大距離（Z方向)
	int nLoopNum = 0;	// 試行回数

	do
	{// 必要数設置できるまで繰り返す
		// 位置を決める
		pos.x = center.x + ((float)(rand() % (int)(fLengthX ))) - fRadius;
		pos.y = center.y;
		pos.z = center.z + ((float)(rand() % (int)(fLengthZ ))) - fRadius;

		// 重なり確認
		if (CBall::Collision(pos, m_fRadius, nullptr) == nullptr)
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
		m_pWindLeft = CWind::Create(D3DXVECTOR3(center.x - 200.0f, center.y, center.z - 200.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 3.0f, 0.4f, true);
	}

	if (m_pWindRight == nullptr)
	{// NULLチェック
		m_pWindRight = CWind::Create(D3DXVECTOR3(center.x + 200.0f, center.y, center.z - 200.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 3.0f, 0.4f, true);
	}
}