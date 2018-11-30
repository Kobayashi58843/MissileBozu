#pragma once

#include "..\\\..\\Global.h"
#include <vector>

#include "..\\..\\..\\Singleton.h"

#include "..\\..\\Camera\\Camera.h"

#include "..\\..\\Model\\ModelResource\\ModelResource.h"

#include "..\\..\\Model\\Character\\Player\\Player.h"
#include "..\\..\\Model\\Character\\Enemy\\Enemy.h"
#include "..\\..\\Model\\Character\\Event\\EventModel.h"

#include "..\\..\\Sprite\\BlurFilter.h"
#include "..\\..\\Sprite\\DisplayBackBuffer.h"
#include "..\\..\\Sprite\\MaskSprite.h"
#include "..\\..\\Sprite\\NumberSprite.h"
#include "..\\..\\Sprite\\Sprite.h"
#include "..\\..\\Sprite\\TransitionsSprite.h"

#include "..\\..\\BackBuffer.h"

#include "..\\..\\Sprite\\DebugText.h"
#include "..\\..\\RawInput.h"

#include "..\\..\\Sound\\SoundManager.h"

//�`�揇�̍ő吔.
static const int MAX_RENDER_LEVEL = 10;

struct SCENE_NEED_POINTER
{
	//�E�C���h�E�n���h��.
	HWND hWnd;
	//�f�o�C�X�I�u�W�F�N�g.
	ID3D11Device* pDevice;
	//�f�o�C�X�R���e�L�X�g.
	ID3D11DeviceContext* pContext;

	//�����_�[�^�[�Q�b�g�r���[.
	ID3D11RenderTargetView* pBackBuffer_RTV;

	//�V�F�[�_�[���\�[�X�r���[.
	ID3D11ShaderResourceView* pBackBuffer_SRV;

	//�f�v�X�X�e���V���r���[.
	ID3D11DepthStencilView*	 pBackBuffer_DSV;

	//�e�|�C���^�[�ƃC���X�^���X���Z�b�g.
	void SetSceneNeedPointer(const HWND Wnd, ID3D11Device* const Device, ID3D11DeviceContext* const Context,
		ID3D11RenderTargetView* const BackBuffer_RTV, ID3D11ShaderResourceView* const BackBuffer_SRV, ID3D11DepthStencilView* const BackBuffer_DSV)
	{
		hWnd = Wnd;
		pDevice = Device;
		pContext = Context;

		pBackBuffer_RTV = BackBuffer_RTV;
		pBackBuffer_SRV = BackBuffer_SRV;
		pBackBuffer_DSV = BackBuffer_DSV;
	}
};

class BaseScene
{
public:
	BaseScene(SCENE_NEED_POINTER PointerGroup);
	virtual ~BaseScene();

	/*====/ �ǂ̃V�[���ł������������������ /====*/
	//�쐬.
	void Create(const enSwitchToNextScene enNextScene);

	//���.
	virtual void Release() = 0;

	//�X�V.
	void Update(enSwitchToNextScene &enNextScene);

	//3D���f���̕`��.
	void RenderModel(const int iRenderLevel);

	//2D�X�v���C�g�̕`��.
	void RenderSprite(const int iRenderLevel);

protected:
	SCENE_NEED_POINTER m_SceneNeedPointer;

	//�X�v���C�g.
	std::vector<Sprite*> m_vpSprite;

	/*====/ �e�V�[�����ƂɈႤ������������� /====*/
	//�e�V�[���̍쐬.
	virtual void CreateProduct(const enSwitchToNextScene enNextScene) = 0;

	//�e�V�[���̍X�V.
	virtual void UpdateProduct(enSwitchToNextScene &enNextScene) = 0;

	//�e�V�[����3D���f���̕`��.
	virtual void RenderModelProduct(const int iRenderLevel) = 0;

	//�e�V�[����2D�X�v���C�g�̕`��.
	virtual void RenderSpriteProduct(const int iRenderLevel) = 0;

	//�X�v���C�g�̍쐬.
	virtual void CreateSprite() = 0;

	//�X�v���C�g�̉��.
	virtual void ReleaseSprite() = 0;

	//�X�v���C�g�̃t���[�����Ƃ̍X�V.
	virtual void UpdateSprite() = 0;

	//�X�v���C�g�̈ʒu.
	virtual void UpdateSpritePositio(int iSpriteNo) = 0;

	//�X�v���C�g�̃A�j���[�V����.
	virtual void UpdateSpriteAnimation(int iSpriteNo) = 0;

	//�X�v���C�g���m�̐ڐG.
	bool IsHittingOfSprite(int iSourceSpriteNo, int iTargetSpriteNo);

	/*====/ �f�o�b�O�p /====*/
#if _DEBUG

	void InputUpdate(enSwitchToNextScene &enNextScene);

	DebugText* m_pDebugText;

	//�f�o�b�O�e�L�X�g�̕\��.
	virtual void RenderDebugText() = 0;

#endif //#if _DEBUG.
};