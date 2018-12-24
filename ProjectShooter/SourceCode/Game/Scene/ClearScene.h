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

	/*====/ �G�t�F�N�g�֘A /====*/
	clsEffects* m_pEffect;
	//�n���h��.
	::Effekseer::Handle m_MissileHandle;

	/*====/ �J�����֘A /====*/
	EventCamera* m_pModelSpriteCamera;

	/*====/ ���f���֘A /====*/
	EventModel* m_pPlayerModel;

	//���o�̒i�K.
	int m_iPhase;

	//���o���i�񂾃^�C�~���O�p.
	bool m_bWhenProgress;

	/*====/ �X�v���C�g�֘A /====*/
	DisplayBackBuffer*	m_pModelSprite;
	BackBuffer*			m_pModelBuff;

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

	void ModelControl(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase);

#if _DEBUG

	//�f�o�b�O�e�L�X�g�̕\��.
	void RenderDebugText();

#endif //#if _DEBUG.

};