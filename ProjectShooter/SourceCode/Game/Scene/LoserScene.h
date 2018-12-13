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
	::Effekseer::Handle m_ExpHandle;
	::Effekseer::Handle m_MissileHandle;

	/*====/ �J�����֘A /====*/
	EventCamera* m_pEventCamera;

	/*====/ ���f���֘A /====*/
	EventModel* m_pPlayerModel;
	EventModel* m_pEnemyModel;

	clsDX9Mesh* m_pSky;
	clsDX9Mesh* m_pStage;

	/*====/ �X�v���C�g�֘A /====*/
	DisplayBackBuffer*	m_pOneFrameSprite;
	BackBuffer*			m_pOneFrameBuff;

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

#if _DEBUG

	//�f�o�b�O�e�L�X�g�̕\��.
	void RenderDebugText();

	//�f�o�b�O���݂̂̑���.
	void DebugKeyControl();

#endif //#if _DEBUG.

};