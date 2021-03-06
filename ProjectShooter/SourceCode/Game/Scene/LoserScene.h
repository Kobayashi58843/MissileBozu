#pragma once

#include "Base\\BaseScene.h"

class LoserScene : public BaseScene
{
public:
	LoserScene(SCENE_NEED_POINTER PointerGroup);
	virtual ~LoserScene();

	void Release();

private:
	enum enSprite
	{
		enSprite_Text = 0,
		enSprite_BackGround,

		enSprite_Max
	};

	void CreateProduct(const enSwitchToNextScene enNextScene);

	void UpdateProduct(enSwitchToNextScene &enNextScene);

	void RenderModelProduct(const int iRenderLevel);

	void RenderSpriteProduct(const int iRenderLevel);

	int m_iTime;

	/*====/ カメラ関連 /====*/
	EventCamera* m_pEventCamera;

	/*====/ モデル関連 /====*/
	EventModel* m_pPlayerModel;

	/*====/ スプライト関連 /====*/
	DisplayBackBuffer*	m_pOneFrameSprite;
	BackBuffer*			m_pOneFrameBuff;

	TransitionsSprite*	m_pLightSprite;
	Sprite*				m_pLightMaskSprite;
	BackBuffer*			m_pLightMaskBuffer;

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

	//スプライトのマスクの描画.
	void RenderLightMaskBuffer();

#if _DEBUG

	//デバッグテキストの表示.
	void RenderDebugText();

	//デバッグ中のみの操作.
	void DebugKeyControl();

#endif //#if _DEBUG.

};