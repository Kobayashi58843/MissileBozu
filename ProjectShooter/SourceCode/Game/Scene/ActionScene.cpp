#include "ActionScene.h"

const float CAMERA_MOVE_SPEED = 0.4f;

ActionScene::ActionScene(SCENE_NEED_POINTER PointerGroup)
	: BaseScene(PointerGroup)
	, m_pCamera(nullptr)
	, m_pPlayer(nullptr)
	, m_pEnemy(nullptr)
	, m_pGround(nullptr)
	, m_pSky(nullptr)
	, m_pBulletManager(nullptr)
{
	//�S�T�E���h���~����.
	Singleton<SoundManager>().GetInstance().StopSound();
}

ActionScene::~ActionScene()
{
	Release();
}

//�쐬.
void ActionScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//�X�v���C�g�̍쐬.
	CreateSprite();

	m_pCamera = new Camera(WINDOW_WIDTH,WINDOW_HEIGHT);
	m_pCamera->SetDisplaceHorizontally(0.5f);
	m_pCamera->SetFocusingSpacePos({ 0.0f, 0.0f, -5.0f });

	m_pBulletManager = new BulletManager;

	m_pPlayer = new Player(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Player), m_pBulletManager);

	m_pEnemy = new Enemy(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Enemy));

	m_pGround = Singleton<ModelResource>().GetInstance().GetStaticModels(ModelResource::enStaticModel_Ground);
	m_pGround->SetScale(30.0f);
	m_pGround->SetPos({ 0.0f, -1.5f, 0.0f });

	m_pSky = Singleton<ModelResource>().GetInstance().GetStaticModels(ModelResource::enStaticModel_SkyBox);
	m_pSky->SetScale(10.0f);

}

//���.
void ActionScene::Release()
{
	SAFE_DELETE(m_pCamera);

	SAFE_DELETE(m_pPlayer);

	SAFE_DELETE(m_pEnemy);

	m_pSky = nullptr;

	m_pGround = nullptr;

	SAFE_DELETE(m_pBulletManager);

	ReleaseSprite();
}

//�X�V.
void ActionScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
#if _DEBUG

	//�f�o�b�O���݂̂̑���.
	DebugKeyControl();

#endif //#if _DEBUG.

	//BGM�����[�v�ōĐ�.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Action);

	//�X�v���C�g�X�V.
	UpdateSprite();

	//�L�����N�^�[�X�V.
	m_pPlayer->DecideTargetDirection(m_pCamera->GetWorldPos());
	m_pPlayer->Update();
	m_pPlayer->RayHitToMesh(m_pGround);

	m_pEnemy->DecideTargetDirection(m_pPlayer->GetPos());
	m_pEnemy->Update();
	m_pEnemy->RayHitToMesh(m_pGround);

	m_pPlayer->HitToSphere(m_pEnemy->GetCollisionSphere());

	//�J�����X�V.
	D3DXVECTOR3 vLookAt = m_pPlayer->GetPos();
	vLookAt.y += 1.5f;
	m_pCamera->SetLookAt(vLookAt);
	ControlCameraMove();
	m_pCamera->Update();
	m_pCamera->RayHitToMesh(m_pGround);

	m_pBulletManager->Update(m_pCamera->GetCameraPose(), m_pPlayer->GetPos(), m_pEnemy);
	m_pBulletManager->CollisionJudgmentBullet(m_pEnemy->GetCollisionSphere(), m_pGround);

	//�V�[���ύX.
	if (m_pEnemy->IsDeadFlg())
	{
		enNextScene = enSwitchToNextScene::Winning;
	}
	else if (m_pPlayer->IsDeadFlg())
	{
		enNextScene = enSwitchToNextScene::Loser;
	}
}

//3D���f���̕`��.
void ActionScene::RenderModelProduct(const int iRenderLevel)
{
	D3DXMATRIX mView = m_pCamera->GetView();
	D3DXMATRIX mProj = m_pCamera->GetProjection();
	D3DXVECTOR3 vCamPos = m_pCamera->GetWorldPos();

	switch (iRenderLevel)
	{
	case 0:
		m_pPlayer->RenderModel(mView, mProj);

		m_pEnemy->RenderModel(mView, mProj);

		m_pSky->Render(mView, mProj);

		m_pGround->Render(mView, mProj);

		m_pBulletManager->Render(mView, mProj, vCamPos);

		//�G�t�F�N�g�̕`��.
		clsEffects::GetInstance()->Render(mView, mProj, vCamPos);

		break;
	default:
		break;
	}
}

//�J�����̑���.
void ActionScene::ControlCameraMove()
{
	D3DXVECTOR2 vMouseMovingDistance = Singleton<RawInput>().GetInstance().GetMouseMovingDistance();

	//�}�E�X�̈ړ����x�Ŏ��_�ړ����鑬����ς���.
	float fCameraMoveSpeed = fabs(CAMERA_MOVE_SPEED * (vMouseMovingDistance.x + vMouseMovingDistance.y));
	m_pCamera->SetRotPerFrame(fCameraMoveSpeed);

	//�J�����̈ړ�.
	if (0 < vMouseMovingDistance.y)
	{
		m_pCamera->MoveUp();
	}
	else if (0 > vMouseMovingDistance.y)
	{
		m_pCamera->MoveDown();
	}

	if (0 < vMouseMovingDistance.x)
	{
		m_pCamera->MoveLeft();
	}
	else if (0 > vMouseMovingDistance.x)
	{
		m_pCamera->MoveRight();
	}

	/*====/ �J�����̋��� /====*/
	if (Singleton<RawInput>().GetInstance().IsRButtonHoldDown())
	{
		if (m_pCamera->GetDistance() > 1.0f)
		{
			m_pCamera->SetOffsetZ(1.0f);
		}
	}
	else
	{
		if (m_pCamera->GetDistance() < 5.0f)
		{
			m_pCamera->SetOffsetZ(-1.0f);
		}
	}
}

/*====/ �X�v���C�g�֘A /====*/
//2D�X�v���C�g�̕`��.
void ActionScene::RenderSpriteProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 0:
		for (unsigned int i = 0; i < m_vpSprite.size(); i++)
		{
			m_vpSprite[i]->Render();
		}

		break;
	default:
		break;
	}
}

//�X�v���C�g�̍쐬.
void ActionScene::CreateSprite()
{
	//�L���p�V�e�B���m�肳����.
	m_vpSprite.reserve(enSprite_Max);

	SPRITE_STATE SpriteData;

	//�e�X�v���C�g�̐ݒ�.
	for (int i = 0; i < enSprite_Max; i++)
	{
		switch (i)
		{
		case enSprite_Alignment:
			SpriteData = { "Data\\Image\\AimingIcon.png", { 1.0f, 1.0f } };

			break;
		case enSprite_PlayerHp:
			SpriteData = { "Data\\Image\\PlayerHpGage.png", { 2.0f, 1.0f } };

			break;
		case enSprite_PlayerHpFrame:
			SpriteData = { "Data\\Image\\HpFrame.png", { 1.0f, 1.0f } };

			break;
		case enSprite_EnemyHp:
			SpriteData = { "Data\\Image\\EnemyHpGage.png", { 2.0f, 1.0f } };

			break;
		case enSprite_EnemyHpFrame:
			SpriteData = { "Data\\Image\\HpFrame.png", { 1.0f, 1.0f } };

			break;

		default:
			ERR_MSG("Action::CreateSprite()", "error");

			break;
		}

		//�z�������₷.
		m_vpSprite.push_back(new Sprite(SpriteData.vDivisionQuantity.x, SpriteData.vDivisionQuantity.y));
		m_vpSprite[i]->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, SpriteData.sPath);
	}
}

//�X�v���C�g�̉��.
void ActionScene::ReleaseSprite()
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
void ActionScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//�X�v���C�g�̈ʒu.
void ActionScene::UpdateSpritePositio(int iSpriteNo)
{
	//�E�C���h�E�̒��S.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;

	//�X�v���C�g�ʒu.
	D3DXVECTOR2 vPosition;

	//Hp�Q�[�W�̑傫��.
	const float fGageScale = 0.2f;

	//�g�ƃQ�[�W�̊Ԋu.
	const float fGageSpacing = 25.0f;

	switch (iSpriteNo)
	{
	case enSprite_Alignment:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter;

		break;
	case enSprite_PlayerHp:
		vPosition.x = (m_vpSprite[iSpriteNo]->GetSize().x / 2.0f) + fGageSpacing;
		vPosition.y = (m_vpSprite[iSpriteNo]->GetSize().y / 2.0f) + fGageSpacing;

		break;
	case enSprite_PlayerHpFrame:
		//�v���C���[��Hp�Q�[�W�̈ʒu�ɂ��킹��.
		vPosition.x = m_vpSprite[enSprite_PlayerHp]->GetPos().x;
		vPosition.y = m_vpSprite[enSprite_PlayerHp]->GetPos().y;

		break;
	case enSprite_EnemyHp:
		vPosition.x = WINDOW_WIDTH - ((m_vpSprite[iSpriteNo]->GetSize().x / 2.0f) + fGageSpacing);
		vPosition.y = (m_vpSprite[iSpriteNo]->GetSize().y / 2.0f) + fGageSpacing;

		break;
	case enSprite_EnemyHpFrame:
		//�G�l�~�[��Hp�Q�[�W�̈ʒu�ɂ��킹��.
		vPosition.x = m_vpSprite[enSprite_EnemyHp]->GetPos().x;
		vPosition.y = m_vpSprite[enSprite_EnemyHp]->GetPos().y;

		break;
	default:
		ERR_MSG("Action::UpdateSpritePositio()", "error");

		break;
	}

	m_vpSprite[iSpriteNo]->SetPos(vPosition.x, vPosition.y);
}

//�X�v���C�g�̃A�j���[�V����.
void ActionScene::UpdateSpriteAnimation(int iSpriteNo)
{
	const float fGageMoveSpeed = 2.0f;

	switch (iSpriteNo)
	{
	case enSprite_Alignment:
		break;
	case enSprite_PlayerHp:
		m_vpSprite[iSpriteNo]->SpriteAsGage(m_pPlayer->GetHpMax(), m_pPlayer->GetHp(), fGageMoveSpeed);

		break;
	case enSprite_PlayerHpFrame:
		break;
	case enSprite_EnemyHp:
		m_vpSprite[iSpriteNo]->SpriteAsGage(m_pEnemy->GetHpMax(), m_pEnemy->GetHp(), fGageMoveSpeed);

		break;
	case enSprite_EnemyHpFrame:
		break;
	default:
		ERR_MSG("Action::UpdateSpriteAnimation()", "error");

		break;
	}
}

#if _DEBUG

//�f�o�b�O�e�L�X�g�̕\��.
void ActionScene::RenderDebugText()
{
	char cStrDbgTxt[512];

	sprintf_s(cStrDbgTxt, "Scene : Action");
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 0));

	sprintf_s(cStrDbgTxt, "MouseMoveDis : X[%f] , Y[%f]",
		Singleton<RawInput>().GetInstance().GetMouseMovingDistance().x,
		Singleton<RawInput>().GetInstance().GetMouseMovingDistance().y);

	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 1));

	if (Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		sprintf_s(cStrDbgTxt, "IsLButtonDown : true");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 2));
	}
	else
	{
		sprintf_s(cStrDbgTxt, "IsLButtonDown : false");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 2));
	}

	if (Singleton<RawInput>().GetInstance().IsLButtonHoldDown())
	{
		sprintf_s(cStrDbgTxt, "IsLButtonHoldDown : true");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 3));
	}
	else
	{
		sprintf_s(cStrDbgTxt, "IsLButtonHoldDown : false");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 3));
	}

	if (Singleton<RawInput>().GetInstance().IsRButtonDown())
	{
		sprintf_s(cStrDbgTxt, "IsRButtonDown : true");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 4));
	}
	else
	{
		sprintf_s(cStrDbgTxt, "IsRButtonDown : false");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 4));
	}

	if (Singleton<RawInput>().GetInstance().IsRButtonHoldDown())
	{
		sprintf_s(cStrDbgTxt, "IsRButtonHoldDown : true");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 5));
	}
	else
	{
		sprintf_s(cStrDbgTxt, "IsRButtonHoldDown : false");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 5));
	}

	if (Singleton<RawInput>().GetInstance().IsWheelForward())
	{
		sprintf_s(cStrDbgTxt, "IsWheelForward : true");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 6));
	}
	else
	{
		sprintf_s(cStrDbgTxt, "IsWheelForward : false");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 6));
	}

	if (Singleton<RawInput>().GetInstance().IsWheelBackward())
	{
		sprintf_s(cStrDbgTxt, "IsWheelBackward : true");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 7));
	}
	else
	{
		sprintf_s(cStrDbgTxt, "IsWheelBackward : false");
		m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 7));
	}

	float fCamAngle = atanf(m_pCamera->GetFocusingSpacePos().y);
	sprintf_s(cStrDbgTxt, "CamAngle : [%f]", fCamAngle);
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 8));
}

//�f�o�b�O���݂̂̑���.
void ActionScene::DebugKeyControl()
{
}

#endif //#if _DEBUG.