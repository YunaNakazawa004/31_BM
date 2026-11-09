//========================================================================
// 
// オブジェクト3D [ object3D.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "object3D.h"

#include "renderer.h"
#include "manager.h"
#include "texture.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define OBJECT_WIDTH	(180.0f)		// オブジェクトの幅
#define OBJECT_HEIGHT	(90.0f)			// オブジェクトの高
#define ERROR_HEIGHT	(-99999.0f)		// 高さを算出できなかった場合の数値

//========================================================================
// オブジェクト3Dクラスの生成処理
//========================================================================
CObject3D* CObject3D::Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const float fDepth,
	const CObject::TYPE type, const char* pFilename, const int nPriority, const MAINPOS mainpos)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CObject3D* pObject3D = nullptr;

	if (pObject3D == nullptr)
	{// NULLチェック
		// オブジェクト3Dの生成
		pObject3D = new CObject3D(nPriority);
	}

	if (pObject3D != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pObject3D->Init(pos, fWidth, fHeight, fDepth, mainpos)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! オブジェクト3Dの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// テクスチャを設定
		CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
		pObject3D->m_nIdxTexture = pTexture->Register(pFilename);

		// 種類を設定
		pObject3D->SetType(type);

		return pObject3D;
	}

	OutputDebugStringA("! ! ! オブジェクト3Dの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// オブジェクト3Dクラスのコンストラクタ
//========================================================================
CObject3D::CObject3D(const int nPriority) :CObject(nPriority)
{
	// オブジェクト3Dクラスの値をクリア
	m_pVtxBuff = nullptr;
	m_nIdxTexture = -1;
	m_pos = DEFAULT_VECTOR3;
	m_rot = DEFAULT_VECTOR3;
	m_scale = DEFAULT_VECTOR3;
	m_col = COLOR_WHITE;
	m_bDisp = true;
}

//========================================================================
// オブジェクト3Dクラスのデストラクタ
//========================================================================
CObject3D::~CObject3D()
{
}

//========================================================================
// オブジェクト3Dクラスの初期化処理(オーバーロード)
//========================================================================
HRESULT CObject3D::Init(const D3DXVECTOR3 pos,
	const float fWidth, const float fHeight, const float fDepth, const MAINPOS mainpos)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	VERTEX_3D* pVtx;
	D3DXVECTOR3 vec0, vec1, vec2, vec3;
	D3DXVECTOR3 norA, norB;

	// クラスの値を初期化
	m_pos = pos;
	m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

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
	switch (mainpos)
	{
	case MAINPOS_CENTER:		// posが真ん中の場合
		// 頂点座標の設定
		pVtx[0].pos.x = -fWidth;
		pVtx[0].pos.y = +fHeight;
		pVtx[0].pos.z = +fDepth;
		pVtx[1].pos.x = +fWidth;
		pVtx[1].pos.y = +fHeight;
		pVtx[1].pos.z = +fDepth;
		pVtx[2].pos.x = -fWidth;
		pVtx[2].pos.y = -fHeight;
		pVtx[2].pos.z = -fDepth;
		pVtx[3].pos.x = +fWidth;
		pVtx[3].pos.y = -fHeight;
		pVtx[3].pos.z = -fDepth;

		break;

	case MAINPOS_TOPLEFT:		// posが左上の場合
		// 頂点座標の設定
		pVtx[0].pos.x = 0.0f;
		pVtx[0].pos.y = 0.0f;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = fWidth * 2.0f;
		pVtx[1].pos.y = 0.0f;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = 0.0f;
		pVtx[2].pos.y = -fHeight * 2.0f;
		pVtx[2].pos.z = -fDepth * 2.0f;
		pVtx[3].pos.x = +fWidth * 2.0f;
		pVtx[3].pos.y = -fHeight * 2.0f;
		pVtx[3].pos.z = -fDepth * 2.0f;

		break;

	case MAINPOS_BOTTOMMID:		// posが真ん中下の場合
		// 頂点座標の設定
		pVtx[0].pos.x = -fWidth;
		pVtx[0].pos.y = +fHeight * 2.0f;
		pVtx[0].pos.z = +fDepth * 2.0f;
		pVtx[1].pos.x = +fWidth;
		pVtx[1].pos.y = +fHeight * 2.0f;
		pVtx[1].pos.z = +fDepth * 2.0f;
		pVtx[2].pos.x = -fWidth;
		pVtx[2].pos.y = 0.0f;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = +fWidth;
		pVtx[3].pos.y = 0.0f;
		pVtx[3].pos.z = 0.0f;

		break;
	}

	// それぞれの頂点間のベクトルを計算
	vec0 = pVtx[1].pos - pVtx[0].pos;
	vec1 = pVtx[2].pos - pVtx[0].pos;
	vec2 = pVtx[1].pos - pVtx[3].pos;
	vec3 = pVtx[2].pos - pVtx[3].pos;

	// 法線を計算
	D3DXVec3Cross(&norA, &vec0, &vec1);
	D3DXVec3Cross(&norB, &vec3, &vec2);

	// 法線を正規化
	D3DXVec3Normalize(&norA, &norA);
	D3DXVec3Normalize(&norB, &norB);

	// 法線の設定
	pVtx[0].nor = norA;
	pVtx[1].nor = (norA + norB) * 0.5f;
	pVtx[2].nor = (norA + norB) * 0.5f;
	pVtx[3].nor = norB;

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
// オブジェクト3Dクラスの終了処理
//========================================================================
void CObject3D::Uninit(void)
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
// オブジェクト3Dクラスの更新処理
//========================================================================
void CObject3D::Update(void)
{
}

//========================================================================
// オブジェクト3Dクラスの描画処理
//========================================================================
void CObject3D::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 100);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

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
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,		// 描画する最初の頂点インデックス
		2);		// 描画するプリミティブ数

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 100);
}

//========================================================================
// 位置設定
//========================================================================
void CObject3D::SetPosition(const D3DXVECTOR3 pos)
{
	// 位置を変更
	m_pos = pos;
}

//========================================================================
// スケール設定
//========================================================================
void CObject3D::SetScale(const D3DXVECTOR3 scale)
{
	// スケールを変更
	m_scale = scale;
}

//========================================================================
// 角度設定
//========================================================================
void CObject3D::SetRotation(const D3DXVECTOR3 rot)
{
	// X向きを調整
	CorrectAngle(&m_rot.x, m_rot.x);

	// Y向きを調整
	CorrectAngle(&m_rot.y, m_rot.y);

	// Z向きを調整
	CorrectAngle(&m_rot.z, m_rot.z);

	// 向きを変更
	m_rot = rot;
}

//========================================================================
// 頂点カラー設定
//========================================================================
void CObject3D::SetColor(const D3DXCOLOR col)
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
void CObject3D::SetTexUV(const int nPatternAnim, const int nTexWidth, const int nTexHeight, const float fScrollX, const float fScrollY)
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

//========================================================================
// テクスチャ座標設定
//========================================================================
void CObject3D::SetTexUV(const int nTexWidth, const int nTexHeight)
{
	VERTEX_3D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2((float)nTexWidth, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, (float)nTexHeight);
	pVtx[3].tex = D3DXVECTOR2((float)nTexWidth, (float)nTexHeight);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// 現在位置の高さを取得
//========================================================================
float CObject3D::GetHeight(const D3DXVECTOR3 pos)
{
	// ローカル変数
	D3DXVECTOR3 vecLine, vecToPos;		// 境界線ベクトル、現在位置へのベクトル
	D3DXVECTOR3 posCorrect = pos;		// 角度を含めて調整した現在位置
	float fposLine;			// 外積
	float fDist;			// ポリゴンと現在位置との距離
	int nCntLine = 0;		// 内側にいた回数
	VERTEX_3D* pVtx;

	fDist = sqrtf((((m_pos.x - pos.x) * 2) * ((m_pos.x - pos.x) * 2)) + 
		(((m_pos.z - pos.z) * 2) * ((m_pos.z - pos.z) * 2))) * 0.5f;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// プレイヤーが対象ポリゴン(0-1-2)の内側にいるかどうか
	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		vecLine = pVtx[(nCnt + 1) % 3].pos - pVtx[nCnt].pos;		// 辺のベクトル
		vecToPos = posCorrect - pVtx[nCnt].pos;							// 現在位置へのベクトル

		// 外積を計算
		fposLine = (float)((int)(((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z)) * 1.0f) / (int)1);

		if (fposLine > 0.0f)
		{// 内側にいる
			nCntLine++;
		}
	}

	if (nCntLine >= 3)
	{// 対象ポリゴン(0-1-2)の内側にいる
		D3DXVECTOR3 vec1, vec2, nor;	// ベクトル、法線
		float fHeight = 0.0f;			// 求める高さ

		// ベクトルを取得
		vec1 = pVtx[1].pos - pVtx[0].pos;
		vec2 = pVtx[2].pos - pVtx[0].pos;

		// 法線の計算
		D3DXVec3Cross(&nor, &vec1, &vec2);

		// 法線を正規化
		D3DXVec3Normalize(&nor, &nor);

		if (nor.y != 0.0f)
		{// yの法線が0ではない
			// 高さを求める計算
			// 元の式 : (pos.x - pVtx[0].pos.x) * nor.x + (pos.y - pVtx[0].pos.y) * nor.y + (pos.z - pVtx[0].pos.z) * nor.z = 0.0f
			fHeight = ((-(posCorrect.x - pVtx[0].pos.x) * nor.x - (posCorrect.z - pVtx[0].pos.z) * nor.z) / nor.y) + pVtx[0].pos.y;

			// 頂点バッファをアンロックする
			m_pVtxBuff->Unlock();

			return fHeight;
		}
	}
	else
	{// 内側にいない→次のポリゴンの内側にいるかどうか調べる
		nCntLine = 0;		// リセット
	}

	// プレイヤーが対象ポリゴン(3-2-1)の内側にいるかどうか
	for (int nCnt = 3; nCnt > 0; nCnt--)
	{
		vecLine = pVtx[(nCnt + 1) % 3 + 1].pos - pVtx[nCnt].pos;		// 辺のベクトル
		vecToPos = posCorrect - pVtx[nCnt].pos;								// 現在位置へのベクトル

		// 外積を計算
		fposLine = (float)((int)(((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z)) * 1.0f) / (int)1);

		if (fposLine > 0.0f)
		{// 内側にいる
			nCntLine++;
		}
	}

	if (nCntLine >= 3)
	{// 対象ポリゴン(3-2-1)の内側にいる
		D3DXVECTOR3 vec1, vec2, nor;	// ベクトル、法線
		float fHeight = 0.0f;			// 求める高さ

		// ベクトルを取得
		vec1 = pVtx[2].pos - pVtx[3].pos;
		vec2 = pVtx[1].pos - pVtx[3].pos;

		// 法線の計算
		D3DXVec3Cross(&nor, &vec1, &vec2);

		// 法線を正規化
		D3DXVec3Normalize(&nor, &nor);

		if (nor.y != 0.0f)
		{// yの法線が0ではない
			// 高さを求める計算
			// 元の式 : (pos.x - pVtx[3].pos.x) * nor.x + (pos.y - pVtx[3].pos.y) * nor.y + (pos.z - pVtx[3].pos.z) * nor.z = 0.0f
			fHeight = ((-(posCorrect.x - pVtx[3].pos.x) * nor.x - (posCorrect.z - pVtx[3].pos.z) * nor.z) / nor.y) + pVtx[3].pos.y;

			// 頂点バッファをアンロックする
			m_pVtxBuff->Unlock();

			return fHeight;
		}
	}
	else
	{// どちらのポリゴンの内側にもいない
		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();

		return ERROR_HEIGHT;
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return ERROR_HEIGHT;
}