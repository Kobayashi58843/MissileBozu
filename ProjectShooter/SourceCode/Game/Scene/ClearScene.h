#pragma once

#include "Base\\BaseScene.h"

class ClearScene : public BaseScene
{
public:
	ClearScene(SCENE_NEED_POINTER PointerGroup);
	virtual ~ClearScene();

	void Release();

private:
	enum enSprite
	{
		enSprite_BackGround = 0,
		enSprite_Logo,
		enSprite_ReturnButton,
		enSprite_ReturnButtonText,
		enSprite_Cursor,

		enSprite_Max
	};

	void CreateProduct(const enSwitchToNextScene enNextScene);

	void UpdateProduct(enSwitchToNextScene &enNextScene);

	void RenderModelProduct(const int iRenderLevel);

	void RenderSpriteProduct(const int iRenderLevel);

	int m_iTime;

	/*====/ エフェクト関連 /====*/
	clsEffects* m_pEffect;
	//ハンドル.
	::Effekseer::Handle m_MissileHandle;

	/*====/ カメラ関連 /====*/
	EventCamera* m_pModelSpriteCamera;

	/*====/ モデル関連 /====*/
	EventModel* m_pPlayerModel;

	//演出の段階.
	int m_iPhase;

	//演出が進んだタイミング用.
	bool m_bWhenProgress;

	/*====/ スプライト関連 /====*/
	DisplayBackBuffer*	m_pModelSprite;
	BackBuffer*			m_pModelBuff;

	//カーソルのアニメーション用のカウント.
	int m_iCursorAnimationCount;

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

	void ModelControl(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase);

#if _DEBUG

	//デバッグテキストの表示.
	void RenderDebugText();

#endif //#if _DEBUG.

};