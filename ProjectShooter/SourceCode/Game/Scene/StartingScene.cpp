#include "StartingScene.h"

#include "..\\..\\Singleton.h"

//モデルの初期アニメーション速度.
const double ANIMETION_SPEED = 0.01;

//演出の段階の最大数.
const int MAX_PHASE = 5;

//フェードの速度.
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
	//全サウンドを停止する.
	Singleton<SoundManager>().GetInstance().StopSound();

	m_pOneFrameBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	/*====/ フェード用スプライト関連 /====*/
	m_pFadeMaskBuffer = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	D3DXVECTOR2 vDivisionQuantity = { 1.0f, 1.0f };
	m_pFadeSprite = new TransitionsSprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pFadeSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\White.jpg");
	m_pFadeSprite->SetMaskTexture(m_pFadeMaskBuffer->GetShaderResourceView());

	m_pFadeMaskSprite = new Sprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pFadeMaskSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\Transitions1.png");

	//位置をウインドウの中心に設定.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;
	m_pFadeSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
	m_pFadeMaskSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);

	//はじめは非表示に設定しておく.
	m_pFadeSprite->SetDispFlg(false);

	//透過値を0にする.
	m_pFadeSprite->SetAlpha(0.0f);
}

StartingScene::~StartingScene()
{
	SAFE_DELETE(m_pFadeSprite);

	SAFE_DELETE(m_pFadeMaskSprite);

	SAFE_DELETE(m_pFadeMaskBuffer);

	Release();
}

//作成.
void StartingScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//スプライトの作成.
	CreateSprite();

	//カメラを作成.
	m_pEventCamera = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	//スキンモデルの作成.
	m_pPlayerModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Player), 0.0005f, ANIMETION_SPEED);
	m_pPlayerModel->ChangeAnimation(1);

	m_pEnemyModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Enemy), 0.08f, ANIMETION_SPEED);
	m_pEnemyModel->ChangeAnimation(5);

	m_pSky = Singleton<ModelResource>().GetInstance().GetStaticModels(ModelResource::enStaticModel_SkyBox);
	m_pSky->SetScale(32.0f);
}

//解放.
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

//更新.
void StartingScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
#if _DEBUG

	//デバッグ中のみの操作.
	DebugKeyControl();

#endif //#if _DEBUG.

	//BGMをループで再生.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Title);



	//スプライト更新.
	UpdateSprite();

	//カメラ更新.
	m_pEventCamera->Update();

	//次のシーンへ.
	if (m_iPhase >= MAX_PHASE)
	{
		enNextScene = enSwitchToNextScene::Title;
	}

	if (Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		enNextScene = enSwitchToNextScene::Title;
	}
}

//3Dモデルの描画.
void StartingScene::RenderModelProduct(const int iRenderLevel)
{
	D3DXMATRIX mView = m_pEventCamera->GetView();
	D3DXMATRIX mProj = m_pEventCamera->GetProjection();

	switch (iRenderLevel)
	{
	case 0:
	{
		//レンダーターゲットをセット.
		ID3D11RenderTargetView* pRTV = m_pOneFrameBuff->GetRenderTargetView();
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

		//画面のクリア.
		const float fClearColor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
		m_SceneNeedPointer.pContext->ClearRenderTargetView(m_pOneFrameBuff->GetRenderTargetView(), fClearColor);
		m_SceneNeedPointer.pContext->ClearDepthStencilView(m_SceneNeedPointer.pBackBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//演出の段階ごとの描画.
		PhaseDrawing(mView, mProj, m_iPhase);

		//レンダーターゲットを元に戻す.
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &m_SceneNeedPointer.pBackBuffer_RTV, m_SceneNeedPointer.pBackBuffer_DSV);
	}
		break;
	default:
		break;
	}
}

/*====/ スプライト関連 /====*/
//2Dスプライトの描画.
void StartingScene::RenderSpriteProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 0:
		//3Dモデルの描画側でGBuffer用のものを描画しているので.
		//スプライトでも描画したいものがあるならここで描画する.
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
		//フェード用画像の描画.
		m_pFadeSprite->Render();

		break;
	default:
		break;
	}
}

//スプライトの作成.
void StartingScene::CreateSprite()
{
	//キャパシティを確定させる.
	m_vpSprite.reserve(enSprite_Max);

	SPRITE_STATE SpriteData;

	//各スプライトの設定.
	for (int i = 0; i < enSprite_Max; i++)
	{
		switch (i)
		{
		case enSprite_BackGround:
			SpriteData =
			{ "Data\\Image\\BackGround.jpg"	//ファイルまでのパス.
			, { 1.0f, 1.0f } };				//元画像を何分割するか.

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

		//配列を一つ増やす.
		m_vpSprite.push_back(new Sprite(SpriteData.vDivisionQuantity.x, SpriteData.vDivisionQuantity.y));
		m_vpSprite[i]->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, SpriteData.sPath);
	}

	m_pOneFrameSprite = new DisplayBackBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pOneFrameSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext);
	m_pOneFrameSprite->SetSRV(m_pOneFrameBuff->GetShaderResourceView());
}

//スプライトの解放.
void StartingScene::ReleaseSprite()
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
void StartingScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//スプライトの位置.
void StartingScene::UpdateSpritePositio(int iSpriteNo)
{
	//ウインドウの中心.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;

	//スプライト位置.
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

//スプライトのアニメーション.
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

//演出の段階ごとの描画.
void StartingScene::PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
{
	//初期化.
	PhaseInit(iPhase);

	//カメラの操作.
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

	//進行.
	PhaseProgress(iPhase);
}

//演出の段階ごとのカメラ操作.
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

//演出の段階の進行.
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

//演出の各段階の初期化.
void StartingScene::PhaseInit(const int iPhase)
{
	if (!m_bWhenProgress)
	{
		return;
	}

	//注視位置をクリア.
	D3DXVECTOR3 vLookAt;
	CrearVECTOR3(vLookAt);

	switch (iPhase)
	{
	case 0:
		//カメラの注視位置を設定する.
		vLookAt = m_pPlayerModel->GetPos();
		vLookAt.y += 1.2f;
		m_pEventCamera->SetLookAt(vLookAt);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 0.8f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.8f });

		m_pPlayerModel->SetPos({ 0.0f, 0.0f, 0.0f });

		//名前の表示フラグ.
		m_vpSprite[enSprite_PlayerText]->SetDispFlg(false);
		m_vpSprite[enSprite_EnemyText]->SetDispFlg(false);

		break;
	case 1:
		//カメラの注視位置を設定する.
		vLookAt = m_pPlayerModel->GetPos();
		vLookAt.x += 0.5f;
		vLookAt.y += 1.0f;
		m_pEventCamera->SetLookAt(vLookAt);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 2.5f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		m_pPlayerModel->SetPos({ 0.0f, 0.0f, 0.0f });

		//名前の表示フラグ.
		m_vpSprite[enSprite_PlayerText]->SetDispFlg(true);
		m_vpSprite[enSprite_EnemyText]->SetDispFlg(false);

		break;
	case 2:
		//カメラの注視位置を設定する.
		vLookAt = m_pPlayerModel->GetPos();
		m_pEventCamera->SetLookAt(vLookAt);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y + 1.0f, vLookAt.z - 1.5f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 0.0f });

		//名前の表示フラグ.
		m_vpSprite[enSprite_PlayerText]->SetDispFlg(false);
		m_vpSprite[enSprite_EnemyText]->SetDispFlg(false);

		break;
	case 3:
		//カメラの注視位置を設定する.
		vLookAt = m_pPlayerModel->GetPos();
		vLookAt.x += 1.0f;
		vLookAt.y += 2.0f;
		m_pEventCamera->SetLookAt(vLookAt);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 5.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 0.0f });

		//名前の表示フラグ.
		m_vpSprite[enSprite_PlayerText]->SetDispFlg(false);
		m_vpSprite[enSprite_EnemyText]->SetDispFlg(true);

		break;
	case 4:
		//カメラの注視位置を設定する.
		CrearVECTOR3(vLookAt);
		vLookAt.y -= 4.0f;
		m_pEventCamera->SetLookAt(vLookAt);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x, 0.5f, vLookAt.z - 4.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		//モデルの位置を設定.
		{
			//モデル同士の距離.
			float fGap = 1.0f;
			m_pPlayerModel->SetPos({ -fGap, 0.0f, 0.0f });
			m_pEnemyModel->SetPos({ fGap, 0.0f, 0.0f });
		}

		//名前の表示フラグ.
		m_vpSprite[enSprite_PlayerText]->SetDispFlg(false);
		m_vpSprite[enSprite_EnemyText]->SetDispFlg(false);

		break;
	default:
		break;
	}

	m_bWhenProgress = false;
}

//フェード用のマスクの描画
void StartingScene::RenderFadeMaskBuffer()
{
	//レンダーターゲットをフェード用画像に使うマスク用バッファに変える.
	ID3D11RenderTargetView* pRTV = m_pFadeMaskBuffer->GetRenderTargetView();
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

	//画面のクリア.
	const float fClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_SceneNeedPointer.pContext->ClearRenderTargetView(m_pFadeMaskBuffer->GetRenderTargetView(), fClearColor);
	m_SceneNeedPointer.pContext->ClearDepthStencilView(m_SceneNeedPointer.pBackBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//ルール画像を描画する.
	m_pFadeMaskSprite->Render();

	//レンダーターゲットを元に戻す.
	pRTV = m_SceneNeedPointer.pBackBuffer_RTV;
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);
}

#if _DEBUG

//デバッグテキストの表示.
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

//デバッグ中のみの操作.
void StartingScene::DebugKeyControl()
{
}

#endif //#if _DEBUG.