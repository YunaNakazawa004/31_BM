//========================================================================
// 
// マップオブジェクト [ map_object.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "map_object.h"

#include "manager.h"
#include "debugproc.h"
#include "input.h"
#include "sound.h"

#include "object.h"
#include "objectX.h"
#include "particle3D.h"
#include "Xmodel.h"

#include <fstream>
#include <iostream>

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************

//========================================================================
// モデルのロード処理
//========================================================================
HRESULT CMapObject::Load(const char* pFilename)
{
	// ローカル変数宣言
	CXModel* pXModel = CManager::GetXModel();			// モデル管理へのポインタ

	// テクスチャの設定
	m_vModelFileIdx.push_back(pXModel->Register(pFilename));

	if (m_vModelFileIdx.at(m_nNumObjectFile) == -1)
	{// モデルが設定できていない
		OutputDebugStringA("! ! ! モデルの設定に失敗しました ! ! !\n");

		return E_FAIL;
	}

	m_nNumObjectFile++;

	return S_OK;
}

//========================================================================
// モデルのアンロード処理
//========================================================================
void CMapObject::Unload(void)
{
	// モデルのインデックスを削除
	m_vModelFileIdx.clear();
	m_nNumObjectFile = 0;
}

//========================================================================
// 生成処理
//========================================================================
void CMapObject::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const int mapObj, const int nIdxModel)
{
	switch (mapObj)
	{
	case MAP_OBJ_ROCK:			// 岩を生成
		m_vpObject.push_back(CObjectX::Create(pos, rot, CObject::TYPE_MAPOBJECT, m_vModelFileIdx.at(nIdxModel)));
		m_vType.push_back(mapObj);

		break;
	}

	if (m_vpObject.at(m_nNumObject) != NULL)
	{// 生成できた
		// 総数をカウントアップ
		m_nNumObject++;
	}
}

//========================================================================
// マップオブジェクトのコンストラクタ
//========================================================================
CMapObject::CMapObject()
{
	// 値のクリア
	m_nNumObject = 0;
	m_nNumObjectFile = 0;
	m_vpObject.clear();
	m_vType.clear();
	m_vModelFileIdx.clear();
}

//========================================================================
// マップオブジェクトのデストラクタ
//========================================================================
CMapObject::~CMapObject()
{
}

//========================================================================
// マップオブジェクトの初期化処理
//========================================================================
HRESULT CMapObject::Init(void)
{
	// 値の初期化
	m_nNumObject = 0;
	m_nNumObjectFile = 0;
	m_vpObject.clear();
	m_vType.clear();
	m_vModelFileIdx.clear();

	return S_OK;
}

//========================================================================
// マップオブジェクトの終了処理
//========================================================================
void CMapObject::Uninit(void)
{
}

//========================================================================
// マップオブジェクトの更新処理
//========================================================================
void CMapObject::Update(void)
{
}

//========================================================================
// 当たり判定
//========================================================================
bool CMapObject::Collision(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, D3DXVECTOR3* pRotDest,
	D3DXVECTOR3* pVecAxis, float* pValueRot, const float fRadius)
{
	bool bColl = false;

	for (int nCntObj = 0; nCntObj < m_nNumObject; nCntObj++)
	{
		D3DXVECTOR3 posObj = m_vpObject.at(nCntObj)->GetPosition();
		D3DXVECTOR3 dist = posObj - *pPos;
		float fRange = (m_vpObject.at(nCntObj)->GetVtxMax().x >= m_vpObject.at(nCntObj)->GetVtxMax().z) ?
			m_vpObject.at(nCntObj)->GetVtxMax().x : m_vpObject.at(nCntObj)->GetVtxMax().z;

		if (D3DXVec3Length(&dist) > fRange + fRadius * 2.0f)
		{// 遠い距離
			continue;
		}

		// 近い場合は当たり判定をする
		if (m_vpObject.at(nCntObj)->Reflect(pPos, pPosOld, pMove, pRotDest, pVecAxis, pValueRot, fRadius) == true)
		{// 当たった
			bColl = true;
		}
	}

	return bColl;
}

//========================================================================
// リセット
//========================================================================
void CMapObject::Reset(void)
{
	// クリア
	m_nNumObject = 0;
	m_nNumObjectFile = 0;
	m_vpObject.clear();
	m_vType.clear();
	m_vModelFileIdx.clear();
}