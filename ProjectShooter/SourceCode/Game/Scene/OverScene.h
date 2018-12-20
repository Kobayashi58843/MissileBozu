#pragma once

#include "Base\\BaseScene.h"

class OverScene : public BaseScene
{
public:
	OverScene(SCENE_NEED_POINTER PointerGroup);
	virtual ~OverScene();

	void Release();

private:
	enum enSprite
	{
		enSprite_BackGround = 0,
		enSprite_Logo,
		enSprite_ReturnButton,
		enSprite_ReturnButtonText,
		enSprite_Cursor,
		enSprite_Cupid1,
		enSprite_Cupid2,

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

	/*====/ �X�v���C�g�֘A /====*/
	DisplayBackBuffer*	m_pOneFrameSprite;
	BackBuffer*			m_pOneFrameBuff;

	TransitionsSprite*	m_pLightSprite;
	Sprite*				m_pLightMaskSprite;
	BackBuffer*			m_pLightMaskBuffer;

	//���o�̒i�K.
	int m_iPhase;

	//���o���i�񂾃^�C�~���O�p.
	bool m_bWhenProgress;

	float m_fCupidAddPos;

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

	//���o�̒i�K���Ƃ̕`��.
	void PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase);

	//���o�̒i�K���Ƃ̃J��������.
	void PhaseCameraControl(const int iPhase);

	//���o�̊e�i�K�̐i�s.
	void PhaseProgress(const int iPhase);

	//���o�̊e�i�K�̏�����.
	void PhaseInit(const int iPhase);

	//�X�v���C�g�̃}�X�N�̕`��.
	void RenderLightMaskBuffer();

#if _DEBUG

	//�f�o�b�O�e�L�X�g�̕\��.
	void RenderDebugText();

#endif //#if _DEBUG.

};