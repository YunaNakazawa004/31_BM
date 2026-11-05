//========================================================================
// 
// オブジェクトビルボード [ objectBillboard.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "objectBillboard.h"

#include "renderer.h"
#include "manager.h"
#include "texture.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define OBJECT_WIDTH	(180.0f)		// オブジェクトの幅
#define OBJECT_HEIGHT	(90.0f)			// オブジェクトの高さ

//========================================================================
// オブジェクトビルボードクラスの生成処理
//========================================================================
CObjectBillboard* CObjectBillboard::Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight,
	const CObject::TYPE type, const char* pFilename, const int nPriority)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CObjectBillboard* pObjectBillboard = nullptr;

	if (pObjectBillboard == nullptr)
	{// NULLチェック
		// オブジェクトビルボードの生成
		pObjectBillboard = new CObjectBillboard(nPriority);
	}

	if (pObjectBillboard != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pObjectBillboard->Init(pos, fWidth, fHeight)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! オブジェクトビルボードの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// テクスチャを設定
		CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
		pObjectBillboard->m_nTextureIdx = pTexture->Register(pFilename);

		// 種類を設定
		pObjectBillboard->SetType(type);

		return pObjectBillboard;
	}

	OutputDebugStringA("! ! ! オブジェクトビルボードの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// オブジェクトビルボードクラスのコンストラクタ
//========================================================================
CObjectBillboard::CObjectBillboard(const int nPriority) :CObject(nPriority)
{
	// オブジェクトビルボードクラスの値をクリア
	m_pVtxBuff = nullptr;
	m_nTextureIdx = -1;
	m_pos = DEFAULT_VECTOR3;
	m_scale = DEFAULT_VECTOR3;
	m_fRadius = 0.0f;
	m_col = COLOR_WHITE;
	m_bDisp = true;
}

//========================================================================
// オブジェクトビルボードクラスのデストラクタ
//========================================================================
CObjectBillboard::~CObjectBillboard()
{
}

//========================================================================
// オブジェクトビルボードクラスの初期化処理(オーバーロード)
//========================================================================
HRESULT CObjectBillboard::Init(const D3DXVECTOR3 pos,
	const float fWidth, const float fHeight)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	VERTEX_3D* pVtx;

	// クラスの値を初期化
	m_pos = pos;
	m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_fRadius = fWidth;

	// 頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr)))
	{// もし失敗したら
		OutputDebugStringA("! ! ! 頂点バッファの生成に失敗しました ! ! !\n");

		return E_FAIL;
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	// 頂点座標の設定
	pVtx[0].pos.x = -fWidth;
	pVtx[0].pos.y = +fHeight;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = +fWidth;
	pVtx[1].pos.y = +fHeight;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = -fWidth;
	pVtx[2].pos.y = -fHeight;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = +fWidth;
	pVtx[3].pos.y = -fHeight;
	pVtx[3].pos.z = 0.0f;

	// 法線の設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}

//========================================================================
// オブジェクトビルボードクラスの終了処理
//========================================================================
void CObjectBillboard::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// オブジェクトビルボードクラスの更新処理
//========================================================================
void CObjectBillboard::Update(void)
{
}

//========================================================================
// オブジェクトビルボードクラスの描画処理
//========================================================================
void CObjectBillboard::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	D3DXMATRIX mtxTrans, mtxScale;		// 計算用マトリックス
	D3DXMATRIX mtxView;					// ビューマトリックス取得用

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// ポリゴンをカメラに対して正面に向ける
	D3DXMatrixInverse(&m_mtxWorld, nullptr, &mtxView);	// 逆行列を求める
	m_mtxWorld._41 = 0.0f;
	m_mtxWorld._42 = 0.0f;
	m_mtxWorld._43 = 0.0f;

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nTextureIdx));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,		// 描画する最初の頂点インデックス
		2);		// 描画するプリミティブ数

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//========================================================================
// オブジェクトXクラスの影描画処理
//========================================================================
void CObjectBillboard::DrawShadow(const D3DXMATRIX mtxWorld, const D3DXVECTOR3 point, const D3DXVECTOR3 normal)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	D3DXCOLOR col;					// 色保存用
	D3DXMATRIX mtxShadow;			// シャドウマトリックス
	D3DLIGHT9 light;				// ライト情報
	D3DXVECTOR4 posLight;			// ライトの位置
	D3DXPLANE plane;				// 平面情報
	float fBias = -0.0001f;			// 深度

	// ワールドマトリックスの初期化
	m_mtxWorld = mtxWorld;

	// ライトの位置を設定
	pDevice->GetLight(3, &light);
	posLight = D3DXVECTOR4(light.Position.x, light.Position.y, light.Position.z, 1.0f);

	// 平面情報を設定
	D3DXPlaneFromPointNormal(&plane, &point, &normal);

	// Zファイティング防止
	pDevice->SetRenderState(D3DRS_DEPTHBIAS, *((DWORD*)&fBias));

	// シャドウマトリックスの初期化
	D3DXMatrixIdentity(&mtxShadow);

	// シャドウマトリックスの作成
	D3DXMatrixShadow(&mtxShadow, &posLight, &plane);
	D3DXMatrixMultiply(&mtxShadow, &m_mtxWorld, &mtxShadow);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadow);

	// 色を保存
	col = GetColor();

	// 色を黒にする
	SetColor(COLOR_BLACK);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nTextureIdx));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,		// 描画する最初の頂点インデックス
		2);		// 描画するプリミティブ数

	// 色を戻す
	SetColor(col);

	// Zファイティング防止を元に戻す
	pDevice->SetRenderState(D3DRS_DEPTHBIAS, 0);
}

//========================================================================
// 位置設定
//========================================================================
void CObjectBillboard::SetPosition(const D3DXVECTOR3 pos)
{
	// 位置を変更
	m_pos = pos;
}

//========================================================================
// スケール設定
//========================================================================
void CObjectBillboard::SetScale(const D3DXVECTOR3 scale)
{
	// スケールを変更
	m_scale = scale;
}

//========================================================================
// サイズ設定
//========================================================================
void CObjectBillboard::SetSize(const float fRadius)
{
	// 半径変更
	m_fRadius = fRadius;

	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos.x = -m_fRadius;
	pVtx[0].pos.y = +m_fRadius;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = +m_fRadius;
	pVtx[1].pos.y = +m_fRadius;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = -m_fRadius;
	pVtx[2].pos.y = -m_fRadius;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = +m_fRadius;
	pVtx[3].pos.y = -m_fRadius;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// 頂点カラー設定
//========================================================================
void CObjectBillboard::SetColor(const D3DXCOLOR col)
{
	m_col = col;

	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// テクスチャ座標設定
//========================================================================
void CObjectBillboard::SetTexUV(const int nPatternAnim, const int nTexWidth, const int nTexHeight, const float fScrollX, const float fScrollY)
{
	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(nPatternAnim * (1.0f / (float)nTexWidth) + fScrollX, (nPatternAnim / nTexWidth) * (1.0f / (float)nTexHeight) + fScrollY);
	pVtx[1].tex = D3DXVECTOR2((nPatternAnim + 1) * (1.0f / (float)nTexWidth) + fScrollX, (nPatternAnim / nTexWidth) * (1.0f / (float)nTexHeight) + fScrollY);
	pVtx[2].tex = D3DXVECTOR2(nPatternAnim * (1.0f / (float)nTexWidth) + fScrollX, ((nPatternAnim / nTexWidth) + 1) * (1.0f / (float)nTexHeight) + fScrollY);
	pVtx[3].tex = D3DXVECTOR2((nPatternAnim + 1) * (1.0f / (float)nTexWidth) + fScrollX, ((nPatternAnim / nTexWidth) + 1) * (1.0f / (float)nTexHeight) + fScrollY);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}