//=============================================================================
// 
// メッシュオービット [ meshorbit.cpp ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#include "meshorbit.h"

#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "input.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ALPHA_MINUS				(0.03f)								// アルファ値の減衰

//========================================================================
// メッシュオービットクラスの生成処理
//========================================================================
CMeshOrbit* CMeshOrbit::Create(const D3DXVECTOR3 Offset0, const D3DXVECTOR3 Offset1, const D3DXCOLOR col0, 
	const D3DXCOLOR col1, const D3DMATRIX* pMtxParent, 
	const char* pFilename, const CObject::TYPE type, const int nPriority)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return NULL;
	}
#endif

	CMeshOrbit* pMeshOrbit = NULL;

	if (pMeshOrbit == NULL)
	{// NULLチェック
		// メッシュオービットの生成
		pMeshOrbit = new CMeshOrbit(nPriority);
	}

	if (pMeshOrbit != NULL)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pMeshOrbit->Init(Offset0, Offset1, col0, col1, pMtxParent)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! メッシュオービットの初期化に失敗しました ! ! !\n");

			return NULL;
		}

		// テクスチャを設定
		CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
		pMeshOrbit->m_nIdxTexture = pTexture->Register(pFilename);

		// 種類を設定
		pMeshOrbit->SetType(type);

		return pMeshOrbit;
	}

	OutputDebugStringA("! ! ! メッシュオービットの生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// メッシュオービットクラスのコンストラクタ
//========================================================================
CMeshOrbit::CMeshOrbit(const int nPriority) :CMesh(nPriority)
{
	// メッシュオービットクラスの値をクリア
	memset(&m_aOffset[0], 0, sizeof m_aOffset);
	memset(&m_aCol[0], 0, sizeof m_aCol);
	memset(&m_aPosPoint[0], 0, sizeof m_aPosPoint);
	memset(&m_aColPoint[0], 0, sizeof m_aColPoint);
	m_bDisp = true;
}

//========================================================================
// メッシュオービットクラスのデストラクタ
//========================================================================
CMeshOrbit::~CMeshOrbit()
{
}

//=============================================================================
// メッシュオービットの初期化処理
//=============================================================================
HRESULT CMeshOrbit::Init(const D3DXVECTOR3 Offset0, const  D3DXVECTOR3 Offset1, const  D3DXCOLOR col0, 
	const D3DXCOLOR col1, const  D3DMATRIX* pMtxParent)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	VERTEX_3D* pVtx;						// 頂点情報へのポインタ
	DWORD* pIdx;							// インデックス情報へのポインタ

	SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	m_aOffset[0] = Offset0;
	m_aOffset[1] = Offset1;
	m_aCol[0] = col0;
	m_aCol[1] = col1;
	m_pMtxParent = *pMtxParent;
	D3DXMATRIX mtxParent = m_pMtxParent;

	for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint += 2)
	{
		D3DXVec3TransformCoord(&m_aPosPoint[nCntPoint], &m_aOffset[0], &mtxParent);
		D3DXVec3TransformCoord(&m_aPosPoint[nCntPoint + 1], &m_aOffset[1], &mtxParent);
	}

	// 頂点バッファを生成
	if (FAILED(CreateVtxBuff(MAX_ORBIT_VTX)))
	{// 失敗した場合
		return E_FAIL;
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
	{
		// 頂点座標の設定
		pVtx[0].pos = m_aPosPoint[nCntPoint];

		// rhwの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 頂点カラーの設定
		pVtx[0].col = COLOR_WHITE;

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2((float)(nCntPoint / 2), (float)((nCntPoint % 2) ? 1.0f : 0.0f));

		pVtx++;
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();

	// インデックスバッファを生成
	if (FAILED(CreateIdxBuff(MAX_ORBIT_VTX)))
	{// 失敗した場合
		return E_FAIL;
	}

	// インデックスバッファをロックし、頂点番号データへのポインタを取得
	GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

	for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
	{
		pIdx[0] = (DWORD)nCntPoint;

		pIdx++;
	}

	// インデックスバッファをアンロックする
	GetIdxBuff()->Unlock();

	return S_OK;
}

//=============================================================================
// メッシュオービットの終了処理
//=============================================================================
void CMeshOrbit::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//=============================================================================
// メッシュオービットの更新処理
//=============================================================================
void CMeshOrbit::Update(void)
{
	// 保存してある頂点座標と頂点カラーをずらす
	for (int nCntPoint = MAX_ORBIT_VTX - 1; nCntPoint >= 3; nCntPoint -= 2)
	{
		m_aPosPoint[nCntPoint] = m_aPosPoint[nCntPoint - 2];
		m_aPosPoint[nCntPoint - 1] = m_aPosPoint[nCntPoint - 3];
		m_aColPoint[nCntPoint] = m_aColPoint[nCntPoint - 2];
		m_aColPoint[nCntPoint - 1] = m_aColPoint[nCntPoint - 3];
	}

	D3DXMATRIX mtxParent = m_pMtxParent;

	// 親のマトリックスにオフセットを掛けて新しい位置を算出
	D3DXVec3TransformCoord(&m_aPosPoint[0], &m_aOffset[0], &mtxParent);
	D3DXVec3TransformCoord(&m_aPosPoint[1], &m_aOffset[1], &mtxParent);

	// 色の初期値を設定
	m_aColPoint[0] = m_aCol[0];
	m_aColPoint[1] = m_aCol[1];

	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
	{
		// 頂点座標の設定
		pVtx[0].pos = m_aPosPoint[nCntPoint];

		m_aColPoint[nCntPoint].a -= ALPHA_MINUS;

		if (m_aColPoint[nCntPoint].a < 0.0f)
		{// 最低値
			m_aColPoint[nCntPoint].a = 0.0f;
		}

		// 頂点カラーの設定
		pVtx[0].col = m_aColPoint[nCntPoint];

		pVtx++;
	}

	// 頂点バッファをアンロックする
	GetVtxBuff()->Unlock();
}

//=============================================================================
// メッシュオービットの描画処理
//=============================================================================
void CMeshOrbit::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	D3DXMATRIX mtxWorld;

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// レンダーステートを加算合成にする
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	// マトリックスを設定
	CMesh::SetMatrix(mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, GetVtxBuff(), 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(GetIdxBuff());

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, MAX_ORBIT_VTX, 0, MAX_ORBIT_VTX - 2);

	// レンダーステートを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//========================================================================
// テクスチャを結びつけ
//========================================================================
void CMeshOrbit::BindTexture(const int nTexture)
{
	m_nIdxTexture = nTexture;
}

//=============================================================================
// メッシュオービットの場所設定処理
//=============================================================================
void CMeshOrbit::SetPosition(const  D3DXVECTOR3 Offset0, const  D3DXVECTOR3 Offset1,
	const D3DXCOLOR col0, const D3DXCOLOR col1, const D3DMATRIX* pMtxParent)
{
	m_aOffset[0] = Offset0;
	m_aOffset[1] = Offset1;
	m_aCol[0] = col0;
	m_aCol[1] = col1;
	m_pMtxParent = *pMtxParent;
}