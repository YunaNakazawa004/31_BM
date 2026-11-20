//=============================================================================
// 
// メッシュドーム [ meshdome.cpp ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#include "meshdome.h"

#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "input.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//========================================================================
// メッシュドームクラスの生成処理
//========================================================================
CMeshDome* CMeshDome::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block,
	const float fRadius, const char* pFilename, const CObject::TYPE type, const int nPriority)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return NULL;
	}
#endif

	CMeshDome* pMeshDome = NULL;

	if (pMeshDome == NULL)
	{// NULLチェック
		// メッシュドームの生成
		pMeshDome = new CMeshDome(nPriority);
	}

	if (pMeshDome != NULL)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pMeshDome->Init(pos, rot, block, fRadius)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! メッシュドームの初期化に失敗しました ! ! !\n");

			return NULL;
		}

		// テクスチャを設定
		CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
		pMeshDome->m_nIdxTexture = pTexture->Register(pFilename);

		// 種類を設定
		pMeshDome->SetType(type);

		return pMeshDome;
	}

	OutputDebugStringA("! ! ! メッシュドームの生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// メッシュドームクラスのコンストラクタ
//========================================================================
CMeshDome::CMeshDome(const int nPriority) :CMesh(nPriority)
{
	// メッシュドームクラスの値をクリア
	m_fRadius = 0.0f;
	m_col = COLOR_WHITE;
	m_nNumVtx = 0;
	m_nNumIdx = 0;
	m_bDisp = true;
}

//========================================================================
// メッシュドームクラスのデストラクタ
//========================================================================
CMeshDome::~CMeshDome()
{
}

//=============================================================================
// メッシュドームの初期化処理
//=============================================================================
HRESULT CMeshDome::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
	const D3DXVECTOR2 block, const float fRadius)
{
	// ローカル変数宣言
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ
	DWORD* pIdx;							// インデックス情報へのポインタ

	// クラスの値を初期化
	SetPosition(pos);
	SetRotation(rot);
	SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	SetBlock(block);
	SetSize(D3DXVECTOR2(fRadius, fRadius));
	m_fRadius = fRadius;

	// 頂点バッファの数
	int nNumVtxCylinder = ((int)block.x + 1) * ((int)block.y + 1);
	int nNumVtxFan = (int)block.x + 1;
	m_nNumVtx = nNumVtxCylinder + nNumVtxFan;

	// インデックスバッファの数
	int nNumIdxCylinder = (((int)block.x) * ((int)block.y) * 2) + (((int)block.y - 1) * 4) + 2;
	int nNumIdxFan = (int)block.x + 2;
	m_nNumIdx = nNumIdxCylinder + nNumIdxFan;

	// 頂点バッファを生成
	if (FAILED(CreateVtxBuff(m_nNumVtx)))
	{// 失敗した場合
		return E_FAIL;
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	float fAngleX = ((D3DX_PI * 2.0f) / block.x);
	float fAngleY = (D3DX_PI / 2.0f) / (block.y + 1);

	for (int nCntMeshDome1 = 0; nCntMeshDome1 < nNumVtxFan; nCntMeshDome1++)
	{
		if (nCntMeshDome1 == 0)
		{// 真ん中
			pVtx[0].pos.x = 0.0f;
			pVtx[0].pos.y = m_fRadius;
			pVtx[0].pos.z = 0.0f;
		}
		else
		{// 真ん中以外
			// 頂点座標の設定
			pVtx[0].pos.x = sinf(-nCntMeshDome1 * fAngleX) * m_fRadius * sinf(fAngleY);
			pVtx[0].pos.y = cosf(-fAngleY) * m_fRadius;
			pVtx[0].pos.z = cosf(-nCntMeshDome1 * fAngleX) * m_fRadius * sinf(fAngleY);
		}

		// rhwの設定
		pVtx[0].nor = D3DXVECTOR3(-pVtx[0].pos.x, -pVtx[0].pos.y, -pVtx[0].pos.z);
		D3DXVec3Normalize(&pVtx[0].nor, &pVtx[0].nor);

		// 頂点カラーの設定
		pVtx[0].col = COLOR_WHITE;

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);

		pVtx++;
	}

	// 頂点情報の設定
	for (int nCntMeshDome1 = 0; nCntMeshDome1 < (int)block.y + 1; nCntMeshDome1++)
	{
		for (int nCntMeshDome2 = 0; nCntMeshDome2 < (int)block.x + 1; nCntMeshDome2++)
		{
			// 頂点座標の設定
			pVtx[0].pos.x = sinf(nCntMeshDome2 * fAngleX) * m_fRadius * sinf(fAngleY * (nCntMeshDome1 + 1));
			pVtx[0].pos.y = cosf(fAngleY * (nCntMeshDome1 + 1)) * m_fRadius;
			pVtx[0].pos.z = cosf(nCntMeshDome2 * fAngleX) * m_fRadius * sinf(fAngleY * (nCntMeshDome1 + 1));

			// rhwの設定
			pVtx[0].nor = D3DXVECTOR3(-pVtx[0].pos.x, -pVtx[0].pos.y, -pVtx[0].pos.z);
			D3DXVec3Normalize(&pVtx[0].nor, &pVtx[0].nor);

			// 頂点カラーの設定
			pVtx[0].col = COLOR_WHITE;

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2((float)nCntMeshDome2 / (block.y * 0.25f), (float)nCntMeshDome1 / (block.x * 0.25f));

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
	for (int nCntMeshDome1 = 0; nCntMeshDome1 < nNumIdxFan; nCntMeshDome1++)
	{
		pIdx[0] = nCntMeshDome1;

		if (nCntMeshDome1 == nNumIdxFan - 1)
		{// 最後は戻ってくる
			pIdx[0] = 1;
		}

		pIdx++;
	}

	int nNum = 0;			// 縮退ポリゴン

	// 頂点番号データの設定
	for (int nCntMeshDome1 = 0; nCntMeshDome1 < nNumIdxCylinder / 2; nCntMeshDome1++)
	{
		if (nCntMeshDome1 % ((int)block.x + 2) == ((int)block.x + 1))
		{// 縮退ポリゴンのところ
			nNum++;

			pIdx[0] = ((int)block.x + 1) + nCntMeshDome1 - nNum;
			pIdx[1] = ((int)block.x + 1) + nCntMeshDome1 - nNum + ((int)block.x + 2);
		}
		else
		{// 縮退以外のポリゴン
			pIdx[0] = ((int)block.x + 1) + (nCntMeshDome1 - nNum) + ((int)block.x + 1);
			pIdx[1] = ((int)block.x + 1) + (nCntMeshDome1 - nNum);
		}

		pIdx += 2;
	}

	// インデックスバッファをアンロックする
	GetIdxBuff()->Unlock();

	return S_OK;
}

//=============================================================================
// メッシュドームの終了処理
//=============================================================================
void CMeshDome::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//=============================================================================
// メッシュドームの更新処理
//=============================================================================
void CMeshDome::Update(void)
{
	// 更新処理
	CMesh::Update();
}

//=============================================================================
// メッシュドームの描画処理
//=============================================================================
void CMeshDome::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	D3DXVECTOR2 block = GetBlock();

	// マトリックスを計算
	CMesh::Draw();

	// カリングをOFFに設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 頂点法線正規化をオン
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, GetVtxBuff(), 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(GetIdxBuff());

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ポリゴンの描画(FAN)
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0,
		((int)block.x + 1), 0,
		((int)block.x));

	// ポリゴンの描画(STRIP)
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0,
		((int)block.x + 1) * ((int)block.y + 1),
		((int)block.x + 2),
		(((int)block.x) * ((int)block.y) * 2) + (((int)block.y - 1) * 4));

	// ライティングオン
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 頂点法線正規化をオフ
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

	// カリングを戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//========================================================================
// 頂点の位置設定
//========================================================================
void CMeshDome::SetPosVtx(const int nCntVtx, const D3DXVECTOR3 pos)
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
D3DXVECTOR3 CMeshDome::GetPosVtx(const int nCntVtx)
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
// 頂点カラー設定
//========================================================================
void CMeshDome::SetColor(const D3DXCOLOR col)
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
void CMeshDome::SetColor(const int nCntVtx, const D3DXCOLOR col)
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
void CMeshDome::SetTexUV(const int nCntVtx, const float ftexU, const float ftexV)
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
void CMeshDome::SetTexScroll(const int nCntVtx, const float fScrollU, const float fScrollV)
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
