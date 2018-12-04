#include "Game.h"

#include "Sound\\SoundManager.h"
#include "Effekseer\\CEffects.h"
#include "Model\\ModelResource\\ModelResource.h"

#include "..\\Singleton.h"

//�Q�[���J�n���̃V�[��.
const enSwitchToNextScene START_SCENE = enSwitchToNextScene::StartEvent;

//�t�F�[�h�̑��x.
const float FADE_SPEED = 0.02f;

Game::Game(const HWND hWnd)
	: m_pScene(nullptr)
	, m_bBlackOut(false)
{
	//Direct3D�쐬.
	m_pDirect3D = new Direct3D;
	m_pDirect3D->Create(hWnd);

	//�쐬���ꂽDirect3D����f�o�C�X�Ȃǂ�����Ă���.
	m_pDevice = m_pDirect3D->GetDevice();
	m_pDeviceContext = m_pDirect3D->GetDeviceContext();

	//�V�F�[�_�̍쐬.
	Singleton<ShaderGathering>().GetInstance().InitShader(m_pDevice, m_pDeviceContext);

	//���\�[�X�̍쐬.
	Singleton<ModelResource>().GetInstance().CreateModelAll(hWnd, m_pDevice, m_pDeviceContext);

	//BGM��SE���쐬.
	Singleton<SoundManager>().GetInstance().LoadSound(hWnd);

	//�G�t�F�N�g�̍쐬.
	clsEffects::GetInstance()->Create(m_pDevice, m_pDeviceContext);
	clsEffects::GetInstance()->LoadData();

	//RawInput������.
	Singleton<RawInput>().GetInstance().InitRawInput(hWnd);

	//�V�[���ɕK�v�ȃ|�C���^���Z�b�g.
	m_SceneNeedPointer.SetSceneNeedPointer(hWnd, m_pDevice, m_pDeviceContext,
		m_pDirect3D->GetRenderTargetView(), nullptr, m_pDirect3D->GetDepthStencilView());

	SwitchScene(START_SCENE);

	/*====/ �t�F�[�h�p�X�v���C�g�֘A /====*/
	m_pFadeMaskBuffer = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	D3DXVECTOR2 vDivisionQuantity = { 1.0f, 1.0f };
	m_pFadeSprite = new TransitionsSprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pFadeSprite->Create(m_pDevice, m_pDeviceContext, "Data\\Image\\Fade.jpg");
	m_pFadeSprite->SetMaskTexture(m_pFadeMaskBuffer->GetShaderResourceView());

	m_pFadeMaskSprite = new Sprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pFadeMaskSprite->Create(m_pDevice, m_pDeviceContext, "Data\\Image\\Transitions.png");

	//�ʒu���E�C���h�E�̒��S�ɐݒ�.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;
	m_pFadeSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
	m_pFadeMaskSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);

	//�͂��߂͔�\���ɐݒ肵�Ă���.
	m_pFadeSprite->SetDispFlg(false);

	//���ߒl��0�ɂ���.
	m_pFadeSprite->SetAlpha(0.0f);

	m_NextSceneTemporary = enSwitchToNextScene::Nothing;
}

Game::~Game()
{
	SAFE_DELETE(m_pFadeSprite);

	SAFE_DELETE(m_pFadeMaskSprite);

	SAFE_DELETE(m_pFadeMaskBuffer);

	SAFE_DELETE(m_pScene);

	clsEffects::GetInstance()->Destroy();

	SAFE_DELETE(m_pDirect3D);

	m_pDeviceContext = nullptr;
	m_pDevice = nullptr;
}

//���b�Z�[�W���̏���.
void Game::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		//�L�[�{�[�h�������ꂽ�Ƃ��̃L�[�ʂ̏���.
		switch ((char)wParam)
		{
		case VK_ESCAPE:
			//�E�B���h�E��j������.
			DestroyWindow(hWnd);

			break;
		}
		break;
	}

	Singleton<RawInput>().GetInstance().MsgProc(uMsg, lParam);
}

//�X�V.
void Game::Update()
{
	enSwitchToNextScene enSwitchNextScene = enSwitchToNextScene::Nothing;

	assert(m_pScene != nullptr);
	m_pScene->Update(enSwitchNextScene);

	//�V�[���̕ύX������΃t�F�[�h�����Ȃ���V�[����؂�ւ���.
	Fade(enSwitchNextScene);

	//�V�[���̕`��.
	Render();

	//�}�E�X�̓��͏����N���A.
	Singleton<RawInput>().GetInstance().ClearPerFrame();
}

//�t�F�[�h.
void Game::Fade(const enSwitchToNextScene enNextScene)
{
	//�V�[���̕ύX���������Ƃ�.
	if (enNextScene != enSwitchToNextScene::Nothing)
	{
		//�t�F�[�h�p�̉摜���\���\���ǂ����Ńt�F�[�h���𔻒f����.
		//�t�F�[�h���łȂ��ꍇ.
		if (!m_pFadeSprite->IsDispFlg())
		{
			//�t�F�[�h�p�̉摜��\���\�ɂ���.
			m_pFadeSprite->SetDispFlg(true);
			m_bBlackOut = true;
			m_NextSceneTemporary = enNextScene;
		}
	}

	if (m_pFadeSprite->IsDispFlg())
	{
		//�����_�[�^�[�Q�b�g���t�F�[�h�p�摜�Ɏg���}�X�N�p�o�b�t�@�ɕς���.
		ID3D11RenderTargetView* pRTV = m_pFadeMaskBuffer->GetRenderTargetView();
		m_pDeviceContext->OMSetRenderTargets(1, &pRTV, m_pDirect3D->GetDepthStencilView());

		//��ʂ̃N���A.
		const float fClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_pDeviceContext->ClearRenderTargetView(m_pFadeMaskBuffer->GetRenderTargetView(), fClearColor);
		m_pDeviceContext->ClearDepthStencilView(m_pDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//���[���摜��`�悷��.
		m_pFadeMaskSprite->Render();

		//�����_�[�^�[�Q�b�g�����ɖ߂�.
		pRTV = m_pDirect3D->GetRenderTargetView();
		m_pDeviceContext->OMSetRenderTargets(1, &pRTV, m_pDirect3D->GetDepthStencilView());

		if (m_bBlackOut)
		{
			if (1.0f > m_pFadeSprite->GetAlpha())
			{
				m_pFadeSprite->AddAlpha(FADE_SPEED);
			}
			else
			{
				//���ߒl���ő�ɂȂ������ɃV�[����؂�ւ���.
				SwitchScene(m_NextSceneTemporary);
				m_bBlackOut = false;
			}
		}
		else
		{
			if (0.0f < m_pFadeSprite->GetAlpha())
			{
				m_pFadeSprite->AddAlpha(-FADE_SPEED);
			}
			else
			{
				//�t�F�[�h�p�̉摜���\���ɂ���.
				m_pFadeSprite->SetDispFlg(false);
				m_pFadeSprite->SetAlpha(0.0f);
			}
		}
	}
}

//�V�[���؂�ւ�.
void Game::SwitchScene(const enSwitchToNextScene enNextScene)
{
	//�O�̃V�[��������.
	SAFE_DELETE(m_pScene);

	//���̃V�[���ɐ؂�ւ���.
	m_pScene = m_SceneFactory.Create(enNextScene, m_SceneNeedPointer);
	m_pScene->Create(enNextScene);
}

//�`��.
void Game::Render()
{
	m_pDirect3D->ClearBackBuffer();

	assert(m_pScene != nullptr);

	for (unsigned int i = 0; i <= MAX_RENDER_LEVEL; i++)
	{
		//�����_�[�^�[�Q�b�g�����ɖ߂�.
		ID3D11RenderTargetView* pRTV = m_pDirect3D->GetRenderTargetView();
		m_pDeviceContext->OMSetRenderTargets(1, &pRTV, m_pDirect3D->GetDepthStencilView());
		m_pDeviceContext->ClearDepthStencilView(m_pDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//�[�x�e�X�g��L��.
		m_pDirect3D->SetDepth(true);

		//3D���f���̕`��.
		m_pScene->RenderModel(i);

		//�[�x�e�X�g�𖳌�.
		m_pDirect3D->SetDepth(false);

		//2D�X�v���C�g�̕`��.
		m_pScene->RenderSprite(i);
	}

	//�t�F�[�h�p�摜�̕`��.
	m_pFadeSprite->Render();

	m_pDirect3D->RenderSwapChain();
}