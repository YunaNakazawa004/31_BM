//========================================================================
// 
// 壁 [ wallMM.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "wallMM.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

#include "line.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define FLOW_SPEED			(m_fFlowSpeed)			// 波紋の速さ
#define FLOW_DEPTH			(m_fFlowDepth)			// 波紋の幅
#define FLOW_RADIUS			(m_fFlowRadius)			// 波紋の半径

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
int CWallMM::m_aIdxTexture[3] = {};			// テクスチャのインデックス

//========================================================================
// テクスチャの生成
//========================================================================
HRESULT CWallMM::Load(const char* pFilename1, const char* pFilename2, const char* pFilename3)
{
	// ローカル変数宣言
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ

	// テクスチャの設定
	m_aIdxTexture[0] = pTexture->Register(pFilename1);
	m_aIdxTexture[1] = pTexture->Register(pFilename2);
	m_aIdxTexture[2] = pTexture->Register(pFilename3);

	if (m_aIdxTexture[0] == -1 || m_aIdxTexture[1] == -1 || m_aIdxTexture[2] == -1)
	{// テクスチャが設定できていない
		OutputDebugStringA("! ! ! テクスチャの設定に失敗しました ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}

//========================================================================
// テクスチャの破棄
//========================================================================
void CWallMM::Unload(void)
{
	// テクスチャのインデックスを削除
	memset(&m_aIdxTexture[0], -1, sizeof m_aIdxTexture);
}

//========================================================================
// 壁クラスの生成処理
//========================================================================
CWallMM* CWallMM::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CWallMM* pWallMM = nullptr;

	if (pWallMM == nullptr)
	{// NULLチェック
		// 壁の生成
		pWallMM = new CWallMM;
	}

	if (pWallMM != nullptr)
	{// NULLチェック
		// テクスチャを設定
		pWallMM->BindTexture(&m_aIdxTexture[0]);

		// 初期化処理
		if (FAILED(pWallMM->Init(pos, rot, block, size)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! 壁の初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// 種類を設定
		pWallMM->SetType(TYPE_MESHWALL);

		return pWallMM;
	}

	OutputDebugStringA("! ! ! 壁の生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// 壁クラスのコンストラクタ
//========================================================================
CWallMM::CWallMM(const int nPriority) : CMeshWallM(nPriority)
{
	// 壁クラスの値をクリア
	memset(&m_start[0], 0, sizeof m_start);
	m_fFlowSpeed = 0.0f;
	m_fFlowDepth = 0.0f;
	m_fFlowRadius = 0.0f;
	memset(&m_fFlow[0], 0, sizeof m_fFlow);
	m_type = 0;
}

//========================================================================
// 壁クラスのデストラクタ
//========================================================================
CWallMM::~CWallMM()
{
}

//========================================================================
// 壁クラスの初期化処理
//========================================================================
HRESULT CWallMM::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size)
{
	// 初期化処理
	CMeshWallM::Init(pos, rot, block, size);

	// 値を初期化
	for (int nCntStart = 0; nCntStart < MAX_BOUND; nCntStart++)
	{
		m_start[nCntStart] = D3DXVECTOR2(-1.0f, -1.0f);
	}

	m_fFlowSpeed = 0.3f;
	m_fFlowDepth = 1.0f;
	m_fFlowRadius = 0.04f;
	memset(&m_fFlow[0], 0, sizeof m_fFlow);
	m_type = -1;
	SetTexType(0);

	SetColor(COLOR_BLACK);

	return S_OK;
}

//========================================================================
// 壁クラスの終了処理
//========================================================================
void CWallMM::Uninit(void)
{
	// 終了処理
	CMeshWallM::Uninit();
}

//========================================================================
// 壁クラスの更新処理
//========================================================================
void CWallMM::Update(void)
{
	// 波紋処理
	Flow();

	if (m_type == TYPE_NORMAL)
	{// 通常テクスチャは何もしない
		return;
	}

	// テクスチャを動かす
	SetTexMScrollAll(0.005f, 0.0f);
}

//========================================================================
// 壁クラスの描画処理
//========================================================================
void CWallMM::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// テクスチャステージステートの設定
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	if (m_type != TYPE_NORMAL)
	{// 通常テクスチャのとき以外
		pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
	}

	// 描画処理
	CMeshWallM::Draw();
}

//========================================================================
// 波紋のスタート位置を設定する
//========================================================================
void CWallMM::SetStartFlow(const D3DXVECTOR2 start)
{
	if (start.x < 0 || start.y < 0)
	{// 範囲外の数値だったら抜ける
		return;
	}

	for (int nCntStart = 0; nCntStart < MAX_BOUND; nCntStart++)
	{
		if (m_start[nCntStart].x < 0.0f && m_start[nCntStart].y < 0.0f)
		{// 使っていない
			m_start[nCntStart] = start;
			m_fFlow[nCntStart] = 0.0f;

			return;
		}
	}
}

//========================================================================
// 波紋処理
//========================================================================
void CWallMM::Flow(void)
{
	int aStartVtx[MAX_BOUND];
	memset(&aStartVtx[0], -1, sizeof aStartVtx);
	int nNonStart = 0;		// 無効なスタート地点

	for (int nCntStart = 0; nCntStart < MAX_BOUND; nCntStart++)
	{
		// スタート位置の頂点
		aStartVtx[nCntStart] = (int)m_start[nCntStart].x + ((int)m_start[nCntStart].y * ((int)GetBlock().x + 1));

		if (aStartVtx[nCntStart] < 0 || aStartVtx[nCntStart] > GetNumVtx())
		{// 範囲外
			nNonStart++;
		}
	}

	if (nNonStart >= MAX_BOUND)
	{// 全て無効なスタート地点
		Reset();

		return;
	}

	// 最も遠い頂点
	float aLongest[MAX_BOUND];
	int aLongestVtx[MAX_BOUND];
	memset(&aLongest[0], 0, sizeof aLongest);
	memset(&aLongestVtx[0], -1, sizeof aLongestVtx);

	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{
		// 現在位置
		D3DXVECTOR3 posVtx = GetPosVtx(nCnt);
		D3DXCOLOR colVtx = GetColor(nCnt);

		D3DXVECTOR2 dist;		// 距離
		float fAverage = 0.0f;

		for (int nCntStart = 0; nCntStart < MAX_BOUND; nCntStart++)
		{
			if (aStartVtx[nCntStart] < 0 || aStartVtx[nCntStart] > GetNumVtx())
			{// 範囲外
				continue;
			}

			// 距離の計算
			dist.x = posVtx.x - GetPosVtx(aStartVtx[nCntStart]).x;
			dist.y = posVtx.y - GetPosVtx(aStartVtx[nCntStart]).y;
			float fLength = D3DXVec2Length(&dist);

			if (aLongest[nCntStart] < fLength)
			{// 最も遠い距離を更新
				aLongest[nCntStart] = fLength;
				aLongestVtx[nCntStart] = nCnt;
			}

			// 距離に応じた波紋の深さ
			float fFlowCorrect = m_fFlow[nCntStart] - (FLOW_RADIUS * fLength);

			if (fFlowCorrect < 0)
			{// 下限
				fFlowCorrect = 0.0f;
			}

			if (fFlowCorrect >= D3DX_PI * 2.0f)
			{// 一周分
				fFlowCorrect = 0.0f;

				if (nCnt == aLongestVtx[nCntStart])
				{// 最も遠い頂点が一周した
					m_start[nCntStart] = D3DXVECTOR2(-1.0f, -1.0f);
				}
			}

			fAverage += fFlowCorrect;		// 平均を取るために加算
		}

		float fChange = sinf(fAverage / 2.0f) * FLOW_DEPTH;
		colVtx.r = fChange;
		colVtx.g = fChange;
		colVtx.b = fChange;

		SetColor(nCnt, colVtx);
	}

	for (int nCntStart = 0; nCntStart < MAX_BOUND; nCntStart++)
	{
		if (aStartVtx[nCntStart] < 0 || aStartVtx[nCntStart] > GetNumVtx())
		{// 範囲外
			continue;
		}

		m_fFlow[nCntStart] += FLOW_SPEED;
	}

	// 法線の計算
	SetNor();
}

//========================================================================
// 頂点位置リセット
//========================================================================
void CWallMM::Reset(void)
{
	memset(&m_fFlow[0], 0, sizeof m_fFlow);

	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{
		// 現在位置
		D3DXVECTOR3 posVtx = GetPosVtx(nCnt);
		posVtx.z = 0.0f;

		SetPosVtx(nCnt, posVtx);
	}

	// 色をリセット
	SetColor(COLOR_BLACK);

	// 法線の計算
	SetNor();
}

//========================================================================
// 全てのメッシュウォールと当たり判定
//========================================================================
bool CWallMM::CollisionAll(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, const float fRadius)
{
	bool bColl = false;

	for (int nCntPri = 0; nCntPri < PRIORITY_MAX; nCntPri++)
	{
#ifdef LIST
		CObject* pObj = CObject::GetTop(nCntPri);

		while (pObj != nullptr)
		{
			CObject* pObjNext = pObj->GetNext();			// 次のオブジェクトを保存
			CObject::TYPE type;

			// オブジェクトの種類を取得
			type = pObj->GetType();

			if (type == CObject::TYPE_MESHWALL)
			{// メッシュウォールなら当たり判定する
				// 当たり判定
				if (dynamic_cast<CWallMM*>(pObj)->Collision(pPos, pPosOld, fRadius) == true)
				{// 当たった
					bColl = true;
				}
			}

			pObj = pObjNext;			// 次のオブジェクトを代入
		}

#else
		for (int nCntObj = 0; nCntObj < MAX_OBJECT; nCntObj++)
		{
			CObject* pObj;

			// オブジェクトを取得
			pObj = GetObject(nCntPri, nCntObj);

			if (pObj != nullptr)
			{// NULLチェック
				CObject::TYPE type;

				// オブジェクトの種類を取得
				type = pObj->GetType();

				if (type == CObject::TYPE_MESHWALL)
				{// メッシュウォールなら当たり判定する
					// 当たり判定
					if (dynamic_cast<CWallMM*>(pObj)->Collision(pPos, pPosOld, fRadius) == true)
					{// 当たった
						bColl = true;
					}
				}
			}
		}

#endif
	}

	return bColl;
}

//========================================================================
// 全てのメッシュウォールと当たり判定
//========================================================================
bool CWallMM::SetTypeAll(const int nType)
{
	bool bColl = false;

	for (int nCntPri = 0; nCntPri < PRIORITY_MAX; nCntPri++)
	{
#ifdef LIST
		CObject* pObj = CObject::GetTop(nCntPri);

		while (pObj != nullptr)
		{
			CObject* pObjNext = pObj->GetNext();			// 次のオブジェクトを保存
			CObject::TYPE type;

			// オブジェクトの種類を取得
			type = pObj->GetType();

			if (type == CObject::TYPE_MESHWALL)
			{// メッシュウォールならタイプを設定
				dynamic_cast<CWallMM*>(pObj)->SetTexType(nType);
			}

			pObj = pObjNext;			// 次のオブジェクトを代入
		}

#else
		for (int nCntObj = 0; nCntObj < MAX_OBJECT; nCntObj++)
		{
			CObject* pObj;

			// オブジェクトを取得
			pObj = GetObject(nCntPri, nCntObj);

			if (pObj != nullptr)
			{// NULLチェック
				CObject::TYPE type;

				// オブジェクトの種類を取得
				type = pObj->GetType();

				if (type == CObject::TYPE_MESHWALL)
				{// メッシュウォールならタイプを設定
					dynamic_cast<CWallMM*>(pObj)->SetTexType(type);
				}
			}
		}

#endif
	}

	return bColl;
}

//========================================================================
// 当たり判定
//========================================================================
bool CWallMM::Collision(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, const float fRadius)
{
	CDebugProc* pDebug = CManager::GetDebugProc();
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 rot = GetRotation();
	D3DXVECTOR2 block = GetBlock();
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR3 start, end;			// 始点/終点
	float fXS, fZS, fXE, fZE;		// 始点のXZ座標/終点のXZ座標
	float fMinY, fMaxY;				// Y座標の最低値と最大値

	D3DXVECTOR3 vecLine, vecMove, vecToPos, vecToPosOld, vecNor, vecMoveRef, vecMoveDest;		// 各ベクトル
	D3DXVECTOR3 vecLineW, posDest;

	float fRate, fDot;		// 交点の割合/内積

	float fOffXS = -(block.x * size.x);		// 向き0のときの座標
	float fOffXE = block.x * size.x;		// 向き0のときの座標

	float fSin = sinf(-rot.y);		// 現在向きのsin
	float fCos = cosf(-rot.y);		// 現在向きのcos

	// Y向きを調整
	CorrectAngle(&rot.y, rot.y);

	// 始点のXZ座標
	fXS = fOffXS * fCos - (-fRadius * 1.0f) * fSin;
	fZS = fOffXS * fSin + (-fRadius * 1.0f) * fCos;

	// 終点のXZ座標
	fXE = fOffXE * fCos - (-fRadius * 0.99f) * fSin;
	fZE = fOffXE * fSin + (-fRadius * 0.99f) * fCos;

	// Y座標の最低値/最大値
	fMinY = pos.y;
	fMaxY = pos.y + (block.y * size.y * 2.0f);

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
	vecNor.x = vecLine.z;
	vecNor.y = 0.0f;
	vecNor.z = -vecLine.x;
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
		if (fPosLine < 0.0f && (fPosOldLine >= 0.0f))
		{// 交差した
			if ((fMinY <= pPos->y) &&
				(fMaxY >= pPos->y))
			{// 高さが合っている
				// 当たっているので波紋を設定する
				SetStartFlow(GetPolygonIdx(*pPos));

				return true;
			}
		}
	}

	return false;
}

//========================================================================
// テクスチャ設定
//========================================================================
void CWallMM::SetTexType(const int type)
{
	if (m_type == type)
	{// 同じものを設定しない
		return;
	}

	m_type = type;
	BindTexture(1, m_aIdxTexture[type]);

	if (type == TYPE_NORMAL)
	{// 通常テクスチャ
		// 最小単位でテクスチャを設定
		SetTexMSmall();
	}
	else
	{// それ以外のテクスチャ
		// 最大サイズでテクスチャを設定
		SetTexMBig();
	}
}