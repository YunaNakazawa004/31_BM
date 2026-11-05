//========================================================================
// 
// オブジェクトX [ objectX.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "objectX.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"
#include "texture.h"
#include "Xmodel.h"

#include "camera.h"
#include "line.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define MOVEMENT				(D3DXVECTOR3(0.6f, 0.6f, 0.6f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量

//========================================================================
// オブジェクトXクラスの生成処理
//========================================================================
CObjectX* CObjectX::Create(const D3DXVECTOR3 pos, const CObject::TYPE type, const char* pFilename, const int nPriority)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CObjectX* pObjectX = nullptr;

	if (pObjectX == nullptr)
	{// NULLチェック
		// オブジェクトXの生成
		pObjectX = new CObjectX(nPriority);
	}

	if (pObjectX != nullptr)
	{// NULLチェック
		// モデルファイル設定
		CXModel* pXModel = CManager::GetXModel();
		pObjectX->m_nIdxModel = pXModel->Register(pFilename);
		pObjectX->BindModel(pObjectX->m_nIdxModel);

		// 初期化処理
		if (FAILED(pObjectX->Init(pos)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! オブジェクトXの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// 種類を設定
		pObjectX->SetType(type);

		return pObjectX;
	}

	OutputDebugStringA("! ! ! オブジェクトXの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// オブジェクトXクラスのコンストラクタ
//========================================================================
CObjectX::CObjectX(const int nPriority) :CObject(nPriority)
{
	// オブジェクトXクラスの値をクリア
	m_nIdxModel = -1;
	memset(&m_aTexture[0], -1, sizeof m_aTexture);
	m_pStrFileName = nullptr;
	m_VtxMax = DEFAULT_VECTOR3;
	m_VtxMin = DEFAULT_VECTOR3;
	m_pos = DEFAULT_VECTOR3;
	m_rot = DEFAULT_VECTOR3;
	m_scale = DEFAULT_VECTOR3;
	memset(&m_col[0], COLOR_WHITE, sizeof m_col);
	m_bDisp = true;
}

//========================================================================
// オブジェクトXクラスのデストラクタ
//========================================================================
CObjectX::~CObjectX()
{
}

//========================================================================
// オブジェクトXクラスの初期化処理(オーバーロード)
//========================================================================
HRESULT CObjectX::Init(const D3DXVECTOR3 pos)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	CXModel* pXModel = CManager::GetXModel();				// モデルへのポインタ
	D3DXMATERIAL* pMat;
	int nNumVtx;			// 頂点数
	DWORD dwSizeFVF;		// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;			// 頂点バッファへのポインタ

	// モデルの情報の初期化
	m_pos = pos;
	m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	// 最大値最小値の初期化
	m_VtxMax = D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f);
	m_VtxMin = D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pXModel->GetBuffMat(m_nIdxModel)->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)pXModel->GetNumMat(m_nIdxModel); nCntMat++)
	{
		// 色を保存
		m_col[nCntMat] = pMat[nCntMat].MatD3D.Diffuse;

		if (pMat[nCntMat].pTextureFilename != nullptr)
		{// テクスチャファイルが存在する
			m_aTexture[nCntMat] = pTexture->Register(pMat[nCntMat].pTextureFilename);
		}
	}

	// 頂点数を取得
	nNumVtx = pXModel->GetMesh(m_nIdxModel)->GetNumVertices();

	// 頂点フォーマットのサイズを取得
	dwSizeFVF = D3DXGetFVFVertexSize(pXModel->GetMesh(m_nIdxModel)->GetFVF());

	// 頂点バッファをロック
	pXModel->GetMesh(m_nIdxModel)->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

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

	// オブジェクトの最大値最小値を補正
	m_VtxMax.x = (float)((int)(m_VtxMax.x * 100.0f) / (int)1) / 100.0f;
	m_VtxMax.y = (float)((int)(m_VtxMax.y * 100.0f) / (int)1) / 100.0f;
	m_VtxMax.z = (float)((int)(m_VtxMax.z * 100.0f) / (int)1) / 100.0f;
	m_VtxMin.x = (float)((int)(m_VtxMin.x * 100.0f) / (int)1) / 100.0f;
	m_VtxMin.y = (float)((int)(m_VtxMin.y * 100.0f) / (int)1) / 100.0f;
	m_VtxMin.z = (float)((int)(m_VtxMin.z * 100.0f) / (int)1) / 100.0f;

	// 頂点バッファをアンロック
	pXModel->GetMesh(m_nIdxModel)->UnlockVertexBuffer();

	return S_OK;
}

//========================================================================
// オブジェクトXクラスの終了処理
//========================================================================
void CObjectX::Uninit(void)
{
	// テクスチャの破棄
	for (int nCntModel = 0; nCntModel < MAX_MODEL_TEXTURE; nCntModel++)
	{
		m_aTexture[nCntModel] = -1;
	}

	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// オブジェクトXクラスの更新処理
//========================================================================
void CObjectX::Update(void)
{
	// ローカル変数
	CCamera* pCamera = CManager::GetCamera();							// カメラの取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	D3DXVECTOR3 pos = CObjectX::GetPosition();
	D3DXVECTOR3 rot = CObjectX::GetRotation();
	D3DXVECTOR3 CameraRot = pCamera->GetRotation();
	static D3DXVECTOR3 move = {};

	if (pInputKeyboard->GetPress(DIK_W) == true)		// キーボード移動
	{// 奥に移動
		if (pInputKeyboard->GetPress(DIK_A) == true)
		{// 左奥に移動
			move.x += sinf(-D3DX_PI * 0.75f - CameraRot.y) * MOVEMENT.x;
			move.z += cosf(-D3DX_PI * 0.25f + CameraRot.y) * MOVEMENT.z;
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true)
		{// 右奥に移動
			move.x += sinf(D3DX_PI * 0.75f - CameraRot.y) * MOVEMENT.x;
			move.z += cosf(D3DX_PI * 0.25f + CameraRot.y) * MOVEMENT.z;
		}
		else if (pInputKeyboard->GetPress(DIK_W) == true)
		{// 奥に移動
			move.x += sinf(D3DX_PI * 0.0f + CameraRot.y) * MOVEMENT.x;
			move.z += cosf(D3DX_PI * 0.0f + CameraRot.y) * MOVEMENT.z;
		}

		// 進んだ方向に角度を向ける
		rot.y = atan2f(move.x, move.z) + D3DX_PI;
	}
	else if (pInputKeyboard->GetPress(DIK_S) == true)
	{// 手前に移動
		if (pInputKeyboard->GetPress(DIK_A) == true)
		{// 左手前に移動
			move.x += sinf(-D3DX_PI * 0.25f - CameraRot.y) * MOVEMENT.x;
			move.z += cosf(-D3DX_PI * 0.75f + CameraRot.y) * MOVEMENT.z;
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true)
		{// 右手前に移動
			move.x += sinf(D3DX_PI * 0.25f - CameraRot.y) * MOVEMENT.x;
			move.z += cosf(D3DX_PI * 0.75f + CameraRot.y) * MOVEMENT.z;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{// 手前に移動
			move.x += sinf(D3DX_PI * 1.0f + CameraRot.y) * MOVEMENT.x;
			move.z += cosf(D3DX_PI * 1.0f + CameraRot.y) * MOVEMENT.z;
		}

		// 進んだ方向に角度を向ける
		rot.y = atan2f(move.x, move.z) + D3DX_PI;
	}
	else if (pInputKeyboard->GetPress(DIK_A) == true)
	{// 左に移動
		move.x += sinf(-D3DX_PI * 0.5f + CameraRot.y) * MOVEMENT.x;
		move.z += cosf(-D3DX_PI * 0.5f + CameraRot.y) * MOVEMENT.z;

		// 進んだ方向に角度を向ける
		rot.y = atan2f(move.x, move.z) + D3DX_PI;
	}
	else if (pInputKeyboard->GetPress(DIK_D) == true)
	{// 右に移動
		move.x += sinf(D3DX_PI * 0.5f + CameraRot.y) * MOVEMENT.x;
		move.z += cosf(D3DX_PI * 0.5f + CameraRot.y) * MOVEMENT.z;

		// 進んだ方向に角度を向ける
		rot.y = atan2f(move.x, move.z) + D3DX_PI;
	}

	if (pInputKeyboard->GetTrigger(DIK_BACKSPACE) == true)
	{// 位置回転リセット
		pos = DEFAULT_VECTOR3;
		rot = DEFAULT_VECTOR3;
		move = DEFAULT_VECTOR3;
	}

	// 位置に移動量を加算
	pos += move;

	// 移動量に慣性
	move.x += (0.0f - move.x) * 0.3f;
	move.z += (0.0f - move.z) * 0.3f;

	// 位置/向きを適用
	SetPosition(pos);
	SetRotation(rot);
}

//========================================================================
// オブジェクトXクラスの描画処理
//========================================================================
void CObjectX::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	CXModel* pXModel = CManager::GetXModel();				// モデルへのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス
	D3DMATERIAL9 matDef;						// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;							// マテリアルデータへのポインタ

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

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pXModel->GetBuffMat(m_nIdxModel)->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)pXModel->GetNumMat(m_nIdxModel); nCntMat++)
	{
		// 色保存用
		D3DXCOLOR MatCol = pMat[nCntMat].MatD3D.Diffuse;

		// マテリアルの色を変更
		pMat[nCntMat].MatD3D.Diffuse = m_col[nCntMat];

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// 色を戻す
		pMat[nCntMat].MatD3D.Diffuse = MatCol;

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_aTexture[nCntMat]));

		// モデルパーツの描画
		pXModel->GetMesh(m_nIdxModel)->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//========================================================================
// オブジェクトXクラスの描画処理(オーバーロード)
//========================================================================
void CObjectX::Draw(const D3DXMATRIX mtxWorld)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	CXModel* pXModel = CManager::GetXModel();				// モデルへのポインタ
	D3DMATERIAL9 matDef;						// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;							// マテリアルデータへのポインタ

	m_mtxWorld = mtxWorld;

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pXModel->GetBuffMat(m_nIdxModel)->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)pXModel->GetNumMat(m_nIdxModel); nCntMat++)
	{
		// 色保存用
		D3DXCOLOR MatCol = pMat[nCntMat].MatD3D.Diffuse;

		// マテリアルの色を変更
		pMat[nCntMat].MatD3D.Diffuse = m_col[nCntMat];

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// 色を戻す
		pMat[nCntMat].MatD3D.Diffuse = MatCol;

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_aTexture[nCntMat]));

		// モデルパーツの描画
		pXModel->GetMesh(m_nIdxModel)->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//========================================================================
// オブジェクトXクラスの影描画処理
//========================================================================
void CObjectX::DrawShadow(const D3DXMATRIX mtxWorld, const D3DXVECTOR3 point, const D3DXVECTOR3 normal)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	CXModel* pXModel = CManager::GetXModel();				// モデルへのポインタ
	D3DMATERIAL9 matDef;						// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;							// マテリアルデータへのポインタ
	D3DXMATRIX mtxShadow;			// シャドウマトリックス
	D3DLIGHT9 light;				// ライト情報
	D3DXVECTOR4 posLight;			// ライトの位置
	D3DXPLANE plane;				// 平面情報
	float fBias = -0.0001f;			// 深度

	// ワールドマトリックスの初期化
	m_mtxWorld = mtxWorld;

	// ライトの位置を設定
	pDevice->GetLight(0, &light);
	posLight = D3DXVECTOR4(-light.Direction.x, -light.Direction.y, -light.Direction.z, 0.0f);

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

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pXModel->GetBuffMat(m_nIdxModel)->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)pXModel->GetNumMat(m_nIdxModel); nCntMat++)
	{
		// 色保存用
		D3DXCOLOR MatCol = pMat[nCntMat].MatD3D.Diffuse;
		D3DCOLORVALUE MatEmissive = pMat[nCntMat].MatD3D.Emissive;

		// マテリアルの色を変更
		pMat[nCntMat].MatD3D.Diffuse = COLOR_BLACK;
		pMat[nCntMat].MatD3D.Emissive = D3DCOLORVALUE();

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// 色を戻す
		pMat[nCntMat].MatD3D.Diffuse = MatCol;
		pMat[nCntMat].MatD3D.Emissive = MatEmissive;

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_aTexture[nCntMat]));

		// モデルパーツの描画
		pXModel->GetMesh(m_nIdxModel)->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);

	// Zファイティング防止を元に戻す
	pDevice->SetRenderState(D3DRS_DEPTHBIAS, 0);
}

//========================================================================
// モデルを紐づける
//========================================================================
void CObjectX::BindModel(const int nIdxModel)
{
	CXModel* pXModel = CManager::GetXModel();

	m_nIdxModel = nIdxModel;
}

//========================================================================
// 位置設定
//========================================================================
void CObjectX::SetPosition(const D3DXVECTOR3 pos)
{
	// 位置を変更
	m_pos = pos;
}

//========================================================================
// スケール設定
//========================================================================
void CObjectX::SetScale(const D3DXVECTOR3 scale)
{
	// スケールを変更
	m_scale = scale;
}

//========================================================================
// 角度設定
//========================================================================
void CObjectX::SetRotation(const D3DXVECTOR3 rot)
{
	// 向きを変更
	m_rot = rot;

	// X向きを調整
	CorrectAngle(&m_rot.x, m_rot.x);

	// Y向きを調整
	CorrectAngle(&m_rot.y, m_rot.y);

	// Z向きを調整
	CorrectAngle(&m_rot.z, m_rot.z);
}

//========================================================================
// 頂点カラー設定
//========================================================================
void CObjectX::SetColor(const D3DXCOLOR col)
{
	m_col[0] = col;
}

//========================================================================
// 当たり判定
//========================================================================
bool CObjectX::Collision(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove,
	const float fRadius, const float fHeight, bool* pHead)
{
	bool bLand = false;		// 着地しているか
	int nCntLand = 0;	// 辺の内側に入った数(4回入っていれば、オブジェクトの内側にいる)

	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		D3DXVECTOR3 start, end;			// 始点/終点
		float fXS, fZS, fXE, fZE;		// 始点のXZ座標/終点のXZ座標
		float fMinY, fMaxY;				// Y座標の最低値と最大値

		D3DXVECTOR3 vecLine, vecMove, vecToPos, vecToPosOld, vecNor, vecMoveRef, vecMoveDest;		// 各ベクトル
		D3DXVECTOR3 vecLineW, posDest;

		float fRate, fDot;		// 交点の割合/内積
		float fXLength = m_VtxMax.x - m_VtxMin.x;		// X方向の長さ
		float fZLength = m_VtxMax.z - m_VtxMin.z;		// Z方向の長さ

		float fOffXS = (nCnt == 0 || nCnt == 3) ? +(fXLength * 0.5f + fRadius) : -(fXLength * 0.5f + fRadius);		// 向き0のときの座標
		float fOffZS = (nCnt == 0 || nCnt == 1) ? +(fZLength * 0.5f + fRadius) : -(fZLength * 0.5f + fRadius);		// 向き0のときの座標
		float fOffXE = (nCnt == 1 || nCnt == 0) ? +(fXLength * 0.5f + fRadius) : -(fXLength * 0.5f + fRadius);		// 向き0のときの座標
		float fOffZE = (nCnt == 1 || nCnt == 2) ? +(fZLength * 0.5f + fRadius) : -(fZLength * 0.5f + fRadius);		// 向き0のときの座標

		float fSin = sinf(-m_rot.y);		// 現在向きのsin
		float fCos = cosf(-m_rot.y);		// 現在向きのcos

		// Y向きを調整
		CorrectAngle(&m_rot.y, m_rot.y);

		// 始点のXZ座標
		fXS = fOffXS * fCos - fOffZS * fSin;
		fZS = fOffXS * fSin + fOffZS * fCos;

		// 終点のXZ座標
		fXE = fOffXE * fCos - fOffZE * fSin;
		fZE = fOffXE * fSin + fOffZE * fCos;

		// Y座標の最低値/最大値
		fMinY = m_pos.y + (m_VtxMin.y * m_scale.y) - fHeight;
		fMaxY = m_pos.y + (m_VtxMax.y * m_scale.y);

		// 始点
		start.x = m_pos.x + fXS;
		start.y = 0.0f;
		start.z = m_pos.z + fZS;

		// 終点
		end.x = m_pos.x + fXE;
		end.y = 0.0f;
		end.z = m_pos.z + fZE;

#ifdef _DEBUG
		CLine::Create(D3DXVECTOR3(start.x, fMinY, start.z), D3DXVECTOR3(end.x, fMinY, end.z), COLOR_RED);
		CLine::Create(D3DXVECTOR3(start.x, fMaxY, start.z), D3DXVECTOR3(end.x, fMaxY, end.z), COLOR_RED);
#endif

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
			if (fPosLine > 0.0f && (fPosOldLine <= 0.0f))
			{// 交差した
				if ((m_pos.y + m_VtxMin.y - fHeight <= pPos->y) &&
					(m_pos.y + m_VtxMax.y >= pPos->y))
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

		if (fPosLine > 0.0f)
		{// 今の位置が内側にいる
			nCntLand++;

			if (nCntLand == 4)
			{// 全ての内側に入っていたら
				if ((m_pos.y + m_VtxMin.y - fHeight <= pPos->y) &&
					(m_pos.y + m_VtxMax.y >= pPos->y))
				{// 範囲内
				}

				// 位置調整
				if ((m_pos.y + m_VtxMin.y - fHeight >= pPosOld->y) &&
					(m_pos.y + m_VtxMin.y - fHeight <= pPos->y))
				{// 下からの当たり判定
					pPos->y = m_pos.y + m_VtxMin.y - fHeight;
					pMove->y = -0.5f;							// 移動量を0にする

					if (pHead != nullptr)
					{// NULLチェック
						*pHead = true;
					}
				}
				else if ((m_pos.y + m_VtxMax.y <= pPosOld->y) &&
					(m_pos.y + m_VtxMax.y >= pPos->y))
				{// 上からの当たり判定
					pPos->y = m_pos.y + m_VtxMax.y;
					pMove->y = 0.0f;							// 移動量を0にする

					bLand = true;		// 着地フラグ
				}
			}
		}
	}

	return bLand;
}

//========================================================================
// 反射
//========================================================================
bool CObjectX::Reflect(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, D3DXVECTOR3* pRotDest,
	D3DXVECTOR3* pVecAxis, float* pValueRot, const float fRadius)
{
	bool bLand = false;		// 着地しているか
	int nCntLand = 0;	// 辺の内側に入った数(4回入っていれば、オブジェクトの内側にいる)

	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		D3DXVECTOR3 start, end;			// 始点/終点
		float fXS, fZS, fXE, fZE;		// 始点のXZ座標/終点のXZ座標
		float fMinY, fMaxY;				// Y座標の最低値と最大値

		D3DXVECTOR3 vecLine, vecMove, vecToPos, vecToPosOld, vecNor, vecMoveRef, vecMoveDest;		// 各ベクトル
		D3DXVECTOR3 vecLineW, posDest;

		float fRate, fDot;		// 交点の割合/内積
		float fXLength = m_VtxMax.x - m_VtxMin.x;		// X方向の長さ
		float fZLength = m_VtxMax.z - m_VtxMin.z;		// Z方向の長さ

		float fOffXS = (nCnt == 0 || nCnt == 3) ? +(fXLength * 0.5f + fRadius) : -(fXLength * 0.5f + fRadius);		// 向き0のときの座標
		float fOffZS = (nCnt == 0 || nCnt == 1) ? +(fZLength * 0.5f + fRadius) : -(fZLength * 0.5f + fRadius);		// 向き0のときの座標
		float fOffXE = (nCnt == 1 || nCnt == 0) ? +(fXLength * 0.5f + fRadius) : -(fXLength * 0.5f + fRadius);		// 向き0のときの座標
		float fOffZE = (nCnt == 1 || nCnt == 2) ? +(fZLength * 0.5f + fRadius) : -(fZLength * 0.5f + fRadius);		// 向き0のときの座標

		float fSin = sinf(-m_rot.y);		// 現在向きのsin
		float fCos = cosf(-m_rot.y);		// 現在向きのcos

		// Y向きを調整
		CorrectAngle(&m_rot.y, m_rot.y);

		// 始点のXZ座標
		fXS = fOffXS * fCos - fOffZS * fSin;
		fZS = fOffXS * fSin + fOffZS * fCos;

		// 終点のXZ座標
		fXE = fOffXE * fCos - fOffZE * fSin;
		fZE = fOffXE * fSin + fOffZE * fCos;

		// Y座標の最低値/最大値
		fMinY = m_pos.y + (m_VtxMin.y * m_scale.y) - fRadius;
		fMaxY = m_pos.y + (m_VtxMax.y * m_scale.y);

		// 始点
		start.x = m_pos.x + fXS;
		start.y = 0.0f;
		start.z = m_pos.z + fZS;

		// 終点
		end.x = m_pos.x + fXE;
		end.y = 0.0f;
		end.z = m_pos.z + fZE;

#ifdef _DEBUG
		CLine::Create(D3DXVECTOR3(start.x, fMinY, start.z), D3DXVECTOR3(end.x, fMinY, end.z), COLOR_RED);
		CLine::Create(D3DXVECTOR3(start.x, fMaxY, start.z), D3DXVECTOR3(end.x, fMaxY, end.z), COLOR_RED);
#endif

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
			if (fPosLine > 0.0f && (fPosOldLine <= 0.0f))
			{// 交差した
				if ((m_pos.y + m_VtxMin.y - fRadius <= pPos->y) &&
					(m_pos.y + m_VtxMax.y >= pPos->y))
				{// 高さが合っている
					pMove->x = vecMoveRef.x * 1.5f;
					pMove->z = vecMoveRef.z * 1.5f;
					pRotDest->y = atan2f(vecMoveRef.x, vecMoveRef.z) + D3DX_PI;
					pVecAxis->x = pMove->z;
					pVecAxis->z = -pMove->x;
					D3DXVec3Normalize(pVecAxis, pVecAxis);
				}
			}
		}

		if (fPosLine > 0.0f)
		{// 今の位置が内側にいる
			nCntLand++;

			if (nCntLand == 4)
			{// 全ての内側に入っていたら
				if ((m_pos.y + m_VtxMin.y - fRadius <= pPos->y) &&
					(m_pos.y + m_VtxMax.y >= pPos->y))
				{// 範囲内
				}

				if ((m_pos.y + m_VtxMax.y <= pPosOld->y) &&
					(m_pos.y + m_VtxMax.y >= pPos->y))
				{// 上からの当たり判定
					bLand = true;
				}
			}
		}
	}

	return bLand;
}