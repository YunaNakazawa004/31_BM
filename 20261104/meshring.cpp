//=============================================================================
// 
// メッシュリング [ meshring.cpp ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#include "meshring.h"

#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "input.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//========================================================================
// メッシュリングクラスの生成処理
//========================================================================
CMeshRing* CMeshRing::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const D3DXVECTOR3 rot, const D3DXVECTOR2 block,
	const D3DXVECTOR2 size, const D3DXCOLOR col, const char* pFilename, const CObject::TYPE type, const int nPriority)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return NULL;
	}
#endif

	CMeshRing* pMeshRing = NULL;

	if (pMeshRing == NULL)
	{// NULLチェック
		// メッシュリングの生成
		pMeshRing = new CMeshRing(nPriority);
	}

	if (pMeshRing != NULL)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pMeshRing->Init(pos, move, rot, block, size, col)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! メッシュリングの初期化に失敗しました ! ! !\n");

			return NULL;
		}

		// テクスチャを設定
		CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
		pMeshRing->m_nIdxTexture = pTexture->Register(pFilename);

		// 種類を設定
		pMeshRing->SetType(type);

		return pMeshRing;
	}

	OutputDebugStringA("! ! ! メッシュリングの生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// メッシュリングクラスのコンストラクタ
//========================================================================
CMeshRing::CMeshRing(const int nPriority) :CMesh(nPriority)
{
	// メッシュリングクラスの値をクリア
	m_col = COLOR_WHITE;
	m_nNumVtx = 0;
	m_nNumIdx = 0;
	m_bDisp = true;
}

//========================================================================
// メッシュリングクラスのデストラクタ
//========================================================================
CMeshRing::~CMeshRing()
{
}

//=============================================================================
// メッシュリングの初期化処理
//=============================================================================
HRESULT CMeshRing::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const D3DXVECTOR3 rot,
	const D3DXVECTOR2 block, const D3DXVECTOR2 size, const D3DXCOLOR col)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	VERTEX_3D* pVtx;						// 頂点情報へのポインタ
	DWORD* pIdx;							// インデックス情報へのポインタ

	SetPosition(pos);
	SetRotation(rot);
	SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	SetBlock(block);
	CMesh::SetSize(size);
	m_move = move;
	m_col = col;
	m_nNumVtx = (((int)block.x + 1) * ((int)block.y + 1));
	m_nNumIdx = (((int)block.x + 1) * 2) * ((int)block.y);

	// 頂点バッファを生成
	if (FAILED(CreateVtxBuff(m_nNumVtx)))
	{// 失敗した場合
		return E_FAIL;
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntMeshRing1 = 0; nCntMeshRing1 < (int)block.y + 1; nCntMeshRing1++)
	{
		for (int nCntMeshRing2 = 0; nCntMeshRing2 < (int)block.x + 1; nCntMeshRing2++)
		{
			float fAngle = ((D3DX_PI * 2.0f) / block.x);
			float fSize = (size.x + (size.y * -(float)((nCntMeshRing2 + (nCntMeshRing1 * (((int)block.x) + 1))) % ((int)block.y + 1))));

			// 頂点座標の設定
			pVtx[0].pos.x = sinf((float)((nCntMeshRing2 + (nCntMeshRing1 * ((int)block.x + 1))) / ((int)block.y + 1)) * -fAngle) * fSize;
			pVtx[0].pos.y = 0.0f;
			pVtx[0].pos.z = cosf((float)((nCntMeshRing2 + (nCntMeshRing1 * ((int)block.x + 1))) / ((int)block.y + 1)) * fAngle) * fSize;

			// 法線ベクトルの設定
			pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 頂点カラーの設定
			pVtx[0].col = m_col;

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2((float)nCntMeshRing2, (float)nCntMeshRing1);

			pVtx++;
		}
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();

	// インデックスバッファを生成
	if (FAILED(CreateIdxBuff(m_nNumIdx)))
	{// 失敗した場合
		return E_FAIL;
	}

	// インデックスバッファをロックし、頂点番号データへのポインタを取得
	GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

	// 頂点番号データの設定
	for (int nCntMeshRing1 = 0; nCntMeshRing1 < m_nNumIdx / 2; nCntMeshRing1++)
	{
		// 今作っている円の外側の頂点番号
		int nOuter = (nCntMeshRing1 % ((int)block.x + 1)) * ((int)block.y + 1) + (nCntMeshRing1 / ((int)block.x + 1));

		pIdx[0] = (DWORD)nOuter;
		pIdx[1] = (DWORD)nOuter + 1;
		pIdx += 2;
	}

	// インデックスバッファをアンロックする
	GetIdxBuff()->Unlock();

	return S_OK;
}

//=============================================================================
// メッシュリングの終了処理
//=============================================================================
void CMeshRing::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//=============================================================================
// メッシュリングの更新処理
//=============================================================================
void CMeshRing::Update(void)
{
	// 更新処理
	CMesh::Update();
}

//=============================================================================
// メッシュリングの描画処理
//=============================================================================
void CMeshRing::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ

	// カリングをOFFに設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ライティングオフ
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 頂点法線正規化をオン
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// マトリックスを計算
	CMesh::Draw();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, GetVtxBuff(), 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(GetIdxBuff());

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVtx, 0,
		m_nNumIdx - 2);

	// 頂点法線正規化をオフ
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

	// ライティングオン
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// カリングを戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//========================================================================
// テクスチャを結びつけ
//========================================================================
void CMeshRing::BindTexture(const int nTexture)
{
	m_nIdxTexture = nTexture;
}

//========================================================================
// 頂点の位置設定
//========================================================================
void CMeshRing::SetPosVtx(const int nCntVtx, const D3DXVECTOR3 pos)
{
	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[nCntVtx].pos = pos;

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// 頂点の位置取得
//========================================================================
D3DXVECTOR3 CMeshRing::GetPosVtx(const int nCntVtx)
{
	VERTEX_3D* pVtx;
	D3DXVECTOR3 pos;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pos = pVtx[nCntVtx].pos;

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();

	return pos;
}

//========================================================================
// サイズ設定
//========================================================================
void CMeshRing::SetSize(const D3DXVECTOR2 size)
{
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ
	D3DXVECTOR2 block = GetBlock();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntMeshRing1 = 0; nCntMeshRing1 < (int)block.y + 1; nCntMeshRing1++)
	{
		for (int nCntMeshRing2 = 0; nCntMeshRing2 < (int)block.x + 1; nCntMeshRing2++)
		{
			float fAngle = ((D3DX_PI * 2.0f) / block.x);
			float fSize = (size.x + (size.y * -(float)((nCntMeshRing2 + (nCntMeshRing1 * (((int)block.x) + 1))) % ((int)block.y + 1))));

			// 頂点座標の設定
			pVtx[0].pos.x = sinf((float)((nCntMeshRing2 + (nCntMeshRing1 * ((int)block.x + 1))) / ((int)block.y + 1)) * -fAngle) * fSize;
			pVtx[0].pos.y = 0.0f;
			pVtx[0].pos.z = cosf((float)((nCntMeshRing2 + (nCntMeshRing1 * ((int)block.x + 1))) / ((int)block.y + 1)) * fAngle) * fSize;

			pVtx++;
		}
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// 頂点カラー設定
//========================================================================
void CMeshRing::SetColor(const D3DXCOLOR col)
{
	m_col = col;

	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
	{
		// 頂点カラーの設定
		pVtx[nCntVtx].col = m_col;
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// 頂点カラー設定(オーバーロード)
//========================================================================
void CMeshRing::SetColor(const int nCntVtx, const D3DXCOLOR col)
{
	m_col = col;

	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[nCntVtx].col = m_col;

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}


//========================================================================
// テクスチャ座標設定
//========================================================================
void CMeshRing::SetTexUV(const int nCntVtx, const float ftexU, const float ftexV)
{
	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[nCntVtx].tex = D3DXVECTOR2(ftexU, ftexV);

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// テクスチャスクロール
//========================================================================
void CMeshRing::SetTexScroll(const int nCntVtx, const float fScrollU, const float fScrollV)
{
	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[nCntVtx].tex.x += fScrollU;
	pVtx[nCntVtx].tex.y += fScrollV;

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}