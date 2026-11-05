//========================================================================
// 
// 数字 [ number.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "number.h"

#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "object.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define NUMBER_WIDTH	(90.0f)		// 数字の幅
#define NUMBER_HEIGHT	(180.0f)	// 数字の高さ
#define NUMBER_TEXFILE	"data\\TEXTURE\\UI\\time_number.png"			// 数字のテクスチャ

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
int CNumber::m_nIdxTexture = -1;				// テクスチャのインデックス

//========================================================================
// テクスチャの生成
//========================================================================
HRESULT CNumber::Load(void)
{
	// ローカル変数宣言
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ

	// テクスチャの設定
	m_nIdxTexture = pTexture->Register(NUMBER_TEXFILE);

	if (m_nIdxTexture == -1)
	{// テクスチャが設定できていない
		OutputDebugStringA("! ! ! テクスチャの設定に失敗しました ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}

//========================================================================
// テクスチャの破棄
//========================================================================
void CNumber::Unload(void)
{
	// テクスチャのインデックスを削除
	m_nIdxTexture = -1;
}

//========================================================================
// 数字クラスの生成処理
//========================================================================
CNumber* CNumber::Create(const D3DXVECTOR3 pos,
	const float fWidth, const float fHeight)
{
	CNumber* pNumber = nullptr;

	if (pNumber == nullptr)
	{// NULLチェック
		// スコアの生成
		pNumber = new CNumber;
	}

	if (pNumber != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pNumber->Init(pos, fWidth, fHeight)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! 数字の初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		return pNumber;
	}

	OutputDebugStringA("! ! ! 数字の生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// 数字クラスのコンストラクタ
//========================================================================
CNumber::CNumber()
{
	// 数字クラスの値をクリア
	m_pVtxBuff = nullptr;
	m_pos = DEFAULT_VECTOR3;
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_bDisp = true;
}

//========================================================================
// 数字クラスのデストラクタ
//========================================================================
CNumber::~CNumber()
{
}

//========================================================================
// 数字クラスの初期化処理
//========================================================================
HRESULT CNumber::Init(const D3DXVECTOR3 pos, 
	const float fWidth, const float fHeight)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	VERTEX_2D* pVtx;

	// クラスの値を初期化
	m_pos = pos;
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_bDisp = true;

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

	// 頂点情報の設定
	// 頂点座標の設定
	pVtx[0].pos.x = m_pos.x;
	pVtx[0].pos.y = m_pos.y;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = m_pos.x + m_fWidth;
	pVtx[1].pos.y = m_pos.y;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = m_pos.x;
	pVtx[2].pos.y = m_pos.y + m_fHeight;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = m_pos.x + m_fWidth;
	pVtx[3].pos.y = m_pos.y + m_fHeight;
	pVtx[3].pos.z = 0.0f;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

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
// 数字クラスの終了処理
//========================================================================
void CNumber::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}
}

//========================================================================
// 数字クラスの更新処理
//========================================================================
void CNumber::Update(void)
{
}

//========================================================================
// 数字クラスの描画処理
//========================================================================
void CNumber::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//========================================================================
// 位置設定
//========================================================================
void CNumber::SetPosition(const D3DXVECTOR3 pos)
{
	VERTEX_2D* pVtx;

	// 位置を変更
	m_pos = pos;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos.x = m_pos.x;
	pVtx[0].pos.y = m_pos.y;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = m_pos.x + m_fWidth;
	pVtx[1].pos.y = m_pos.y;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = m_pos.x;
	pVtx[2].pos.y = m_pos.y + m_fHeight;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = m_pos.x + m_fWidth;
	pVtx[3].pos.y = m_pos.y + m_fHeight;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// 頂点カラー設定
//========================================================================
void CNumber::SetColor(const D3DXCOLOR col)
{
	VERTEX_2D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// テクスチャ座標設定
//========================================================================
void CNumber::SetTexUV(const int nNumber, const int nTexWidth, const int nTexHeight)
{
	VERTEX_2D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(nNumber * (1.0f / (float)nTexWidth), (nNumber / nTexWidth) * (1.0f / (float)nTexHeight));
	pVtx[1].tex = D3DXVECTOR2((nNumber + 1) * (1.0f / (float)nTexWidth), (nNumber / nTexWidth) * (1.0f / (float)nTexHeight));
	pVtx[2].tex = D3DXVECTOR2(nNumber * (1.0f / (float)nTexWidth), ((nNumber / nTexWidth) + 1) * (1.0f / (float)nTexHeight));
	pVtx[3].tex = D3DXVECTOR2((nNumber + 1) * (1.0f / (float)nTexWidth), ((nNumber / nTexWidth) + 1) * (1.0f / (float)nTexHeight));

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}