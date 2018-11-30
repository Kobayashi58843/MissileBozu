#pragma once

#include "Base\\BaseScene.h"

class WinningScene : public BaseScene
{
public:
	WinningScene(SCENE_NEED_POINTER PointerGroup);
	virtual ~WinningScene();

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

	/*====/ �X�v���C�g�֘A /====*/
	//�J�[�\���̃A�j���[�V�����p�̃J�E���g.
	int m_iCursorAnimationCount;

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

#endif //#if _DEBUG.

};