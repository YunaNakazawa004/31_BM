//========================================================================
// 
// メッシュフィールド [ meshfieldM.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "meshfieldM.h"

#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "input.h"
#include "debugproc.h"

#include <fstream>
#include <iostream>

//************************************************************************
// マクロ定義
//************************************************************************

//========================================================================
// メッシュフィールドクラスの生成処理
//========================================================================
CMeshFieldM* CMeshFieldM::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block,
	const D3DXVECTOR2 size, const CObject::TYPE type, const char* pFilename, const int nPriority)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return NULL;
	}
#endif

	CMeshFieldM* pMeshFIeld = NULL;

	if (pMeshFIeld == NULL)
	{// NULLチェック
		// メッシュフィールドの生成
		pMeshFIeld = new CMeshFieldM(nPriority);
	}

	if (pMeshFIeld != NULL)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pMeshFIeld->Init(pos, rot, block, size)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! メッシュフィールドの初期化に失敗しました ! ! !\n");

			return NULL;
		}

		// テクスチャを設定
		CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
		pMeshFIeld->m_aIdxTexture[0] = pTexture->Register("data\\TEXTURE\\field002.jpg");
		pMeshFIeld->m_aIdxTexture[1] = pTexture->Register("data\\TEXTURE\\rock.jpg");

		// 種類を設定
		pMeshFIeld->SetType(type);

		return pMeshFIeld;
	}

	OutputDebugStringA("! ! ! メッシュフィールドの生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// メッシュフィールドクラスのコンストラクタ
//========================================================================
CMeshFieldM::CMeshFieldM(const int nPriority) :CMesh(nPriority)
{
	// メッシュフィールドクラスの値をクリア
	memset(&m_aIdxTexture[0], -1, sizeof m_aIdxTexture);
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_nNumVtx = 0;
	m_nNumIdx = 0;
	m_bDisp = true;
}

//========================================================================
// メッシュフィールドクラスのデストラクタ
//========================================================================
CMeshFieldM::~CMeshFieldM()
{
}

//========================================================================
// メッシュフィールドクラスの初期化処理(オーバーロード)
//========================================================================
HRESULT CMeshFieldM::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
	const D3DXVECTOR2 block, const D3DXVECTOR2 size)
{
	// ローカル変数宣言
	VERTEX_3D_MULTI* pVtx;					// 頂点情報へのポインタ
	DWORD* pIdx;							// インデックス情報へのポインタ

	// クラスの値を初期化
	SetPosition(pos);
	SetRotation(rot);
	SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	SetBlock(block);
	SetSize(size);
	m_nNumVtx = ((int)block.x + 1) * ((int)block.y + 1);
	m_nNumIdx = (((int)block.x) * ((int)block.y) * 2) + (((int)block.y - 1) * 4) + 2;

	// 頂点バッファを生成
	if (FAILED(CreateVtxBuffMulti(m_nNumVtx)))
	{// 失敗した場合
		return E_FAIL;
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntMeshFieldM1 = 0; nCntMeshFieldM1 < (int)block.y + 1; nCntMeshFieldM1++)
	{
		for (int nCntMeshFieldM2 = 0; nCntMeshFieldM2 < (int)block.x + 1; nCntMeshFieldM2++)
		{
			// 頂点番号
			int nVtx = nCntMeshFieldM2 + (nCntMeshFieldM1 * ((int)block.x + 1));

			// 頂点座標の設定
			pVtx[nVtx].pos.x = ((float)nCntMeshFieldM2 * size.x * 2.0f) - ((block.x * size.x * 2.0f) * 0.5f);
			pVtx[nVtx].pos.y = 0.0f;
			pVtx[nVtx].pos.z = -((float)nCntMeshFieldM1 * size.y * 2.0f) + ((block.y * size.y * 2.0f) * 0.5f);

			// 頂点カラーの設定
			pVtx[nVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			pVtx[nVtx].tex = D3DXVECTOR2((float)nCntMeshFieldM2, (float)nCntMeshFieldM1);
			pVtx[nVtx].texM = D3DXVECTOR2((float)nCntMeshFieldM2, (float)nCntMeshFieldM1);
		}
	}

	// 法線の設定
	SetNor();

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();

	// インデックスバッファを生成
	if (FAILED(CreateIdxBuff(m_nNumIdx)))
	{// 失敗した場合
		return E_FAIL;
	}

	// インデックスバッファをロックし、頂点番号データへのポインタを取得
	GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

	int nNum = 0;			// 縮退ポリゴン

	// 頂点番号データの設定
	for (int nCntMeshFieldM1 = 0; nCntMeshFieldM1 < (m_nNumIdx / 2); nCntMeshFieldM1++)
	{
		if (nCntMeshFieldM1 % ((int)block.x + 2) == ((int)block.x + 1))
		{// 縮退ポリゴンのところ
			pIdx[0] = (nCntMeshFieldM1 - nNum) - 1;
			pIdx[1] = (nCntMeshFieldM1 - nNum) + ((int)block.x + 1);

			nNum++;
		}
		else
		{// 縮退以外のポリゴン
			pIdx[0] = (nCntMeshFieldM1 - nNum) + ((int)block.x + 1);
			pIdx[1] = (nCntMeshFieldM1 - nNum);
		}

		pIdx += 2;
	}

	// インデックスバッファをアンロックする
	GetIdxBuff()->Unlock();

	return S_OK;
}

//========================================================================
// メッシュフィールドクラスの終了処理
//========================================================================
void CMeshFieldM::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//========================================================================
// メッシュフィールドクラスの更新処理
//========================================================================
void CMeshFieldM::Update(void)
{
}

//========================================================================
// メッシュフィールドクラスの描画処理
//========================================================================
void CMeshFieldM::Draw(void)
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

	// 頂点法線正規化をオン
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// マトリックスを計算
	CMesh::Draw();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, GetVtxBuff(), 0, sizeof(VERTEX_3D_MULTI));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(GetIdxBuff());

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D_MULTI);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_aIdxTexture[0]));
	pDevice->SetTexture(1, pTexture->GetAddress(m_aIdxTexture[1]));

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, ((int)block.x + 1) * ((int)block.y + 1), 0,
		(((int)block.x) * ((int)block.y) * 2) + (((int)block.y - 1) * 4));

	// 頂点法線正規化をオフ
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}

//========================================================================
// テクスチャを結びつけ
//========================================================================
void CMeshFieldM::BindTexture(const int* pTexture)
{
	if (pTexture != NULL)
	{// NULLチェック
		m_aIdxTexture[0] = pTexture[0];
		m_aIdxTexture[1] = pTexture[1];
	}
}

//========================================================================
// 頂点の位置設定
//========================================================================
void CMeshFieldM::SetPosVtx(const int nCntVtx, const D3DXVECTOR3 pos)
{
	VERTEX_3D_MULTI* pVtx;

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
D3DXVECTOR3 CMeshFieldM::GetPosVtx(const int nCntVtx)
{
	VERTEX_3D_MULTI* pVtx;
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
void CMeshFieldM::SetColor(const D3DXCOLOR col)
{
	m_col = col;

	VERTEX_3D_MULTI* pVtx;

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
void CMeshFieldM::SetColor(const int nCntVtx, const D3DXCOLOR col)
{
	m_col = col;

	VERTEX_3D_MULTI* pVtx;

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
void CMeshFieldM::SetTexUV(const int nCntVtx, const float ftexU, const float ftexV)
{
	VERTEX_3D_MULTI* pVtx;

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
void CMeshFieldM::SetTexScroll(const int nCntVtx, const float fScrollU, const float fScrollV)
{
	VERTEX_3D_MULTI* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[nCntVtx].tex.x += fScrollU;
	pVtx[nCntVtx].tex.y += fScrollV;

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// マルチテクスチャスクロール
//========================================================================
void CMeshFieldM::SetTexMScroll(const int nCntVtx, const float fScrollU, const float fScrollV)
{
	VERTEX_3D_MULTI* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[nCntVtx].texM.x += fScrollU;
	pVtx[nCntVtx].texM.y += fScrollV;

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// 法線を設定
//========================================================================
void CMeshFieldM::SetNor(void)
{
	VERTEX_3D_MULTI* pVtx;					// 頂点情報へのポインタ
	D3DXVECTOR3 nor, nor1, nor2, nor3, nor4, nor5, nor6, vec1, vec2;
	D3DXVECTOR2 block = GetBlock();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 法線の設定
	for (int nCntMeshFieldM1 = 0; nCntMeshFieldM1 < (int)block.y + 1; nCntMeshFieldM1++)
	{
		for (int nCntMeshFieldM2 = 0; nCntMeshFieldM2 < (int)block.x + 1; nCntMeshFieldM2++)
		{
			// 頂点番号
			int nVtx = nCntMeshFieldM2 + (nCntMeshFieldM1 * ((int)block.x + 1));

			D3DXVECTOR3 vec1, vec2;
			D3DXVECTOR3 nor, norA, norB, norC, norD, norE, norF;

			nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 法線の設定
			if ((nCntMeshFieldM1 == 0 && nCntMeshFieldM2 == (int)block.x))
			{// 法線ひとつ(右上)(▽)
				vec1 = pVtx[nVtx + ((int)block.x + 1)].pos - pVtx[nVtx].pos;
				vec2 = pVtx[nVtx - 1].pos - pVtx[nVtx].pos;

				// 外積
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&nor, &nor);
			}
			else if (nCntMeshFieldM1 == (int)block.y && nCntMeshFieldM2 == 0)
			{// 法線ひとつ(左下)(△)
				vec1 = pVtx[nVtx - ((int)block.x + 1)].pos - pVtx[nVtx].pos;
				vec2 = pVtx[nVtx + 1].pos - pVtx[nVtx].pos;

				// 外積
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&nor, &nor);
			}
			else if (nCntMeshFieldM1 == 0 && nCntMeshFieldM2 == 0)
			{// 法線ふたつ(左上)
				// (△)
				int nMainVtx = nVtx + ((int)block.x + 1);		// 真下の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norA, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norA, &norA);

				// (▽)
				nMainVtx = nVtx + 1;							// 真右の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norB, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norB, &norB);

				nor = (norA + norB) / 2.0f;		// 最終的な法線方向
			}
			else if (nCntMeshFieldM1 == (int)block.y && nCntMeshFieldM2 == (int)block.x)
			{// 法線ふたつ(右下)
				// (△)
				int nMainVtx = nVtx - 1;						// 真左の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norA, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norA, &norA);

				// (▽)
				nMainVtx = nVtx - ((int)block.x + 1);			// 真上の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norB, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norB, &norB);

				nor = (norA + norB) / 2.0f;		// 最終的な法線方向
			}
			else if (nCntMeshFieldM1 == 0)
			{// 法線みっつ(上の辺)
				// (▽)
				vec1 = pVtx[nVtx + ((int)block.x + 1)].pos - pVtx[nVtx].pos;
				vec2 = pVtx[nVtx - 1].pos - pVtx[nVtx].pos;

				// 外積
				D3DXVec3Cross(&norA, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norA, &norA);

				// (△)
				int nMainVtx = nVtx + ((int)block.x + 1);		// 真下の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norB, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norB, &norB);

				// (▽)
				nMainVtx = nVtx + 1;							// 真右の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norC, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norC, &norC);

				nor = (norA + norB + norC) / 3.0f;		// 最終的な法線方向
			}
			else if (nCntMeshFieldM1 == (int)block.y)
			{// 法線みっつ(下の辺)
				// (△)
				vec1 = pVtx[nVtx - ((int)block.x + 1)].pos - pVtx[nVtx].pos;
				vec2 = pVtx[nVtx + 1].pos - pVtx[nVtx].pos;

				// 外積
				D3DXVec3Cross(&norA, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norA, &norA);

				// (△)
				int nMainVtx = nVtx - 1;						// 真左の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norB, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norB, &norB);

				// (▽)
				nMainVtx = nVtx - ((int)block.x + 1);			// 真上の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norC, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norC, &norC);

				nor = (norA + norB + norC) / 3.0f;		// 最終的な法線方向
			}
			else if (nCntMeshFieldM2 == 0)
			{// 法線みっつ(左の辺)
				// (△)
				vec1 = pVtx[nVtx - ((int)block.x + 1)].pos - pVtx[nVtx].pos;
				vec2 = pVtx[nVtx + 1].pos - pVtx[nVtx].pos;

				// 外積
				D3DXVec3Cross(&norA, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norA, &norA);

				// (△)
				int nMainVtx = nVtx + ((int)block.x + 1);		// 真下の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norB, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norB, &norB);

				// (▽)
				nMainVtx = nVtx + 1;							// 真右の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norC, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norC, &norC);

				nor = (norA + norB + norC) / 3.0f;		// 最終的な法線方向
			}
			else if (nCntMeshFieldM2 == (int)block.x)
			{// 法線みっつ(右の辺)
				// (▽)
				vec1 = pVtx[nVtx + ((int)block.x + 1)].pos - pVtx[nVtx].pos;
				vec2 = pVtx[nVtx - 1].pos - pVtx[nVtx].pos;

				// 外積
				D3DXVec3Cross(&norA, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norA, &norA);

				// (△)
				int nMainVtx = nVtx - 1;						// 真左の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norB, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norB, &norB);

				// (▽)
				nMainVtx = nVtx - ((int)block.x + 1);			// 真上の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norC, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norC, &norC);

				nor = (norA + norB + norC) / 3.0f;		// 最終的な法線方向
			}
			else
			{// 中心部
				// (右上△)
				vec1 = pVtx[nVtx - ((int)block.x + 1)].pos - pVtx[nVtx].pos;
				vec2 = pVtx[nVtx + 1].pos - pVtx[nVtx].pos;

				// 外積
				D3DXVec3Cross(&norA, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norA, &norA);

				// (左下▽)
				vec1 = pVtx[nVtx + ((int)block.x + 1)].pos - pVtx[nVtx].pos;
				vec2 = pVtx[nVtx - 1].pos - pVtx[nVtx].pos;

				// 外積
				D3DXVec3Cross(&norB, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norB, &norB);

				// (左上△)
				int nMainVtx = nVtx - 1;						// 真左の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norC, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norC, &norC);

				// (左上▽)
				nMainVtx = nVtx - ((int)block.x + 1);			// 真上の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norD, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norD, &norD);

				// (右下△)
				nMainVtx = nVtx + ((int)block.x + 1);		// 真下の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norE, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norE, &norE);

				// (右下▽)
				nMainVtx = nVtx + 1;							// 真右の頂点に主頂点を設定

				vec1 = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
				vec2 = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx].pos;

				// 外積
				D3DXVec3Cross(&norF, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&norF, &norF);

				nor = (norA + norB + norC + norD + norE + norF) / 6.0f;		// 最終的な法線方向
			}

			pVtx[nVtx].nor = nor;
		}
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// 最も近い頂点番号を取得
//========================================================================
int CMeshFieldM::GetNearestVtx(const D3DXVECTOR3 pos, const float fRadius)
{
	int nNearestIdx = -1;				// 一番近い頂点番号
	float fNearestDist = 99999.9f;		// 一番近い距離
	VERTEX_3D_MULTI* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
	{
		D3DXVECTOR2 dist = { 0.0f,0.0f };
		float fDist = 0.0f;

		// 頂点との距離を計算
		dist.x = pos.x - pVtx[nCntVtx].pos.x;
		dist.y = pos.z - pVtx[nCntVtx].pos.z;
		fDist = D3DXVec2Length(&dist);

		if (fDist < fNearestDist)
		{// 現在一番近い距離より近いなら
			// 距離と番号を保存
			fNearestDist = fDist;
			nNearestIdx = nCntVtx;
		}
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();

	// 最終チェック
	if (fNearestDist > fRadius)
	{// 一番近い距離でも、半径より遠い場所にあったら
		nNearestIdx = -1;		// どの番号も渡さない
	}

	return nNearestIdx;
}

//========================================================================
// 指定された頂点の位置を設定
//========================================================================
void CMeshFieldM::MoveVtx(const int nVtx, const D3DXVECTOR3 move, const float fRadius, const int terrain)
{
	VERTEX_3D_MULTI* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
	{
		D3DXVECTOR2 dist;			// 距離
		dist.x = pVtx[nCntVtx].pos.x - pVtx[nVtx].pos.x;
		dist.y = pVtx[nCntVtx].pos.z - pVtx[nVtx].pos.z;

		float fDist = D3DXVec2Length(&dist);

		switch (terrain)
		{
		case TERRAIN_DOT:				// 点
			if (nCntVtx == nVtx)
			{// 指定の頂点
				pVtx[nCntVtx].pos += move;
			}

			break;

		case TERRAIN_FLAT:				// 平面
			if (fDist < fRadius)
			{// 半径より小さい頂点
				pVtx[nCntVtx].pos += move;
			}

			break;

		case TERRAIN_HILL:				// 丘
			if (fRadius > fDist)
			{// それ以外で半径より近い範囲の頂点
				pVtx[nCntVtx].pos += move * (fRadius / (fDist + fRadius));
			}

			break;

		case TERRAIN_MOUTAIN:			// 山
			float fMoveRate = (fRadius - fDist);

			if (fMoveRate > 0.0f)
			{// 0 より大きいときだけ
				pVtx[nCntVtx].pos += move * (fMoveRate / fRadius);
			}

			break;
		}
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// 現在位置に対するポリゴン番号を取得
//========================================================================
D3DXVECTOR2 CMeshFieldM::GetPolygonIdx(const D3DXVECTOR3 pos)
{
	// ローカル変数
	D3DXVECTOR2 polygonIdx = { -1.0f,-1.0f };
	D3DXVECTOR3 posM = GetPosition();
	D3DXVECTOR2 block = GetBlock();
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2 posP;			// 与えられた位置のオフセット位置
	D3DXVECTOR2 posOff;			// メッシュフィールドのオフセット位置

	// オフセット位置を算出
	posP.x = pos.x + (size.x * block.x);
	posP.y = pos.z - (size.y * block.y);
	posOff.x = posM.x + (size.x * block.x);
	posOff.y = posM.z - (size.y * block.y);

	polygonIdx.x = (posP.x / (size.x * 2.0f));
	polygonIdx.y = (posP.y / -(size.y * 2.0f));

	if (polygonIdx.x >= 0 && polygonIdx.x < block.x && polygonIdx.y >= 0 && polygonIdx.y < block.y)
	{// 範囲内の数値
		return polygonIdx;
	}

	return D3DXVECTOR2(-1.0f, -1.0f);
}

//========================================================================
// 現在位置の高さを取得
//========================================================================
float CMeshFieldM::GetHeight(const D3DXVECTOR3 pos, const D3DXVECTOR2 polygonIdx)
{
	if (polygonIdx.x < 0 || polygonIdx.y < 0)
	{// 範囲外の数値
		return ERROR_HEIGHT;
	}

	// ローカル変数
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	D3DXVECTOR3 aVecLine[3], aVecToPos[3];		// 境界線ベクトル、現在位置へのベクトル
	D3DXVECTOR2 block = GetBlock();
	D3DXVECTOR2 size = GetSize();
	float fposLine;			// 外積
	int nCntLine = 0;		// 内側にいた回数
	int nVtx = (int)polygonIdx.x + ((int)polygonIdx.y * ((int)block.x + 1));		// 現在位置の頂点
	int nMainVtx = -1;		// 主となる頂点
	VERTEX_3D_MULTI* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 下の点を主頂点にする(△)
	nMainVtx = nVtx + (int)block.x + 1;

	// プレイヤーが対象ポリゴン(nMainVtx - (nMainVtx - block.x + 1) - nMainVtx + 1)の内側にいるかどうか
	// nMainVtx - (nMainVtx - (int)block.x + 1) 間のベクトル
	aVecLine[0] = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;		// 辺のベクトル
	aVecToPos[0] = pos - pVtx[nMainVtx].pos;										// 現在位置へのベクトル

	// (nMainVtx - (int)block.x + 1) - nMainVtx + 1 間のベクトル
	aVecLine[1] = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx - ((int)block.x + 1)].pos;	// 辺のベクトル
	aVecToPos[1] = pos - pVtx[nMainVtx - ((int)block.x + 1)].pos;					// 現在位置へのベクトル

	// nMainVtx + 1 - nMainVtx 間のベクトル
	aVecLine[2] = pVtx[nMainVtx].pos - pVtx[nMainVtx + 1].pos;						// 辺のベクトル
	aVecToPos[2] = pos - pVtx[nMainVtx + 1].pos;									// 現在位置へのベクトル

	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		// 外積を計算
		fposLine = (float)((int)(((aVecLine[nCnt].z * aVecToPos[nCnt].x) - (aVecLine[nCnt].x * aVecToPos[nCnt].z)) * 1.0f) / (int)1);

		if (fposLine >= 0.0f)
		{// 内側にいる
			nCntLine++;
		}
	}

	if (nCntLine >= 3)
	{// 対象ポリゴン(nMainVtx - (nMainVtx - block.x + 1) - nMainVtx + 1)の内側にいる
		D3DXVECTOR3 vec1, vec2, nor;	// ベクトル、法線
		float fHeight = 0.0f;			// 求める高さ

		// ベクトルを取得
		vec1 = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
		vec2 = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx].pos;

		// 法線の計算
		D3DXVec3Cross(&nor, &vec1, &vec2);

		// 法線を正規化
		D3DXVec3Normalize(&nor, &nor);

		if (nor.y != 0.0f)
		{// yの法線が0ではない
			// 高さを求める計算
			// 元の式 : (pos.x - pVtx[0].pos.x) * nor.x + (pos.y - pVtx[0].pos.y) * nor.y + (pos.z - pVtx[0].pos.z) * nor.z = 0.0f
			fHeight = ((-(pos.x - pVtx[nMainVtx].pos.x) * nor.x - (pos.z - pVtx[nMainVtx].pos.z) * nor.z) / nor.y) + pVtx[nMainVtx].pos.y;

			// 頂点バッファをアンロックする
			GetVtxBuff()->Unlock();

			return fHeight;
		}
	}
	else
	{// 内側にいない→次のポリゴンの内側にいるかどうか調べる
		nCntLine = 0;		// リセット
	}

	// 右の点を主頂点にする(▽)
	nMainVtx = nVtx + 1;

	// プレイヤーが対象ポリゴン(nMainVtx - (nMainVtx + block.x + 1) - (nMainVtx - 1))の内側にいるかどうか
	// nMainVtx - (nMainVtx + (int)block.x + 1) 間のベクトル
	aVecLine[0] = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;		// 辺のベクトル
	aVecToPos[0] = pos - pVtx[nMainVtx].pos;										// 現在位置へのベクトル

	// (nMainVtx + (int)block.x + 1) - (nMainVtx - 1) 間のベクトル
	aVecLine[1] = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx + ((int)block.x + 1)].pos;	// 辺のベクトル
	aVecToPos[1] = pos - pVtx[nMainVtx + ((int)block.x + 1)].pos;					// 現在位置へのベクトル

	// (nMainVtx - 1) - nMainVtx 間のベクトル
	aVecLine[2] = pVtx[nMainVtx].pos - pVtx[nMainVtx - 1].pos;						// 辺のベクトル
	aVecToPos[2] = pos - pVtx[nMainVtx - 1].pos;									// 現在位置へのベクトル

	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		// 外積を計算
		fposLine = (float)((int)(((aVecLine[nCnt].z * aVecToPos[nCnt].x) - (aVecLine[nCnt].x * aVecToPos[nCnt].z)) * 1.0f) / (int)1);

		if (fposLine >= 0.0f)
		{// 内側にいる
			nCntLine++;
		}
	}

	if (nCntLine >= 3)
	{// 対象ポリゴン(3-2-1)の内側にいる
		D3DXVECTOR3 vec1, vec2, nor;	// ベクトル、法線
		float fHeight = 0.0f;			// 求める高さ

		// ベクトルを取得
		vec1 = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
		vec2 = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx].pos;

		// 法線の計算
		D3DXVec3Cross(&nor, &vec1, &vec2);

		// 法線を正規化
		D3DXVec3Normalize(&nor, &nor);

		if (nor.y != 0.0f)
		{// yの法線が0ではない
			// 高さを求める計算
			// 元の式 : (pos.x - pVtx[3].pos.x) * nor.x + (pos.y - pVtx[3].pos.y) * nor.y + (pos.z - pVtx[3].pos.z) * nor.z = 0.0f
			fHeight = ((-(pos.x - pVtx[nMainVtx].pos.x) * nor.x - (pos.z - pVtx[nMainVtx].pos.z) * nor.z) / nor.y) + pVtx[nMainVtx].pos.y;

			// 頂点バッファをアンロックする
			GetVtxBuff()->Unlock();

			return fHeight;
		}
	}
	else
	{// どちらのポリゴンの内側にもいない
		// 頂点バッファをアンロックする
		GetVtxBuff()->Unlock();

		return ERROR_HEIGHT;
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();

	return ERROR_HEIGHT;
}

//========================================================================
// 現在位置の傾斜を取得
//========================================================================
float CMeshFieldM::GetSlope(const D3DXVECTOR3 pos, const D3DXVECTOR2 polygonIdx)
{
	if (polygonIdx.x < 0 || polygonIdx.y < 0)
	{// 範囲外の数値
		return 1.0f;
	}

	// ローカル変数
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	D3DXVECTOR3 aVecLine[3], aVecToPos[3];		// 境界線ベクトル、現在位置へのベクトル
	D3DXVECTOR2 block = GetBlock();
	D3DXVECTOR2 size = GetSize();
	float fposLine;			// 外積
	int nCntLine = 0;		// 内側にいた回数
	int nVtx = (int)polygonIdx.x + ((int)polygonIdx.y * ((int)block.x + 1));		// 現在位置の頂点
	int nMainVtx = -1;		// 主となる頂点
	VERTEX_3D_MULTI* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 下の点を主頂点にする(△)
	nMainVtx = nVtx + (int)block.x + 1;

	// プレイヤーが対象ポリゴン(nMainVtx - (nMainVtx - block.x + 1) - nMainVtx + 1)の内側にいるかどうか
	// nMainVtx - (nMainVtx - (int)block.x + 1) 間のベクトル
	aVecLine[0] = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;		// 辺のベクトル
	aVecToPos[0] = pos - pVtx[nMainVtx].pos;										// 現在位置へのベクトル

	// (nMainVtx - (int)block.x + 1) - nMainVtx + 1 間のベクトル
	aVecLine[1] = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx - ((int)block.x + 1)].pos;	// 辺のベクトル
	aVecToPos[1] = pos - pVtx[nMainVtx - ((int)block.x + 1)].pos;					// 現在位置へのベクトル

	// nMainVtx + 1 - nMainVtx 間のベクトル
	aVecLine[2] = pVtx[nMainVtx].pos - pVtx[nMainVtx + 1].pos;						// 辺のベクトル
	aVecToPos[2] = pos - pVtx[nMainVtx + 1].pos;									// 現在位置へのベクトル

	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		// 外積を計算
		fposLine = (float)((int)(((aVecLine[nCnt].z * aVecToPos[nCnt].x) - (aVecLine[nCnt].x * aVecToPos[nCnt].z)) * 1.0f) / (int)1);

		if (fposLine >= 0.0f)
		{// 内側にいる
			nCntLine++;
		}
	}

	if (nCntLine >= 3)
	{// 対象ポリゴン(nMainVtx - (nMainVtx - block.x + 1) - nMainVtx + 1)の内側にいる
		D3DXVECTOR3 vec1, vec2, nor;	// ベクトル、法線
		float fHeight = 0.0f;			// 求める高さ

		// ベクトルを取得
		vec1 = pVtx[nMainVtx - ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
		vec2 = pVtx[nMainVtx + 1].pos - pVtx[nMainVtx].pos;

		// 法線の計算
		D3DXVec3Cross(&nor, &vec1, &vec2);

		// 法線を正規化
		D3DXVec3Normalize(&nor, &nor);

		return nor.y;
	}
	else
	{// 内側にいない→次のポリゴンの内側にいるかどうか調べる
		nCntLine = 0;		// リセット
	}

	// 右の点を主頂点にする(▽)
	nMainVtx = nVtx + 1;

	// プレイヤーが対象ポリゴン(nMainVtx - (nMainVtx + block.x + 1) - (nMainVtx - 1))の内側にいるかどうか
	// nMainVtx - (nMainVtx + (int)block.x + 1) 間のベクトル
	aVecLine[0] = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;		// 辺のベクトル
	aVecToPos[0] = pos - pVtx[nMainVtx].pos;										// 現在位置へのベクトル

	// (nMainVtx + (int)block.x + 1) - (nMainVtx - 1) 間のベクトル
	aVecLine[1] = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx + ((int)block.x + 1)].pos;	// 辺のベクトル
	aVecToPos[1] = pos - pVtx[nMainVtx + ((int)block.x + 1)].pos;					// 現在位置へのベクトル

	// (nMainVtx - 1) - nMainVtx 間のベクトル
	aVecLine[2] = pVtx[nMainVtx].pos - pVtx[nMainVtx - 1].pos;						// 辺のベクトル
	aVecToPos[2] = pos - pVtx[nMainVtx - 1].pos;									// 現在位置へのベクトル

	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		// 外積を計算
		fposLine = (float)((int)(((aVecLine[nCnt].z * aVecToPos[nCnt].x) - (aVecLine[nCnt].x * aVecToPos[nCnt].z)) * 1.0f) / (int)1);

		if (fposLine >= 0.0f)
		{// 内側にいる
			nCntLine++;
		}
	}

	if (nCntLine >= 3)
	{// 対象ポリゴン(3-2-1)の内側にいる
		D3DXVECTOR3 vec1, vec2, nor;	// ベクトル、法線
		float fHeight = 0.0f;			// 求める高さ

		// ベクトルを取得
		vec1 = pVtx[nMainVtx + ((int)block.x + 1)].pos - pVtx[nMainVtx].pos;
		vec2 = pVtx[nMainVtx - 1].pos - pVtx[nMainVtx].pos;

		// 法線の計算
		D3DXVec3Cross(&nor, &vec1, &vec2);

		// 法線を正規化
		D3DXVec3Normalize(&nor, &nor);

		return nor.y;
	}
	else
	{// どちらのポリゴンの内側にもいない
		// 頂点バッファをアンロックする
		GetVtxBuff()->Unlock();

		return 1.0f;
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();

	return 1.0f;
}

//========================================================================
// 頂点データをリセット
//========================================================================
void CMeshFieldM::ResetData(void)
{
	VERTEX_3D_MULTI* pVtx;
	DWORD* pIdx;
	D3DXVECTOR2 block = GetBlock();
	D3DXVECTOR2 size = GetSize();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntMeshFieldM1 = 0; nCntMeshFieldM1 < (int)block.y + 1; nCntMeshFieldM1++)
	{
		for (int nCntMeshFieldM2 = 0; nCntMeshFieldM2 < (int)block.x + 1; nCntMeshFieldM2++)
		{
			// 頂点番号
			int nVtx = nCntMeshFieldM2 + (nCntMeshFieldM1 * ((int)block.x + 1));

			// 頂点座標の設定
			pVtx[nVtx].pos.x = -((block.x * size.x * 2.0f) * 0.5f) + (nCntMeshFieldM2 * size.x * 2.0f);
			pVtx[nVtx].pos.y = 0.0f;
			pVtx[nVtx].pos.z = ((block.y * size.y * 2.0f) * 0.5f) - (nCntMeshFieldM1 * size.y * 2.0f);

			// 頂点カラーの設定
			pVtx[nVtx].col = COLOR_WHITE;

			// テクスチャ座標の設定
			pVtx[nVtx].tex = D3DXVECTOR2((float)nCntMeshFieldM2, (float)nCntMeshFieldM1);
			pVtx[nVtx].texM = D3DXVECTOR2((float)nCntMeshFieldM2, (float)nCntMeshFieldM1);
		}
	}

	// 法線の設定
	SetNor();

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();

	// インデックスバッファをロックし、頂点番号データへのポインタを取得
	GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

	int nNum = 0;			// 縮退ポリゴン

	// 頂点番号データの設定
	for (int nCntMeshFieldM1 = 0; nCntMeshFieldM1 < m_nNumIdx / 2; nCntMeshFieldM1++)
	{
		if (nCntMeshFieldM1 % ((int)block.x + 2) == ((int)block.x + 1))
		{// 縮退ポリゴンのところ
			pIdx[0] = (nCntMeshFieldM1 - nNum) - 1;
			pIdx[1] = (nCntMeshFieldM1 - nNum) + ((int)block.x + 1);

			nNum++;
		}
		else
		{// 縮退以外のポリゴン
			pIdx[0] = (nCntMeshFieldM1 - nNum) + ((int)block.x + 1);
			pIdx[1] = (nCntMeshFieldM1 - nNum);
		}

		pIdx += 2;
	}

	// インデックスバッファをアンロックする
	GetIdxBuff()->Unlock();
}

//========================================================================
// 外部ファイルに頂点情報を書き出す
//========================================================================
HRESULT CMeshFieldM::WriteData(const char* pFilename)
{
	D3DXVECTOR2 block = GetBlock();
	D3DXVECTOR2 size = GetSize();

	// バイナリ形式でファイルオープン
	std::ofstream file(pFilename, std::ios_base::out | std::ios_base::binary);

	if (file.is_open() == true)
	{// ファイルが開けた
		VERTEX_3D_MULTI* pVtx;
		DWORD* pIdx;

		// 法線を設定
		SetNor();

		// 書き出す
		file.write((const char*)&block, sizeof(block));
		file.write((const char*)&size, sizeof(size));
		file.write((const char*)&m_nNumVtx, sizeof(m_nNumVtx));
		file.write((const char*)&m_nNumIdx, sizeof(m_nNumIdx));

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
		{
			file.write((const char*)&pVtx[nCntVtx], sizeof(*pVtx));
		}

		// 頂点バッファをアンロックする
		GetVtxBuff()->Unlock();

		// インデックスバッファをロックし、頂点番号データへのポインタを取得
		GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntIdx = 0; nCntIdx < m_nNumIdx; nCntIdx++)
		{
			file.write((const char*)&pIdx[nCntIdx], sizeof(*pIdx));
		}

		// インデックスバッファをアンロックする
		GetIdxBuff()->Unlock();

		// ファイルを閉じる
		file.close();
	}
	else
	{// ファイルが開けなかった
		OutputDebugStringA("! ! ! ファイルを開けませんでした ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}

//========================================================================
// 外部ファイルから頂点情報を読み込む
//========================================================================
HRESULT CMeshFieldM::ReadData(const char* pFilename)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// バイナリ形式でファイルオープン
	std::ifstream file(pFilename, std::ios_base::in | std::ios_base::binary);

	if (file.is_open() == true)
	{// ファイルが開けた
		VERTEX_3D_MULTI* pVtx;
		DWORD* pIdx;
		D3DXVECTOR2 block, size;
		int nNumVtx, nNumIdx;

		// 読み込む
		file.read((char*)&block, sizeof(block));
		file.read((char*)&size, sizeof(size));
		file.read((char*)&nNumVtx, sizeof(nNumVtx));
		file.read((char*)&nNumIdx, sizeof(nNumIdx));

		// 現在の設定との整合性をチェック
		if (block == block && size == size && m_nNumVtx == nNumVtx && m_nNumIdx == nNumIdx)
		{// 全て同じなら代入
			block = block;
			size = size;
			m_nNumVtx = nNumVtx;
			m_nNumIdx = nNumIdx;
		}
		else
		{// 1つでも違う場合
			// 頂点バッファを生成
			CreateVtxBuffMulti(nNumVtx);

			// インデックスバッファの生成
			CreateIdxBuff(nNumIdx);

			// 代入
			block = block;
			size = size;
			m_nNumVtx = nNumVtx;
			m_nNumIdx = nNumIdx;
		}

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{
			file.read((char*)&pVtx[nCntVtx], sizeof(*pVtx));
		}

		// 頂点バッファをアンロックする
		GetVtxBuff()->Unlock();

		// インデックスバッファをロックし、頂点番号データへのポインタを取得
		GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntIdx = 0; nCntIdx < nNumIdx; nCntIdx++)
		{
			file.read((char*)&pIdx[nCntIdx], sizeof(*pIdx));
		}

		// インデックスバッファをアンロックする
		GetIdxBuff()->Unlock();

		// 法線を設定
		SetNor();

		// ファイルを閉じる
		file.close();
	}
	else
	{// ファイルが開けなかった
		OutputDebugStringA("! ! ! ファイルを開けませんでした ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}