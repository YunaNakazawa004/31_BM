//========================================================================
// 
// モーション [ motion.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "motion.h"

#include "debugproc.h"
#include "manager.h"

#include "model.h"

//========================================================================
// モーションの生成
//========================================================================
CMotion* CMotion::Create(void)
{
	CMotion* pMotion = NULL;

	if (pMotion == NULL)
	{// NULLチェック
		// モーションの生成
		pMotion = new CMotion;
	}

	if (pMotion != NULL)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pMotion->Init()))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! モーションの初期化に失敗しました ! ! !\n");

			return NULL;
		}

		return pMotion;
	}

	OutputDebugStringA("! ! ! モーションの生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// モーションクラスのコンストラクタ
//========================================================================
CMotion::CMotion()
{
	// モーションクラスの値をクリア
	memset(&m_aInfo[0], NULL, sizeof m_aInfo);
	m_nNumAll = 0;
	m_ppModel = NULL;
	m_nNumModel = 0;

	m_nType = 0;
	m_bLoop = false;
	m_nNumKey = 0;
	m_nKey = 0;
	m_nCounter = 0;
	m_bFinish = false;

	m_bBlend = false;
	m_nTypeBlend = -1;
	m_bLoopBlend = false;
	m_nNumKeyBlend = 0;
	m_nKeyBlend = 0;
	m_nCounterBlend = 0;
	m_nFrameBlend = 0;
	m_nCounterBlend = 0;
}

//========================================================================
// モーションクラスのデストラクタ
//========================================================================
CMotion::~CMotion()
{
}

//========================================================================
// モーションクラスの初期化処理
//========================================================================
HRESULT CMotion::Init(void)
{
	// モーションクラスの値を初期化
	memset(&m_aInfo[0], NULL, sizeof m_aInfo);
	m_nNumAll = 0;
	m_ppModel = NULL;
	m_nNumModel = 0;

	m_nType = -1;
	m_bLoop = false;
	m_nNumKey = 0;
	m_nKey = 0;
	m_nCounter = 0;
	m_bFinish = false;

	m_bBlend = false;
	m_nTypeBlend = -1;
	m_bLoopBlend = false;
	m_nNumKeyBlend = 0;
	m_nKeyBlend = 0;
	m_nCounterBlend = 0;
	m_nFrameBlend = 0;
	m_nCounterBlend = 0;

	return S_OK;
}

//========================================================================
// モーションクラスの終了処理
//========================================================================
void CMotion::Uninit(void)
{
	m_ppModel = NULL;
}

//========================================================================
// モーション情報の設定
//========================================================================
void CMotion::SetInfo(const INFO info)
{
	// 情報を設定
	m_aInfo[m_nNumAll] = info;

	// モーションの総数をカウントアップ
	m_nNumAll++;
}

//========================================================================
// モデル情報の設定
//========================================================================
void CMotion::SetModel(CModel** ppModel, const int nNumModel)
{
	if (ppModel != NULL)
	{// NULLチェック
		// モデル情報を設定
		m_ppModel = ppModel;
		m_nNumModel = nNumModel;
	}
	else
	{// 渡されたポインタがNULL
		OutputDebugStringA("! ! ! モデルの情報が設定できませんでした ! ! !\n");
	}
}

//========================================================================
// モーションの設定処理
//========================================================================
void CMotion::Set(const int nType, bool bBlend, int nFrameBlend)
{
	if (m_nType != nType)
	{// 違うモーションが設定されたときだけ
		if (bBlend == true)
		{// ブレンドあり
			m_nTypeBlend = nType;
			m_bLoopBlend = m_aInfo[m_nTypeBlend].bLoop;
			m_nNumKeyBlend = m_aInfo[m_nTypeBlend].nNumKey;
			m_nKeyBlend = 0;
			m_nCounterBlend = 0;
			m_bFinish = false;

			m_bBlend = bBlend;
			m_nFrameBlend = nFrameBlend;
			m_nCounterBlend = 0;
}
		else
		{// ブレンドなし
			// 各値の初期値を設定
			m_nType = nType;
			m_bLoop = m_aInfo[m_nType].bLoop;
			m_nNumKey = m_aInfo[m_nType].nNumKey;
			m_nKey = 0;
			m_nCounter = 0;
			m_bFinish = false;

			m_bBlend = bBlend;

			// 全モデル(パーツ)の初期設定
			for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
			{
				// ローカル変数
				D3DXVECTOR3 pos, rot;

				// 位置を保存
				pos.x = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fPosX;
				pos.y = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fPosY;
				pos.z = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fPosZ;

				// 向きを保存
				rot.x = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fRotX;
				rot.y = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fRotY;
				rot.z = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fRotZ;

				// パーツの位置/向きを設定
				m_ppModel[nCntModel]->SetPosition(pos + m_ppModel[nCntModel]->GetPosOff());
				m_ppModel[nCntModel]->SetRotation(rot + m_ppModel[nCntModel]->GetRotOff());
			}
		}
	}
}

//========================================================================
// モーションの更新処理
//========================================================================
void CMotion::Update(void)
{
	int nNextKey;
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得

	// 全モデル(パーツ)の更新
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		// ローカル変数宣言
		KEY* pKey;
		KEY* pKeyNext;
		CModel* pModel;
		D3DXVECTOR3 posCurrent, rotCurrent;
		D3DXVECTOR3 posDest, rotDest;
		float fDiffKey, fRateKey;

		// 1フレームあたりに動く割合
		fRateKey = ((float)m_nCounter / (float)m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame);

		// 次のキー
		if (m_bLoop == true)
		{// ループモーション
			nNextKey = (m_nKey + 1) % (m_aInfo[m_nType].nNumKey);
		}
		else
		{// ループしないモーション
			nNextKey = m_nKey + 1;

			if (nNextKey > m_aInfo[m_nType].nNumKey - 1)
			{// キーの総数を超えた
				nNextKey = m_aInfo[m_nType].nNumKey - 1;
			}
		}

		// 現在のキーのポインタ
		pKey = &m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel];

		// 次のキーのポインタ
		pKeyNext = &m_aInfo[m_nType].aKeyInfo[nNextKey].aKey[nCntModel];

		// モデルのポインタ
		pModel = m_ppModel[nCntModel];

		if (m_bBlend == true)
		{// ブレンドあり
			// ローカル変数宣言
			KEY* pKeyBlend;
			KEY* pKeyNextBlend;
			D3DXVECTOR3 posBlend, rotBlend;
			float fDiffKeyBlend, fDiffBlend, fRateKeyBlend, fRateBlend;
			int nNextKeyBlend;

			// 1フレームあたりに動く割合
			fRateKeyBlend = ((float)m_nCounterBlend / (float)m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame);

			// ブレンドの相対値
			fRateBlend = (float)m_nCounterFrameBlend / (float)m_nFrameBlend;

			// 次のキー
			if (m_bLoopBlend == true)
			{// ループモーション
				nNextKeyBlend = (m_nKeyBlend + 1) % (m_aInfo[m_nTypeBlend].nNumKey);
			}
			else
			{// ループしないモーション
				nNextKeyBlend = m_nKeyBlend + 1;

				if (nNextKeyBlend > m_aInfo[m_nTypeBlend].nNumKey - 1)
				{// キーの総数を超えた
					nNextKeyBlend = m_aInfo[m_nTypeBlend].nNumKey - 1;
				}
			}

			// 現在のキーのポインタ
			pKeyBlend = &m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].aKey[nCntModel];

			// 次のキーのポインタ
			pKeyNextBlend = &m_aInfo[m_nTypeBlend].aKeyInfo[nNextKeyBlend].aKey[nCntModel];

			// キー情報から位置・向きを算出
			// 位置X
			fDiffKey = pKeyNext->fPosX - pKey->fPosX;					// 現在のモーション
			posCurrent.x = pKey->fPosX + (fDiffKey * fRateKey);
			fDiffKeyBlend = pKeyNextBlend->fPosX - pKeyBlend->fPosX;	// ブレンドモーション
			posBlend.x = pKeyBlend->fPosX + (fDiffKeyBlend * fRateKeyBlend);
			fDiffBlend = posBlend.x - posCurrent.x;						// 差分
			posDest.x = posCurrent.x + (fDiffBlend * fRateBlend);		// 求める値

			// 位置Y
			fDiffKey = pKeyNext->fPosY - pKey->fPosY;					// 現在のモーション
			posCurrent.y = pKey->fPosY + (fDiffKey * fRateKey);
			fDiffKeyBlend = pKeyNextBlend->fPosY - pKeyBlend->fPosY;	// ブレンドモーション
			posBlend.y = pKeyBlend->fPosY + (fDiffKeyBlend * fRateKeyBlend);
			fDiffBlend = posBlend.y - posCurrent.y;						// 差分
			posDest.y = posCurrent.y + (fDiffBlend * fRateBlend);		// 求める値

			// 位置Z
			fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;					// 現在のモーション
			posCurrent.z = pKey->fPosZ + (fDiffKey * fRateKey);
			fDiffKeyBlend = pKeyNextBlend->fPosZ - pKeyBlend->fPosZ;	// ブレンドモーション
			posBlend.z = pKeyBlend->fPosZ + (fDiffKeyBlend * fRateKeyBlend);
			fDiffBlend = posBlend.z - posCurrent.z;						// 差分
			posDest.z = posCurrent.z + (fDiffBlend * fRateBlend);		// 求める値

			// 向きX
			fDiffKey = pKeyNext->fRotX - pKey->fRotX;					// 現在のモーション
			rotCurrent.x = pKey->fRotX + (fDiffKey * fRateKey);
			fDiffKeyBlend = pKeyNextBlend->fRotX - pKeyBlend->fRotX;	// ブレンドモーション
			rotBlend.x = pKeyBlend->fRotX + (fDiffKeyBlend * fRateKeyBlend);
			fDiffBlend = rotBlend.x - rotCurrent.x;						// 差分
			rotDest.x = rotCurrent.x + (fDiffBlend * fRateBlend);		// 求める値

			// 向きを調整
			CObject::CorrectAngle(&rotDest.x, rotDest.x);

			// 向きY
			fDiffKey = pKeyNext->fRotY - pKey->fRotY;					// 現在のモーション
			rotCurrent.y = pKey->fRotY + (fDiffKey * fRateKey);
			fDiffKeyBlend = pKeyNextBlend->fRotY - pKeyBlend->fRotY;	// ブレンドモーション
			rotBlend.y = pKeyBlend->fRotY + (fDiffKeyBlend * fRateKeyBlend);
			fDiffBlend = rotBlend.y - rotCurrent.y;						// 差分
			rotDest.y = rotCurrent.y + (fDiffBlend * fRateBlend);		// 求める値

			// 向きを調整
			CObject::CorrectAngle(&rotDest.y, rotDest.y);

			// 向きZ
			fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;					// 現在のモーション
			rotCurrent.z = pKey->fRotZ + (fDiffKey * fRateKey);
			fDiffKeyBlend = pKeyNextBlend->fRotZ - pKeyBlend->fRotZ;	// ブレンドモーション
			rotBlend.z = pKeyBlend->fRotZ + (fDiffKeyBlend * fRateKeyBlend);
			fDiffBlend = rotBlend.z - rotCurrent.z;						// 差分
			rotDest.z = rotCurrent.z + (fDiffBlend * fRateBlend);		// 求める値

			// 向きを調整
			CObject::CorrectAngle(&rotDest.z, rotDest.z);
}
		else
		{// ブレンドなし
			// キー情報から位置・向きを算出
			// 位置X
			fDiffKey = pKeyNext->fPosX - pKey->fPosX;
			posDest.x = pKey->fPosX + (fDiffKey * fRateKey);

			// 位置Y
			fDiffKey = pKeyNext->fPosY - pKey->fPosY;
			posDest.y = pKey->fPosY + (fDiffKey * fRateKey);

			// 位置Z
			fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;
			posDest.z = pKey->fPosZ + (fDiffKey * fRateKey);

			// 向きX
			fDiffKey = pKeyNext->fRotX - pKey->fRotX;
			rotDest.x = pKey->fRotX + (fDiffKey * fRateKey);

			// 向きを調整
			CObject::CorrectAngle(&rotDest.x, rotDest.x);

			// 向きY
			fDiffKey = pKeyNext->fRotY - pKey->fRotY;
			rotDest.y = pKey->fRotY + (fDiffKey * fRateKey);

			// 向きを調整
			CObject::CorrectAngle(&rotDest.y, rotDest.y);

			// 向きZ
			fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;
			rotDest.z = pKey->fRotZ + (fDiffKey * fRateKey);

			// 向きを調整
			CObject::CorrectAngle(&rotDest.z, rotDest.z);
		}

		// パーツの位置/向きを設定
		m_ppModel[nCntModel]->SetPosition(posDest + m_ppModel[nCntModel]->GetPosOff());
		m_ppModel[nCntModel]->SetRotation(rotDest + m_ppModel[nCntModel]->GetRotOff());
	}

	if (m_bBlend == true)
	{// ブレンドあり
		m_nCounter++;

		if (m_nCounter >= m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame)
		{// 再生フレーム数に達したら現在のキーを1つ進める
			m_nCounter = 0;	// カウンターを戻す

			// ループかどうか
			if (m_bLoop == true)
			{// ループモーション
				m_nKey = (m_nKey + 1) % (m_aInfo[m_nType].nNumKey);
			}
			else
			{// ループしないモーション
				m_nKey++;

				if (m_nKey >= m_aInfo[m_nType].nNumKey - 1)
				{// キーの総数を超えた
					m_nKey = m_aInfo[m_nType].nNumKey - 1;

					m_bFinish = true;
				}
			}
		}

		m_nCounterBlend++;

		if (m_nCounterBlend >= m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame)
		{// 再生フレーム数に達したら現在のキーを1つ進める
			m_nCounterBlend = 0;	// カウンターを戻す

			// ループかどうか
			if (m_bLoopBlend == true)
			{// ループモーション
				m_nKeyBlend = (m_nKeyBlend + 1) % (m_aInfo[m_nTypeBlend].nNumKey);
			}
			else
			{// ループしないモーション
				m_nKeyBlend++;

				if (m_nKeyBlend >= m_aInfo[m_nTypeBlend].nNumKey - 1)
				{// キーの総数を超えた
					m_nKeyBlend = m_aInfo[m_nTypeBlend].nNumKey - 1;

					m_bFinish = true;
				}
			}
		}

		m_nCounterFrameBlend++;

		if (m_nCounterFrameBlend >= m_nFrameBlend)
		{// ブレンドフレームに到達
			// ブレンドモーションを現在のモーションに設定
			m_nType = m_nTypeBlend;
			m_bLoop = m_aInfo[m_nTypeBlend].bLoop;
			m_nNumKey = m_aInfo[m_nTypeBlend].nNumKey;
			m_nKey = m_nKeyBlend;
			m_nCounter = m_nCounterBlend;

			m_nKeyBlend = 0;
			m_nCounterBlend = 0;
			m_bBlend = false;
			m_nCounterFrameBlend = 0;
			m_nFrameBlend = 0;
		}
	}
	else
	{// ブレンドなし
		m_nCounter++;

		if (m_nCounter >= m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame)
		{// 再生フレーム数に達したら現在のキーを1つ進める
			m_nCounter = 0;	// カウンターを戻す

			// ループかどうか
			if (m_bLoop == true)
			{// ループモーション
				m_nKey = (m_nKey + 1) % (m_aInfo[m_nType].nNumKey);
			}
			else
			{// ループしないモーション
				m_nKey++;

				if (m_nKey >= m_aInfo[m_nType].nNumKey - 1)
				{// キーの総数を超えた
					m_nKey = m_aInfo[m_nType].nNumKey - 1;

					m_bFinish = true;
				}
			}
		}
	}
}