#pragma once

#include "Base\\BaseScene.h"

class StartEventScene : public BaseScene
{
public:
	StartEventScene(SCENE_NEED_POINTER PointerGroup);
	virtual ~StartEventScene();

	void Release();

private:
	enum enSprite
	{
		enSprite_BackGround = 0,
		enSprite_Number,
		enSprite_Fire,

		enSprite_Max
	};

	void CreateProduct(const enSwitchToNextScene enNextScene);

	void UpdateProduct(enSwitchToNextScene &enNextScene);

	void RenderModelProduct(const int iRenderLevel);

	void RenderSpriteProduct(const int iRenderLevel);

	int m_iTime;

	/*====/ カメラ関連 /====*/
	EventCamera* m_pEventCamera;
	EventCamera* m_pModelSpriteCamera;

	/*====/ モデル関連 /====*/
	EventModel* m_pPlayerModel;

	clsDX9Mesh* m_pGround;

	/*====/ スプライト関連 /====*/
	DisplayBackBuffer*	m_pModelSprite;
	BackBuffer*			m_pModelBuff;

	//シーン切り替え時のフェード用画像.
	TransitionsSprite*	m_pFadeSprite;
	Sprite*				m_pFadeMaskSprite;
	BackBuffer*			m_pFadeMaskBuffer;

	//演出の段階.
	int m_iPhase;

	//演出が進んだタイミング用.
	bool m_bWhenProgress;

	//スプライトの作成.
	void CreateSprite();

	//スプライトの解放.
	void ReleaseSprite();

	//スプライトのフレームごとの更新.
	void UpdateSprite();

	//スプライトの位置.
	void UpdateSpritePositio(int iSpriteNo);

	//スプライトのアニメーション.
	void UpdateSpriteAnimation(int iSpriteNo);

	//演出の段階ごとの描画.
	void PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase);

	//演出の段階ごとのカメラ操作.
	void PhaseCameraControl(const int iPhase);

	//演出の各段階の進行.
	void PhaseProgress(const int iPhase);

	//演出の各段階の初期化.
	void PhaseInit(const int iPhase);

	//.
	void ModelControl(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase);

	//フェードアウト.
	bool FadeOut();
	//フェードイン.
	bool FadeIn();
	//フェード用のマスクの描画.
	void RenderFadeMaskBuffer();

#if _DEBUG

	//デバッグテキストの表示.
	void RenderDebugText();

	//デバッグ中のみの操作.
	void DebugKeyControl();

#endif //#if _DEBUG.

};