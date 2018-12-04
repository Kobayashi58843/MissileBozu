#include "StartEventScene.h"

#include "..\\..\\Singleton.h"

//���f���̏����A�j���[�V�������x.
const double ANIMETION_SPEED = 0.01;

//���o�̒i�K�̍ő吔.
const int MAX_PHASE = 8;

//�t�F�[�h�̑��x.
const float FADE_SPEED = 0.1f;

StartEventScene::StartEventScene(SCENE_NEED_POINTER PointerGroup)
	: BaseScene(PointerGroup)
	, m_iTime(0)
	, m_pModelSprite(nullptr)
	, m_pModelBuff(nullptr)
	, m_pEventCamera(nullptr)
	, m_pModelSpriteCamera(nullptr)
	, m_pPlayerModel(nullptr)
	, m_pGround(nullptr)
	, m_iPhase(0)
	, m_bWhenProgress(true)
{
	//�S�T�E���h���~����.
	Singleton<SoundManager>().GetInstance().StopSound();

	m_pModelBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	/*====/ �t�F�[�h�p�X�v���C�g�֘A /====*/
	m_pFadeMaskBuffer = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	D3DXVECTOR2 vDivisionQuantity = { 1.0f, 1.0f };
	m_pFadeSprite = new TransitionsSprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pFadeSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\White.jpg");
	m_pFadeSprite->SetMaskTexture(m_pFadeMaskBuffer->GetShaderResourceView());

	m_pFadeMaskSprite = new Sprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pFadeMaskSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\Transitions_Starting.png");

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

StartEventScene::~StartEventScene()
{
	SAFE_DELETE(m_pFadeSprite);

	SAFE_DELETE(m_pFadeMaskSprite);

	SAFE_DELETE(m_pFadeMaskBuffer);

	Release();
}

//�쐬.
void StartEventScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//�X�v���C�g�̍쐬.
	CreateSprite();

	//�J�������쐬.
	m_pEventCamera = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	m_pModelSpriteCamera = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	//�X�L�����f���̍쐬.
	m_pPlayerModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Player), 0.0005f, ANIMETION_SPEED);
	m_pPlayerModel->ChangeAnimation(1);

	m_pGround = Singleton<ModelResource>().GetInstance().GetStaticModels(ModelResource::enStaticModel_Ground);
}

//���.
void StartEventScene::Release()
{
	m_pGround = nullptr;

	SAFE_DELETE(m_pPlayerModel);

	SAFE_DELETE(m_pModelSpriteCamera);
	SAFE_DELETE(m_pEventCamera);

	SAFE_DELETE(m_pModelBuff);
	SAFE_DELETE(m_pModelSprite);

	ReleaseSprite();
}

//�X�V.
void StartEventScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
#if _DEBUG

	//�f�o�b�O���݂̂̑���.
	DebugKeyControl();

#endif //#if _DEBUG.

	//BGM�����[�v�ōĐ�.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Clear);

	m_iTime++;

	//�X�v���C�g�X�V.
	UpdateSprite();

	//�J�����X�V.
	m_pEventCamera->Update();
	m_pModelSpriteCamera->Update();

	//���̃V�[����.
	if (m_iPhase >= MAX_PHASE)
	{
		enNextScene = enSwitchToNextScene::Action;
	}
}

//3D���f���̕`��.
void StartEventScene::RenderModelProduct(const int iRenderLevel)
{
	D3DXMATRIX mView = m_pEventCamera->GetView();
	D3DXMATRIX mProj = m_pEventCamera->GetProjection();

	switch (iRenderLevel)
	{
	case 0:
	{
		//�����_�[�^�[�Q�b�g���Z�b�g.
		ID3D11RenderTargetView* pRTV = m_pModelBuff->GetRenderTargetView();
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

		//��ʂ̃N���A.
		const float fClearColor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
		m_SceneNeedPointer.pContext->ClearRenderTargetView(m_pModelBuff->GetRenderTargetView(), fClearColor);
		m_SceneNeedPointer.pContext->ClearDepthStencilView(m_SceneNeedPointer.pBackBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ModelControl(m_pModelSpriteCamera->GetView(), m_pModelSpriteCamera->GetProjection(),m_iPhase);

		//�����_�[�^�[�Q�b�g�����ɖ߂�.
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &m_SceneNeedPointer.pBackBuffer_RTV, m_SceneNeedPointer.pBackBuffer_DSV);
	}
	break;
	case 1:
		m_pGround->Render(mView, mProj);

		//���o�̒i�K���Ƃ̕`��.
		PhaseDrawing(mView, mProj, m_iPhase);

		break;
	case MAX_RENDER_LEVEL:

		break;
	default:
		break;
	}
}

/*====/ �X�v���C�g�֘A /====*/
//2D�X�v���C�g�̕`��.
void StartEventScene::RenderSpriteProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 0:
		m_vpSprite[enSprite_BackGround]->Render();

		break;
	case 1:
		if (m_iPhase == 1 || m_iPhase == 3 || m_iPhase == 5)
		{
			m_vpSprite[enSprite_Number]->Render();
		}

		if (m_iPhase == 7)
		{
			m_vpSprite[enSprite_Fire]->Render();
		}

		break;
	case MAX_RENDER_LEVEL:
		m_pModelSprite->Render();
		
		//�t�F�[�h�p�摜�̕`��.
		m_pFadeSprite->Render();

		break;
	default:
		break;
	}
}

//�X�v���C�g�̍쐬.
void StartEventScene::CreateSprite()
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
		case enSprite_Number:
			SpriteData = { "Data\\Image\\Number.png", { 1.0f, 3.0f } };

			break;
		case enSprite_Fire:
			SpriteData = { "Data\\Image\\Fire.png", { 1.0f, 1.0f } };

			break;
		default:
			ERR_MSG("Clear::CreateSprite()", "error");

			break;
		}

		//�z�������₷.
		m_vpSprite.push_back(new Sprite(SpriteData.vDivisionQuantity.x, SpriteData.vDivisionQuantity.y));
		m_vpSprite[i]->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, SpriteData.sPath);
	}

	m_pModelSprite = new DisplayBackBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pModelSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext);
	m_pModelSprite->SetSRV(m_pModelBuff->GetShaderResourceView());
}

//�X�v���C�g�̉��.
void StartEventScene::ReleaseSprite()
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
void StartEventScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//�X�v���C�g�̈ʒu.
void StartEventScene::UpdateSpritePositio(int iSpriteNo)
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
	case enSprite_Number:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter;

		break;
	case enSprite_Fire:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter;

		break;
	default:
		ERR_MSG("Clear::UpdateSpritePositio()", "error");

		break;
	}

	m_vpSprite[iSpriteNo]->SetPos(vPosition.x, vPosition.y);

	m_pModelSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
}

//�X�v���C�g�̃A�j���[�V����.
void StartEventScene::UpdateSpriteAnimation(int iSpriteNo)
{
	switch (iSpriteNo)
	{
	case enSprite_BackGround:

		break;
	case enSprite_Number:

		break;
	case enSprite_Fire:

		break;
	default:
		ERR_MSG("Clear::UpdateSpriteAnimation()", "error");

		break;
	}
}

//���o�̒i�K���Ƃ̕`��.
void StartEventScene::PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
{
	//������.
	PhaseInit(iPhase);

	//�J�����̑���.
	PhaseCameraControl(iPhase);

	switch (iPhase)
	{
	case 0:

		break;
	case 1:

		break;
	case 2:

		break;
	case 3:

		break;
	case 4:

		break;
	case 5:

		break;
	case 6:

		break;
	case 7:

		break;
	default:
		break;
	}

	//�i�s.
	PhaseProgress(iPhase);
}

//���o�̒i�K���Ƃ̃J��������.
void StartEventScene::PhaseCameraControl(const int iPhase)
{
	float fYawRotSpeed = 0.02f;

	switch (iPhase)
	{
	case 0:
		m_pEventCamera->AddRot({ 0.0f, -fYawRotSpeed, 0.0f });

		break;
	case 1:

		break;
	case 2:
		m_pEventCamera->AddRot({ 0.005f, fYawRotSpeed, 0.01f });

		break;
	case 3:

		break;
	case 4:
		m_pEventCamera->AddRot({ 0.01f, -fYawRotSpeed, -0.005f });

		break;
	case 5:

		break;
	case 6:
		m_pEventCamera->AddRot({ -0.01f, fYawRotSpeed, -0.01f });

		break;
	case 7:

		break;
	default:
		break;
	}
}

//���o�̒i�K�̐i�s.
void StartEventScene::PhaseProgress(const int iPhase)
{
	float fCameraMoveTime = 1.0f;
	float fCountTime = 1.5f;

	switch (iPhase)
	{
	case 0:
		if (m_iTime / FPS >= fCameraMoveTime)
		{
			if (FadeOut())
			{
				m_iPhase++;
				m_bWhenProgress = true;
			}
		}
		else
		{
			FadeIn();
		}

		break;
	case 1:
		if (m_iTime / FPS >= fCountTime)
		{
			if (FadeOut())
			{
				m_iPhase++;
				m_bWhenProgress = true;
				m_vpSprite[enSprite_Number]->AddPatternHeight(1.0f);
			}
		}
		else
		{
			FadeIn();
		}

		break;
	case 2:
		if (m_iTime / FPS >= fCameraMoveTime)
		{
			if (FadeOut())
			{
				m_iPhase++;
				m_bWhenProgress = true;
			}
		}
		else
		{
			FadeIn();
		}

		break;
	case 3:
		if (m_iTime / FPS >= fCountTime)
		{
			if (FadeOut())
			{
				m_iPhase++;
				m_bWhenProgress = true;
				m_vpSprite[enSprite_Number]->AddPatternHeight(1.0f);
			}
		}
		else
		{
			FadeIn();
		}

		break;
	case 4:
		if (m_iTime / FPS >= fCameraMoveTime)
		{
			if (FadeOut())
			{
				m_iPhase++;
				m_bWhenProgress = true;
			}
		}
		else
		{
			FadeIn();
		}

		break;
	case 5:
		if (m_iTime / FPS >= fCountTime)
		{
			if (FadeOut())
			{
				m_iPhase++;
				m_bWhenProgress = true;
			}
		}
		else
		{
			FadeIn();
		}

		break;
	case 6:
		if (m_iTime / FPS >= fCameraMoveTime)
		{
			if (FadeOut())
			{
				m_iPhase++;
				m_bWhenProgress = true;
			}
		}
		else
		{
			FadeIn();
		}

		break;
	case 7:
		if (m_iTime / FPS >= fCountTime)
		{
			if (FadeOut())
			{
				m_iPhase++;
				m_bWhenProgress = true;
			}
		}
		else
		{
			FadeIn();
		}

		break;
	default:
		break;
	}
}

//���o�̊e�i�K�̏�����.
void StartEventScene::PhaseInit(const int iPhase)
{
	if (!m_bWhenProgress)
	{
		return;
	}

	//�����ʒu���N���A.
	D3DXVECTOR3 vLookAt;
	CrearVECTOR3(vLookAt);

	m_iTime = 0;

	switch (iPhase)
	{
	case 0:
		//�J�����̒����ʒu��ݒ肷��.
		vLookAt.y += 5.0f;
		m_pEventCamera->SetLookAt(vLookAt);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 1.0f});

		break;
	case 1:
		//.
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(-45) });
		m_pPlayerModel->SetPos({ -4.0f, -4.0f, 0.0f });

		break;
	case 2:

		break;
	case 3:
		//.
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(180) });
		m_pPlayerModel->SetPos({ 0.0f, 4.0f, 0.0f });

		break;
	case 4:

		break;
	case 5:
		//.
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(45) });
		m_pPlayerModel->SetPos({ 4.0f, -4.0f, 0.0f });

		break;
	case 6:

		break;
	case 7:
		//.
		m_pPlayerModel->SetRot({ D3DXToRadian(60), D3DXToRadian(180), 0.0f });
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, -8.0f });

		break;
	default:
		break;
	}

	m_bWhenProgress = false;
}

//.
void StartEventScene::ModelControl(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
{
	//�����ʒu���N���A.
	D3DXVECTOR3 vLookAt;
	CrearVECTOR3(vLookAt);

	//�J�����̒����ʒu��ݒ肷��.
	m_pModelSpriteCamera->SetLookAt(vLookAt);

	//�J�����̈ʒu��ݒ肷��.
	m_pModelSpriteCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 2.5f });

	float fMoveSpeed = 0.08f;
	switch (iPhase)
	{
	case 1:
		m_pPlayerModel->AddPos({ fMoveSpeed, fMoveSpeed, 0.0f });

		m_pPlayerModel->RenderModel(mView, mProj);

		break;
	case 3:
		m_pPlayerModel->AddPos({ 0.0f, -fMoveSpeed, 0.0f });

		m_pPlayerModel->RenderModel(mView, mProj);

		break;
	case 5:
		m_pPlayerModel->AddPos({ -fMoveSpeed, fMoveSpeed, 0.0f });

		m_pPlayerModel->RenderModel(mView, mProj);

		break;
	case 7:
		m_pPlayerModel->AddPos({ 0.0f, 0.0f, fMoveSpeed * 2.0f });

		m_pPlayerModel->RenderModel(mView, mProj);

		break;
	default:
		break;
	}
}

//�t�F�[�h�A�E�g.
bool StartEventScene::FadeOut()
{
	//�t�F�[�h�p�̉摜���\���\���ǂ����Ńt�F�[�h���𔻒f����.
	//�t�F�[�h���łȂ��ꍇ.
	if (!m_pFadeSprite->IsDispFlg())
	{
		//�t�F�[�h�p�̉摜��\���\�ɂ���.
		m_pFadeSprite->SetDispFlg(true);
	}

	if (m_pFadeSprite->IsDispFlg())
	{
		RenderFadeMaskBuffer();

		if (1.0f > m_pFadeSprite->GetAlpha())
		{
			m_pFadeSprite->AddAlpha(FADE_SPEED);
		}
		else
		{
			//�t�F�[�h�A�E�g����.
			return true;
		}
	}

	return false;
}

//�t�F�[�h�C��.
bool StartEventScene::FadeIn()
{
	if (m_pFadeSprite->IsDispFlg())
	{
		RenderFadeMaskBuffer();

		if (0.0f < m_pFadeSprite->GetAlpha())
		{
			m_pFadeSprite->AddAlpha(-FADE_SPEED);
		}
		else
		{
			//�t�F�[�h�p�̉摜���\���ɂ���.
			m_pFadeSprite->SetDispFlg(false);
			m_pFadeSprite->SetAlpha(0.0f);

			//�t�F�[�h�C������.
			return true;
		}
	}

	return false;
}

//�t�F�[�h�p�̃}�X�N�̕`��
void StartEventScene::RenderFadeMaskBuffer()
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
void StartEventScene::RenderDebugText()
{
	char cStrDbgTxt[512];

	sprintf_s(cStrDbgTxt, "Scene : StartEvent");
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 0));

	sprintf_s(cStrDbgTxt, "Time : [%i]", m_iTime);
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 1));

	sprintf_s(cStrDbgTxt, "SpriteSize : ( %f, %f )", m_pModelSprite->GetSize().x, m_pModelSprite->GetSize().y);
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 2));

	sprintf_s(cStrDbgTxt, "SpriteScale : [%f]", m_pModelSprite->GetScale());
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 3));

	sprintf_s(cStrDbgTxt, "Phase : [%i]", m_iPhase);
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 4));
}

//�f�o�b�O���݂̂̑���.
void StartEventScene::DebugKeyControl()
{
}

#endif //#if _DEBUG.