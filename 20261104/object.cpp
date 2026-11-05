//========================================================================
// 
// オブジェクト [ object.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "object.h"

#include "renderer.h"
#include "MTrenderer.h"
#include "manager.h"
#include "camera.h"

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
#ifdef LIST
CObject* CObject::m_apTop[PRIORITY_MAX] = {};					// 先頭のオブジェクトへのポインタ
CObject* CObject::m_apCur[PRIORITY_MAX] = {};					// 最後尾のオブジェクトへのポインタ

#else
CObject* CObject::m_apObject[PRIORITY_MAX][MAX_OBJECT] = {};	// オブジェクトのインスタンス

#endif

int CObject::m_nNumAll = 0;				// オブジェクトの総数

//========================================================================
// オブジェクトクラスのコンストラクタ
//========================================================================
CObject::CObject()
{
	for (int nCntPri = 0; nCntPri < PRIORITY_MAX; nCntPri++)
	{
#ifdef LIST
		// 自分自身の前のオブジェクトに、最後尾のオブジェクトを入れる
		this->m_pPrev = m_apCur[nCntPri];

		if (m_apTop[nCntPri] == nullptr)
		{// 先頭がnullptrならそこに入れる
			m_apTop[nCntPri] = this;
		}

		if (m_apCur[nCntPri] != nullptr)
		{// 現在のオブジェクトがnullptrじゃないなら
			// 現在のオブジェクトの次のオブジェクトに自分自身を入れる
			m_apCur[nCntPri]->m_pNext = this;
		}

		// 自分自身を最後尾にする
		m_apCur[nCntPri] = this;

		m_nPriority = nCntPri;	// 優先順位を保存
		m_nNumAll++;			// オブジェクトの総数をカウントアップ
		m_bDeath = false;		// 死亡フラグを消しておく

#else
		for (int nCntObj = 0; nCntObj < MAX_OBJECT; nCntObj++)
		{
			if (m_apObject[nCntPri][nCntObj] == nullptr)
			{// NULLチェック/使っていない場合
				m_apObject[nCntPri][nCntObj] = this;		// 自分自身を代入
				m_nID = nCntObj;		// 自分自身のIDを保存
				m_nPriority = nCntPri;	// 優先順位を保存

				m_nNumAll++;		// オブジェクトの総数をカウントアップ

				break;
			}
		}

#endif
	}
}

//========================================================================
// オブジェクトクラスのコンストラクタ ( オーバーロード )
//========================================================================
CObject::CObject(int nPriority)
{
#ifdef LIST
	// 自分自身の前のオブジェクトに、最後尾のオブジェクトを入れる
	this->m_pPrev = m_apCur[nPriority];

	if (m_apTop[nPriority] == nullptr)
	{// 先頭がnullptrならそこに入れる
		m_apTop[nPriority] = this;
	}

	if (m_apCur[nPriority] != nullptr)
	{// 現在のオブジェクトがnullptrじゃないなら
		// 現在のオブジェクトの次のオブジェクトに自分自身を入れる
		m_apCur[nPriority]->m_pNext = this;
	}

	// 自分自身を最後尾にする
	m_apCur[nPriority] = this;

	m_nPriority = nPriority;	// 優先順位を保存
	m_nNumAll++;				// オブジェクトの総数をカウントアップ
	m_bDeath = false;		// 死亡フラグを消しておく

#else
	for (int nCntObj = 0; nCntObj < MAX_OBJECT; nCntObj++)
	{
		if (m_apObject[nPriority][nCntObj] == nullptr)
		{// NULLチェック/使っていない場合
			m_apObject[nPriority][nCntObj] = this;		// 自分自身を代入
			m_nID = nCntObj;			// 自分自身のIDを保存
			m_nPriority = nPriority;	// 優先順位を保存

			m_nNumAll++;		// オブジェクトの総数をカウントアップ

			break;
		}
	}

#endif
}

//========================================================================
// オブジェクトクラスのデストラクタ
//========================================================================
CObject::~CObject()
{
}

//========================================================================
// 全てのオブジェクトを解放
//========================================================================
void CObject::ReleaseAll(void)
{
	for (int nCntPri = 0; nCntPri < PRIORITY_MAX; nCntPri++)
	{
#ifdef LIST
		CObject* pObject = m_apTop[nCntPri];		// 先頭のオブジェクトを代入

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;			// 次のオブジェクトを保存

			// 終了処理
			pObject->Uninit();

			pObject = pObjectNext;			// 次のオブジェクトを代入
		}

#else
		for (int nCntObj = 0; nCntObj < MAX_OBJECT; nCntObj++)
		{
			if (m_apObject[nCntPri][nCntObj] != nullptr)
			{// NULLチェック
				// 終了処理
				m_apObject[nCntPri][nCntObj]->Uninit();
			}
		}

#endif
	}

#ifdef LIST
	for (int nCntPri = 0; nCntPri < PRIORITY_MAX; nCntPri++)
	{
		CObject* pObject = m_apTop[nCntPri];		// 先頭のオブジェクトを代入

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;			// 次のオブジェクトを保存

			if (pObject->m_bDeath == true)
			{// 死亡フラグが立っている
				CObject* pObjectPrev = pObject->m_pPrev;		// 前のオブジェクト
				CObject* pObjectNext = pObject->m_pNext;		// 次のオブジェクト
				int nPriority = pObject->m_nPriority;		// 優先順位を保存

				if (m_apTop[nPriority] != pObject && m_apCur[nPriority] != pObject)
				{// 先頭でも最後尾でもない
					if (pObjectPrev != nullptr && pObjectNext != nullptr)
					{// NULLチェック
						// 前のオブジェクトの次のオブジェクトに、自分の次のオブジェクトを代入
						pObjectPrev->m_pNext = pObjectNext;

						// 次のオブジェクトの前のオブジェクトに、自分の前のオブジェクトを代入
						pObjectNext->m_pPrev = pObjectPrev;
					}
				}

				if (m_apTop[nPriority] == pObject)
				{// 自分が先頭の場合
					if (pObjectNext != nullptr)
					{// NULLチェック
						// 次のオブジェクトの前のオブジェクトをnullptrにする
						pObjectNext->m_pPrev = nullptr;
					}

					// 先頭のオブジェクトを次のオブジェクトにする
					m_apTop[nPriority] = pObjectNext;
				}

				if (m_apCur[nPriority] == pObject)
				{// 自分が最後尾の場合
					if (pObjectPrev != nullptr)
					{// NULLチェック
						// 前のオブジェクトの次のオブジェクトをnullptrにする
						pObjectPrev->m_pNext = nullptr;
					}

					// 最後尾のオブジェクトを前のオブジェクトにする
					m_apCur[nPriority] = pObjectPrev;
				}

				// オブジェクトの破棄
				delete pObject;
				pObject = nullptr;

				m_nNumAll--;		// オブジェクトの総数を減らす
			}

			pObject = pObjectNext;			// 次のオブジェクトを代入
		}
	}
#endif
}

//========================================================================
// オブジェクト(自分自身)を解放
//========================================================================
void CObject::Release(void)
{
#ifdef LIST
	m_bDeath = true;		// 死亡フラグを立てる

#else
	if (m_apObject[m_nPriority][m_nID] != nullptr)
	{// NULLチェック
		int nID = m_nID;				// IDを保存
		int nPriority = m_nPriority;	// 優先順位を保存

		// オブジェクトの破棄
		delete m_apObject[nPriority][nID];
		m_apObject[nPriority][nID] = nullptr;

		m_nNumAll--;		// オブジェクトの総数を減らす
	}

#endif
}

//========================================================================
// 全てのオブジェクトを更新
//========================================================================
void CObject::UpdateAll(void)
{
	for (int nCntPri = 0; nCntPri < PRIORITY_MAX; nCntPri++)
	{
#ifdef LIST
		CObject* pObject = m_apTop[nCntPri];		// 先頭のオブジェクトを代入

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;			// 次のオブジェクトを保存

			// 更新処理
			pObject->Update();

			pObject = pObjectNext;			// 次のオブジェクトを代入
		}

#else
		for (int nCntObj = 0; nCntObj < MAX_OBJECT; nCntObj++)
		{
			if (m_apObject[nCntPri][nCntObj] != nullptr)
			{// NULLチェック
				// 更新処理
				m_apObject[nCntPri][nCntObj]->Update();
			}
		}

#endif
	}

#ifdef LIST
	for (int nCntPri = 0; nCntPri < PRIORITY_MAX; nCntPri++)
	{
		CObject* pObject = m_apTop[nCntPri];		// 先頭のオブジェクトを代入

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;			// 次のオブジェクトを保存

			if (pObject->m_bDeath == true)
			{// 死亡フラグが立っている
				CObject* pObjectPrev = pObject->m_pPrev;		// 前のオブジェクト
				CObject* pObjectNext = pObject->m_pNext;		// 次のオブジェクト
				int nPriority = pObject->m_nPriority;		// 優先順位を保存

				if (m_apTop[nPriority] != pObject && m_apCur[nPriority] != pObject)
				{// 先頭でも最後尾でもない
					if (pObjectPrev != nullptr && pObjectNext != nullptr)
					{// NULLチェック
						// 前のオブジェクトの次のオブジェクトに、自分の次のオブジェクトを代入
						pObjectPrev->m_pNext = pObjectNext;

						// 次のオブジェクトの前のオブジェクトに、自分の前のオブジェクトを代入
						pObjectNext->m_pPrev = pObjectPrev;
					}
				}

				if (m_apTop[nPriority] == pObject)
				{// 自分が先頭の場合
					if (pObjectNext != nullptr)
					{// NULLチェック
						// 次のオブジェクトの前のオブジェクトをnullptrにする
						pObjectNext->m_pPrev = nullptr;
					}

					// 先頭のオブジェクトを次のオブジェクトにする
					m_apTop[nPriority] = pObjectNext;
				}

				if (m_apCur[nPriority] == pObject)
				{// 自分が最後尾の場合
					if (pObjectPrev != nullptr)
					{// NULLチェック
						// 前のオブジェクトの次のオブジェクトをnullptrにする
						pObjectPrev->m_pNext = nullptr;
					}

					// 最後尾のオブジェクトを前のオブジェクトにする
					m_apCur[nPriority] = pObjectPrev;
				}

				// オブジェクトの破棄
				delete pObject;
				pObject = nullptr;

				m_nNumAll--;		// オブジェクトの総数を減らす
			}

			pObject = pObjectNext;			// 次のオブジェクトを代入
		}
	}
#endif
}

//========================================================================
// 全てのオブジェクトを描画
//========================================================================
void CObject::DrawAll(void)
{
	CCamera* pCamera = CManager::GetCamera();		// カメラの取得

	// カメラを設定
	pCamera->SetCamera();

	for (int nCntPri = 0; nCntPri < PRIORITY_MAX; nCntPri++)
	{
#ifdef LIST
		CObject* pObject = m_apTop[nCntPri];		// 先頭のオブジェクトを代入

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;			// 次のオブジェクトを保存

			// 描画処理
			pObject->Draw();

			pObject = pObjectNext;			// 次のオブジェクトを代入
		}

#else
		for (int nCntObj = 0; nCntObj < MAX_OBJECT; nCntObj++)
		{
			if (m_apObject[nCntPri][nCntObj] != nullptr)
			{// NULLチェック
				// 描画処理
				m_apObject[nCntPri][nCntObj]->Draw();
			}
		}

#endif
	}
}

//========================================================================
// UI以外のオブジェクトを描画
//========================================================================
void CObject::DrawWOUI(void)
{
	CCamera* pCamera = CManager::GetCamera();		// カメラの取得

	// カメラを設定
	pCamera->SetCamera();

	for (int nCntPri = 0; nCntPri < PRIORITY_UI; nCntPri++)
	{
#ifdef LIST
		CObject* pObject = m_apTop[nCntPri];		// 先頭のオブジェクトを代入

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;			// 次のオブジェクトを保存

			// 描画処理
			pObject->Draw();

			pObject = pObjectNext;			// 次のオブジェクトを代入
		}

#else
		for (int nCntObj = 0; nCntObj < MAX_OBJECT; nCntObj++)
		{
			if (m_apObject[nCntPri][nCntObj] != nullptr)
			{// NULLチェック
				// 描画処理
				m_apObject[nCntPri][nCntObj]->Draw();
			}
		}

#endif
	}
}

//========================================================================
// UIオブジェクトを描画
//========================================================================
void CObject::DrawUI(void)
{
	CCamera* pCamera = CManager::GetCamera();		// カメラの取得

	// カメラを設定
	pCamera->SetCamera();

#ifdef LIST
	CObject* pObject = m_apTop[PRIORITY_UI];		// 先頭のオブジェクトを代入

	while (pObject != nullptr)
	{
		CObject* pObjectNext = pObject->m_pNext;			// 次のオブジェクトを保存

		// 描画処理
		pObject->Draw();

		pObject = pObjectNext;			// 次のオブジェクトを代入
	}

#else
	for (int nCntObj = 0; nCntObj < MAX_OBJECT; nCntObj++)
	{
		if (m_apObject[nCntPri][nCntObj] != nullptr)
		{// NULLチェック
			// 描画処理
			m_apObject[nCntPri][nCntObj]->Draw();
		}
	}

#endif
}

//=============================================================================
// 角度を修正
//=============================================================================
void CObject::CorrectAngle(float* pAngle, float fAngle)
{
	// 向きを調整
	if (fAngle > D3DX_PI)
	{
		*pAngle -= D3DX_PI * 2.0f;
	}
	else if (fAngle < -D3DX_PI)
	{
		*pAngle += D3DX_PI * 2.0f;
	}
}