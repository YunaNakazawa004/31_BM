//========================================================================
// 
// 砂パーティクルヘッダー [ sand.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _SAND_H_		// このマクロ定義がされていなかったら
#define _SAND_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "particle_batching.h"

//************************************************************************
// 砂パーティクルクラス
//************************************************************************
class CSand : public CParticleBatching
{
public:
	CSand(const int nPriority = PRIORITY_2);
	~CSand();

	static CSand* Create(const D3DXVECTOR3 posP, const D3DXVECTOR3 nor, const float fStrength);
	HRESULT Init(const D3DXVECTOR3 posP, const D3DXVECTOR3 nor, const float fStrength);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
};

#endif