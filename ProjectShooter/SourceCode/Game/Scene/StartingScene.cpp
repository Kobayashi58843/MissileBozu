#include "StartingScene.h"

#include "..\\..\\Singleton.h"

//���f���̏����A�j���[�V�������x.
const double ANIMETION_SPEED = 0.01;

//���o�̒i�K�̍ő吔.
const int MAX_PHASE = 5;

//�t�F�[�h�̑��x.
const float FADE_SPEED = 0.05f;

StartingScene::StartingScene(SCENE_NEED_POINTER PointerGroup)
	: BaseScene(PointerGroup)
	, m_pOneFrameSprite(nullptr)
	, m_pOneFrameBuff(nullptr)
	, m_pEventCamera(nullptr)
	, m_pPlayerModel(nullptr)
	, m_pEnemyModel(nullptr)
	, m_pSky(nullptr)
	, m_iPhase(0)
	, m_bWhenProgress(true)
{
	//�S�T�E���h���~����.
	Singleton<SoundManager>().GetInstance().StopSound();

	m_pOneFrameBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	/*====/ �t�F�[�h�p�X�v���C�g�֘A /====*/
	m_pFadeMaskBuffer = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	D3DXVECTOR2 vDivisionQuantity = { 1.0f, 1.0f };
	m_pFadeSprite = new TransitionsSprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pFadeSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\White.jpg");
	m_pFadeSprite->SetMaskTexture(m_pFadeMaskBuffer->GetShaderResourceView());

	m_pFadeMaskSprite = new Sprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pFadeMaskSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\Transitions1.png");

	//�ʒu���E�C���h�E�̒��S�ɐݒ�.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;
	m_pFadeSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
	m_pFadeMaskSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);

	//�͂��߂͔�\���ɐݒ肵�Ă���.
	m_pFadeSprite->SetDispFlg(false);

	//���ߒl��0�ɂ���.
	m_pFadeSprite->SetAlpha(0.0f);
}

StartingScene::~StartingScene()
{
	SAFE_DELETE(m_pFadeSprite);

	SAFE_DELETE(m_pFadeMaskSprite);

	SAFE_DELETE(m_pFadeMaskBuffer);

	Release();
}

//�쐬.
void StartingScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//�X�v���C�g�̍쐬.
	CreateSprite();

	//�J�������쐬.
	m_pEventCamera = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	//�X�L�����f���̍쐬.
	m_pPlayerModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Player), 0.0005f, ANIMETION_SPEED);
	m_pPlayerModel->ChangeAnimation(1);

	m_pEnemyModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Enemy), 0.08f, ANIMETION_SPEED);
	m_pEnemyModel->ChangeAnimation(5);

	m_pSky = Singleton<ModelResource>().GetInstance().GetStaticModels(ModelResource::enStaticModel_SkyBox);
	m_pSky->SetScale(32.0f);
}

//���.
void StartingScene::Release()
{
	SAFE_DELETE(m_pEnemyModel);
	SAFE_DELETE(m_pPlayerModel);

	m_pSky = nullptr;

	SAFE_DELETE(m_pEventCamera);

	SAFE_DELETE(m_pOneFrameBuff);
	SAFE_DELETE(m_pOneFrameSprite);

	ReleaseSprite();
}

//�X�V.
void StartingScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
#if _DEBUG

	//�f�o�b�O���݂̂̑���.
	DebugKeyControl();

#endif //#if _DEBUG.

	//BGM�����[�v�ōĐ�.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Title);



	//�X�v���C�g�X�V.
	UpdateSprite();

	//�J�����X�V.
	m_pEventCamera->Update();

	//���̃V�[����.
	if (m_iPhase >= MAX_PHASE)
	{
		enNextScene = enSwitchToNextScene::Title;
	}

	if (Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		enNextScene = enSwitchToNextScene::Title;
	}
}

//3D���f���̕`��.
void StartingScene::RenderModelProduct(const int iRenderLevel)
{
	D3DXMATRIX mView = m_pEventCamera->GetView();
	D3DXMATRIX mProj = m_pEventCamera->GetProjection();

	switch (iRenderLevel)
	{
	case 0:
	{
		//�����_�[�^�[�Q�b�g���Z�b�g.
		ID3D11RenderTargetView* pRTV = m_pOneFrameBuff->GetRenderTargetView();
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

		//��ʂ̃N���A.
		const float fClearColor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
		m_SceneNeedPointer.pContext->ClearRenderTargetView(m_pOneFrameBuff->GetRenderTargetView(), fClearColor);
		m_SceneNeedPointer.pContext->ClearDepthStencilView(m_SceneNeedPointer.pBackBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//���o�̒i�K���Ƃ̕`��.
		PhaseDrawing(mView, mProj, m_iPhase);

		//�����_�[�^�[�Q�b�g�����ɖ߂�.
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &m_SceneNeedPointer.pBackBuffer_RTV, m_SceneNeedPointer.pBackBuffer_DSV);
	}
		break;
	default:
		break;
	}
}

/*====/ �X�v���C�g�֘A /====*/
//2D�X�v���C�g�̕`��.
void StartingScene::RenderSpriteProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 0:
		//3D���f���̕`�摤��GBuffer�p�̂��̂�`�悵�Ă���̂�.
		//�X�v���C�g�ł��`�悵�������̂�����Ȃ炱���ŕ`�悷��.
		if (m_iPhase < 2)
		{
			m_vpSprite[enSprite_BackGround]->Render();
		}
		else if (m_iPhase < 4)
		{
			m_vpSprite[enSprite_BackGroundSub]->Render();
		}

		m_vpSprite[enSprite_PlayerText]->Render();
		m_vpSprite[enSprite_EnemyText]->Render();

		break;
	case 1:
		m_pOneFrameSprite->Render();

		break;
	case MAX_RENDER_LEVEL:

		RenderFadeMaskBuffer();
		//�t�F�[�h�p�摜�̕`��.
		m_pFadeSprite->Render();

		break;
	default:
		break;
	}
}

//�X�v���C�g�̍쐬.
void StartingScene::CreateSprite()
{
	//�L���p�V�e�B���m�肳����.
	m_vpSprite.reserve(enSprite_Max);

	SPRITE_STATE SpriteData;

	//�e�X�v���C�g�̐ݒ�.
	for (int i = 0; i < enSprite_Max; i++)
	{
		switch (i)
		{
		case enSprite_BackGround:
			SpriteData =
			{ "Data\\Image\\BackGround.jpg"	//�t�@�C���܂ł̃p�X.
			, { 1.0f, 1.0f } };				//���摜�����������邩.

			break;
		case enSprite_BackGroundSub:
			SpriteData = { "Data\\Image\\BackGround1.jpg", { 1.0f, 1.0f } };

			break;
		case enSprite_PlayerText:
			SpriteData ={ "Data\\Image\\PlayerText.png", { 1.0f, 1.0f } };

			break;
		case enSprite_EnemyText:
			SpriteData = { "Data\\Image\\EnemyText.png", { 1.0f, 1.0f } };

			break;
		default:
			ERR_MSG("Clear::CreateSprite()", "error");

			break;
		}

		//�z�������₷.
		m_vpSprite.push_back(new Sprite(SpriteData.vDivisionQuantity.x, SpriteData.vDivisionQuantity.y));
		m_vpSprite[i]->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, SpriteData.sPath);
	}

	m_pOneFrameSprite = new DisplayBackBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pOneFrameSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext);
	m_pOneFrameSprite->SetSRV(m_pOneFrameBuff->GetShaderResourceView());
}

//�X�v���C�g�̉��.
void StartingScene::ReleaseSprite()
{
	for (unsigned int i = 0; i < m_vpSprite.size(); i++)
	{
		SAFE_DELETE(m_vpSprite[i]);
	}
	//�g���Ă���T�C�Y��0�ɂ���.
	m_vpSprite.clear();
	//�L���p�V�e�B�����݂̃T�C�Y�ɂ��킹��.
	m_vpSprite.shrink_to_fit();
}

//�X�v���C�g�̃t���[�����Ƃ̍X�V.
void StartingScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//�X�v���C�g�̈ʒu.
void StartingScene::UpdateSpritePositio(int iSpriteNo)
{
	//�E�C���h�E�̒��S.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;

	//�X�v���C�g�ʒu.
	D3DXVECTOR2 vPosition;

	switch (iSpriteNo)
	{
	case enSprite_BackGround:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter;

		break;
	case enSprite_BackGroundSub:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter;

		break;
	case enSprite_PlayerText:
		vPosition.x = fWindowWidthCenter + 150.0f;
		vPosition.y = fWindowHeightCenter;

		break;
	case enSprite_EnemyText:
		vPosition.x = fWindowWidthCenter + 150.0f;
		vPosition.y = fWindowHeightCenter;

		break;
	default:
		ERR_MSG("Clear::UpdateSpritePositio()", "error");

		break;
	}

	m_vpSprite[iSpriteNo]->SetPos(vPosition.x, vPosition.y);

	m_pOneFrameSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
}

//�X�v���C�g�̃A�j���[�V����.
void StartingScene::UpdateSpriteAnimation(int iSpriteNo)
{
	switch (iSpriteNo)
	{
	case enSprite_BackGround:

		break;
	case enSprite_BackGroundSub:

		break;
	case enSprite_PlayerText:
		m_vpSprite[iSpriteNo]->SetScale(1.5f);

		break;
	case enSprite_EnemyText:
		m_vpSprite[iSpriteNo]->SetScale(1.5f);

		break;
	default:
		ERR_MSG("Clear::UpdateSpriteAnimation()", "error");

		break;
	}
}

//���o�̒i�K���Ƃ̕`��.
void StartingScene::PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
{
	//������.
	PhaseInit(iPhase);

	//�J�����̑���.
	PhaseCameraControl(iPhase);

	switch (iPhase)
	{
	case 0:
		m_pPlayerModel->RenderModel(mView, mProj);

		break;
	case 1:
		m_pPlayerModel->RenderModel(mView, mProj);

		break;
	case 2:
		m_pEnemyModel->RenderModel(mView, mProj);

		break;
	case 3:
		m_pEnemyModel->RenderModel(mView, mProj);

		break;
	case 4:
		m_pPlayerModel->RenderModel(mView, mProj);
		m_pEnemyModel->RenderModel(mView, mProj);

		m_pSky->Render(mView, mProj);

		break;
	default:
		break;
	}

	//�i�s.
	PhaseProgress(iPhase);
}

//���o�̒i�K���Ƃ̃J��������.
void StartingScene::PhaseCameraControl(const int iPhase)
{
	switch (iPhase)
	{
	case 0:
		m_pEventCamera->AddPos({ 0.0f, 0.0f, -0.004f });

		break;
	case 1:
		m_pEventCamera->AddPos({ 0.0f, 0.0f, -0.002f });

		break;
	case 2:
		m_pEventCamera->AddLookAt({ 0.0f, 0.01f, 0.0f });
		m_pEventCamera->AddPos({ 0.0f, 0.0f, -0.002f });

		break;
	case 3:
		m_pEventCamera->AddPos({ 0.0f, 0.0f, -0.002f });

		break;
	case 4:
		m_pEventCamera->AddLookAt({ 0.0f, 0.05f, 0.0f });

		break;
	default:
		break;
	}
}

//���o�̒i�K�̐i�s.
void StartingScene::PhaseProgress(const int iPhase)
{
	switch (iPhase)
	{
	case 0:
		if (m_pEventCamera->GetPos().z <= -1.5f)
		{
			if (m_pFadeSprite->FadeOut(FADE_SPEED))
			{
				m_iPhase++;
				m_bWhenProgress = true;
			}
		}
		else
		{
			m_pFadeSprite->FadeIn(FADE_SPEED);
		}

		break;
	case 1:
		if (m_pEventCamera->GetPos().z <= -3.0f)
		{
			if (m_pFadeSprite->FadeOut(FADE_SPEED))
			{
				m_iPhase++;
				m_bWhenProgress = true;
			}
		}
		else
		{
			m_pFadeSprite->FadeIn(FADE_SPEED);
		}

		break;
	case 2:
		if (m_pEventCamera->GetPos().z <= -2.0f)
		{
			if (m_pFadeSprite->FadeOut(FADE_SPEED))
			{
				m_iPhase++;
				m_bWhenProgress = true;
			}
		}
		else
		{
			m_pFadeSprite->FadeIn(FADE_SPEED);
		}

		break;
	case 3:
		if (m_pEventCamera->GetPos().z <= -5.5f)
		{
			if (m_pFadeSprite->FadeOut(FADE_SPEED))
			{
				m_iPhase++;
				m_bWhenProgress = true;
			}
		}
		else
		{
			m_pFadeSprite->FadeIn(FADE_SPEED);
		}

		break;
	case 4:
		if (m_pEventCamera->GetLookAt().y >= 4.0f)
		{
			if (m_pFadeSprite->FadeOut(FADE_SPEED))
			{
				m_iPhase++;
				m_bWhenProgress = true;
			}
		}
		else
		{
			m_pFadeSprite->FadeIn(FADE_SPEED);
		}

		break;
	default:
		break;
	}
}

//���o�̊e�i�K�̏�����.
void StartingScene::PhaseInit(const int iPhase)
{
	if (!m_bWhenProgress)
	{
		return;
	}

	//�����ʒu���N���A.
	D3DXVECTOR3 vLookAt;
	CrearVECTOR3(vLookAt);

	switch (iPhase)
	{
	case 0:
		//�J�����̒����ʒu��ݒ肷��.
		vLookAt = m_pPlayerModel->GetPos();
		vLookAt.y += 1.2f;
		m_pEventCamera->SetLookAt(vLookAt);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 0.8f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.8f });

		m_pPlayerModel->SetPos({ 0.0f, 0.0f, 0.0f });

		//���O�̕\���t���O.
		m_vpSprite[enSprite_PlayerText]->SetDispFlg(false);
		m_vpSprite[enSprite_EnemyText]->SetDispFlg(false);

		break;
	case 1:
		//�J�����̒����ʒu��ݒ肷��.
		vLookAt = m_pPlayerModel->GetPos();
		vLookAt.x += 0.5f;
		vLookAt.y += 1.0f;
		m_pEventCamera->SetLookAt(vLookAt);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 2.5f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		m_pPlayerModel->SetPos({ 0.0f, 0.0f, 0.0f });

		//���O�̕\���t���O.
		m_vpSprite[enSprite_PlayerText]->SetDispFlg(true);
		m_vpSprite[enSprite_EnemyText]->SetDispFlg(false);

		break;
	case 2:
		//�J�����̒����ʒu��ݒ肷��.
		vLookAt = m_pPlayerModel->GetPos();
		m_pEventCamera->SetLookAt(vLookAt);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y + 1.0f, vLookAt.z - 1.5f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 0.0f });

		//���O�̕\���t���O.
		m_vpSprite[enSprite_PlayerText]->SetDispFlg(false);
		m_vpSprite[enSprite_EnemyText]->SetDispFlg(false);

		break;
	case 3:
		//�J�����̒����ʒu��ݒ肷��.
		vLookAt = m_pPlayerModel->GetPos();
		vLookAt.x += 1.0f;
		vLookAt.y += 2.0f;
		m_pEventCamera->SetLookAt(vLookAt);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 5.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 0.0f });

		//���O�̕\���t���O.
		m_vpSprite[enSprite_PlayerText]->SetDispFlg(false);
		m_vpSprite[enSprite_EnemyText]->SetDispFlg(true);

		break;
	case 4:
		//�J�����̒����ʒu��ݒ肷��.
		CrearVECTOR3(vLookAt);
		vLookAt.y -= 4.0f;
		m_pEventCamera->SetLookAt(vLookAt);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x, 0.5f, vLookAt.z - 4.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		//���f���̈ʒu��ݒ�.
		{
			//���f�����m�̋���.
			float fGap = 1.0f;
			m_pPlayerModel->SetPos({ -fGap, 0.0f, 0.0f });
			m_pEnemyModel->SetPos({ fGap, 0.0f, 0.0f });
		}

		//���O�̕\���t���O.
		m_vpSprite[enSprite_PlayerText]->SetDispFlg(false);
		m_vpSprite[enSprite_EnemyText]->SetDispFlg(false);

		break;
	default:
		break;
	}

	m_bWhenProgress = false;
}

//�t�F�[�h�p�̃}�X�N�̕`��
void StartingScene::RenderFadeMaskBuffer()
{
	//�����_�[�^�[�Q�b�g���t�F�[�h�p�摜�Ɏg���}�X�N�p�o�b�t�@�ɕς���.
	ID3D11RenderTargetView* pRTV = m_pFadeMaskBuffer->GetRenderTargetView();
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

	//��ʂ̃N���A.
	const float fClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_SceneNeedPointer.pContext->ClearRenderTargetView(m_pFadeMaskBuffer->GetRenderTargetView(), fClearColor);
	m_SceneNeedPointer.pContext->ClearDepthStencilView(m_SceneNeedPointer.pBackBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//���[���摜��`�悷��.
	m_pFadeMaskSprite->Render();

	//�����_�[�^�[�Q�b�g�����ɖ߂�.
	pRTV = m_SceneNeedPointer.pBackBuffer_RTV;
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);
}

#if _DEBUG

//�f�o�b�O�e�L�X�g�̕\��.
void StartingScene::RenderDebugText()
{
	char cStrDbgTxt[512];

	sprintf_s(cStrDbgTxt, "Scene : Starting");
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 0));

	sprintf_s(cStrDbgTxt, "SpriteSize : ( %f, %f )", m_pOneFrameSprite->GetSize().x, m_pOneFrameSprite->GetSize().y);
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 1));

	sprintf_s(cStrDbgTxt, "SpriteScale : [%f]", m_pOneFrameSprite->GetScale());
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 2));

	sprintf_s(cStrDbgTxt, "Phase : [%i]", m_iPhase);
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 3));

	sprintf_s(cStrDbgTxt, "FadeAlpha : [%f]", m_pFadeSprite->GetAlpha());
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 4));
}

//�f�o�b�O���݂̂̑���.
void StartingScene::DebugKeyControl()
{
}

#endif //#if _DEBUG.