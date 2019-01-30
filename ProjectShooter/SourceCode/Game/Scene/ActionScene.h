#pragma once

#include "Base\\BaseScene.h"

#include "..\\Model\\Shot\\BulletManager.h"

class ActionScene : public BaseScene
{
public:
	ActionScene(SCENE_NEED_POINTER PointerGroup);
	virtual ~ActionScene();

	void Release();

private:
	enum enSprite
	{
		enSprite_Alignment,
		enSprite_PlayerHp,
		enSprite_PlayerHpFrame,
		enSprite_EnemyHp,
		enSprite_EnemyHpFrame,

		enSprite_Max
	};

	void CreateProduct(const enSwitchToNextScene enNextScene);

	void UpdateProduct(enSwitchToNextScene &enNextScene);

	void RenderModelProduct(const int iRenderLevel);

	void RenderSpriteProduct(const int iRenderLevel);

	/*====/ カメラ関連 /====*/
	Camera* m_pCamera;
	EventCamera* m_pLightView;

	//カメラの操作.
	void ControlCameraMove();

	/*====/ モデル関連 /====*/
	Player* m_pPlayer;
	Enemy* m_pEnemy;

	clsDX9Mesh* m_pGround;
	clsDX9Mesh* m_pSky;

	BulletManager* m_pBulletManager;

	/*====/ スプライト関連 /====*/

	//シャドウマップ用のバックバッファ.
	BackBuffer* m_pDepthBuffer;
	DisplayBackBuffer* m_pDisplayDepthBuffer;

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

#if _DEBUG

	//デバッグテキストの表示.
	void RenderDebugText();

	//デバッグ中のみの操作.
	void DebugKeyControl();

#endif //#if _DEBUG.

};