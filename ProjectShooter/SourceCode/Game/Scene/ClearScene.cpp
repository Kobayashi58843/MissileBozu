#include "ClearScene.h"

#include "..\\..\\Singleton.h"

//���f���̏����A�j���[�V�������x.
const double ANIMETION_SPEED = 0.01;

//���o�̒i�K�̍ő吔.
const int MAX_PHASE = 8;

//��t�F�[�Y�̎���.
const float COUNT_TIME = 2.5f;

ClearScene::ClearScene(SCENE_NEED_POINTER PointerGroup)
	: BaseScene(PointerGroup)
	, m_iTime(0)
	, m_pModelSprite(nullptr)
	, m_pModelBuff(nullptr)
	, m_pModelSpriteCamera(nullptr)
	, m_iPhase(0)
	, m_bWhenProgress(true)
	, m_pPlayerModel(nullptr)
	, m_iCursorAnimationCount(0)
{
	//�S�T�E���h���~����.
	Singleton<SoundManager>().GetInstance().StopSound();

	m_pModelBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	m_pEffect = clsEffects::GetInstance();

	//�N����1��ڂ̃G�t�F�N�g���Đ�����Ȃ����߂��̑Ή��Ƃ���1��Đ����Ă���.
	m_MissileHandle = m_pEffect->Play({ 0.0f, 0.0f, 0.0f }, clsEffects::enEfcType_Missile);
	m_pEffect->Stop(m_MissileHandle);
}

ClearScene::~ClearScene()
{
	m_pEffect->Stop(m_MissileHandle);

	Release();
}

//�쐬.
void ClearScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//�X�v���C�g�̍쐬.
	CreateSprite();

	//�J�������쐬.
	m_pModelSpriteCamera = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	//�X�L�����f���̍쐬.
	m_pPlayerModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Player), 0.0005f, ANIMETION_SPEED);
	m_pPlayerModel->ChangeAnimation(1);
}

//���.
void ClearScene::Release()
{
	SAFE_DELETE(m_pPlayerModel);

	SAFE_DELETE(m_pModelSpriteCamera);

	SAFE_DELETE(m_pModelBuff);
	SAFE_DELETE(m_pModelSprite);

	ReleaseSprite();
}

//�X�V.
void ClearScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
	//BGM�����[�v�ōĐ�.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Clear);

	m_iTime++;

	//�X�v���C�g�X�V.
	UpdateSprite();

	//�J�����X�V.
	m_pModelSpriteCamera->Update();

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
void ClearScene::RenderModelProduct(const int iRenderLevel)
{
	D3DXMATRIX mView = m_pModelSpriteCamera->GetView();
	D3DXMATRIX mProj = m_pModelSpriteCamera->GetProjection();

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

		ModelControl(m_pModelSpriteCamera->GetView(), m_pModelSpriteCamera->GetProjection(), m_iPhase);

		//�G�t�F�N�g�̕`��.
		clsEffects::GetInstance()->Render(mView, mProj, m_pModelSpriteCamera->GetPos());

		//�����_�[�^�[�Q�b�g�����ɖ߂�.
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &m_SceneNeedPointer.pBackBuffer_RTV, m_SceneNeedPointer.pBackBuffer_DSV);
	}
	break;
	case 1:
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
void ClearScene::RenderSpriteProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 0:
		m_vpSprite[enSprite_BackGround]->Render();

		break;
	case MAX_RENDER_LEVEL:
		m_pModelSprite->Render();

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
void ClearScene::CreateSprite()
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
		case enSprite_Logo:
			SpriteData = { "Data\\Image\\LogoText.png", { 1.0f, 3.0f } };

			break;
		case enSprite_ReturnButton:
			SpriteData = { "Data\\Image\\Push.jpg", { 1.0f, 2.0f } };

			break;
		case enSprite_ReturnButtonText:
			SpriteData = { "Data\\Image\\ButtonText.png", { 1.0f, 3.0f } };

			break;
		case enSprite_Cursor:
			SpriteData = { "Data\\Image\\Cursor.png", { 8.0f, 1.0f } };

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
void ClearScene::ReleaseSprite()
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
void ClearScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//�X�v���C�g�̈ʒu.
void ClearScene::UpdateSpritePositio(int iSpriteNo)
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
	default:
		ERR_MSG("Clear::UpdateSpritePositio()", "error");

		break;
	}

	m_vpSprite[iSpriteNo]->SetPos(vPosition.x, vPosition.y);

	m_pModelSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
}

//�X�v���C�g�̃A�j���[�V����.
void ClearScene::UpdateSpriteAnimation(int iSpriteNo)
{
	switch (iSpriteNo)
	{
	case enSprite_BackGround:

		break;
	case enSprite_Logo:
		m_vpSprite[iSpriteNo]->SetPatternHeight(1.0f);

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
	default:
		ERR_MSG("Clear::UpdateSpriteAnimation()", "error");

		break;
	}
}

//���o�̒i�K���Ƃ̕`��.
void ClearScene::PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
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
void ClearScene::PhaseCameraControl(const int iPhase)
{
	float fYawRotSpeed = 0.02f;

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
}

//���o�̒i�K�̐i�s.
void ClearScene::PhaseProgress(const int iPhase)
{
	switch (iPhase)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		if (m_iTime / FPS >= COUNT_TIME)
		{
			m_iPhase++;
			m_bWhenProgress = true;

			if (m_iPhase >= MAX_PHASE)
			{
				m_iPhase = 0;
			}
		}

		break;
	default:
		break;
	}
}

//���o�̊e�i�K�̏�����.
void ClearScene::PhaseInit(const int iPhase)
{
	if (!m_bWhenProgress)
	{
		return;
	}

	m_iTime = 0;

	//�����ʒu���N���A.
	D3DXVECTOR3 vLookAt;
	CrearVECTOR3(vLookAt);

	//�J�����̒����ʒu��ݒ肷��.
	m_pModelSpriteCamera->SetLookAt(vLookAt);

	//�J�����̈ʒu��ݒ肷��.
	m_pModelSpriteCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 5.0f });

	//�G�t�F�N�g���Đ�.
	if (!m_pEffect->PlayCheck(m_MissileHandle))
	{
		m_MissileHandle = m_pEffect->Play(vLookAt, clsEffects::enEfcType_Missile);
		//�G�t�F�N�g�̑傫��.
		float fMissileScale = 0.2f;
		m_pEffect->SetScale(m_MissileHandle, { fMissileScale, fMissileScale, fMissileScale });

		D3DXVECTOR3 vRot = { D3DXToRadian(-180), 0.0f, 0.0f };
		m_pEffect->SetRotation(m_MissileHandle, vRot);
	}

	float fStartPosDis = 6.0f;

	Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Missile);

	switch (iPhase)
	{
	case 0:
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, 0.0f });
		m_pPlayerModel->SetPos({ 0.0f, -fStartPosDis, 0.0f });

		break;
	case 1:
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(90) });
		m_pPlayerModel->SetPos({ fStartPosDis, 0.0f, 0.0f });

		break;
	case 2:
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(-45) });
		m_pPlayerModel->SetPos({ -fStartPosDis, -fStartPosDis, 0.0f });

		break;
	case 3:
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(45) });
		m_pPlayerModel->SetPos({ fStartPosDis, -fStartPosDis, 0.0f });

		break;
	case 4:
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(135) });
		m_pPlayerModel->SetPos({ fStartPosDis, fStartPosDis, 0.0f });

		break;
	case 5:
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(-90) });
		m_pPlayerModel->SetPos({ -fStartPosDis, 0.0f, 0.0f });

		break;
	case 6:
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(-135) });
		m_pPlayerModel->SetPos({ -fStartPosDis, fStartPosDis, 0.0f });

		break;
	case 7:
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(180) });
		m_pPlayerModel->SetPos({ 0.0f, fStartPosDis, 0.0f });

		break;
	default:
		break;
	}

	m_bWhenProgress = false;
}

void ClearScene::ModelControl(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
{
	float fMoveSpeed = 0.08f;

	switch (iPhase)
	{
	case 0:
		m_pPlayerModel->AddPos({ 0.0f, fMoveSpeed, 0.0f });

		break;
	case 1:
		m_pPlayerModel->AddPos({ -fMoveSpeed, 0.0f, 0.0f });

		break;
	case 2:
		m_pPlayerModel->AddPos({ fMoveSpeed, fMoveSpeed, 0.0f });

		break;
	case 3:
		m_pPlayerModel->AddPos({ -fMoveSpeed, fMoveSpeed, 0.0f });

		break;
	case 4:
		m_pPlayerModel->AddPos({ -fMoveSpeed, -fMoveSpeed, 0.0f });

		break;
	case 5:
		m_pPlayerModel->AddPos({ fMoveSpeed, 0.0f, 0.0f });

		break;
	case 6:
		m_pPlayerModel->AddPos({ fMoveSpeed, -fMoveSpeed, 0.0f });

		break;
	case 7:
		m_pPlayerModel->AddPos({ 0.0f, -fMoveSpeed, 0.0f });

		break;
	default:
		break;
	}

	m_pPlayerModel->AddYaw(15.0f);

	D3DXVECTOR3 vPos = m_pPlayerModel->GetPos();
	m_pEffect->SetLocation(m_MissileHandle, vPos);

	m_pPlayerModel->RenderModel(mView, mProj);
}

#if _DEBUG

//�f�o�b�O�e�L�X�g�̕\��.
void ClearScene::RenderDebugText()
{
	char cStrDbgTxt[512];

	sprintf_s(cStrDbgTxt, "Scene : Clear");
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 0));
}

#endif //#if _DEBUG.