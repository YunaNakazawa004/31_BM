//========================================================================
// 
// パーツヘッダー [ part.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _PART_H_		// このマクロ定義がされていなかったら
#define _PART_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// パーツクラス
//************************************************************************
class CPart
{
public:
	CPart();
	virtual ~CPart();

	virtual HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	void SetMtxWorld(D3DXMATRIX mtxWorld) { m_mtxWorld = mtxWorld; }
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }

	void SetPosition(const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	void SetPosOff(const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosOff(void) { return m_posOff; }
	D3DXVECTOR3 GetPosOffC(void) { return m_posOffC; }
	void SetRotation(const D3DXVECTOR3 rot);
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	void SetRotOff(const D3DXVECTOR3 rot);
	D3DXVECTOR3 GetRotOff(void) { return m_rotOff; }
	D3DXVECTOR3 GetRotOffC(void) { return m_rotOffC; }
	void SetScale(const D3DXVECTOR3 scale);
	D3DXVECTOR3 GetScale(void) { return m_scale; }

private:
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_rot;			// 向き
	D3DXVECTOR3 m_posOff;		// 位置（オフセット（モーション以外でいじる用））
	D3DXVECTOR3 m_rotOff;		// 向き（オフセット（モーション以外でいじる用））
	D3DXVECTOR3 m_posOffC;		// 位置（オフセット（絶対いじらない保存用））
	D3DXVECTOR3 m_rotOffC;		// 向き（オフセット（絶対いじらない保存用））
	D3DXVECTOR3 m_scale;		// スケール
};

#endif