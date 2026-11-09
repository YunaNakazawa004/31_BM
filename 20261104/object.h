//========================================================================
// 
// オブジェクトヘッダー [ object.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _OBJECT_H_		// このマクロ定義がされていなかったら
#define _OBJECT_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define DEFAULT_VECTOR3		(D3DXVECTOR3(0.0f,0.0f,0.0f))		// デフォルトのVec3座標
#define DEFAULT_VECTOR2		(D3DXVECTOR2(0.0f,0.0f))			// デフォルトのVec2座標
#define RANDOM_ROT(x)		((float)(rand() % x - (x / 2)) / 100.0f)		// 角度をランダム
#define FULL_PI				(D3DX_PI * 2.0f)								// 一周の円周率

#define COLOR_RED			(D3DXCOLOR(1.0f,0.0f,0.0f,1.0f))				// 赤
#define COLOR_GREEN			(D3DXCOLOR(0.0f,0.5f,0.0f,1.0f))				// 緑
#define COLOR_BLUE			(D3DXCOLOR(0.0f,0.0f,1.0f,1.0f))				// 青
#define COLOR_YELLOW		(D3DXCOLOR(1.0f,1.0f,0.0f,1.0f))				// 黄
#define COLOR_WHITE			(D3DXCOLOR(1.0f,1.0f,1.0f,1.0f))				// 白
#define COLOR_BLACK			(D3DXCOLOR(0.0f,0.0f,0.0f,1.0f))				// 黒
#define COLOR_GRAY			(D3DXCOLOR(0.6f,0.6f,0.6f,1.0f))				// 薄灰
#define COLOR_DARKGRAY		(D3DXCOLOR(0.3f,0.3f,0.3f,1.0f))				// 灰
#define COLOR_BLUEGRAY		(D3DXCOLOR(0.439f,0.501f,0.564f,1.0f))			// 青灰
#define COLOR_CYAN			(D3DXCOLOR(0.0f,1.0f,1.0f,1.0f))				// シアン
#define COLOR_MAGENTA		(D3DXCOLOR(1.0f,0.0f,1.0f,1.0f))				// マゼンタ
#define COLOR_VIOLET		(D3DXCOLOR(0.933f,0.509f,0.933f,1.0f))			// 紫
#define COLOR_ORANGE		(D3DXCOLOR(1.0f,0.647f,0.0f,1.0f))				// 橙
#define COLOR_BEIGE			(D3DXCOLOR(0.76f,0.647f,0.0f,1.0f))				// ベージュ
#define COLOR_LIMEGREEN		(D3DXCOLOR(0.0f,1.0f,0.0f,1.0f))				// ライムグリーン
#define COLOR_SKYBLUE		(D3DXCOLOR(0.529f,0.807f,0.921f,1.0f))			// 空色
#define COLOR_HISUI			(D3DXCOLOR(0.219f,0.705f,0.545f,1.0f))			// 翡翠色
#define COLOR_BROWN			(D3DXCOLOR(0.3f, 0.1f, 0.0f, 1.0f))				// 茶色
#define COLOR_RETROFILTER	(D3DXCOLOR(1.000f,	1.000f,	1.000f,	0.080f))	// レトロ風
#define COLOR_DISALPHA		(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.35f))			// ほぼ透明
#define COLOR_WHITE_ALPHA	(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f))				// 半透明（白）
#define COLOR_NONE			(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f))				// カラーの初期値

#define LIST				// リスト構造にするかどうか
//#undef LIST

//************************************************************************
// オブジェクトクラス
//************************************************************************
class CObject
{
public:
	// オブジェクトの種類
	typedef enum
	{
		TYPE_NONE = 0,			// 種類なし
		TYPE_EFFECT2D,			// エフェクト2D
		TYPE_EFFECT3D,			// エフェクト3D
		TYPE_PARTICLE2D,		// パーティクル2D
		TYPE_PARTICLE3D,		// パーティクル3D
		TYPE_PLAYER,			// プレイヤー
		TYPE_WIND,				// 風
		TYPE_FIELD,				// 地面
		TYPE_WALL,				// 壁
		TYPE_MESHWALL,			// メッシュウォール
		TYPE_MESHRING,			// メッシュリング
		TYPE_MESHORBIT,			// メッシュオービット
		TYPE_LINE,				// 線
		TYPE_SCORE,				// スコア
		TYPE_RANKINGSCORE,		// ランキングスコア
		TYPE_UI,				// UI
		TYPE_MAX
	}TYPE;

	// 優先順位
	enum PRIORITY
	{
		PRIORITY_0 = 0,		// 優先順位0
		PRIORITY_1,			// 優先順位1
		PRIORITY_2,			// 優先順位2
		PRIORITY_3,			// 優先順位3
		PRIORITY_4,			// 優先順位4
		PRIORITY_5,			// 優先順位5
		PRIORITY_6,			// 優先順位6
		PRIORITY_7,			// 優先順位7
		PRIORITY_UI,		// UI用の優先順位
		PRIORITY_MAX,
	};

	CObject();
	CObject(const int nPriority = 3);
	virtual ~CObject();

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	virtual D3DXVECTOR3 GetPosition(void) = 0;
	virtual D3DXVECTOR3 GetRotation(void) = 0;

	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static void DrawWOUI(void);
	static void DrawUI(void);

	static int GetNumAll(void) { return m_nNumAll; }
	void SetType(const TYPE type) { m_type = type; }
	TYPE GetType(void) { return m_type; }

	static void CorrectAngle(float* pAngle, float fAngle);

#ifdef LIST
	static CObject* GetTop(const int nPriority) { return m_apTop[nPriority]; }
	CObject* GetNext(void) { return m_pNext; }

#else
	static CObject* GetObject(const int nPriority, const int nIdx) { return m_apObject[nPriority][nIdx]; }

#endif

protected:
	void Release(void);

private:
#ifdef LIST
	static CObject* m_apTop[PRIORITY_MAX];			// 先頭のオブジェクトへのポインタ
	static CObject* m_apCur[PRIORITY_MAX];			// 最後尾のオブジェクトへのポインタ
	CObject* m_pPrev;				// 前のオブジェクトへのポインタ
	CObject* m_pNext;				// 次のオブジェクトへのポインタ
	bool m_bDeath;					// 死亡フラグ

#else
	static CObject* m_apObject[PRIORITY_MAX][MAX_OBJECT];	// オブジェクトのインスタンス
	int m_nID;						// 自分自身のID

#endif
	int m_nPriority;				// 自分自身の優先順位
	static int m_nNumAll;			// オブジェクトの総数
	TYPE m_type;					// 自分の種類
};

#endif