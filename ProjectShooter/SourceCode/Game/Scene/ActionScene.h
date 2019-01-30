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

	/*====/ �J�����֘A /====*/
	Camera* m_pCamera;
	EventCamera* m_pLightView;

	//�J�����̑���.
	void ControlCameraMove();

	/*====/ ���f���֘A /====*/
	Player* m_pPlayer;
	Enemy* m_pEnemy;

	clsDX9Mesh* m_pGround;
	clsDX9Mesh* m_pSky;

	BulletManager* m_pBulletManager;

	/*====/ �X�v���C�g�֘A /====*/

	//�V���h�E�}�b�v�p�̃o�b�N�o�b�t�@.
	BackBuffer* m_pDepthBuffer;
	DisplayBackBuffer* m_pDisplayDepthBuffer;

	//�X�v���C�g�̍쐬.
	void CreateSprite();

	//�X�v���C�g�̉��.
	void ReleaseSprite();

	//�X�v���C�g�̃t���[�����Ƃ̍X�V.
	void UpdateSprite();

	//�X�v���C�g�̈ʒu.
	void UpdateSpritePositio(int iSpriteNo);

	//�X�v���C�g�̃A�j���[�V����.
	void UpdateSpriteAnimation(int iSpriteNo);

#if _DEBUG

	//�f�o�b�O�e�L�X�g�̕\��.
	void RenderDebugText();

	//�f�o�b�O���݂̂̑���.
	void DebugKeyControl();

#endif //#if _DEBUG.

};