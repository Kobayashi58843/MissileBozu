#include "TitleScene.h"

#include "..\\..\\Singleton.h"

//��ʂ̃N���A�J���[.
const float CLEAR_COLOR[4] = { 1.0f, 1.0f, 1.0f, 0.0f };

//���f���̏����A�j���[�V�������x.
const double ANIMETION_SPEED = 0.01;

TitleScene::TitleScene(SCENE_NEED_POINTER PointerGroup)
	: BaseScene(PointerGroup)
	, m_iCursorAnimationCount(0)
	, m_pTitleBackMainBuff(nullptr)
	, m_pTitleBackSubBuff(nullptr)
	, m_pEventCamera(nullptr)
	, m_pPlayerModel(nullptr)
	, m_pEnemyModel(nullptr)
{
	//�S�T�E���h���~����.
	Singleton<SoundManager>().GetInstance().StopSound();

	//�V�[���ړ�����SE.
	Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_PushButton);
}

TitleScene::~TitleScene()
{
	Release();
}

//�쐬.
void TitleScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//�X�v���C�g�̍쐬.
	CreateSprite();

	m_pTitleBackMainBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	m_pTitleBackSubBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	m_pBackGroundSprite->SetSRV(m_pTitleBackMainBuff->GetShaderResourceView(), m_pTitleBackSubBuff->GetShaderResourceView());

	//�J�������쐬.
	m_pEventCamera = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	//�X�L�����f���̍쐬.
	m_pPlayerModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Player), 0.0005f, ANIMETION_SPEED);
	m_pPlayerModel->ChangeAnimation(1);

	m_pEnemyModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Enemy), 0.08f, ANIMETION_SPEED);
	m_pEnemyModel->ChangeAnimation(5);
}

//���.
void TitleScene::Release()
{
	SAFE_DELETE(m_pEnemyModel);
	SAFE_DELETE(m_pPlayerModel);

	SAFE_DELETE(m_pEventCamera);

	SAFE_DELETE(m_pTitleBackSubBuff);
	SAFE_DELETE(m_pTitleBackMainBuff);

	ReleaseSprite();
}

//�X�V.
void TitleScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
	//BGM�����[�v�ōĐ�.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Title);

	//�X�v���C�g�X�V.
	UpdateSprite();

	//�V�[���ύX.
	ChangeScene(enNextScene);
}

//3D���f���̕`��.
void TitleScene::RenderModelProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 1:
		RenderBackGroundBuffMain();
		RenderBackGroundBuffSub();

		break;
	default:
		break;
	}
}

/*====/ �X�v���C�g�֘A /====*/
//2D�X�v���C�g�̕`��.
void TitleScene::RenderSpriteProduct(const int iRenderLevel)
{
	ID3D11RenderTargetView* pRTV;

	switch (iRenderLevel)
	{
	case 0:
		//�^�C�g���̔w�i�p�o�b�t�@�̃��C���̃����_�[�^�[�Q�b�g���Z�b�g.
		pRTV = m_pTitleBackMainBuff->GetRenderTargetView();
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

		m_vpSprite[enSprite_BackGround]->Render();

		//�^�C�g���̔w�i�p�o�b�t�@�̃T�u�̃����_�[�^�[�Q�b�g���Z�b�g.
		pRTV = m_pTitleBackSubBuff->GetRenderTargetView();
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

		m_vpSprite[enSprite_BackGroundSub]->Render();

		break;
	case MAX_RENDER_LEVEL:
		m_pBackGroundSprite->Render();

		for (unsigned int i = 0; i < m_vpSprite.size(); i++)
		{
			//UI�⃍�S�ȊO�͕\�����Ȃ�.
			if (i == enSprite_BackGround || i == enSprite_BackGroundSub)
			{
				continue;
			}

			m_vpSprite[i]->Render();
		}

		break;
	default:
		break;
	}
}

//�X�v���C�g�̍쐬.
void TitleScene::CreateSprite()
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
		case enSprite_Logo:
			SpriteData = { "Data\\Image\\LogoText.png", { 1.0f, 3.0f } };

			break;
		case enSprite_StartButton:
			SpriteData = { "Data\\Image\\Push.jpg", { 1.0f, 2.0f } };

			break;
		case enSprite_EndButton:
			SpriteData = { "Data\\Image\\End.jpg", { 1.0f, 2.0f } };

			break;
		case enSprite_StartButtonText:
			SpriteData = { "Data\\Image\\ButtonText.png", { 1.0f, 3.0f } };

			break;
		case enSprite_EndButtonText:
			SpriteData = { "Data\\Image\\ButtonText.png", { 1.0f, 3.0f } };

			break;
		case enSprite_Cursor:
			SpriteData = { "Data\\Image\\Cursor.png", { 8.0f, 1.0f } };

			break;
		default:
			ERR_MSG("Title::CreateSprite()","error");

			break;
		}

		//�z�������₷.
		m_vpSprite.push_back(new Sprite(SpriteData.vDivisionQuantity.x, SpriteData.vDivisionQuantity.y));
		m_vpSprite[i]->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, SpriteData.sPath);
	}

	m_pBackGroundSprite = new TitleBackgroundSprite(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pBackGroundSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\TitleMask1.png");
}

//�X�v���C�g�̉��.
void TitleScene::ReleaseSprite()
{
	SAFE_DELETE(m_pBackGroundSprite);

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
void TitleScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//�X�v���C�g�̈ʒu.
void TitleScene::UpdateSpritePositio(int iSpriteNo)
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
	case enSprite_Logo:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter / 2.0f;

		break;
	case enSprite_StartButton:
		vPosition.x = fWindowWidthCenter - (fWindowWidthCenter / 2.0f);
		vPosition.y = fWindowHeightCenter + (fWindowHeightCenter / 2.0f);

		break;
	case enSprite_EndButton:
		vPosition.x = fWindowWidthCenter + (fWindowWidthCenter / 2.0f);
		vPosition.y = fWindowHeightCenter + (fWindowHeightCenter / 2.0f);
		
		break;
	case enSprite_StartButtonText:
		vPosition.x = fWindowWidthCenter - (fWindowWidthCenter / 2.0f);
		vPosition.y = fWindowHeightCenter + (fWindowHeightCenter / 2.0f);

		break;
	case enSprite_EndButtonText:
		vPosition.x = fWindowWidthCenter + (fWindowWidthCenter / 2.0f);
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
	default:
		ERR_MSG("Title::UpdateSpritePositio()", "error");

		break;
	}

	m_vpSprite[iSpriteNo]->SetPos(vPosition.x, vPosition.y);

	m_pBackGroundSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
}

//�X�v���C�g�̃A�j���[�V����.
void TitleScene::UpdateSpriteAnimation(int iSpriteNo)
{
	switch (iSpriteNo)
	{
	case enSprite_BackGround:

		break;
	case enSprite_BackGroundSub:

		break;
	case enSprite_Logo:

		break;
	case enSprite_StartButton:
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
	case enSprite_EndButton:
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
	case enSprite_StartButtonText:

		break;
	case enSprite_EndButtonText:
		m_vpSprite[iSpriteNo]->SetPatternHeight(1.0f);

		break;
	case enSprite_Cursor:
		if (IsHittingOfSprite(enSprite_Cursor, enSprite_StartButton)
			|| IsHittingOfSprite(enSprite_Cursor, enSprite_EndButton))
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
	default:
		ERR_MSG("Title::UpdateSpriteAnimation()", "error");

		break;
	}
}

//�^�C�g���̔w�i�p�̊e�o�b�t�@��`��.
void TitleScene::RenderBackGroundBuffMain()
{
	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	//�����ʒu.
	D3DXVECTOR3 vLookAt;

	//�^�C�g���̔w�i�p�o�b�t�@�̃��C���̃����_�[�^�[�Q�b�g���Z�b�g.
	ID3D11RenderTargetView* pRTV = m_pTitleBackMainBuff->GetRenderTargetView();
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

	//�J�����̒����ʒu��ݒ肷��.
	CrearVECTOR3(vLookAt);
	vLookAt.x += 0.8f;
	vLookAt.y += 1.2f;
	m_pEventCamera->SetLookAt(vLookAt);

	//�J�����̈ʒu��ݒ肷��.
	m_pEventCamera->SetPos({ vLookAt.x - 1.5f, vLookAt.y + 0.5f, vLookAt.z - 1.0f });

	m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

	//�J�����X�V.
	m_pEventCamera->Update();

	//�J�����̃r���[�A�v���W�F�N�V�������X�V.
	mView = m_pEventCamera->GetView();
	mProj = m_pEventCamera->GetProjection();

	//���f���̈ʒu��ݒ�.
	m_pPlayerModel->SetPos({ 0.0f, 0.0f, 0.0f });

	//��ʂɏo���������̂�`�悷��.
	m_pPlayerModel->RenderModel(mView, mProj);
}

void TitleScene::RenderBackGroundBuffSub()
{
	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	//�����ʒu.
	D3DXVECTOR3 vLookAt;

	//�^�C�g���̔w�i�p�o�b�t�@�̃T�u�̃����_�[�^�[�Q�b�g���Z�b�g.
	ID3D11RenderTargetView* pRTV = m_pTitleBackSubBuff->GetRenderTargetView();
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

	//�J�����̒����ʒu��ݒ肷��.
	CrearVECTOR3(vLookAt);
	vLookAt.x -= 1.0f;
	vLookAt.y += 2.4f;
	m_pEventCamera->SetLookAt(vLookAt);

	//�J�����̈ʒu��ݒ肷��.
	m_pEventCamera->SetPos({ vLookAt.x + 1.5f, vLookAt.y - 0.8f, vLookAt.z - 1.5f });

	m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

	//�J�����X�V.
	m_pEventCamera->Update();

	//�J�����̃r���[�A�v���W�F�N�V�������X�V.
	mView = m_pEventCamera->GetView();
	mProj = m_pEventCamera->GetProjection();

	//���f���̈ʒu��ݒ�.
	m_pEnemyModel->SetPos({ 0.0f, 0.0f, 0.0f });

	//��ʂɏo���������̂�`�悷��.
	m_pEnemyModel->RenderModel(mView, mProj);
}


/*====/ �V�[���ύX�֘A /====*/
//�V�[���ύX.
void TitleScene::ChangeScene(enSwitchToNextScene &enNextScene)
{
	//���N���b�N���ꂽ��.
	if (Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		//�J�[�\�����X�^�[�g�{�^���̏�ɂ��邩.
		if (IsHittingOfSprite(enSprite_Cursor, enSprite_StartButton))
		{
			enNextScene = enSwitchToNextScene::StartEvent;
		}

		//�J�[�\�����G���h�{�^���̏�ɂ��邩.
		if (IsHittingOfSprite(enSprite_Cursor, enSprite_EndButton))
		{
			DestroyWindow(m_SceneNeedPointer.hWnd);
		}
	}
}

#if _DEBUG

//�f�o�b�O�e�L�X�g�̕\��.
void TitleScene::RenderDebugText()
{
	char cStrDbgTxt[512];

	sprintf_s(cStrDbgTxt, "Scene : Title");
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 0));

	if (Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		sprintf_s(cStrDbgTxt, "IsLButtonDown : true");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 1));
	}
	else
	{
		sprintf_s(cStrDbgTxt, "IsLButtonDown : false");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 1));
	}
	
	if (Singleton<RawInput>().GetInstance().IsRButtonDown())
	{
		sprintf_s(cStrDbgTxt, "IsRButtonDown : true");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 2));
	}
	else
	{
		sprintf_s(cStrDbgTxt, "IsRButtonDown : false");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 2));
	}
}

#endif //#if _DEBUG.