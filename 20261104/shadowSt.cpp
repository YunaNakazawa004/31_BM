//========================================================================
// 
// ステンシルシャドウ [	shadowSt.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "shadowSt.h"

#include "renderer.h"
#include "MTrenderer.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "texture.h"
#include "XModel.h"
#include "debugproc.h"

#include "light.h"

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
int CShadowSt::m_nIdxModel = {};			// モデルのインデックス

//========================================================================
// モデルのロード処理
//========================================================================
HRESULT CShadowSt::Load(const char* pFilename)
{
	// ローカル変数宣言
	CXModel* pXModel = CManager::GetXModel();			// モデル管理へのポインタ

	// テクスチャの設定
	m_nIdxModel = pXModel->Register(pFilename);

	if (m_nIdxModel == -1)
	{// モデルが設定できていない
		OutputDebugStringA("! ! ! モデルの設定に失敗しました ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}

//========================================================================
// モデルのアンロード処理
//========================================================================
void CShadowSt::Unload(void)
{
	// モデルのインデックスを削除
	memset(&m_nIdxModel, -1, sizeof m_nIdxModel);
}

//========================================================================
// ステンシルシャドウクラスの生成処理
//========================================================================
CShadowSt* CShadowSt::Create(const D3DXVECTOR3 pos)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CShadowSt* pShadowSt = nullptr;

	if (pShadowSt == nullptr)
	{// NULLチェック
		// ステンシルシャドウの生成
		pShadowSt = new CShadowSt;
	}

	if (pShadowSt != nullptr)
	{// NULLチェック
		// モデルファイル設定
		pShadowSt->BindModel(m_nIdxModel);

		// 初期化処理
		if (FAILED(pShadowSt->Init(pos)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! ステンシルシャドウの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// 種類を設定
		pShadowSt->SetType(TYPE_SHADOW);

		return pShadowSt;
	}

	OutputDebugStringA("! ! ! ステンシルシャドウの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// ステンシルシャドウクラスのコンストラクタ
//========================================================================
CShadowSt::CShadowSt(const int nPriority) :CObjectX(nPriority)
{
	// ステンシルシャドウクラスの値をクリア
	m_pVtxBuff = nullptr;
}

//========================================================================
// ステンシルシャドウクラスのデストラクタ
//========================================================================
CShadowSt::~CShadowSt()
{
}

//========================================================================
// ステンシルシャドウクラスの初期化処理
//========================================================================
HRESULT CShadowSt::Init(const D3DXVECTOR3 pos)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	VERTEX_2D* pVtx;

	// 初期化処理
	CObjectX::Init(pos);

	// 頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr)))
	{// もし失敗したら
		OutputDebugStringA("! ! ! 頂点バッファの生成に失敗しました ! ! !\n");

		return E_FAIL;
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);
	pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);
	pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);
	pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);

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
// ステンシルシャドウクラスの終了処理
//========================================================================
void CShadowSt::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// 終了処理
	CObjectX::Uninit();
}

//========================================================================
// ステンシルシャドウクラスの更新処理
//========================================================================
void CShadowSt::Update(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXVECTOR3 rot = GetRotation();						// 位置を取得
	D3DLIGHT9 light;				// ライト情報
	D3DXVECTOR3 vecLight;			// ライトのベクトル

	// ライトの位置を設定
	pDevice->GetLight(0, &light);
	vecLight = D3DXVECTOR3(light.Direction.x, light.Direction.y, light.Direction.z);

	// 向きを設定
	rot.x = sinf(vecLight.y * D3DX_PI);
	rot.y = atan2f(vecLight.x, vecLight.z);
	rot.z = 0.0f;
	SetRotation(rot);
}

//========================================================================
// ステンシルシャドウクラスの描画処理
//========================================================================
void CShadowSt::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxWorld;		// 計算用マトリックス
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	static bool bShadow = false;

	if (pInputKeyboard->GetTrigger(DIK_O))
	{
		bShadow = (bShadow) ? false : true;
	}

	// 頂点法線正規化をオン
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// マトリックスを計算
	mtxWorld = MatrixCalc();

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	// ステンシルテストを有効にする
	pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);

	// Zバッファへの書き込みを無効にする
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// カラーバッファへの書き込みを無効にする
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000);

	// ステンシルバッファの比較パラメータを設定
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR);
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);

	// 表面をカリング
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// 影モデルを描画
	CObjectX::Draw(mtxWorld);

	// ステンシルバッファの参照値を設定（"1"にする）
	pDevice->SetRenderState(D3DRS_STENCILREF, 0x01);

	// ステンシルバッファの比較パラメータを設定
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);

	// 裏面をカリング
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// 影モデルを描画
	CObjectX::Draw(mtxWorld);

	// カラーバッファへの書き込みを有効にする
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	// ステンシルバッファの参照値を設定（"2"にする）
	pDevice->SetRenderState(D3DRS_STENCILREF, 0x02);

	// ステンシルバッファの比較パラメータを設定
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);

	// 画面の大きさの黒いポリゴンを描画
	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, nullptr);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// Zバッファへの書き込みを有効にする
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// ステンシルテストを無効にする
	pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	if(bShadow)
	// 影モデルを描画
	CObjectX::Draw(mtxWorld);

	// 頂点法線正規化をオフ
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}

//========================================================================
// マトリックスを計算
//========================================================================
D3DXMATRIX CShadowSt::MatrixCalc(void)
{
	D3DXMATRIX mtxWorld, mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス
	D3DXVECTOR3 pos, rot, scale;	// ステンシルシャドウの情報
	scale = GetScale();				// スケールを取得
	rot = GetRotation();			// 位置を取得
	pos = GetPosition();			// 位置を取得

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, scale.x, scale.y, scale.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScale);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	return mtxWorld;
}