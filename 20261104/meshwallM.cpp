//=============================================================================
// 
// メッシュウォール [ meshwallM.cpp ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#include "meshwallM.h"

#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "input.h"
#include "debugproc.h"
#include "line.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//========================================================================
// メッシュウォールクラスの生成処理
//========================================================================
CMeshWallM* CMeshWallM::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size,
	const char* pFilename1, const char* pFilename2, const CObject::TYPE type, const int nPriority)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return NULL;
	}
#endif

	CMeshWallM* pMeshWallM = NULL;

	if (pMeshWallM == NULL)
	{// NULLチェック
		// メッシュウォールの生成
		pMeshWallM = new CMeshWallM(nPriority);
	}

	if (pMeshWallM != NULL)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pMeshWallM->Init(pos, rot, block, size)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! メッシュウォールの初期化に失敗しました ! ! !\n");

			return NULL;
		}

		// テクスチャを設定
		CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
		pMeshWallM->m_aIdxTexture[0] = pTexture->Register(pFilename1);
		pMeshWallM->m_aIdxTexture[1] = pTexture->Register(pFilename2);

		// 種類を設定
		pMeshWallM->SetType(type);

		return pMeshWallM;
	}

	OutputDebugStringA("! ! ! メッシュウォールの生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// メッシュウォールクラスのコンストラクタ
//========================================================================
CMeshWallM::CMeshWallM(const int nPriority) :CMesh(nPriority)
{
	// メッシュウォールクラスの値をクリア
	m_col = COLOR_WHITE;
	m_nNumVtx = 0;
	m_nNumIdx = 0;
	m_bDisp = true;
}

//========================================================================
// メッシュウォールクラスのデストラクタ
//========================================================================
CMeshWallM::~CMeshWallM()
{
}

//========================================================================
// メッシュウォールクラスの初期化処理(オーバーロード)
//========================================================================
HRESULT CMeshWallM::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
	const D3DXVECTOR2 block, const D3DXVECTOR2 size)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
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
	for (int nCntMeshWall1 = 0; nCntMeshWall1 < (int)block.y + 1; nCntMeshWall1++)
	{
		for (int nCntMeshWall2 = 0; nCntMeshWall2 < (int)block.x + 1; nCntMeshWall2++)
		{
			// 頂点座標の設定
			pVtx[0].pos.x = ((float)nCntMeshWall2 * size.x * 2.0f) - ((block.x * size.x * 2.0f) * 0.5f);
			pVtx[0].pos.y = -((float)nCntMeshWall1 * size.y * 2.0f) + (block.y * size.y * 2.0f);
			pVtx[0].pos.z = 0.0f;

			// rhwの設定
			pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

			// 頂点カラーの設定
			pVtx[0].col = COLOR_WHITE;

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2((float)nCntMeshWall2, (float)nCntMeshWall1);
			pVtx[0].texM = D3DXVECTOR2((float)nCntMeshWall2, (float)nCntMeshWall1);

			pVtx++;
		}
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();

	// 法線の設定
	SetNor();

	// インデックスバッファを生成
	if (FAILED(CreateIdxBuff(m_nNumIdx)))
	{// 失敗した場合
		return E_FAIL;
	}

	// インデックスバッファをロックし、頂点番号データへのポインタを取得
	GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

	int nNum = 0;			// 縮退ポリゴン

	// 頂点番号データの設定
	for (int nCntMeshWall1 = 0; nCntMeshWall1 < m_nNumIdx / 2; nCntMeshWall1++)
	{
		if (nCntMeshWall1 % ((int)block.x + 2) == ((int)block.x + 1))
		{// 縮退ポリゴンのところ
			nNum++;

			pIdx[0] = nCntMeshWall1 - nNum;
			pIdx[1] = nCntMeshWall1 - nNum + ((int)block.x + 2);
		}
		else
		{// 縮退以外のポリゴン
			pIdx[0] = (nCntMeshWall1 - nNum) + ((int)block.x + 1);
			pIdx[1] = (nCntMeshWall1 - nNum);
		}

		pIdx += 2;
	}

	// インデックスバッファをアンロックする
	GetIdxBuff()->Unlock();

	return S_OK;
}

//========================================================================
// メッシュウォールクラスの終了処理
//========================================================================
void CMeshWallM::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//=============================================================================
// メッシュウォールの更新処理
//=============================================================================
void CMeshWallM::Update(void)
{
}

//========================================================================
// メッシュウォールクラスの描画処理
//========================================================================
void CMeshWallM::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ

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
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVtx, 0,
		m_nNumIdx - 2);

	// テクスチャステージステートの設定
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	// 頂点法線正規化をオフ
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}

//========================================================================
// テクスチャを結びつけ
//========================================================================
void CMeshWallM::BindTexture(const int* pTexture)
{
	if (pTexture != NULL)
	{// NULLチェック
		m_aIdxTexture[0] = pTexture[0];
		m_aIdxTexture[1] = pTexture[1];
	}
}

//========================================================================
// テクスチャを結びつけ(オーバーライド)
//========================================================================
void CMeshWallM::BindTexture(const int nIdx, const int nTexture)
{
	if (nIdx < 0 || nIdx > 1)
	{// 範囲外
		return;
	}

	m_aIdxTexture[nIdx] = nTexture;
}

//========================================================================
// 頂点の位置設定
//========================================================================
void CMeshWallM::SetPosVtx(const int nCntVtx, const D3DXVECTOR3 pos)
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
D3DXVECTOR3 CMeshWallM::GetPosVtx(const int nCntVtx)
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
void CMeshWallM::SetColor(const D3DXCOLOR col)
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
void CMeshWallM::SetColor(const int nCntVtx, const D3DXCOLOR col)
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
// 頂点カラー取得(オーバーロード)
//========================================================================
D3DXCOLOR CMeshWallM::GetColor(const int nCntVtx)
{
	D3DXCOLOR col;
	VERTEX_3D_MULTI* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	col = pVtx[nCntVtx].col;

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();

	return col;
}

//========================================================================
// テクスチャ座標設定
//========================================================================
void CMeshWallM::SetTexUV(const int nCntVtx, const float ftexU, const float ftexV)
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
// マルチテクスチャ座標設定
//========================================================================
void CMeshWallM::SetTexMUV(const int nCntVtx, const float ftexU, const float ftexV)
{
	VERTEX_3D_MULTI* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[nCntVtx].texM = D3DXVECTOR2(ftexU, ftexV);

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// テクスチャスクロール
//========================================================================
void CMeshWallM::SetTexScroll(const int nCntVtx, const float fScrollU, const float fScrollV)
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
void CMeshWallM::SetTexMScroll(const int nCntVtx, const float fScrollU, const float fScrollV)
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
// 全ての頂点をマルチテクスチャスクロール
//========================================================================
void CMeshWallM::SetTexMScrollAll(const float fScrollU, const float fScrollV)
{
	VERTEX_3D_MULTI* pVtx;
	D3DXVECTOR2 block = GetBlock();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntMeshWall1 = 0; nCntMeshWall1 < (int)block.y + 1; nCntMeshWall1++)
	{
		for (int nCntMeshWall2 = 0; nCntMeshWall2 < (int)block.x + 1; nCntMeshWall2++)
		{
			// テクスチャ座標の設定
			pVtx[0].texM.x += fScrollU;
			pVtx[0].texM.y += fScrollV;

			pVtx++;
		}
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// マルチテクスチャを最大サイズで設定
//========================================================================
void CMeshWallM::SetTexMBig(void)
{
	VERTEX_3D_MULTI* pVtx;
	D3DXVECTOR2 block = GetBlock();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntMeshWall1 = 0; nCntMeshWall1 < (int)block.y + 1; nCntMeshWall1++)
	{
		for (int nCntMeshWall2 = 0; nCntMeshWall2 < (int)block.x + 1; nCntMeshWall2++)
		{
			// テクスチャ座標の設定
			pVtx[0].texM.x = (float)(nCntMeshWall2) / (block.x);
			pVtx[0].texM.y = (float)(nCntMeshWall1) / (block.y);

			pVtx++;
		}
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// マルチテクスチャを最小サイズで設定
//========================================================================
void CMeshWallM::SetTexMSmall(void)
{
	VERTEX_3D_MULTI* pVtx;
	D3DXVECTOR2 block = GetBlock();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntMeshWall1 = 0; nCntMeshWall1 < (int)block.y + 1; nCntMeshWall1++)
	{
		for (int nCntMeshWall2 = 0; nCntMeshWall2 < (int)block.x + 1; nCntMeshWall2++)
		{
			// テクスチャ座標の設定
			pVtx[0].texM.x = (float)(nCntMeshWall2);
			pVtx[0].texM.y = (float)(nCntMeshWall1);

			pVtx++;
		}
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// 法線を設定
//========================================================================
void CMeshWallM::SetNor(void)
{
	VERTEX_3D_MULTI* pVtx;					// 頂点情報へのポインタ
	D3DXVECTOR3 nor, nor1, nor2, nor3, nor4, nor5, nor6, vec1, vec2;
	D3DXVECTOR2 block = GetBlock();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 法線の設定
	for (int nCntMeshWallM1 = 0; nCntMeshWallM1 < (int)block.y + 1; nCntMeshWallM1++)
	{
		for (int nCntMeshWallM2 = 0; nCntMeshWallM2 < (int)block.x + 1; nCntMeshWallM2++)
		{
			// 頂点番号
			int nVtx = nCntMeshWallM2 + (nCntMeshWallM1 * ((int)block.x + 1));

			D3DXVECTOR3 vec1, vec2;
			D3DXVECTOR3 nor, norA, norB, norC, norD, norE, norF;

			nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 法線の設定
			if ((nCntMeshWallM1 == 0 && nCntMeshWallM2 == (int)block.x))
			{// 法線ひとつ(右上)(▽)
				vec1 = pVtx[nVtx + ((int)block.x + 1)].pos - pVtx[nVtx].pos;
				vec2 = pVtx[nVtx - 1].pos - pVtx[nVtx].pos;

				// 外積
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&nor, &nor);
			}
			else if (nCntMeshWallM1 == (int)block.y && nCntMeshWallM2 == 0)
			{// 法線ひとつ(左下)(△)
				vec1 = pVtx[nVtx - ((int)block.x + 1)].pos - pVtx[nVtx].pos;
				vec2 = pVtx[nVtx + 1].pos - pVtx[nVtx].pos;

				// 外積
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 正規化
				D3DXVec3Normalize(&nor, &nor);
			}
			else if (nCntMeshWallM1 == 0 && nCntMeshWallM2 == 0)
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
			else if (nCntMeshWallM1 == (int)block.y && nCntMeshWallM2 == (int)block.x)
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
			else if (nCntMeshWallM1 == 0)
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
			else if (nCntMeshWallM1 == (int)block.y)
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
			else if (nCntMeshWallM2 == 0)
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
			else if (nCntMeshWallM2 == (int)block.x)
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
// 現在位置に対するポリゴン番号を取得
//========================================================================
D3DXVECTOR2 CMeshWallM::GetPolygonIdx(const D3DXVECTOR3 pos)
{
	// ローカル変数
	D3DXVECTOR2 polygonIdx = { -1.0f,-1.0f };
	D3DXVECTOR2 posRotDefP;		// 角度を直した位置
	D3DXVECTOR2 posP;			// 与えられた位置のオフセット位置
	D3DXVECTOR3 posM = GetPosition();
	D3DXVECTOR3 rot = GetRotation();
	D3DXVECTOR2 block = GetBlock();
	D3DXVECTOR2 size = GetSize();

	// 壁の位置角度分自分の位置をずらす
	posRotDefP.x = cosf(-rot.y) * (pos.x - posM.x) + sinf(-rot.y) * (pos.z - posM.z);
	posP.x = posRotDefP.x + (size.x * block.x);		// 角度を直した位置をずらす
	posP.y = pos.y - (size.y * block.y * 2.0f);		// 壁は原点が底辺にあるので高さを２倍減らす

	// ポリゴン番号を計算
	polygonIdx.x = (posP.x / (size.x * 2.0f));
	polygonIdx.y = (posP.y / -(size.y * 2.0f));

	if (polygonIdx.x >= 0 && polygonIdx.x < block.x && polygonIdx.y >= 0 && polygonIdx.y < block.y)
	{// 範囲内の数値
		return polygonIdx;
	}

	return D3DXVECTOR2(-1.0f, -1.0f);
}