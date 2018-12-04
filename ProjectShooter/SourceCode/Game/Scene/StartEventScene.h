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

	/*====/ �J�����֘A /====*/
	EventCamera* m_pEventCamera;
	EventCamera* m_pModelSpriteCamera;

	/*====/ ���f���֘A /====*/
	EventModel* m_pPlayerModel;

	clsDX9Mesh* m_pGround;

	/*====/ �X�v���C�g�֘A /====*/
	DisplayBackBuffer*	m_pModelSprite;
	BackBuffer*			m_pModelBuff;

	//�V�[���؂�ւ����̃t�F�[�h�p�摜.
	TransitionsSprite*	m_pFadeSprite;
	Sprite*				m_pFadeMaskSprite;
	BackBuffer*			m_pFadeMaskBuffer;

	//���o�̒i�K.
	int m_iPhase;

	//���o���i�񂾃^�C�~���O�p.
	bool m_bWhenProgress;

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

	//.
	void ModelControl(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase);

	//�t�F�[�h�A�E�g.
	bool FadeOut();
	//�t�F�[�h�C��.
	bool FadeIn();
	//�t�F�[�h�p�̃}�X�N�̕`��.
	void RenderFadeMaskBuffer();

#if _DEBUG

	//�f�o�b�O�e�L�X�g�̕\��.
	void RenderDebugText();

	//�f�o�b�O���݂̂̑���.
	void DebugKeyControl();

#endif //#if _DEBUG.

};