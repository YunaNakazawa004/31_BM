//========================================================================
// 
// モデル [ model.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "model.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

#include "line.h"
#include "object.h"

//************************************************************************
// マクロ定義
//************************************************************************

//========================================================================
// モデルクラスの生成処理
//========================================================================
CModel* CModel::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const char* pFilename)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return NULL;
	}
#endif

	CModel* pModel = NULL;

	if (pModel == NULL)
	{// NULLチェック
		// モデルの生成
		pModel = new CModel;
	}

	if (pModel != NULL)
	{// NULLチェック
		// モデルファイル設定
		pModel->BindModel(pFilename);

		// 初期化処理
		if (FAILED(pModel->Init(pos, rot)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! モデルの初期化に失敗しました ! ! !\n");

			return NULL;
		}

		return pModel;
	}

	OutputDebugStringA("! ! ! モデルの生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// モデルクラスのコンストラクタ
//========================================================================
CModel::CModel()
{
	// モデルクラスの値をクリア
	m_pMesh = NULL;
	m_pBuffMat = NULL;
	memset(&m_apTexture[0], -1, sizeof m_apTexture);
	m_pFileName = NULL;
	m_dwNumMat = 0;
	m_VtxMax = DEFAULT_VECTOR3;
	m_VtxMin = DEFAULT_VECTOR3;
	m_col = COLOR_WHITE;
	m_bDisp = true;
	m_pParent = NULL;
}

//========================================================================
// モデルクラスのデストラクタ
//========================================================================
CModel::~CModel()
{
}

//========================================================================
// モデルクラスの初期化処理
//========================================================================
HRESULT CModel::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	D3DXMATERIAL* pMat;
	int nNumVtx;			// 頂点数
	DWORD dwSizeFVF;		// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;			// 頂点バッファへのポインタ

	// 初期化処理
	CPart::Init(pos, rot);

	// 最大値最小値の初期化
	m_VtxMax = D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f);
	m_VtxMin = D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f);

	// Xファイルの読み込み
	if (FAILED(D3DXLoadMeshFromX(m_pFileName,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&m_pBuffMat,
		NULL,
		&m_dwNumMat,
		&m_pMesh)))
	{// ファイルの読み込みに失敗
		OutputDebugStringA("! ! ! Xファイルの読み込みに失敗しました ! ! !\n");

		return E_FAIL;
	}

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	// 色を保存
	m_col = pMat->MatD3D.Diffuse;

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != NULL)
		{// テクスチャファイルが存在する
			m_apTexture[nCntMat] = pTexture->Register(pMat[nCntMat].pTextureFilename);
		}
	}

	// 頂点数を取得
	nNumVtx = m_pMesh->GetNumVertices();

	// 頂点フォーマットのサイズを取得
	dwSizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	// 頂点バッファをロック
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;		// 頂点座標の代入

		if (vtx.x > m_VtxMax.x)
		{// Xの最大値
			m_VtxMax.x = vtx.x;
		}

		if (vtx.x < m_VtxMin.x)
		{// Xの最小値
			m_VtxMin.x = vtx.x;
		}

		if (vtx.y > m_VtxMax.y)
		{// Yの最大値
			m_VtxMax.y = vtx.y;
		}

		if (vtx.y < m_VtxMin.y)
		{// Yの最小値
			m_VtxMin.y = vtx.y;
		}

		if (vtx.z > m_VtxMax.z)
		{// Zの最大値
			m_VtxMax.z = vtx.z;
		}

		if (vtx.z < m_VtxMin.z)
		{// Zの最小値
			m_VtxMin.z = vtx.z;
		}

		pVtxBuff += dwSizeFVF;		// 頂点バッファのサイズ分ポインタを進める
	}

	// 頂点バッファをアンロック
	m_pMesh->UnlockVertexBuffer();

	return S_OK;
}

//========================================================================
// モデルクラスの終了処理
//========================================================================
void CModel::Uninit(void)
{
	// メッシュの破棄
	if (m_pMesh != NULL)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	// マテリアルの破棄
	if (m_pBuffMat != NULL)
	{
		m_pBuffMat->Release();
		m_pBuffMat = NULL;
	}

	// テクスチャの破棄
	for (int nCntModel = 0; nCntModel < MAX_MODEL_TEXTURE; nCntModel++)
	{
		m_apTexture[nCntModel] = -1;
	}

	// 終了処理
	CPart::Uninit();
}

//========================================================================
// モデルクラスの更新処理
//========================================================================
void CModel::Update(void)
{
}

//========================================================================
// モデルクラスの描画処理
//========================================================================
void CModel::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxScale, mtxWorld;		// 計算用マトリックス
	D3DXMATRIX mtxParent;									// 親のマトリックス
	D3DMATERIAL9 matDef;									// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;										// マテリアルデータへのポインタ

	// 描画処理
	CPart::Draw();

	// マトリックスを取得
	mtxWorld = GetMtxWorld();

	// パーツの「親のマトリックス」を設定
	if (m_pParent != NULL)
	{// 親モデルがある場合
		// 親モデルのマトリックスを取得する
		mtxParent = m_pParent->GetMtxWorld();
	}
	else
	{// 親モデルがない場合
		// 現在（最新）のマトリックスを取得する[ = プレイヤーのマトリックス]
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxParent);

	// ワールドマトリックスを保存
	SetMtxWorld(mtxWorld);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		// 色保存用
		D3DXCOLOR MatCol = pMat->MatD3D.Diffuse;

		// マテリアルの色を変更
		pMat->MatD3D.Diffuse = m_col;

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// 色を戻す
		pMat->MatD3D.Diffuse = MatCol;

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_apTexture[nCntMat]));

		// モデルパーツの描画
		m_pMesh->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//========================================================================
// 頂点カラー設定
//========================================================================
void CModel::SetColor(const D3DXCOLOR col)
{
	m_col = col;
}

//========================================================================
// 当たり判定
//========================================================================
bool CModel::Collision(const D3DXVECTOR3 posMe, const D3DXVECTOR3 rotMe, const D3DXVECTOR3 scaleMe, D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove,
	const float fRadius, const float fHeight)
{
	bool bLand = false;		// 着地しているか
	int nCntLand = 0;	// 辺の内側に入った数(4回入っていれば、オブジェクトの内側にいる)
	D3DXVECTOR3 pos = posMe;		// 現在位置

	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		D3DXVECTOR3 start, end;			// 始点/終点
		float fXS, fZS, fXE, fZE;		// 始点のXZ座標/終点のXZ座標
		float fMinY, fMaxY;				// Y座標の最低値と最大値

		D3DXVECTOR3 vecLine, vecMove, vecToPos, vecToPosOld, vecNor, vecMoveRef, vecMoveDest;		// 各ベクトル
		D3DXVECTOR3 vecLineW, posDest;

		float fRate, fDot;		// 交点の割合/内積
		float fXLength = (m_VtxMax.x - m_VtxMin.x) * scaleMe.x;		// X方向の長さ
		float fZLength = (m_VtxMax.z - m_VtxMin.z) * scaleMe.z;		// Z方向の長さ

		float fOffXS = (nCnt == 0 || nCnt == 3) ? +(fXLength * 0.5f + fRadius) : -(fXLength * 0.5f + fRadius);		// 向き0のときの座標
		float fOffZS = (nCnt == 0 || nCnt == 1) ? +(fZLength * 0.5f + fRadius) : -(fZLength * 0.5f + fRadius);		// 向き0のときの座標
		float fOffXE = (nCnt == 1 || nCnt == 0) ? +(fXLength * 0.5f + fRadius) : -(fXLength * 0.5f + fRadius);		// 向き0のときの座標
		float fOffZE = (nCnt == 1 || nCnt == 2) ? +(fZLength * 0.5f + fRadius) : -(fZLength * 0.5f + fRadius);		// 向き0のときの座標

		float fSin = sinf(-rotMe.y);		// 現在向きのsin
		float fCos = cosf(-rotMe.y);		// 現在向きのcos

		// 始点のXZ座標
		fXS = fOffXS * fCos - fOffZS * fSin;
		fZS = fOffXS * fSin + fOffZS * fCos;

		// 終点のXZ座標
		fXE = fOffXE * fCos - fOffZE * fSin;
		fZE = fOffXE * fSin + fOffZE * fCos;

		// Y座標の最低値/最大値
		fMinY = pos.y + (m_VtxMin.y * scaleMe.y) - fHeight;
		fMaxY = pos.y + (m_VtxMax.y * scaleMe.y);

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

		// 始点から現在位置へのベクトル
		vecToPos.x = pPos->x - (start.x);
		vecToPos.y = 0.0f;
		vecToPos.z = pPos->z - (start.z);

		// 始点から前回位置へのベクトル
		vecToPosOld.x = pPosOld->x - (start.x);
		vecToPosOld.y = 0.0f;
		vecToPosOld.z = pPosOld->z - (start.z);

		// 法線ベクトル
		vecNor.x = -vecLine.z;
		vecNor.y = 0.0f;
		vecNor.z = vecLine.x;
		D3DXVec3Normalize(&vecNor, &vecNor);

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
			if (fPosLine >= 0.0f && (fPosOldLine <= 0.0f))
			{// 交差した
				if ((fMinY <= pPos->y) &&
					(fMaxY >= pPos->y))
				{// 高さが合っている
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

		if (fPosLine >= 0.0f)
		{// 今の位置が内側にいる
			nCntLand++;

			if (nCntLand == 4)
			{// 全ての内側に入っていたら
				if ((fMinY <= pPos->y) &&
					(fMaxY >= pPos->y))
				{// 範囲内
				}

				// 位置調整
				if ((fMinY >= pPosOld->y) &&
					(fMinY <= pPos->y))
				{// 下からの当たり判定
					pPos->y = fMinY;
					pMove->y = -0.5f;							// 移動量を0にする
				}
				else if ((fMaxY <= pPosOld->y) &&
					(fMaxY >= pPos->y))
				{// 上からの当たり判定
					pPos->y = fMaxY;
					pMove->y = 0.0f;							// 移動量を0にする

					bLand = true;		// 着地フラグ
				}
			}
		}
	}

	return bLand;
}