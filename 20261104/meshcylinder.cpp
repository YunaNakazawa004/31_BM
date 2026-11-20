//=============================================================================
// 
// メッシュシリンダー [ meshcylinder.cpp ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#include "meshcylinder.h"

#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "input.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//========================================================================
// メッシュシリンダークラスの生成処理
//========================================================================
CMeshCylinder* CMeshCylinder::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block,
	const D3DXVECTOR2 size, const D3DXCOLOR col, const bool bInside, const char* pFilename, const CObject::TYPE type, const int nPriority)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return NULL;
	}
#endif

	CMeshCylinder* pMeshCylinder = NULL;

	if (pMeshCylinder == NULL)
	{// NULLチェック
		// メッシュシリンダーの生成
		pMeshCylinder = new CMeshCylinder(nPriority);
	}

	if (pMeshCylinder != NULL)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pMeshCylinder->Init(pos, rot, block, size, col, bInside)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! メッシュシリンダーの初期化に失敗しました ! ! !\n");

			return NULL;
		}

		// テクスチャを設定
		CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
		pMeshCylinder->m_nIdxTexture = pTexture->Register(pFilename);

		// 種類を設定
		pMeshCylinder->SetType(type);

		return pMeshCylinder;
	}

	OutputDebugStringA("! ! ! メッシュシリンダーの生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// メッシュシリンダークラスのコンストラクタ
//========================================================================
CMeshCylinder::CMeshCylinder(const int nPriority) :CMesh(nPriority)
{
	// メッシュシリンダークラスの値をクリア
	m_col = COLOR_WHITE;
	m_nNumVtx = 0;
	m_nNumIdx = 0;
	m_bInside = true;
	m_bDisp = true;
}

//========================================================================
// メッシュシリンダークラスのデストラクタ
//========================================================================
CMeshCylinder::~CMeshCylinder()
{
}

//=============================================================================
// メッシュシリンダーの初期化処理
//=============================================================================
HRESULT CMeshCylinder::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
	const D3DXVECTOR2 block, const D3DXVECTOR2 size, const D3DXCOLOR col, const bool bInside)
{
	// ローカル変数宣言
	VERTEX_3D* pVtx;						// 頂点情報へのポインタ
	DWORD* pIdx;							// インデックス情報へのポインタ

	SetPosition(pos);
	SetRotation(rot);
	SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	SetBlock(block);
	CMesh::SetSize(size);
	m_col = col;
	m_nNumVtx = ((int)block.x + 1) * ((int)block.y + 1);
	m_nNumIdx = (((int)block.x) * ((int)block.y) * 2) + (((int)block.y - 1) * 4) + 2;
	m_bInside = bInside;

	// 頂点バッファを生成
	if (FAILED(CreateVtxBuff(m_nNumVtx)))
	{// 失敗した場合
		return E_FAIL;
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntMeshCylinder1 = 0; nCntMeshCylinder1 < (int)block.y + 1; nCntMeshCylinder1++)
	{
		for (int nCntMeshCylinder2 = 0; nCntMeshCylinder2 < (int)block.x + 1; nCntMeshCylinder2++)
		{
			// 頂点座標の設定
			if (bInside == true)
			{// 内側
				pVtx[0].pos.x = sinf(nCntMeshCylinder2 * ((D3DX_PI * 2.0f) / block.x)) * size.x;
				pVtx[0].pos.y = size.y - ((size.y / block.y) * nCntMeshCylinder1);
				pVtx[0].pos.z = cosf(nCntMeshCylinder2 * ((D3DX_PI * 2.0f) / block.x)) * size.x;
			}	
			else
			{// 外側
				pVtx[0].pos.x = sinf(-nCntMeshCylinder2 * ((D3DX_PI * 2.0f) / block.x)) * size.x;
				pVtx[0].pos.y = size.y - ((size.y / block.y) * nCntMeshCylinder1);
				pVtx[0].pos.z = cosf(-nCntMeshCylinder2 * ((D3DX_PI * 2.0f) / block.x)) * size.x;
			}

			// rhwの設定
			pVtx[0].nor = D3DXVECTOR3(pVtx[0].pos.x, 0.0f, pVtx[0].pos.z);
			D3DXVec3Normalize(&pVtx[0].nor, &pVtx[0].nor);

			// 頂点カラーの設定
			pVtx[0].col = col;

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2((float)nCntMeshCylinder2, (float)nCntMeshCylinder1);

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

	int nNum = 0;			// 縮退ポリゴン

	// 頂点番号データの設定
	for (int nCntMeshCylinder1 = 0; nCntMeshCylinder1 < m_nNumIdx / 2; nCntMeshCylinder1++)
	{
		if (nCntMeshCylinder1 % ((int)block.x + 2) == ((int)block.x + 1))
		{// 縮退ポリゴンのところ
			pIdx[0] = (nCntMeshCylinder1 - nNum) - 1;
			pIdx[1] = (nCntMeshCylinder1 - nNum) + ((int)block.x + 1);

			nNum++;
		}
		else
		{// 縮退以外のポリゴン
			pIdx[0] = (nCntMeshCylinder1 - nNum) + ((int)block.x + 1);
			pIdx[1] = (nCntMeshCylinder1 - nNum);
		}

		pIdx += 2;
	}

	// インデックスバッファをアンロックする
	GetIdxBuff()->Unlock();

	return S_OK;
}

//=============================================================================
// メッシュシリンダーの終了処理
//=============================================================================
void CMeshCylinder::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//=============================================================================
// メッシュシリンダーの更新処理
//=============================================================================
void CMeshCylinder::Update(void)
{
	// 更新処理
	CMesh::Update();
}

//=============================================================================
// メッシュシリンダーの描画処理
//=============================================================================
void CMeshCylinder::Draw(void)
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

	// カリングを戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//========================================================================
// 頂点の位置設定
//========================================================================
void CMeshCylinder::SetPosVtx(const int nCntVtx, const D3DXVECTOR3 pos)
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
D3DXVECTOR3 CMeshCylinder::GetPosVtx(const int nCntVtx)
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
void CMeshCylinder::SetSize(const D3DXVECTOR2 size)
{
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ
	D3DXVECTOR2 block = GetBlock();

	// サイズを設定
	CMesh::SetSize(size);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntMeshCylinder1 = 0; nCntMeshCylinder1 < (int)block.y + 1; nCntMeshCylinder1++)
	{
		for (int nCntMeshCylinder2 = 0; nCntMeshCylinder2 < (int)block.x + 1; nCntMeshCylinder2++)
		{
			float fAngle = ((D3DX_PI * 2.0f) / block.x);
			float fSize = (size.x + (size.y * -(float)((nCntMeshCylinder2 + (nCntMeshCylinder1 * (((int)block.x) + 1))) % ((int)block.y + 1))));

			// 頂点座標の設定
			pVtx[0].pos.x = sinf((float)((nCntMeshCylinder2 + (nCntMeshCylinder1 * ((int)block.x + 1))) / ((int)block.y + 1)) * -fAngle) * fSize;
			pVtx[0].pos.y = 0.0f;
			pVtx[0].pos.z = cosf((float)((nCntMeshCylinder2 + (nCntMeshCylinder1 * ((int)block.x + 1))) / ((int)block.y + 1)) * fAngle) * fSize;

			pVtx++;
		}
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//========================================================================
// 頂点カラー設定
//========================================================================
void CMeshCylinder::SetColor(const D3DXCOLOR col)
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
void CMeshCylinder::SetColor(const int nCntVtx, const D3DXCOLOR col)
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
void CMeshCylinder::SetTexUV(const int nCntVtx, const float ftexU, const float ftexV)
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
void CMeshCylinder::SetTexScroll(const int nCntVtx, const float fScrollU, const float fScrollV)
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

//=============================================================================
// メッシュシリンダーの当たり判定
//=============================================================================
bool CMeshCylinder::Collision(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, const float fRadius, const float fHeight)
{
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR2 block = GetBlock();
	D3DXVECTOR2 size = GetSize();

	bool bColl = false;

	for (int nCnt = 0; nCnt < block.x; nCnt++)
	{
		D3DXVECTOR3 start, end;
		float fXS, fZS, fXE, fZE;
		D3DXVECTOR3 vecLine, vecMove, vecToPos, vecToPosOld, vecNor, vecMoveRef, vecMoveDest;		// 各ベクトル
		D3DXVECTOR3 vecLineW, posDest;
		float fRate, fDot;

		if (m_bInside == true)
		{// 内側
			fXS = sinf(nCnt * ((D3DX_PI * 2.0f) / block.x)) * (size.x - fRadius);
			fZS = cosf(nCnt * ((D3DX_PI * 2.0f) / block.x)) * (size.x - fRadius);

			fXE = sinf((nCnt + 1) * ((D3DX_PI * 2.0f) / block.x)) * (size.x - fRadius);
			fZE = cosf((nCnt + 1) * ((D3DX_PI * 2.0f) / block.x)) * (size.x - fRadius);
		}
		else
		{// 外側
			fXS = sinf(nCnt * ((D3DX_PI * 2.0f) / block.x)) * (size.x + fRadius);
			fZS = cosf(nCnt * ((D3DX_PI * 2.0f) / block.x)) * (size.x + fRadius);

			fXE = sinf((nCnt + 1) * ((D3DX_PI * 2.0f) / block.x)) * (size.x + fRadius);
			fZE = cosf((nCnt + 1) * ((D3DX_PI * 2.0f) / block.x)) * (size.x + fRadius);
		}

		// 始点
		start.x = pos.x + fXS;
		start.y = 0.0f;
		start.z = pos.z + fZS;

		// 終点
		end.x = pos.x + fXE;
		end.y = 0.0f;
		end.z = pos.z + fZE;

		// 境界線ベクトル
		vecLine.x = (end.x) - (start.x);
		vecLine.y = 0.0f;
		vecLine.z = (end.z) - (start.z);

		// 移動ベクトル
		vecMove.x = pPos->x - pPosOld->x;
		vecMove.y = 0.0f;
		vecMove.z = pPos->z - pPosOld->z;

		vecToPos.x = pPos->x - (start.x);
		vecToPos.y = 0.0f;
		vecToPos.z = pPos->z - (start.z);

		vecToPosOld.x = pPosOld->x - (start.x);
		vecToPosOld.y = 0.0f;
		vecToPosOld.z = pPosOld->z - (start.z);

		// 法線ベクトル
		if (m_bInside == true)
		{// 内側
			vecNor.x = (vecLine.x * cosf(D3DX_PI * 0.5f)) + (vecLine.z * sinf(D3DX_PI * 0.5f));
			vecNor.y = 0.0f;
			vecNor.z = (vecLine.x * sinf(-D3DX_PI * 0.5f)) - (vecLine.z * cosf(-D3DX_PI * 0.5f));
			D3DXVec3Normalize(&vecNor, &vecNor);		// ベクトルを正規化する
		}
		else
		{// 外側
			vecNor.x = (vecLine.x * cosf(-D3DX_PI * 0.5f)) + (vecLine.z * sinf(-D3DX_PI * 0.5f));
			vecNor.y = 0.0f;
			vecNor.z = (vecLine.x * sinf(D3DX_PI * 0.5f)) - (vecLine.z * cosf(D3DX_PI * 0.5f));
			D3DXVec3Normalize(&vecNor, &vecNor);		// ベクトルを正規化する
		}

		// 内積
		fDot = (-vecMove.x * vecNor.x) + (-vecMove.z * vecNor.z);

		// 交点の割合
		fRate = ((vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z)) /
			((vecLine.z * vecMove.x) - (vecLine.x * vecMove.z));

		// 反射後の移動ベクトル
		vecMoveRef.x = vecMove.x + ((vecNor.x * fDot) * 2);
		vecMoveRef.y = 0.0f;
		vecMoveRef.z = vecMove.z + ((vecNor.z * fDot) * 2);

		// 現在の外積
		float fPosLine = (float)((int)(((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z)) * 1.0f) / (int)1);

		// 前回の外積
		float fPosOldLine = (float)((int)(((vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z)) * 1.0f) / (int)1);

		if (fRate >= 0.0f && fRate <= 1.0f)
		{// 交点の割合が範囲内
			if ((m_bInside == true && fPosLine < 0.0f && (fPosOldLine >= 0.0f)) ||
				(m_bInside == false && fPosLine > 0.0f && (fPosOldLine <= 0.0f)))
			{// 交差した
				if ((pos.y - fHeight <= pPos->y) &&
					(pos.y + size.y >= pPos->y))
				{// 高さが合っている
					bColl = true;

					// 現在の移動ベクトル
					D3DXVECTOR3 move = vecMove;
					move.y = 0.0f;

					float fDotN = D3DXVec3Dot(&move, &vecNor);

					if (fDotN < 0.0f)
					{// 壁に向かっているときだけ法線成分を消す
						vecMoveDest = move - (vecNor * fDotN);
					}
					else
					{// 初期値
						vecMoveDest = DEFAULT_VECTOR3;
					}

					// 位置をずらす
					pPos->x = start.x + (vecLine.x * fRate) + vecMoveDest.x;
					pPos->z = start.z + (vecLine.z * fRate) + vecMoveDest.z;
				}
			}
		}
	}

	return bColl;
}