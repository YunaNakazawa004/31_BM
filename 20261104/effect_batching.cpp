//========================================================================
// 
// バッチ描画エフェクト [ effect_batching.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "effect_batching.h"

#include "renderer.h"
#include "MTrenderer.h"
#include "manager.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define MOVE				(5.0f)						// 移動量
#define ALPHA_MINUS			(0.1f)						// アルファ値の減衰
#define SIZE_MINUS			(0.1f)						// 半径の変更量
#define EFFECT_INERTIA		(0.017f)					// 慣性
#define GRAVITY				(-0.15f)					// 重力

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
int CEffectBatching::m_aIdxTexture[2] = {};				// テクスチャのインデックス

//========================================================================
// テクスチャの生成
//========================================================================
HRESULT CEffectBatching::Load(void)
{
	// ローカル変数宣言
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ

	// テクスチャの設定
	m_aIdxTexture[0] = pTexture->Register("data\\TEXTURE\\effect000.jpg");
	m_aIdxTexture[1] = pTexture->Register("data\\TEXTURE\\spray000.png");

	if (m_aIdxTexture[0] == -1 || m_aIdxTexture[1] == -1)
	{// テクスチャが設定できていない
		OutputDebugStringA("! ! ! テクスチャの設定に失敗しました ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}

//========================================================================
// テクスチャの破棄
//========================================================================
void CEffectBatching::Unload(void)
{
	// テクスチャのインデックスを削除
	memset(&m_aIdxTexture[0], -1, sizeof m_aIdxTexture);
}

//========================================================================
// バッチ描画エフェクトクラスの生成処理
//========================================================================
CEffectBatching* CEffectBatching::Create(const int nMax)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CEffectBatching* pEffectBatching = nullptr;

	if (pEffectBatching == nullptr)
	{// NULLチェック
		// バッチ描画エフェクトの生成
		pEffectBatching = new CEffectBatching;
	}

	if (pEffectBatching != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pEffectBatching->Init(nMax)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! バッチ描画エフェクトの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		return pEffectBatching;
	}

	OutputDebugStringA("! ! ! バッチ描画エフェクトの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// バッチ描画エフェクトクラスのコンストラクタ
//========================================================================
CEffectBatching::CEffectBatching()
{
	// バッチ描画エフェクトクラスの値をクリア
	m_pVtxBuff = nullptr;
	m_pIdxBuff = nullptr;
	m_pVtxBuffNormal = nullptr;
	m_pIdxBuffNormal = nullptr;
	m_pVtxBuffShadow = nullptr;
	m_pIdxBuffShadow = nullptr;
	m_nMax = 0;
	m_nNumUse = 0;
	m_nNumUseShadow = 0;
}

//========================================================================
// バッチ描画エフェクトクラスのデストラクタ
//========================================================================
CEffectBatching::~CEffectBatching()
{
}

//========================================================================
// バッチ描画エフェクトクラスの初期化処理
//========================================================================
HRESULT CEffectBatching::Init(const int nMax)
{
	// ローカル変数
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	VERTEX_3D* pVtx, * pVtxAdd;		// 頂点情報の設定
	DWORD* pIdx, * pIdxAdd;			// インデックス情報へのポインタ

	// バッチ描画エフェクトクラスの値を初期化
	m_nMax = nMax;
	m_nNumUse = 0;
	m_nNumUseShadow = 0;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nMax * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntEffect = 0; nCntEffect < m_nMax; nCntEffect++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// 法線の設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 頂点カラーの設定
		pVtx[0].col = COLOR_WHITE;
		pVtx[1].col = COLOR_WHITE;
		pVtx[2].col = COLOR_WHITE;
		pVtx[3].col = COLOR_WHITE;

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 通常描画の頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nMax * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffNormal,
		nullptr);

	// 通常描画の頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuffNormal->Lock(0, 0, (void**)&pVtxAdd, 0);

	for (int nCntEffect = 0; nCntEffect < m_nMax; nCntEffect++)
	{
		// 頂点座標の設定
		pVtxAdd[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtxAdd[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtxAdd[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtxAdd[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// 法線の設定
		pVtxAdd[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtxAdd[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtxAdd[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtxAdd[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 頂点カラーの設定
		pVtxAdd[0].col = COLOR_WHITE;
		pVtxAdd[1].col = COLOR_WHITE;
		pVtxAdd[2].col = COLOR_WHITE;
		pVtxAdd[3].col = COLOR_WHITE;

		// テクスチャ座標の設定
		pVtxAdd[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtxAdd[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtxAdd[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtxAdd[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtxAdd += 4;
	}

	// 通常描画の頂点バッファをアンロック
	m_pVtxBuffNormal->Unlock();

	// 影の頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nMax * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffShadow,
		nullptr);

	// 影の頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuffShadow->Lock(0, 0, (void**)&pVtxAdd, 0);

	for (int nCntEffect = 0; nCntEffect < m_nMax; nCntEffect++)
	{
		// 頂点座標の設定
		pVtxAdd[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtxAdd[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtxAdd[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtxAdd[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// 法線の設定
		pVtxAdd[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtxAdd[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtxAdd[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtxAdd[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 頂点カラーの設定
		pVtxAdd[0].col = COLOR_WHITE;
		pVtxAdd[1].col = COLOR_WHITE;
		pVtxAdd[2].col = COLOR_WHITE;
		pVtxAdd[3].col = COLOR_WHITE;

		// テクスチャ座標の設定
		pVtxAdd[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtxAdd[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtxAdd[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtxAdd[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtxAdd += 4;
	}

	// 影の頂点バッファをアンロック
	m_pVtxBuffShadow->Unlock();

	// インデックスバッファの設定
	pDevice->CreateIndexBuffer(sizeof(DWORD) * (m_nMax * (4 + 2)),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		nullptr);

	// インデックスバッファをロックし、頂点情報へのポインタを取得
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// 6つのインデックスを四角形に指定
	for (int nCntIdx = 0; nCntIdx < m_nMax * 4; nCntIdx += 4)
	{
		pIdx[0] = nCntIdx + 2;
		pIdx[1] = nCntIdx;
		pIdx[2] = nCntIdx + 1;
		pIdx[3] = nCntIdx + 2;
		pIdx[4] = nCntIdx + 1;
		pIdx[5] = nCntIdx + 3;

		pIdx += 6;
	}

	// インデックスバッファをアンロックする
	m_pIdxBuff->Unlock();

	// 通常描画のインデックスバッファの設定
	pDevice->CreateIndexBuffer(sizeof(DWORD) * (m_nMax * (4 + 2)),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_pIdxBuffNormal,
		nullptr);

	// 通常描画のインデックスバッファをロックし、頂点情報へのポインタを取得
	m_pIdxBuffNormal->Lock(0, 0, (void**)&pIdxAdd, 0);

	// 6つのインデックスを四角形に指定
	for (int nCntIdx = 0; nCntIdx < m_nMax * 4; nCntIdx += 4)
	{
		pIdxAdd[0] = nCntIdx + 2;
		pIdxAdd[1] = nCntIdx;
		pIdxAdd[2] = nCntIdx + 1;
		pIdxAdd[3] = nCntIdx + 2;
		pIdxAdd[4] = nCntIdx + 1;
		pIdxAdd[5] = nCntIdx + 3;

		pIdxAdd += 6;
	}

	// 通常描画のインデックスバッファをアンロックする
	m_pIdxBuffNormal->Unlock();

	// 影のインデックスバッファの設定
	pDevice->CreateIndexBuffer(sizeof(DWORD) * (m_nMax * (4 + 2)),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_pIdxBuffShadow,
		nullptr);

	// 影のインデックスバッファをロックし、頂点情報へのポインタを取得
	m_pIdxBuffShadow->Lock(0, 0, (void**)&pIdxAdd, 0);

	// 6つのインデックスを四角形に指定
	for (int nCntIdx = 0; nCntIdx < m_nMax * 4; nCntIdx += 4)
	{
		pIdxAdd[0] = nCntIdx + 2;
		pIdxAdd[1] = nCntIdx;
		pIdxAdd[2] = nCntIdx + 1;
		pIdxAdd[3] = nCntIdx + 2;
		pIdxAdd[4] = nCntIdx + 1;
		pIdxAdd[5] = nCntIdx + 3;

		pIdxAdd += 6;
	}

	// 影のインデックスバッファをアンロックする
	m_pIdxBuffShadow->Unlock();

	return S_OK;
}

//========================================================================
// バッチ描画エフェクトクラスの終了処理
//========================================================================
void CEffectBatching::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// インデックスバッファの破棄
	if (m_pIdxBuff != nullptr)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = nullptr;
	}

	// 通常描画の頂点バッファの破棄
	if (m_pVtxBuffNormal != nullptr)
	{
		m_pVtxBuffNormal->Release();
		m_pVtxBuffNormal = nullptr;
	}

	// 通常描画のインデックスバッファの破棄
	if (m_pIdxBuffNormal != nullptr)
	{
		m_pIdxBuffNormal->Release();
		m_pIdxBuffNormal = nullptr;
	}

	// 影の頂点バッファの破棄
	if (m_pVtxBuffShadow != nullptr)
	{
		m_pVtxBuffShadow->Release();
		m_pVtxBuffShadow = nullptr;
	}

	// 影のインデックスバッファの破棄
	if (m_pIdxBuffShadow != nullptr)
	{
		m_pIdxBuffShadow->Release();
		m_pIdxBuffShadow = nullptr;
	}
}

//========================================================================
// バッチ描画エフェクトクラスの更新処理
//========================================================================
void CEffectBatching::Update(void)
{
	for (int nCntEffect = 0; nCntEffect < m_nNumUse; nCntEffect++)
	{
		// 位置を更新
		m_vPos.at(nCntEffect).x += m_vMove.at(nCntEffect).x * m_vfSpeed.at(nCntEffect);
		m_vPos.at(nCntEffect).z += m_vMove.at(nCntEffect).z * m_vfSpeed.at(nCntEffect);

		if (m_vbHoming.at(nCntEffect) == true)
		{// ホーミングする場合
			D3DXVECTOR3 HomingPos;
			D3DXVECTOR3 dist;

			if (m_vHomingObj.at(nCntEffect) != nullptr)
			{// NULLチェック
				HomingPos = m_vHomingObj.at(nCntEffect)->GetPosition();
			}
			else
			{// ホーミング対象がいなかったら
				HomingPos = m_vHomingPos.at(nCntEffect);
			}

			m_vPos.at(nCntEffect) += (HomingPos - m_vPos.at(nCntEffect)) * m_vfSpeedHoming.at(nCntEffect);

			dist = m_vPos.at(nCntEffect) - HomingPos;

			if (D3DXVec3Length(&dist) < 1.0f)
			{// 着いたら消す
				// 解放処理
				SwapAndPop(nCntEffect);
				nCntEffect--;

				continue;
			}
		}

		if (m_vfGravity.at(nCntEffect) != 0.0f)
		{// 重力あり
			m_vPos.at(nCntEffect).y += m_vMove.at(nCntEffect).y;
			m_vMove.at(nCntEffect).y += m_vfGravity.at(nCntEffect);

			if (m_vPos.at(nCntEffect).y < 0.0f)
			{// 地面との当たり判定
				m_vMove.at(nCntEffect).y *= m_vfBounce.at(nCntEffect);
			}
		}
		else
		{// 重力無し
			m_vPos.at(nCntEffect).y += m_vMove.at(nCntEffect).y * m_vfSpeed.at(nCntEffect);
		}

		// 慣性
		m_vfSpeed.at(nCntEffect) += (0.0f - m_vfSpeed.at(nCntEffect)) * m_vfSpeedInertia.at(nCntEffect);

		// サイズを変更する
		m_vfRadius.at(nCntEffect) += m_vfAddRadius.at(nCntEffect);

		// 不透明度を減らす
		m_vCol.at(nCntEffect).a -= m_vfMinusAlpha.at(nCntEffect);

		// 寿命を減らす
		m_vnLife.at(nCntEffect)--;

		if (m_vfRadius.at(nCntEffect) < 0.0f || m_vCol.at(nCntEffect).a < 0.0f || m_vnLife.at(nCntEffect) < 0)
		{// バッチ描画エフェクトが消える場合
			// 解放処理
			SwapAndPop(nCntEffect);
			nCntEffect--;

			continue;
		}
	}
}

//========================================================================
// バッチ描画エフェクトクラスの描画処理
//========================================================================
void CEffectBatching::Draw(void)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();
	D3DXMATRIX mtxWorld;
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtx;					// 単位行列
	D3DXMATRIX mtxView;				// ビューマトリックス
	VERTEX_3D* pVtx;				// 頂点情報の設定
	int nNumEffect = 0;				// エフェクトの数

	// ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntEffect = 0; nCntEffect < m_nNumUse; nCntEffect++)
	{
		if (m_vType.at(nCntEffect) != TYPE_ADD)
		{// 違う種類
			continue;
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_vMtxWorld.at(nCntEffect));

		// カメラに対して正面に向ける
		D3DXMatrixInverse(&m_vMtxWorld.at(nCntEffect), nullptr, &mtxView);	// 逆行列を求める

		// 位置をリセット
		m_vMtxWorld.at(nCntEffect)._41 = 0.0f;
		m_vMtxWorld.at(nCntEffect)._42 = 0.0f;
		m_vMtxWorld.at(nCntEffect)._43 = 0.0f;

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, m_vPos.at(nCntEffect).x, m_vPos.at(nCntEffect).y, m_vPos.at(nCntEffect).z);
		D3DXMatrixMultiply(&m_vMtxWorld.at(nCntEffect), &m_vMtxWorld.at(nCntEffect), &mtxTrans);

		if (m_vbShadow.at(nCntEffect) == true)
		{// 影がある場合
			ShadowCalc(m_vMtxWorld.at(nCntEffect), m_vfRadius.at(nCntEffect));
		}

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-m_vfRadius.at(nCntEffect), m_vfRadius.at(nCntEffect), 0.0f);
		pVtx[1].pos = D3DXVECTOR3(m_vfRadius.at(nCntEffect), m_vfRadius.at(nCntEffect), 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_vfRadius.at(nCntEffect), -m_vfRadius.at(nCntEffect), 0.0f);
		pVtx[3].pos = D3DXVECTOR3(m_vfRadius.at(nCntEffect), -m_vfRadius.at(nCntEffect), 0.0f);

		// 頂点座標の設定
		pVtx[0].pos = *D3DXVec3TransformCoord(&pVtx[0].pos, &pVtx[0].pos, &m_vMtxWorld.at(nCntEffect));
		pVtx[1].pos = *D3DXVec3TransformCoord(&pVtx[1].pos, &pVtx[1].pos, &m_vMtxWorld.at(nCntEffect));
		pVtx[2].pos = *D3DXVec3TransformCoord(&pVtx[2].pos, &pVtx[2].pos, &m_vMtxWorld.at(nCntEffect));
		pVtx[3].pos = *D3DXVec3TransformCoord(&pVtx[3].pos, &pVtx[3].pos, &m_vMtxWorld.at(nCntEffect));

		// 頂点カラーの設定
		pVtx[0].col = m_vCol.at(nCntEffect);
		pVtx[1].col = m_vCol.at(nCntEffect);
		pVtx[2].col = m_vCol.at(nCntEffect);
		pVtx[3].col = m_vCol.at(nCntEffect);

		// ポインタを進める
		pVtx += 4;

		nNumEffect++;
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// ライティングオフ
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// レンダーステートを加算合成にする
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtx);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtx);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_aIdxTexture[TYPE_ADD]));

	// エフェクトの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,		// STRIPではなくLISTで指定
		0, 0, nNumEffect * 4,								// 頂点数
		0, nNumEffect * 2);									// 一枚のビルボードに三角形を2つ

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// レンダーステートを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// ライティングオン
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//========================================================================
// バッチ描画エフェクトクラスの描画処理（通常描画）
//========================================================================
void CEffectBatching::DrawNormal(void)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();
	D3DXMATRIX mtxWorld;
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtx;					// 単位行列
	D3DXMATRIX mtxView;				// ビューマトリックス
	VERTEX_3D* pVtx;				// 頂点情報の設定
	int nNumEffect = 0;				// エフェクトの数

	// ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntEffect = 0; nCntEffect < m_nNumUse; nCntEffect++)
	{
		if (m_vType.at(nCntEffect) != TYPE_NORMAL)
		{// 違う種類
			continue;
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_vMtxWorld.at(nCntEffect));

		// カメラに対して正面に向ける
		D3DXMatrixInverse(&m_vMtxWorld.at(nCntEffect), nullptr, &mtxView);	// 逆行列を求める

		// 位置をリセット
		m_vMtxWorld.at(nCntEffect)._41 = 0.0f;
		m_vMtxWorld.at(nCntEffect)._42 = 0.0f;
		m_vMtxWorld.at(nCntEffect)._43 = 0.0f;

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, m_vPos.at(nCntEffect).x, m_vPos.at(nCntEffect).y, m_vPos.at(nCntEffect).z);
		D3DXMatrixMultiply(&m_vMtxWorld.at(nCntEffect), &m_vMtxWorld.at(nCntEffect), &mtxTrans);

		if (m_vbShadow.at(nCntEffect) == true)
		{// 影がある場合
			ShadowCalc(m_vMtxWorld.at(nCntEffect), m_vfRadius.at(nCntEffect));
		}

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-m_vfRadius.at(nCntEffect), m_vfRadius.at(nCntEffect), 0.0f);
		pVtx[1].pos = D3DXVECTOR3(m_vfRadius.at(nCntEffect), m_vfRadius.at(nCntEffect), 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_vfRadius.at(nCntEffect), -m_vfRadius.at(nCntEffect), 0.0f);
		pVtx[3].pos = D3DXVECTOR3(m_vfRadius.at(nCntEffect), -m_vfRadius.at(nCntEffect), 0.0f);

		// 頂点座標の設定
		pVtx[0].pos = *D3DXVec3TransformCoord(&pVtx[0].pos, &pVtx[0].pos, &m_vMtxWorld.at(nCntEffect));
		pVtx[1].pos = *D3DXVec3TransformCoord(&pVtx[1].pos, &pVtx[1].pos, &m_vMtxWorld.at(nCntEffect));
		pVtx[2].pos = *D3DXVec3TransformCoord(&pVtx[2].pos, &pVtx[2].pos, &m_vMtxWorld.at(nCntEffect));
		pVtx[3].pos = *D3DXVec3TransformCoord(&pVtx[3].pos, &pVtx[3].pos, &m_vMtxWorld.at(nCntEffect));

		// 頂点カラーの設定
		pVtx[0].col = m_vCol.at(nCntEffect);
		pVtx[1].col = m_vCol.at(nCntEffect);
		pVtx[2].col = m_vCol.at(nCntEffect);
		pVtx[3].col = m_vCol.at(nCntEffect);

		// ポインタを進める
		pVtx += 4;

		nNumEffect++;
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// ライティングオフ
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtx);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtx);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_aIdxTexture[TYPE_NORMAL]));

	// エフェクトの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,		// STRIPではなくLISTで指定
		0, 0, nNumEffect * 4,								// 頂点数
		0, nNumEffect * 2);									// 一枚のビルボードに三角形を2つ

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// レンダーステートを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// ライティングオン
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//========================================================================
// バッチ描画エフェクトクラスの影描画処理
//========================================================================
void CEffectBatching::DrawShadow(void)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();
	float fBias = -0.0001f;			// 深度
	D3DXMATRIX mtx;					// 単位行列

	// Zファイティング防止
	pDevice->SetRenderState(D3DRS_DEPTHBIAS, *((DWORD*)&fBias));

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtx);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtx);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffShadow, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuffShadow);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, nullptr);

	// エフェクトの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,		// STRIPではなくLISTで指定
		0, 0, m_nNumUseShadow * 4,							// 頂点数
		0, m_nNumUseShadow * 2);							// 一枚のビルボードに三角形を2つ

	m_nNumUseShadow = 0;	// 影の使用数をリセット

	// Zファイティング防止を元に戻す
	pDevice->SetRenderState(D3DRS_DEPTHBIAS, 0);
}

//========================================================================
// 影のマトリックス位置を計算
//========================================================================
void CEffectBatching::ShadowCalc(D3DXMATRIX matrix, const float fRadius)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;				// ビューマトリックス
	VERTEX_3D* pVtx;				// 頂点情報の設定
	D3DXMATRIX mtxShadow;			// シャドウマトリックス
	D3DLIGHT9 light;				// ライト情報
	D3DXVECTOR4 posLight;			// ライトの位置
	D3DXPLANE plane;				// 平面情報
	D3DXVECTOR3 point = DEFAULT_VECTOR3;					// 平面情報
	D3DXVECTOR3 normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 平面の法線情報

	// ライトの位置を設定
	pDevice->GetLight(3, &light);
	posLight = D3DXVECTOR4(light.Position.x, light.Position.y, light.Position.z, 1.0f);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuffShadow->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報をずらす
	pVtx += m_nNumUseShadow * 4;

	// 平面情報を設定
	D3DXPlaneFromPointNormal(&plane, &point, &normal);

	// シャドウマトリックスの初期化
	D3DXMatrixIdentity(&mtxShadow);

	// シャドウマトリックスの作成
	D3DXMatrixShadow(&mtxShadow, &posLight, &plane);

	D3DXMatrixMultiply(&matrix, &matrix, &mtxShadow);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-fRadius, fRadius, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(fRadius, fRadius, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-fRadius, -fRadius, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(fRadius, -fRadius, 0.0f);

	// 頂点座標の設定
	pVtx[0].pos = *D3DXVec3TransformCoord(&pVtx[0].pos, &pVtx[0].pos, &matrix);
	pVtx[1].pos = *D3DXVec3TransformCoord(&pVtx[1].pos, &pVtx[1].pos, &matrix);
	pVtx[2].pos = *D3DXVec3TransformCoord(&pVtx[2].pos, &pVtx[2].pos, &matrix);
	pVtx[3].pos = *D3DXVec3TransformCoord(&pVtx[3].pos, &pVtx[3].pos, &matrix);

	// 頂点カラーの設定
	pVtx[0].col = COLOR_BLACK;
	pVtx[1].col = COLOR_BLACK;
	pVtx[2].col = COLOR_BLACK;
	pVtx[3].col = COLOR_BLACK;

	// 頂点バッファをアンロック
	m_pVtxBuffShadow->Unlock();

	m_nNumUseShadow++;		// 影の使用数を増やす
}

//========================================================================
// バッチ描画エフェクトを設定
//========================================================================
void CEffectBatching::Set(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const float fSpeed, const float fSpeedInertia, const int type,
	const float fGravity, const float fBounce, const int nLife, const float fRadius, const float fAddRadius, const float fMinusAlpha,
	const D3DXCOLOR col, const bool bShadow, const bool bHoming,
	CObject* HomingObj, D3DXVECTOR3 HomingPos, const float fSpeedHoming)
{
	if (m_nNumUse >= m_nMax)
	{// 最大数を超えるなら終了
		return;
	}

	// 各配列に追加
	m_vPos.push_back(pos);
	m_vMove.push_back(move);
	m_vfSpeed.push_back(fSpeed);
	m_vfSpeedInertia.push_back(fSpeedInertia);
	m_vType.push_back(type);
	m_vfGravity.push_back(fGravity);
	m_vfBounce.push_back(fBounce);
	m_vnLife.push_back(nLife);
	m_vfRadius.push_back(fRadius);
	m_vfAddRadius.push_back(fAddRadius);
	m_vfMinusAlpha.push_back(fMinusAlpha);
	m_vCol.push_back(col);
	m_vbShadow.push_back(bShadow);
	m_vbHoming.push_back(bHoming);
	m_vHomingObj.push_back(HomingObj);
	m_vHomingPos.push_back(HomingPos);
	m_vfSpeedHoming.push_back(fSpeedHoming);

	D3DXMATRIX mtx;		// 単位行列
	m_vMtxWorld.push_back(mtx);

	// 総数を増やす
	m_nNumUse++;
}

//=============================================================================
// 全消去
//=============================================================================
void CEffectBatching::Reset(void)
{
	// 全部クリアする
	m_vMtxWorld.clear();
	m_vPos.clear();
	m_vMove.clear();
	m_vfSpeed.clear();
	m_vfSpeedInertia.clear();
	m_vType.clear();
	m_vfGravity.clear();
	m_vfBounce.clear();
	m_vnLife.clear();
	m_vfRadius.clear();
	m_vfAddRadius.clear();
	m_vfMinusAlpha.clear();
	m_vCol.clear();
	m_vbShadow.clear();
	m_vbHoming.clear();
	m_vHomingObj.clear();
	m_vHomingPos.clear();
	m_vfSpeedHoming.clear();
	m_nNumUse = 0;
}

//=============================================================================
// 交換
//=============================================================================
void CEffectBatching::SwapAndPop(const int nIdx)
{
	if (nIdx == m_nNumUse - 1)
	{
		// 各要素の最後尾を消去
		m_vMtxWorld.pop_back();
		m_vPos.pop_back();
		m_vMove.pop_back();
		m_vfSpeed.pop_back();
		m_vfSpeedInertia.pop_back();
		m_vType.pop_back();
		m_vfGravity.pop_back();
		m_vfBounce.pop_back();
		m_vnLife.pop_back();
		m_vfRadius.pop_back();
		m_vfAddRadius.pop_back();
		m_vfMinusAlpha.pop_back();
		m_vCol.pop_back();
		m_vbShadow.pop_back();
		m_vbHoming.pop_back();
		m_vHomingObj.pop_back();
		m_vHomingPos.pop_back();
		m_vfSpeedHoming.pop_back();
	}
	else
	{
		// 各要素を最後尾から上書き&最後尾を消去
		// マトリックス
		m_vMtxWorld.at(nIdx) = m_vMtxWorld.back();
		m_vMtxWorld.pop_back();

		// 位置
		m_vPos.at(nIdx) = m_vPos.back();
		m_vPos.pop_back();

		// 移動量
		m_vMove.at(nIdx) = m_vMove.back();
		m_vMove.pop_back();

		// スピード
		m_vfSpeed.at(nIdx) = m_vfSpeed.back();
		m_vfSpeed.pop_back();

		// スピードの慣性
		m_vfSpeedInertia.at(nIdx) = m_vfSpeedInertia.back();
		m_vfSpeedInertia.pop_back();

		// 種類
		m_vType.at(nIdx) = m_vType.back();
		m_vType.pop_back();
		
		// 重力の有無
		m_vfGravity.at(nIdx) = m_vfGravity.back();
		m_vfGravity.pop_back();
		
		// 跳ね返り
		m_vfBounce.at(nIdx) = m_vfBounce.back();
		m_vfBounce.pop_back();

		// 寿命
		m_vnLife.at(nIdx) = m_vnLife.back();
		m_vnLife.pop_back();

		// 半径
		m_vfRadius.at(nIdx) = m_vfRadius.back();
		m_vfRadius.pop_back();

		// 半径の増加量
		m_vfAddRadius.at(nIdx) = m_vfAddRadius.back();
		m_vfAddRadius.pop_back();

		// 透明度の減少量
		m_vfMinusAlpha.at(nIdx) = m_vfMinusAlpha.back();
		m_vfMinusAlpha.pop_back();

		// 色
		m_vCol.at(nIdx) = m_vCol.back();
		m_vCol.pop_back();

		// 影の有無
		m_vbShadow.at(nIdx) = m_vbShadow.back();
		m_vbShadow.pop_back();

		// ホーミングの有無
		m_vbHoming.at(nIdx) = m_vbHoming.back();
		m_vbHoming.pop_back();

		// ホーミング対象
		m_vHomingObj.at(nIdx) = m_vHomingObj.back();
		m_vHomingObj.pop_back();

		// ホーミング対象の位置
		m_vHomingPos.at(nIdx) = m_vHomingPos.back();
		m_vHomingPos.pop_back();

		// ホーミングのスピード
		m_vfSpeedHoming.at(nIdx) = m_vfSpeedHoming.back();
		m_vfSpeedHoming.pop_back();
	}

	// 使用数を減らす
	m_nNumUse--;
}