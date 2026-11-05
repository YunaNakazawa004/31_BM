//=============================================================================
// 
// 線 [ line.cpp ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#include "line.h"

#include "renderer.h"
#include "manager.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//=============================================================================
// 線の生成処理
//=============================================================================
CLine* CLine::Create(const D3DXVECTOR3 pos1, const D3DXVECTOR3 pos2, const D3DXCOLOR col, D3DXCOLOR col2)
{

#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CLine* pLine = nullptr;

	if (pLine == nullptr)
	{// NULLチェック
		// 線の生成
		pLine = new CLine;
	}

	if (pLine != nullptr)
	{// NULLチェック
		if (col2 == COLOR_NONE)
		{// デフォルトのまま
			col2 = col;
		}

		// 初期化処理
		if (FAILED(pLine->Init(pos1, pos2, col, col2)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! 線の初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// 種類を設定
		pLine->SetType(TYPE_LINE);

		return pLine;
	}

	OutputDebugStringA("! ! ! 線の生成に失敗しました ! ! !\n");

	return nullptr;
}

//=============================================================================
// 線のコンストラクタ
//=============================================================================
CLine::CLine(const int nPriority) : CObject(nPriority)
{
	// 値をクリア
	m_pVtxBuff = nullptr;
	m_pos = DEFAULT_VECTOR3;
	m_col = COLOR_WHITE;
	m_nDeleteCounter = 0;
	m_bDisp = true;
}

//=============================================================================
// 線のデストラクタ
//=============================================================================
CLine::~CLine()
{
}

//=============================================================================
// 線の初期化処理
//=============================================================================
HRESULT CLine::Init(const D3DXVECTOR3 pos1, const D3DXVECTOR3 pos2, const D3DXCOLOR col, const D3DXCOLOR col2)
{
	// ローカル変数
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーを取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスを取得
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// 値を初期化
	m_pos = pos1;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 2,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = pos1;
	pVtx[1].pos = pos2;

	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVtx[0].col = col;
	pVtx[1].col = col2;

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}

//=============================================================================
// 線の終了処理
//=============================================================================
void CLine::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// 自分自身を破棄
	Release();
}

//=============================================================================
// 線の更新処理
//=============================================================================
void CLine::Update(void)
{
	if (m_nDeleteCounter >= 1)
	{// 消す
		// 終了処理
		Uninit();
	}

	m_nDeleteCounter++;
}

//=============================================================================
// 線の描画処理
//=============================================================================
void CLine::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーを取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスを取得

			// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);			// Zバッファ更新の無効の設定

	//アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファブレンドを有効に設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// ( , 比較方法(より大きい))
	pDevice->SetRenderState(D3DRS_ALPHAREF, 100);				// ( , 基準値)

	// 減算合成の設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);		// アルファブレンドの設定1
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// アルファブレンドの設定2
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);		// アルファブレンドの設定3

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, nullptr);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_LINELIST, 0, 2);

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Zバッファ更新の有効の設定

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);			// アルファブレンドを無効に設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);		// ( , 比較方法(すべて))
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);						// ( , 基準値)

	// ブレンディング(減算合成)を元に戻す 
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
}