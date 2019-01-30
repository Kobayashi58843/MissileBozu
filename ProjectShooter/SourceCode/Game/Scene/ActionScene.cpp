#include "ActionScene.h"

const float CAMERA_MOVE_SPEED = 0.4f;

const D3DXVECTOR3 LIGHT_CAMERA_POS = { 0.0f, 30.0f, -10.0f };

ActionScene::ActionScene(SCENE_NEED_POINTER PointerGroup)
	: BaseScene(PointerGroup)
	, m_pCamera(nullptr)
	, m_pLightView(nullptr)
	, m_pPlayer(nullptr)
	, m_pEnemy(nullptr)
	, m_pGround(nullptr)
	, m_pSky(nullptr)
	, m_pBulletManager(nullptr)
	, m_pDisplayDepthBuffer(nullptr)
{
	//全サウンドを停止する.
	Singleton<SoundManager>().GetInstance().StopSound();

	const D3DXVECTOR2 vBackBufferSize = { WINDOW_WIDTH, WINDOW_HEIGHT };
	m_pDepthBuffer = new BackBuffer(m_SceneNeedPointer.pDevice, (UINT)vBackBufferSize.x, (UINT)vBackBufferSize.y);
}

ActionScene::~ActionScene()
{
	Release();

	SAFE_DELETE(m_pDisplayDepthBuffer);

	SAFE_DELETE(m_pDepthBuffer);
}

//作成.
void ActionScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//スプライトの作成.
	CreateSprite();

	m_pCamera = new Camera(WINDOW_WIDTH,WINDOW_HEIGHT);
	//カメラを横から覗き込むようにするために横にずらす.
	m_pCamera->SetDisplaceHorizontally(0.5f);
	//カメラを後ろにずらす.
	m_pCamera->SetFocusingSpacePos({ 0.0f, 0.0f, -5.0f });

	m_pLightView = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pLightView->SetLookAt({ 0.0f, 0.0f, 0.0f });

	m_pBulletManager = new BulletManager;

	m_pPlayer = new Player(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Player), m_pBulletManager);

	m_pEnemy = new Enemy(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Enemy));

	m_pGround = Singleton<ModelResource>().GetInstance().GetStaticModels(ModelResource::enStaticModel_Ground);
	//モデルのサイズが小さいためサイズを大きくする.
	m_pGround->SetScale(30.0f);
	//モデルの中心位置がメッシュの中に埋まっているため位置を下に下げる.
	m_pGround->SetPos({ 0.0f, -1.5f, 0.0f });

	m_pSky = Singleton<ModelResource>().GetInstance().GetStaticModels(ModelResource::enStaticModel_SkyBox);
	//モデルのサイズが小さいためサイズを大きくする.
	m_pSky->SetScale(10.0f);

	//シャドウマップ用の深度テクスチャのカメラの位置をプレイヤーの頭上にあわせる.
	m_pLightView->SetPos({ LIGHT_CAMERA_POS.x, LIGHT_CAMERA_POS.y, LIGHT_CAMERA_POS.z });
}

//解放.
void ActionScene::Release()
{
	SAFE_DELETE(m_pCamera);

	SAFE_DELETE(m_pLightView);

	SAFE_DELETE(m_pPlayer);

	SAFE_DELETE(m_pEnemy);

	m_pSky = nullptr;

	m_pGround = nullptr;

	SAFE_DELETE(m_pBulletManager);

	ReleaseSprite();
}

//更新.
void ActionScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
#if _DEBUG

	//デバッグ中のみの操作.
	DebugKeyControl();

#endif //#if _DEBUG.

	//BGMをループで再生.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Action);

	//スプライト更新.
	UpdateSprite();

	//キャラクター更新.
	m_pPlayer->DecideTargetDirection(m_pCamera->GetWorldPos());
	m_pPlayer->Update();
	m_pPlayer->RayHitToMesh(m_pGround);

	m_pEnemy->DecideTargetDirection(m_pPlayer->GetPos());
	m_pEnemy->Update();
	m_pEnemy->RayHitToMesh(m_pGround);

	m_pPlayer->HitToSphere(m_pEnemy->GetCollisionSphere());

	//カメラ更新.
	D3DXVECTOR3 vLookAt = m_pPlayer->GetPos();
	vLookAt.y += 1.5f;
	m_pCamera->SetLookAt(vLookAt);
	ControlCameraMove();
	m_pCamera->Update();
	m_pCamera->RayHitToMesh(m_pGround);

	m_pBulletManager->Update(m_pCamera->GetCameraPose(), m_pPlayer->GetPos(), m_pEnemy);
	m_pBulletManager->CollisionJudgmentBullet(m_pEnemy->GetCollisionSphere(), m_pGround);

	//シーン変更.
	if (m_pEnemy->IsDeadFlg())
	{
		enNextScene = enSwitchToNextScene::Winning;
	}
	else if (m_pPlayer->IsDeadFlg())
	{
		enNextScene = enSwitchToNextScene::Loser;
	}
}

//3Dモデルの描画.
void ActionScene::RenderModelProduct(const int iRenderLevel)
{
	D3DXMATRIX mView = m_pCamera->GetView();
	D3DXMATRIX mProj = m_pCamera->GetProjection();
	D3DXVECTOR3 vCamPos = m_pCamera->GetWorldPos();

	switch (iRenderLevel)
	{
	case 0:
	{
		//シャドウマップ用のカメラの.
		mView = m_pLightView->GetView();

		//レンダーターゲットをシャドウマップ用の深度テクスチャへ.
		ID3D11RenderTargetView* pRTV = m_pDepthBuffer->GetRenderTargetView();

		//レンダーターゲットに深度テクスチャをセット.
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

		//画面のクリア.
		const float fClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_SceneNeedPointer.pContext->ClearRenderTargetView(m_pDepthBuffer->GetRenderTargetView(), fClearColor);
		m_SceneNeedPointer.pContext->ClearDepthStencilView(m_SceneNeedPointer.pBackBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//プレイヤーの影を描画したいためプレイヤーを黒一色で描画する.
		m_pPlayer->SetShader(enSkinModelShader_NoTex);
		m_pPlayer->RenderModel(mView, mProj);

		//敵の影を描画したいためプレイヤーを黒一色で描画する.
		m_pEnemy->SetShader(enSkinModelShader_NoTex);
		m_pEnemy->RenderModel(mView, mProj);

		//レンダーターゲットをもとに戻す.
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &m_SceneNeedPointer.pBackBuffer_RTV, m_SceneNeedPointer.pBackBuffer_DSV);
	}
		break;
	case 1:
	{
		//プレイヤーのシェーダーをそのまま描画するものに変える.
		m_pPlayer->SetShader(enSkinModelShader_Simple);

		//プレイヤーの描画.
		m_pPlayer->RenderModel(mView, mProj);

		//敵のシェーダーをそのまま描画するものに変える.
		m_pEnemy->SetShader(enSkinModelShader_Simple);

		//敵の描画.
		m_pEnemy->RenderModel(mView, mProj);

		//スカイボックスの描画.
		m_pSky->Render(mView, mProj);

		//シェーダーをシャドウマップに変える.
		m_pGround->SetShader(enStaticModelShader_ShadowMap);

		//シャドウマップに必要なポインタを渡す.
		SHADOWMAP_REQUIRED_POINTER ShadowMapPointers;
		ShadowMapPointers.pDepthTexture = m_pDepthBuffer->GetShaderResourceView();
		ShadowMapPointers.mLightView = m_pLightView->GetView();
		ShadowMapPointers.vLightCameraPosition = m_pLightView->GetPos();
		ShadowMapPointers.vCmeraPosition = m_pCamera->GetPos();

		m_pGround->SetShadowMapPointer(ShadowMapPointers);

		//ステージを描画.
		m_pGround->Render(mView, mProj);

		//シャドウマップ以外のシェーダーに戻す.
		m_pGround->SetShader(enStaticModelShader_Simple);

		//弾の描画.
		m_pBulletManager->Render(mView, mProj, vCamPos);

		//エフェクトの描画.
		clsEffects::GetInstance()->Render(mView, mProj, vCamPos);
	}
		break;
	default:
		break;
	}
}

//カメラの操作.
void ActionScene::ControlCameraMove()
{
	//シャドウマップ用の深度テクスチャのカメラ.
	//シャドウマップ用のカメラの注視位置.
	D3DXVECTOR3 vLookAt = { 0.0f, 0.0f, 0.0f };
	m_pLightView->SetLookAt(m_pPlayer->GetPos());

	//シャドウマップ用のカメラの更新.
	m_pLightView->Update();

	D3DXVECTOR2 vMouseMovingDistance = Singleton<RawInput>().GetInstance().GetMouseMovingDistance();

	//マウスの移動速度で視点移動する速さを変える.
	float fCameraMoveSpeed = fabs(CAMERA_MOVE_SPEED * (vMouseMovingDistance.x + vMouseMovingDistance.y));
	m_pCamera->SetRotPerFrame(fCameraMoveSpeed);

	//カメラの移動.
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

	/*====/ カメラの距離 /====*/
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

/*====/ スプライト関連 /====*/
//2Dスプライトの描画.
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
	case MAX_RENDER_LEVEL:
		//シャドウマップ用の深度テクスチャを確認したいときは下のコメントを外す.
		//m_pDisplayDepthBuffer->Render();

		break;
	default:
		break;
	}
}

//スプライトの作成.
void ActionScene::CreateSprite()
{
	//キャパシティを確定させる.
	m_vpSprite.reserve(enSprite_Max);

	SPRITE_STATE SpriteData;

	//各スプライトの設定.
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

		//配列を一つ増やす.
		m_vpSprite.push_back(new Sprite(SpriteData.vDivisionQuantity.x, SpriteData.vDivisionQuantity.y));
		m_vpSprite[i]->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, SpriteData.sPath);
	}

	float f_Scale = 2.5f;

	//シャドウマップの深度テクスチャ用スプライト作成.
	m_pDisplayDepthBuffer = new DisplayBackBuffer(WINDOW_WIDTH / f_Scale, WINDOW_HEIGHT / f_Scale);
	m_pDisplayDepthBuffer->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext);
	m_pDisplayDepthBuffer->SetSRV(m_pDepthBuffer->GetShaderResourceView());

}

//スプライトの解放.
void ActionScene::ReleaseSprite()
{
	for (unsigned int i = 0; i < m_vpSprite.size(); i++)
	{
		SAFE_DELETE(m_vpSprite[i]);
	}
	//使っているサイズを0にする.
	m_vpSprite.clear();
	//キャパシティを現在のサイズにあわせる.
	m_vpSprite.shrink_to_fit();
}

//スプライトのフレームごとの更新.
void ActionScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//スプライトの位置.
void ActionScene::UpdateSpritePositio(int iSpriteNo)
{
	//ウインドウの中心.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;

	//スプライト位置.
	D3DXVECTOR2 vPosition;

	//スプライトの大きさ.
	D3DXVECTOR2 vSize;

	//Hpゲージの大きさ.
	const float fGageScale = 0.2f;

	//枠とゲージの間隔.
	const float fGageSpacing = 25.0f;

	switch (iSpriteNo)
	{
	case enSprite_Alignment:
		vSize = m_vpSprite[iSpriteNo]->GetSize();
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter;

		break;
	case enSprite_PlayerHp:
		vSize = m_vpSprite[iSpriteNo]->GetSize();
		vPosition.x = (m_vpSprite[iSpriteNo]->GetSize().x / 2.0f) + fGageSpacing;
		vPosition.y = (m_vpSprite[iSpriteNo]->GetSize().y / 2.0f) + fGageSpacing;

		break;
	case enSprite_PlayerHpFrame:
		vSize = m_vpSprite[iSpriteNo]->GetSize();
		//プレイヤーのHpゲージの位置にあわせる.
		vPosition.x = m_vpSprite[enSprite_PlayerHp]->GetPos().x;
		vPosition.y = m_vpSprite[enSprite_PlayerHp]->GetPos().y;

		break;
	case enSprite_EnemyHp:
		vSize = m_vpSprite[iSpriteNo]->GetSize();
		vPosition.x = WINDOW_WIDTH - ((m_vpSprite[iSpriteNo]->GetSize().x / 2.0f) + fGageSpacing);
		vPosition.y = (m_vpSprite[iSpriteNo]->GetSize().y / 2.0f) + fGageSpacing;

		break;
	case enSprite_EnemyHpFrame:
		vSize = m_vpSprite[iSpriteNo]->GetSize();
		//エネミーのHpゲージの位置にあわせる.
		vPosition.x = m_vpSprite[enSprite_EnemyHp]->GetPos().x;
		vPosition.y = m_vpSprite[enSprite_EnemyHp]->GetPos().y;

		break;
	default:
		ERR_MSG("Action::UpdateSpritePositio()", "error");

		break;
	}

	m_vpSprite[iSpriteNo]->SetPos(vPosition.x, vPosition.y);

	//シャドウマップの深度テクスチャ用スプライト位置.
	vSize = m_pDisplayDepthBuffer->GetSize();
	m_pDisplayDepthBuffer->SetPos(vSize.x / 2.0f, WINDOW_HEIGHT - vSize.y / 2.0f);
}

//スプライトのアニメーション.
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

//デバッグテキストの表示.
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

//デバッグ中のみの操作.
void ActionScene::DebugKeyControl()
{
}

#endif //#if _DEBUG.