#pragma once

#include "Base\\BaseScene.h"

class StartingScene : public BaseScene
{
public:
	StartingScene(SCENE_NEED_POINTER PointerGroup);
	virtual ~StartingScene();

	void Release();

private:
	enum enSprite
	{
		enSprite_BackGround = 0,

		enSprite_Max
	};

	void CreateProduct(const enSwitchToNextScene enNextScene);

	void UpdateProduct(enSwitchToNextScene &enNextScene);

	void RenderModelProduct(const int iRenderLevel);

	void RenderSpriteProduct(const int iRenderLevel);

	/*====/ �J�����֘A /====*/
	EventCamera* m_pEventCamera;

	/*====/ ���f���֘A /====*/
	EventModel* m_pPlayerModel;
	EventModel* m_pEnemyModel;

	/*====/ �X�v���C�g�֘A /====*/
	DisplayBackBuffer*	m_pOneFrameSprite;
	BackBuffer*			m_pOneFrameBuff;

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