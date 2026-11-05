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
CMeshRing::CMeshRing(const int nPriority) :CObject(nPriority)
{
	// メッシュリングクラスの値をクリア
	m_pVtxBuff = NULL;
	m_pIdxBuff = NULL;
	m_pos = DEFAULT_VECTOR3;
	m_rot = DEFAULT_VECTOR3;
	m_scale = DEFAULT_VECTOR3;
	m_block = DEFAULT_VECTOR2;
	m_size = DEFAULT_VECTOR2;
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

	m_pos = pos;
	m_move = move;
	m_rot = rot;
	m_block = block;
	m_size = size;
	m_col = col;
	m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_nNumVtx = (((int)m_block.x + 1) * ((int)m_block.y + 1));
	m_nNumIdx = (((int)m_block.x + 1) * 2) * ((int)m_block.y);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVtx,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntMeshRing1 = 0; nCntMeshRing1 < (int)m_block.y + 1; nCntMeshRing1++)
	{
		for (int nCntMeshRing2 = 0; nCntMeshRing2 < (int)m_block.x + 1; nCntMeshRing2++)
		{
			float fAngle = ((D3DX_PI * 2.0f) / m_block.x);
			float fSize = (m_size.x + (m_size.y * -(float)((nCntMeshRing2 + (nCntMeshRing1 * (((int)m_block.x) + 1))) % ((int)m_block.y + 1))));

			// 頂点座標の設定
			pVtx[0].pos.x = sinf((float)((nCntMeshRing2 + (nCntMeshRing1 * ((int)m_block.x + 1))) / ((int)m_block.y + 1)) * -fAngle) * fSize;
			pVtx[0].pos.y = 0.0f;
			pVtx[0].pos.z = cosf((float)((nCntMeshRing2 + (nCntMeshRing1 * ((int)m_block.x + 1))) / ((int)m_block.y + 1)) * fAngle) * fSize;

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
	m_pVtxBuff->Unlock();

	// インデックスバッファの生成
	pDevice->CreateIndexBuffer(sizeof(DWORD) * m_nNumIdx,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	// インデックスバッファをロックし、頂点番号データへのポインタを取得
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// 頂点番号データの設定
	for (int nCntMeshRing1 = 0; nCntMeshRing1 < m_nNumIdx / 2; nCntMeshRing1++)
	{
		// 今作っている円の外側の頂点番号
		int nOuter = (nCntMeshRing1 % ((int)m_block.x + 1)) * ((int)m_block.y + 1) + (nCntMeshRing1 / ((int)m_block.x + 1));

		pIdx[0] = (DWORD)nOuter;
		pIdx[1] = (DWORD)nOuter + 1;
		pIdx += 2;
	}

	// インデックスバッファをアンロックする
	m_pIdxBuff->Unlock();

	return S_OK;
}

//=============================================================================
// メッシュリングの終了処理
//=============================================================================
void CMeshRing::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// インデックスバッファの破棄
	if (m_pIdxBuff != NULL)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = NULL;
	}

	// 自分自身を破棄
	CObject::Release();
}

//=============================================================================
// メッシュリングの更新処理
//=============================================================================
void CMeshRing::Update(void)
{
	// 移動
	m_pos += m_move;

	// 段々輪を大きく、細く
	m_size.x += 0.8f;
	m_size.y -= 0.4f;

	// サイズ設定
	SetSize(m_size);

	if (m_size.y <= 0.0f)
	{// 輪が見えなくなる
		// 終了処理
		Uninit();
	}
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
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス

	// カリングをOFFに設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ライティングオフ
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVtx, 0,
		m_nNumIdx - 2);

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
// 位置設定
//========================================================================
void CMeshRing::SetPosition(const D3DXVECTOR3 pos)
{
	// 位置を変更
	m_pos = pos;
}

//========================================================================
// 頂点の位置設定
//========================================================================
void CMeshRing::SetPosVtx(const int nCntVtx, const D3DXVECTOR3 pos)
{
	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[nCntVtx].pos = pos;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// 頂点の位置取得
//========================================================================
D3DXVECTOR3 CMeshRing::GetPosVtx(const int nCntVtx)
{
	VERTEX_3D* pVtx;
	D3DXVECTOR3 pos;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pos = pVtx[nCntVtx].pos;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return pos;
}

//========================================================================
// スケール設定
//========================================================================
void CMeshRing::SetScale(const D3DXVECTOR3 scale)
{
	// スケールを変更
	m_scale = scale;
}

//========================================================================
// サイズ設定
//========================================================================
void CMeshRing::SetSize(const D3DXVECTOR2 size)
{
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntMeshRing1 = 0; nCntMeshRing1 < (int)m_block.y + 1; nCntMeshRing1++)
	{
		for (int nCntMeshRing2 = 0; nCntMeshRing2 < (int)m_block.x + 1; nCntMeshRing2++)
		{
			float fAngle = ((D3DX_PI * 2.0f) / m_block.x);
			float fSize = (m_size.x + (m_size.y * -(float)((nCntMeshRing2 + (nCntMeshRing1 * (((int)m_block.x) + 1))) % ((int)m_block.y + 1))));

			// 頂点座標の設定
			pVtx[0].pos.x = sinf((float)((nCntMeshRing2 + (nCntMeshRing1 * ((int)m_block.x + 1))) / ((int)m_block.y + 1)) * -fAngle) * fSize;
			pVtx[0].pos.y = 0.0f;
			pVtx[0].pos.z = cosf((float)((nCntMeshRing2 + (nCntMeshRing1 * ((int)m_block.x + 1))) / ((int)m_block.y + 1)) * fAngle) * fSize;

			pVtx++;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// 角度設定
//========================================================================
void CMeshRing::SetRotation(const D3DXVECTOR3 rot)
{
	// X向きを調整
	CorrectAngle(&m_rot.x, m_rot.x);

	// Y向きを調整
	CorrectAngle(&m_rot.y, m_rot.y);

	// Z向きを調整
	CorrectAngle(&m_rot.z, m_rot.z);

	// 向きを変更
	m_rot = rot;
}

//========================================================================
// 頂点カラー設定
//========================================================================
void CMeshRing::SetColor(const D3DXCOLOR col)
{
	m_col = col;

	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
	{
		// 頂点カラーの設定
		pVtx[nCntVtx].col = m_col;
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// 頂点カラー設定(オーバーロード)
//========================================================================
void CMeshRing::SetColor(const int nCntVtx, const D3DXCOLOR col)
{
	m_col = col;

	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[nCntVtx].col = m_col;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}


//========================================================================
// テクスチャ座標設定
//========================================================================
void CMeshRing::SetTexUV(const int nCntVtx, const float ftexU, const float ftexV)
{
	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[nCntVtx].tex = D3DXVECTOR2(ftexU, ftexV);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// テクスチャスクロール
//========================================================================
void CMeshRing::SetTexScroll(const int nCntVtx, const float fScrollU, const float fScrollV)
{
	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[nCntVtx].tex.x += fScrollU;
	pVtx[nCntVtx].tex.y += fScrollV;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}