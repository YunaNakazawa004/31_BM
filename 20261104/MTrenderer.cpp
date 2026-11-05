//========================================================================
// 
// マルチターゲットレンダリング [ MTrenderer.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "MTrenderer.h"

#include "renderer.h"
#include "manager.h"
#include "debugproc.h"
#include "effect_batching.h"
#include "object.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENABLE_PERSPECTIVFOV_MT				// レンダリングターゲットを透視投影で描画
//#undef ENABLE_PERSPECTIVFOV_MT

#define MAX_OVERSIZE		(10.0f)			// 大きくする最大サイズ

//========================================================================
// マルチターゲットレンダラークラスのコンストラクタ
//========================================================================
CMTRenderer::CMTRenderer()
{
	// マルチターゲットレンダラークラスの値のクリア
	m_bMultiRendering = false;
	m_fOverSize = 0.0f;
	m_fOverSizeDest = 0.0f;

	m_pRenderDef = nullptr;
	m_pZBuffDef = nullptr;
}

//========================================================================
// マルチターゲットレンダラークラスのデストラクタ
//========================================================================
CMTRenderer::~CMTRenderer()
{
}

//========================================================================
// マルチターゲットレンダラークラスの初期化処理
//========================================================================
HRESULT CMTRenderer::Init(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// 値を初期化
	m_fOverSize = 0.0f;
	m_fOverSizeDest = 0.0f;

	for (int nCnt = 0; nCnt < 2; nCnt++)
	{
		LPDIRECT3DSURFACE9 pRenderDef, pZBuffDef;

		// レンダリングターゲット用テクスチャの生成
		pDevice->CreateTexture(SCREEN_WIDTH, SCREEN_HEIGHT,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&m_apTextureMT[nCnt],
			nullptr);

		// テクスチャレンダリング用インターフェースの生成
		m_apTextureMT[nCnt]->GetSurfaceLevel(0, &m_apRenderMT[nCnt]);

		// テクスチャレンダリング用Zバッファの生成
		pDevice->CreateDepthStencilSurface(SCREEN_WIDTH, SCREEN_HEIGHT,
			D3DFMT_D16,
			D3DMULTISAMPLE_NONE,
			0,
			TRUE,
			&m_pZBuffMT,
			nullptr);

		// 現在のレンダリングターゲットを取得（保存）
		pDevice->GetRenderTarget(0, &pRenderDef);

		// 現在のZバッファを取得（保存）
		pDevice->GetDepthStencilSurface(&pZBuffDef);

		// レンダリングターゲットを生成したテクスチャに設定
		pDevice->SetRenderTarget(0, m_apRenderMT[nCnt]);

		// Zバッファを生成したZバッファに設定
		pDevice->SetDepthStencilSurface(m_pZBuffMT);

		// レンダリングターゲット用テクスチャのクリア
		pDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

		// レンダリングターゲットをもとに戻す
		pDevice->SetRenderTarget(0, pRenderDef);

		// Zバッファを元に戻す
		pDevice->SetDepthStencilSurface(pZBuffDef);

		// テクスチャレンダリング用ビューポートの設定
		m_viewportMT.X = 0;
		m_viewportMT.Y = 0;
		m_viewportMT.Width = SCREEN_WIDTH;
		m_viewportMT.Height = SCREEN_HEIGHT;
		m_viewportMT.MinZ = 0.0f;
		m_viewportMT.MaxZ = 1.0f;
	}

	// フィードバック用ポリゴンを生成
	VERTEX_2D* pVtx;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffMT,
		nullptr);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuffMT->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	// 頂点座標の設定
	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = 0.0f;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = (float)SCREEN_WIDTH;
	pVtx[1].pos.y = 0.0f;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = 0.0f;
	pVtx[2].pos.y = (float)SCREEN_HEIGHT;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = (float)SCREEN_WIDTH;
	pVtx[3].pos.y = (float)SCREEN_HEIGHT;
	pVtx[3].pos.z = 0.0f;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0004f, 0.0007f);
	pVtx[1].tex = D3DXVECTOR2(1.0004f, 0.0007f);
	pVtx[2].tex = D3DXVECTOR2(0.0004f, 1.0007f);
	pVtx[3].tex = D3DXVECTOR2(1.0004f, 1.0007f);

	// 頂点バッファをアンロックする
	m_pVtxBuffMT->Unlock();

	return S_OK;
}

//========================================================================
// マルチターゲットレンダラークラスの終了処理
//========================================================================
void CMTRenderer::Uninit(void)
{
	// テクスチャの破棄
	for (int nCnt = 0; nCnt < 2; nCnt++)
	{
		if (m_apTextureMT[nCnt] != nullptr)
		{
			m_apTextureMT[nCnt]->Release();
			m_apTextureMT[nCnt] = nullptr;
		}
	}

	// インターフェースの破棄
	for (int nCnt = 0; nCnt < 2; nCnt++)
	{
		if (m_apRenderMT[nCnt] != nullptr)
		{
			m_apRenderMT[nCnt]->Release();
			m_apRenderMT[nCnt] = nullptr;
		}
	}

	// Zバッファの破棄
	if (m_pZBuffMT != nullptr)
	{
		m_pZBuffMT->Release();
		m_pZBuffMT = nullptr;
	}

	// 頂点バッファの破棄
	if (m_pVtxBuffMT != nullptr)
	{
		m_pVtxBuffMT->Release();
		m_pVtxBuffMT = nullptr;
	}

	// 保存用バッファの破棄
	if (m_pRenderDef != nullptr)
	{
		m_pRenderDef->Release();
		m_pRenderDef = nullptr;
	}

	// 保存用バッファの破棄
	if (m_pZBuffDef != nullptr)
	{
		m_pZBuffDef->Release();
		m_pZBuffDef = nullptr;
	}
}

//========================================================================
// マルチターゲットレンダラークラスの更新処理
//========================================================================
void CMTRenderer::Update(void)
{
	m_fOverSize += (m_fOverSizeDest - m_fOverSize) * 0.1f;

	CManager::GetDebugProc()->Print("サイズ : ( %f )\n", m_fOverSize);
}

//========================================================================
// マルチターゲットレンダラークラスの描画処理
//========================================================================
void CMTRenderer::Draw(void)
{
	if (m_bMultiRendering == false && m_fOverSize == 0.0f)
	{// マルチターゲットレンダリングしない場合
		// テクスチャをクリア
		ResetMTRendering();

		return;
	}

	// ローカル変数
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	LPDIRECT3DSURFACE9 pRenderWk;
	LPDIRECT3DTEXTURE9 pTextureWk;
	VERTEX_2D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuffMT->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos.x = -m_fOverSize;
	pVtx[0].pos.y = -m_fOverSize;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = (float)SCREEN_WIDTH + m_fOverSize;
	pVtx[1].pos.y = -m_fOverSize;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = -m_fOverSize;
	pVtx[2].pos.y = (float)SCREEN_HEIGHT + m_fOverSize;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = (float)SCREEN_WIDTH + m_fOverSize;
	pVtx[3].pos.y = (float)SCREEN_HEIGHT + m_fOverSize;
	pVtx[3].pos.z = 0.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

	// 頂点バッファをアンロックする
	m_pVtxBuffMT->Unlock();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffMT, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, m_apTextureMT[1]);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// レンダリング設定を元に戻す
	BackMTRendering();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuffMT->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = 0.0f;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = (float)SCREEN_WIDTH;
	pVtx[1].pos.y = 0.0f;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = 0.0f;
	pVtx[2].pos.y = (float)SCREEN_HEIGHT;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = (float)SCREEN_WIDTH;
	pVtx[3].pos.y = (float)SCREEN_HEIGHT;
	pVtx[3].pos.z = 0.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuffMT->Unlock();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffMT, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, m_apTextureMT[0]);

	// αブレンドをオフにして、不透明で描画（画面が暗くなるのを防ぐ）
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// αブレンドを元に戻す
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// テクスチャ[0]とテクスチャ[1]を入れ替える
	pTextureWk = m_apTextureMT[0];
	m_apTextureMT[0] = m_apTextureMT[1];
	m_apTextureMT[1] = pTextureWk;

	pRenderWk = m_apRenderMT[0];
	m_apRenderMT[0] = m_apRenderMT[1];
	m_apRenderMT[1] = pRenderWk;
}

//========================================================================
// マルチターゲットレンダリングターゲットを変更
//========================================================================
void CMTRenderer::ChangeTarget(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 vecU)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxView, mtxProjection;
	float fAspect;

	// レンダリングターゲットを生成したテクスチャに設定
	pDevice->SetRenderTarget(0, m_apRenderMT[0]);

	// Zバッファを生成したZバッファに設定
	pDevice->SetDepthStencilSurface(m_pZBuffMT);

	// テクスチャレンダリング用のビューポートを設定
	pDevice->SetViewport(&m_viewportMT);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&mtxView);

	// ビューマトリックスを作成
	D3DXMatrixLookAtLH(&mtxView,
		&posV,			// 視点
		&posR,			// 注視点
		&vecU);			// 上方向ベクトル

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &mtxView);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&mtxProjection);

#ifdef ENABLE_PERSPECTIVFOV_MT		// 透視投影
	// プロジェクションマトリックスを作成
	fAspect = (float)m_viewportMT.Width / (float)m_viewportMT.Height;
	D3DXMatrixPerspectiveFovLH(&mtxProjection,
		D3DXToRadian(45.0f),					// 視野角
		fAspect,								// 画面のアスペクト比
		10.0f,									// カメラから一番近い描画距離
		80000.0f);								// 最大描画距離

#else		// 平行投影
	// プロジェクションマトリックスを作成
	D3DXMatrixOrthoLH(&mtxProjection,
		(float)m_viewportMT.Width,		// ビューボリュームの幅
		(float)m_viewportMT.Height,		// ビューボリュームの高さ
		10.0f,							// ビューボリュームの近平面までの距離
		80000.0f);						// ビューボリュームの遠平面までの距離
#endif

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);
}

//========================================================================
// マルチターゲットレンダリングの可否を設定
//========================================================================
void CMTRenderer::SetMultiRendering(const bool bMultiRendering) 
{
	m_bMultiRendering = bMultiRendering; 

	if (m_bMultiRendering == true)
	{// マルチターゲットレンダリングする
		m_fOverSizeDest = MAX_OVERSIZE;
	}
	else
	{// しない
		m_fOverSizeDest = 0.0f;
	}
}

//========================================================================
// マルチターゲットレンダリングの初期設定
//========================================================================
void CMTRenderer::FirstMTRendering(void)
{
	if (m_bMultiRendering == false && m_fOverSize == 0.0f)
	{// マルチターゲットレンダリングしない場合
		return;
	}

	// ローカル変数
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CCamera* pCamera = CManager::GetCamera();				// カメラの取得

	// 現在のレンダリングターゲットを取得（保存）
	pDevice->GetRenderTarget(0, &m_pRenderDef);

	// 現在のZバッファを取得（保存）
	pDevice->GetDepthStencilSurface(&m_pZBuffDef);

	// 現在のビューポートを取得（保存）
	pDevice->GetViewport(&m_viewportDef);

	// 現在のビューマトリックスを取得（保存）
	pDevice->GetTransform(D3DTS_VIEW, &m_mtxViewDef);

	// 現在のプロジェクションマトリックスを取得（保存）
	pDevice->GetTransform(D3DTS_PROJECTION, &m_mtxProjectionDef);

	// レンダリングターゲットを変更
	ChangeTarget(pCamera->GetPositionV(), pCamera->GetPositionR(), D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	// レンダリングターゲット用テクスチャのクリア
	pDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
}

//========================================================================
// レンダリング方法を元に戻す
//========================================================================
void CMTRenderer::BackMTRendering(void)
{
	if (m_bMultiRendering == false && m_fOverSize == 0.0f)
	{// マルチターゲットレンダリングしない場合
		return;
	}

	// ローカル変数
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// レンダリングターゲットをもとに戻す
	pDevice->SetRenderTarget(0, m_pRenderDef);

	// Zバッファを元に戻す
	pDevice->SetDepthStencilSurface(m_pZBuffDef);

	// ビューポートを元に戻す
	pDevice->SetViewport(&m_viewportDef);

	// ビューマトリックスを元に戻す
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxViewDef);

	// プロジェクションマトリックスを元に戻す
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjectionDef);
}

//========================================================================
// レンダリングテクスチャをリセットする
//========================================================================
void CMTRenderer::ResetMTRendering(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	for (int nCnt = 0; nCnt < 2; nCnt++)
	{
		LPDIRECT3DSURFACE9 pRenderDef, pZBuffDef;

		// 現在のレンダリングターゲットを取得（保存）
		pDevice->GetRenderTarget(0, &pRenderDef);

		// 現在のZバッファを取得（保存）
		pDevice->GetDepthStencilSurface(&pZBuffDef);

		// レンダリングターゲットを生成したテクスチャに設定
		pDevice->SetRenderTarget(0, m_apRenderMT[nCnt]);

		// Zバッファを生成したZバッファに設定
		pDevice->SetDepthStencilSurface(m_pZBuffMT);

		// レンダリングターゲット用テクスチャのクリア
		pDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

		// レンダリングターゲットをもとに戻す
		pDevice->SetRenderTarget(0, pRenderDef);

		// Zバッファを元に戻す
		pDevice->SetDepthStencilSurface(pZBuffDef);
	}
}