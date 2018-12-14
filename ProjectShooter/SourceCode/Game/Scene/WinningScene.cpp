#include "WinningScene.h"

#include "..\\..\\Singleton.h"

//���f���̏����A�j���[�V�������x.
const double ANIMETION_SPEED = 0.01;

//���o�̒i�K�̍ő吔.
const int MAX_PHASE = 5;

//�t�F�[�h�̑��x.
const float FADE_SPEED = 0.05f;

//��t�F�[�Y�̎���.
const float COUNT_TIME = 1.5f;

WinningScene::WinningScene(SCENE_NEED_POINTER PointerGroup)
	: BaseScene(PointerGroup)
	, m_iTime(0)
	, m_pOneFrameSprite(nullptr)
	, m_pOneFrameBuff(nullptr)
	, m_pEventCamera(nullptr)
	, m_pPlayerModel(nullptr)
	, m_pEnemyModel(nullptr)
	, m_pSky(nullptr)
	, m_pStage(nullptr)
	, m_iPhase(0)
	, m_bWhenProgress(true)
{
	//�S�T�E���h���~����.
	Singleton<SoundManager>().GetInstance().StopSound();

	m_pOneFrameBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	m_pEffect = clsEffects::GetInstance();

	//�N����1��ڂ̃G�t�F�N�g���Đ�����Ȃ����߂��̑Ή��Ƃ���1��Đ����Ă���.
	m_ExpHandle = m_pEffect->Play({ 0.0f, 0.0f, 0.0f }, clsEffects::enEfcType_Explosion);
	m_pEffect->Stop(m_ExpHandle);
}

WinningScene::~WinningScene()
{
	Release();
}

//�쐬.
void WinningScene::CreateProduct(const enSwitchToNextScene enNextScene)
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
	m_pSky->SetPos({ 0.0f, 0.0f, 0.0f });
	m_pSky->SetScale(32.0f);

	m_pStage = Singleton<ModelResource>().GetInstance().GetStaticModels(ModelResource::enStaticModel_Ground);
	m_pStage->SetPos({ 0.0f, -1.5f, 0.0f });
	m_pStage->SetScale(30.0f);
}

//���.
void WinningScene::Release()
{
	m_pEffect->Stop(m_MissileHandle);
	m_pEffect->Stop(m_ExpHandle);

	SAFE_DELETE(m_pEnemyModel);
	SAFE_DELETE(m_pPlayerModel);

	m_pStage = nullptr;
	m_pSky = nullptr;

	SAFE_DELETE(m_pEventCamera);

	SAFE_DELETE(m_pOneFrameBuff);
	SAFE_DELETE(m_pOneFrameSprite);

	ReleaseSprite();
}

//�X�V.
void WinningScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
#if _DEBUG

	//�f�o�b�O���݂̂̑���.
	DebugKeyControl();

#endif //#if _DEBUG.

	//BGM�����[�v�ōĐ�.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Win);

	m_iTime++;

	//�X�v���C�g�X�V.
	UpdateSprite();

	//�J�����X�V.
	m_pEventCamera->Update();

	//���̃V�[����.
	if (m_iPhase >= MAX_PHASE)
	{
		enNextScene = enSwitchToNextScene::Clear;
	}

	if (Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		enNextScene = enSwitchToNextScene::Clear;

		//�V�[���ړ�����SE.
		if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_PushButton))
		{
			//SE���Đ�.
			Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_PushButton);
		}
	}
}

//3D���f���̕`��.
void WinningScene::RenderModelProduct(const int iRenderLevel)
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

		//�G�t�F�N�g�̕`��.
		clsEffects::GetInstance()->Render(mView, mProj, m_pEventCamera->GetPos());

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
void WinningScene::RenderSpriteProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 0:

		break;
	case 1:
		m_pOneFrameSprite->Render();

		if (m_iPhase == 4)
		{
			m_vpSprite[enSprite_Text]->Render();
		}

		break;
	case MAX_RENDER_LEVEL:
		break;
	default:
		break;
	}
}

//�X�v���C�g�̍쐬.
void WinningScene::CreateSprite()
{
	//�L���p�V�e�B���m�肳����.
	m_vpSprite.reserve(enSprite_Max);

	SPRITE_STATE SpriteData;

	//�e�X�v���C�g�̐ݒ�.
	for (int i = 0; i < enSprite_Max; i++)
	{
		switch (i)
		{
		case enSprite_Text:
			SpriteData =
			{ "Data\\Image\\Win.png"	//�t�@�C���܂ł̃p�X.
			, { 1.0f, 1.0f } };			//���摜�����������邩.

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
void WinningScene::ReleaseSprite()
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
void WinningScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//�X�v���C�g�̈ʒu.
void WinningScene::UpdateSpritePositio(int iSpriteNo)
{
	//�E�C���h�E�̒��S.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;

	//�X�v���C�g�ʒu.
	D3DXVECTOR2 vPosition;

	switch (iSpriteNo)
	{
	case enSprite_Text:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter / 2.0f;

		break;
	}

	m_vpSprite[iSpriteNo]->SetPos(vPosition.x, vPosition.y);

	m_pOneFrameSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
}

//�X�v���C�g�̃A�j���[�V����.
void WinningScene::UpdateSpriteAnimation(int iSpriteNo)
{
	switch (iSpriteNo)
	{
	case enSprite_Text:

		break;
	default:
		ERR_MSG("Clear::UpdateSpriteAnimation()", "error");

		break;
	}
}

//���o�̒i�K���Ƃ̕`��.
void WinningScene::PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
{
	//������.
	PhaseInit(iPhase);

	//�J�����̑���.
	PhaseCameraControl(iPhase);

	D3DXVECTOR3 vEfcPos = m_pEnemyModel->GetPos();
	vEfcPos.y += 1.5f;

	const float fRatio = 0.5f;
	if (m_pEnemyModel->IsAnimationRatioEnd(fRatio))
	{
		//�A�j���[�V�������x.
		m_pEnemyModel->SetAnimationSpeed(0);
	}

	switch (iPhase)
	{
	case 0:

		break;
	case 1:

		break;
	case 2:

		break;
	case 3:
		if (m_iTime / FPS == COUNT_TIME / 3.0f)
		{
			//�G�t�F�N�g���Đ�.
			vEfcPos.x -= 1.0f;
			vEfcPos.y -= 1.0f;
			m_ExpHandle = m_pEffect->Play(vEfcPos, clsEffects::enEfcType_Explosion);
			Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_Explosion);
		}

		if (m_iTime / FPS == COUNT_TIME - (COUNT_TIME / 3.0f))
		{
			//�G�t�F�N�g���Đ�.
			vEfcPos.x -= 1.0f;
			vEfcPos.y += 1.0f;
			m_ExpHandle = m_pEffect->Play(vEfcPos, clsEffects::enEfcType_Explosion);
			Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_Explosion);
		}

		break;
	case 4:
		{
			if (m_iTime / FPS >= COUNT_TIME / 2.0f)
			{
				m_pPlayerModel->AddYaw(15.0f);

				m_pPlayerModel->SetPos({ 0.0f, m_pPlayerModel->GetPos().y, -20.0f });
				m_pPlayerModel->AddPos({ 0.0f, 0.5f, 0.0f });

				m_pEffect->SetLocation(m_MissileHandle, m_pPlayerModel->GetPos());

				//�J�����̒����ʒu��ݒ肷��.
				D3DXVECTOR3 vLookAt;
				vLookAt = m_pPlayerModel->GetPos();
				vLookAt.y += 1.5f;
				m_pEventCamera->SetLookAt(vLookAt);

				m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

				//�G�t�F�N�g���Đ�.
				if (!m_pEffect->PlayCheck(m_MissileHandle))
				{
					m_MissileHandle = m_pEffect->Play(m_pPlayerModel->GetPos(), clsEffects::enEfcType_Missile);
					//�G�t�F�N�g�̑傫��.
					float fMissileScale = 0.2f;
					m_pEffect->SetScale(m_MissileHandle, { fMissileScale, fMissileScale, fMissileScale });

					D3DXVECTOR3 vRot = { D3DXToRadian(-180), 0.0f, 0.0f };
					m_pEffect->SetRotation(m_MissileHandle, vRot);
				}

				if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_Fire))
				{
					Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Fire);
				}
			}
			else
			{
				if (m_iTime % 2 == 0)
				{
					float fMoveSpeed = 0.005f;
					if (m_pPlayerModel->GetPos().x >= 0.0f)
					{
						m_pPlayerModel->SetPos({ -fMoveSpeed, m_pPlayerModel->GetPos().y, m_pPlayerModel->GetPos().z });
					}
					else
					{
						m_pPlayerModel->SetPos({ fMoveSpeed, m_pPlayerModel->GetPos().y, m_pPlayerModel->GetPos().z });
					}
				
					//float fRotSpeed = 0.025f;
					//if (m_pEventCamera->GetRot().y >= 0.0f)
					//{
					//	m_pEventCamera->AddRot({ 0.0f, -fRotSpeed, 0.0f });
					//}
					//else
					//{
					//	m_pEventCamera->AddRot({ 0.0f, fRotSpeed, 0.0f });
					//}
				}
			}
		}
		break;
	default:
		break;
	}

	m_pPlayerModel->RenderModel(mView, mProj);
	m_pEnemyModel->RenderModel(mView, mProj);

	m_pSky->Render(mView, mProj);
	m_pStage->Render(mView, mProj);

	//�i�s.
	PhaseProgress(iPhase);
}

//���o�̒i�K���Ƃ̃J��������.
void WinningScene::PhaseCameraControl(const int iPhase)
{
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
	default:
		break;
	}
}

//���o�̒i�K�̐i�s.
void WinningScene::PhaseProgress(const int iPhase)
{
	switch (iPhase)
	{
	case 0:
		if (m_iTime / FPS >= COUNT_TIME)
		{
			m_iPhase++;
			m_bWhenProgress = true;
		}

		break;
	case 1:
		if (m_iTime / FPS >= COUNT_TIME)
		{
			m_iPhase++;
			m_bWhenProgress = true;
		}

		break;
	case 2:
		if (m_iTime / FPS >= COUNT_TIME)
		{
			m_iPhase++;
			m_bWhenProgress = true;
		}

		break;
	case 3:
		if (m_iTime / FPS >= COUNT_TIME)
		{
			m_iPhase++;
			m_bWhenProgress = true;
		}

		break;
	case 4:
		if (m_pPlayerModel->GetPos().y >= 40.0f)
		{
			m_iPhase++;
			m_bWhenProgress = true;
		}

		break;
	default:
		break;
	}
}

//���o�̊e�i�K�̏�����.
void WinningScene::PhaseInit(const int iPhase)
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
		//���f���̈ʒu��ݒ�.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, -20.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//�A�j���[�V������ݒ�.
		{
			m_pEnemyModel->ChangeAnimation(4);
			const double dHitAnimationSpeed = 0.005;
			m_pEnemyModel->SetAnimationSpeed(dHitAnimationSpeed);
		}

		//�J�����̒����ʒu��ݒ肷��.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//�G�t�F�N�g���Đ�.
		m_ExpHandle = m_pEffect->Play(vLookAt, clsEffects::enEfcType_Explosion);
		Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Explosion);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x + 1.0f, vLookAt.y - 1.0f, vLookAt.z - 2.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	case 1:
		//���f���̈ʒu��ݒ�.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, -20.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//�A�j���[�V������ݒ�.
		{
			m_pEnemyModel->ChangeAnimation(4);
			const double dHitAnimationSpeed = 0.005;
			m_pEnemyModel->SetAnimationSpeed(dHitAnimationSpeed);
		}

		//�J�����̒����ʒu��ݒ肷��.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//�G�t�F�N�g���Đ�.
		m_ExpHandle = m_pEffect->Play(vLookAt, clsEffects::enEfcType_Explosion);
		Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Explosion);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x - 2.0f, vLookAt.y + 4.0f, vLookAt.z - 6.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	case 2:
		//���f���̈ʒu��ݒ�.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, -20.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//�A�j���[�V������ݒ�.
		{
			m_pEnemyModel->ChangeAnimation(4);
			const double dHitAnimationSpeed = 0.005;
			m_pEnemyModel->SetAnimationSpeed(dHitAnimationSpeed);
		}

		//�J�����̒����ʒu��ݒ肷��.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//�G�t�F�N�g���Đ�.
		m_ExpHandle = m_pEffect->Play(vLookAt, clsEffects::enEfcType_Explosion);
		Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Explosion);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x + 2.0f, vLookAt.y + 4.0f, vLookAt.z + 6.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	case 3:
		//���f���̈ʒu��ݒ�.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, 0.0f });
		m_pPlayerModel->SetRot({ D3DXToRadian(60), D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//�A�j���[�V������ݒ�.
		{
			m_pEnemyModel->ChangeAnimation(4);
			const double dHitAnimationSpeed = 0.005;
			m_pEnemyModel->SetAnimationSpeed(dHitAnimationSpeed);
		}

		//�J�����̒����ʒu��ݒ肷��.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//�G�t�F�N�g���Đ�.
		m_ExpHandle = m_pEffect->Play(vLookAt, clsEffects::enEfcType_Explosion);
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_Explosion);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x - 1.0f, vLookAt.y + 1.0f, vLookAt.z - 8.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	case 4:
		//���f���̈ʒu��ݒ�.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, -20.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//�J�����̒����ʒu��ݒ肷��.
		vLookAt = m_pPlayerModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//�J�����̈ʒu��ݒ肷��.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z + 4.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	default:
		break;
	}

	m_bWhenProgress = false;
}

#if _DEBUG

//�f�o�b�O�e�L�X�g�̕\��.
void WinningScene::RenderDebugText()
{
	char cStrDbgTxt[512];

	sprintf_s(cStrDbgTxt, "Scene : Winning");
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 0));

	sprintf_s(cStrDbgTxt, "SpriteSize : ( %f, %f )", m_pOneFrameSprite->GetSize().x, m_pOneFrameSprite->GetSize().y);
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 1));

	sprintf_s(cStrDbgTxt, "SpriteScale : [%f]", m_pOneFrameSprite->GetScale());
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 2));

	sprintf_s(cStrDbgTxt, "Phase : [%i]", m_iPhase);
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 3));

	sprintf_s(cStrDbgTxt, "SkyPos : ( %f, &f, %f )", m_pSky->GetPos().x, m_pSky->GetPos().y, m_pSky->GetPos().z);
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 3));
}

//�f�o�b�O���݂̂̑���.
void WinningScene::DebugKeyControl()
{
}

#endif //#if _DEBUG.