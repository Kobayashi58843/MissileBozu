#include "OverScene.h"

#include "..\\..\\Singleton.h"

//���f���̏����A�j���[�V�������x.
const double ANIMETION_SPEED = 0.01;

//���o�̒i�K�̍ő吔.
const int MAX_PHASE = 1;

OverScene::OverScene(SCENE_NEED_POINTER PointerGroup)
	: BaseScene(PointerGroup)
	, m_pOneFrameSprite(nullptr)
	, m_pOneFrameBuff(nullptr)
	, m_pEventCamera(nullptr)
	, m_pPlayerModel(nullptr)
	, m_iPhase(0)
	, m_bWhenProgress(true)
	, m_iCursorAnimationCount(0)
	, m_fCupidAddPos(0)
{
	//�S�T�E���h���~����.
	Singleton<SoundManager>().GetInstance().StopSound();

	m_pOneFrameBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	/*====/ �t�F�[�h�p�X�v���C�g�֘A /====*/
	m_pLightMaskBuffer = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	D3DXVECTOR2 vDivisionQuantity = { 1.0f, 1.0f };
	m_pLightSprite = new TransitionsSprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pLightSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\White.jpg");
	m_pLightSprite->SetMaskTexture(m_pLightMaskBuffer->GetShaderResourceView());

	m_pLightMaskSprite = new Sprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pLightMaskSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\OverSceneLight.png");

	//�ʒu���E�C���h�E�̒��S�ɐݒ�.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;
	m_pLightSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
	m_pLightMaskSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);

	//�͂��߂͔�\���ɐݒ肵�Ă���.
	m_pLightSprite->SetDispFlg(true);

	//���ߒl��0�ɂ���.
	m_pLightSprite->SetAlpha(0.25f);
}

OverScene::~OverScene()
{
	SAFE_DELETE(m_pLightSprite);

	SAFE_DELETE(m_pLightMaskSprite);

	SAFE_DELETE(m_pLightMaskBuffer);

	Release();
}

//�쐬.
void OverScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//�X�v���C�g�̍쐬.
	CreateSprite();

	//�J�������쐬.
	m_pEventCamera = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	//�X�L�����f���̍쐬.
	m_pPlayerModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Player), 0.0005f, ANIMETION_SPEED);
	m_pPlayerModel->ChangeAnimation(1);
}

//���.
void OverScene::Release()
{
	SAFE_DELETE(m_pPlayerModel);

	SAFE_DELETE(m_pEventCamera);

	SAFE_DELETE(m_pOneFrameBuff);
	SAFE_DELETE(m_pOneFrameSprite);

	ReleaseSprite();
}

//�X�V.
void OverScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
	//BGM�����[�v�ōĐ�.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Over);

	//�X�v���C�g�X�V.
	UpdateSprite();

	//�J�����X�V.
	m_pEventCamera->Update();

	//���N���b�N���ꂽ��.
	if (Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		//�J�[�\�����{�^���̏�ɂ��邩.
		if (IsHittingOfSprite(enSprite_Cursor, enSprite_ReturnButton))
		{
			enNextScene = enSwitchToNextScene::Title;

			//�V�[���ړ�����SE.
			if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_PushButton))
			{
				//SE���Đ�.
				Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_PushButton);
			}
		}
	}
}

//3D���f���̕`��.
void OverScene::RenderModelProduct(const int iRenderLevel)
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
void OverScene::RenderSpriteProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 0:
		m_vpSprite[enSprite_BackGround]->Render();

		RenderLightMaskBuffer();
		m_pLightSprite->Render();

		break;
	case 1:
		m_pOneFrameSprite->Render();

		m_vpSprite[enSprite_Cupid1]->Render();
		m_vpSprite[enSprite_Cupid2]->Render();

		m_vpSprite[enSprite_Logo]->Render();
		m_vpSprite[enSprite_ReturnButton]->Render();
		m_vpSprite[enSprite_ReturnButtonText]->Render();
		m_vpSprite[enSprite_Cursor]->Render();

		break;
	default:
		break;
	}
}

//�X�v���C�g�̍쐬.
void OverScene::CreateSprite()
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
			{ "Data\\Image\\Fade.jpg"	//�t�@�C���܂ł̃p�X.
			, { 1.0f, 1.0f } };				//���摜�����������邩.

			break;
		case enSprite_Logo:
			SpriteData = { "Data\\Image\\LogoText.png", { 1.0f, 3.0f } };

			break;
		case enSprite_ReturnButton:
			SpriteData = { "Data\\Image\\End.jpg", { 1.0f, 2.0f } };

			break;
		case enSprite_ReturnButtonText:
			SpriteData = { "Data\\Image\\ButtonText.png", { 1.0f, 3.0f } };

			break;
		case enSprite_Cursor:
			SpriteData = { "Data\\Image\\Cursor.png", { 8.0f, 1.0f } };

			break;
		case enSprite_Cupid1:
			SpriteData = { "Data\\Image\\Cupid1.png", { 1.0f, 1.0f } };

			break;
		case enSprite_Cupid2:
			SpriteData = { "Data\\Image\\Cupid2.png", { 1.0f, 1.0f } };

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
void OverScene::ReleaseSprite()
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
void OverScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//�X�v���C�g�̈ʒu.
void OverScene::UpdateSpritePositio(int iSpriteNo)
{
	//�E�C���h�E�̒��S.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;

	//�X�v���C�g�ʒu.
	D3DXVECTOR2 vPosition;

	m_fCupidAddPos += 0.005f;

	switch (iSpriteNo)
	{
	case enSprite_BackGround:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter;

		break;
	case enSprite_Logo:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter / 2.0f;

		break;
	case enSprite_ReturnButton:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter + (fWindowHeightCenter / 2.0f);

		break;
	case enSprite_ReturnButtonText:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter + (fWindowHeightCenter / 2.0f);

		break;
	case enSprite_Cursor:
	{
		//�}�E�X�̍��W.
		POINT MousePosition;
		GetCursorPos(&MousePosition);
		D3DXVECTOR2 vMousePos = { static_cast<float>(MousePosition.x), static_cast<float>(MousePosition.y) };

		//�E�B���h�E�̍��W.
		//�E�B���h�E����[�̍��W : ( left , top).
		//�E�B���h�E�E���[�̍��W : ( right, bottom).
		RECT WindowPosition;
		GetWindowRect(m_SceneNeedPointer.hWnd, &WindowPosition);
		D3DXVECTOR2 vWindowPos = { static_cast<float>(WindowPosition.left), static_cast<float>(WindowPosition.top) };

		vPosition.x = vMousePos.x - vWindowPos.x;
		vPosition.y = vMousePos.y - vWindowPos.y;
	}
		break;
	case enSprite_Cupid1:
		vPosition.x = fWindowWidthCenter / 2.0f + (10.0f * sin(m_fCupidAddPos));
		vPosition.y = fWindowHeightCenter + (15.0f * sin(m_fCupidAddPos));

		break;
	case enSprite_Cupid2:
		vPosition.x = fWindowWidthCenter * 1.5f - (10.0f * sin(m_fCupidAddPos));
		vPosition.y = fWindowHeightCenter + (15.0f * sin(m_fCupidAddPos));

		break;
	default:
		ERR_MSG("Clear::UpdateSpritePositio()", "error");

		break;
	}

	m_vpSprite[iSpriteNo]->SetPos(vPosition.x, vPosition.y);

	m_pOneFrameSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
}

//�X�v���C�g�̃A�j���[�V����.
void OverScene::UpdateSpriteAnimation(int iSpriteNo)
{
	switch (iSpriteNo)
	{
	case enSprite_BackGround:

		break;
	case enSprite_Logo:
		m_vpSprite[iSpriteNo]->SetPatternHeight(2.0f);

		break;
	case enSprite_ReturnButton:
		//�J�[�\���ƃ{�^�����ڐG���Ă�����.
		if (IsHittingOfSprite(enSprite_Cursor, iSpriteNo))
		{
			m_vpSprite[iSpriteNo]->SetPatternHeight(1.0f);
		}
		else
		{
			m_vpSprite[iSpriteNo]->SetPatternHeight(0.0f);
		}

		break;
	case enSprite_ReturnButtonText:
		m_vpSprite[iSpriteNo]->SetPatternHeight(2.0f);

		break;
	case enSprite_Cursor:
		if (IsHittingOfSprite(enSprite_Cursor, enSprite_ReturnButton))
		{
			m_iCursorAnimationCount++;
			float fCursorAnimationWaitTime = 0.2f;
			if (m_iCursorAnimationCount / FPS >= fCursorAnimationWaitTime)
			{
				m_vpSprite[iSpriteNo]->AddPatternWidth(1.0f);
				m_iCursorAnimationCount = 0;

				int iPattern = static_cast<int>(m_vpSprite[iSpriteNo]->GetPattern().x);
				if (iPattern % 2 == 0)
				{
					Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_Cursor);
				}
			}
		}

		break;
	case enSprite_Cupid1:

		break;
	case enSprite_Cupid2:

		break;
	default:
		ERR_MSG("Clear::UpdateSpriteAnimation()", "error");

		break;
	}
}

//���o�̒i�K���Ƃ̕`��.
void OverScene::PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
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
	default:
		break;
	}

	//�i�s.
	PhaseProgress(iPhase);
}

//���o�̒i�K���Ƃ̃J��������.
void OverScene::PhaseCameraControl(const int iPhase)
{
	switch (iPhase)
	{
	case 0:

		break;
	default:
		break;
	}
}

//���o�̒i�K�̐i�s.
void OverScene::PhaseProgress(const int iPhase)
{
	switch (iPhase)
	{
	case 0:
		//m_iPhase++;
		//m_bWhenProgress = true;

		break;
	default:
		break;
	}
}

//���o�̊e�i�K�̏�����.
void OverScene::PhaseInit(const int iPhase)
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
		//���f���̈ʒu��ݒ�.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, 0.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(135), D3DXToRadian(90) });

		//�A�j���[�V������ݒ�.
		{
			double dHitAnimationSpeed = 0;

			m_pPlayerModel->ChangeAnimation(1);
			m_pPlayerModel->SetAnimationSpeed(dHitAnimationSpeed);
		}

		//�J�����̒����ʒu��ݒ肷��.
		vLookAt = m_pPlayerModel->GetPos();
		vLookAt.x -= 0.8f;
		vLookAt.y += 0.2f;
		m_pEventCamera->SetLookAt(vLookAt);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 3.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	default:
		break;
	}

	m_bWhenProgress = false;
}


//�X�v���C�g�̃}�X�N�̕`��.
void OverScene::RenderLightMaskBuffer()
{
	//�����_�[�^�[�Q�b�g���t�F�[�h�p�摜�Ɏg���}�X�N�p�o�b�t�@�ɕς���.
	ID3D11RenderTargetView* pRTV = m_pLightMaskBuffer->GetRenderTargetView();
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

	//��ʂ̃N���A.
	const float fClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_SceneNeedPointer.pContext->ClearRenderTargetView(m_pLightMaskBuffer->GetRenderTargetView(), fClearColor);
	m_SceneNeedPointer.pContext->ClearDepthStencilView(m_SceneNeedPointer.pBackBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//���[���摜��`�悷��.
	m_pLightMaskSprite->Render();

	//�����_�[�^�[�Q�b�g�����ɖ߂�.
	pRTV = m_SceneNeedPointer.pBackBuffer_RTV;
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);
}

#if _DEBUG

//�f�o�b�O�e�L�X�g�̕\��.
void OverScene::RenderDebugText()
{
	char cStrDbgTxt[512];

	sprintf_s(cStrDbgTxt, "Scene : Over");
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 0));
}

#endif //#if _DEBUG.