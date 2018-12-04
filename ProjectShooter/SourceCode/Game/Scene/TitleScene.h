#pragma once

#include "Base\\BaseScene.h"

class TitleScene : public BaseScene
{
public:
	TitleScene(SCENE_NEED_POINTER PointerGroup);
	virtual ~TitleScene();

	void Release();

private:
	enum enSprite
	{
		enSprite_BackGround = 0,
		enSprite_BackGroundSub,
		enSprite_Logo,
		enSprite_StartButton,
		enSprite_EndButton,
		enSprite_StartButtonText,
		enSprite_EndButtonText,
		enSprite_Cursor,

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
	//�J�[�\���̃A�j���[�V�����p�̃J�E���g.
	int m_iCursorAnimationCount;

	//�w�i�p�̃o�b�t�@.
	BackBuffer* m_pTitleBackMainBuff;
	BackBuffer* m_pTitleBackSubBuff;

	//�w�i�摜.
	TitleBackgroundSprite* m_pBackGroundSprite;

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

	//�^�C�g���̔w�i�p�̃o�b�t�@��`��.
	void RenderBackGroundBuffMain();
	void RenderBackGroundBuffSub();

	/*====/ �V�[���ύX�֘A /====*/
	//�V�[���ύX.
	void ChangeScene(enSwitchToNextScene &enNextScene);

#if _DEBUG

	//�f�o�b�O�e�L�X�g�̕\��.
	void RenderDebugText();

#endif //#if _DEBUG.

};