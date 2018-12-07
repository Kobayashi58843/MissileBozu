#include "StartEventScene.h"

#include "..\\..\\Singleton.h"

//モデルの初期アニメーション速度.
const double ANIMETION_SPEED = 0.01;

//演出の段階の最大数.
const int MAX_PHASE = 4;

//フェードの速度.
const float FADE_SPEED = 0.04f;

//一フェーズの時間.
const float COUNT_TIME = 1.0f;

StartEventScene::StartEventScene(SCENE_NEED_POINTER PointerGroup)
	: BaseScene(PointerGroup)
	, m_iTime(0)
	, m_pModelSprite(nullptr)
	, m_pModelBuff(nullptr)
	, m_pEventCamera(nullptr)
	, m_pModelSpriteCamera(nullptr)
	, m_pPlayerModel(nullptr)
	, m_pSky(nullptr)
	, m_iPhase(0)
	, m_bWhenProgress(true)
	, m_fAddY(0.0f)
{
	//全サウンドを停止する.
	Singleton<SoundManager>().GetInstance().StopSound();

	m_pModelBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	/*====/ カウントダウン用スプライト関連 /====*/
	m_pCountDownMaskBuffer = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	D3DXVECTOR2 vDivisionQuantity = { 1.0f, 4.0f };
	m_pCountDownSprite = new TransitionsSprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pCountDownSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\CountDown.png");
	m_pCountDownSprite->SetMaskTexture(m_pCountDownMaskBuffer->GetShaderResourceView());

	vDivisionQuantity = { 1.0f, 1.0f };
	m_pCountDownMaskSprite = new Sprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pCountDownMaskSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\Transitions2.png");
	m_pCountDownMaskSprite->SetScale(1.0f);

	//位置をウインドウの中心に設定.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;
	m_pCountDownSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
	m_pCountDownMaskSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);

	//はじめは非表示に設定しておく.
	m_pCountDownSprite->SetDispFlg(false);

	//透過値を0にする.
	m_pCountDownSprite->SetAlpha(0.0f);
}

StartEventScene::~StartEventScene()
{
	SAFE_DELETE(m_pCountDownSprite);

	SAFE_DELETE(m_pCountDownMaskSprite);

	SAFE_DELETE(m_pCountDownMaskBuffer);

	Release();
}

//作成.
void StartEventScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//スプライトの作成.
	CreateSprite();

	//カメラを作成.
	m_pEventCamera = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	m_pModelSpriteCamera = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	m_pSky = Singleton<ModelResource>().GetInstance().GetStaticModels(ModelResource::enStaticModel_SkyBox);
	m_pSky->SetScale(32.0f);

	//スキンモデルの作成.
	m_pPlayerModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Player), 0.0005f, ANIMETION_SPEED);
	m_pPlayerModel->ChangeAnimation(1);
}

//解放.
void StartEventScene::Release()
{
	SAFE_DELETE(m_pPlayerModel);

	SAFE_DELETE(m_pModelSpriteCamera);
	SAFE_DELETE(m_pEventCamera);

	SAFE_DELETE(m_pModelBuff);
	SAFE_DELETE(m_pModelSprite);

	m_pSky = nullptr;

	ReleaseSprite();
}

//更新.
void StartEventScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
#if _DEBUG

	//デバッグ中のみの操作.
	DebugKeyControl();

#endif //#if _DEBUG.

	//BGMをループで再生.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_StartEvent);

	m_iTime++;

	//スプライト更新.
	UpdateSprite();

	//カメラ更新.
	m_pEventCamera->Update();
	m_pModelSpriteCamera->Update();

	//次のシーンへ.
	if (m_iPhase >= MAX_PHASE)
	{
		enNextScene = enSwitchToNextScene::Action;
	}
}

//3Dモデルの描画.
void StartEventScene::RenderModelProduct(const int iRenderLevel)
{
	D3DXMATRIX mView = m_pEventCamera->GetView();
	D3DXMATRIX mProj = m_pEventCamera->GetProjection();

	switch (iRenderLevel)
	{
	case 0:
	{
		//レンダーターゲットをセット.
		ID3D11RenderTargetView* pRTV = m_pModelBuff->GetRenderTargetView();
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

		//画面のクリア.
		const float fClearColor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
		m_SceneNeedPointer.pContext->ClearRenderTargetView(m_pModelBuff->GetRenderTargetView(), fClearColor);
		m_SceneNeedPointer.pContext->ClearDepthStencilView(m_SceneNeedPointer.pBackBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ModelControl(m_pModelSpriteCamera->GetView(), m_pModelSpriteCamera->GetProjection(),m_iPhase);

		//レンダーターゲットを元に戻す.
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &m_SceneNeedPointer.pBackBuffer_RTV, m_SceneNeedPointer.pBackBuffer_DSV);
	}
	break;
	case 1:
		//演出の段階ごとの描画.
		PhaseDrawing(mView, mProj, m_iPhase);

		m_pSky->Render(mView, mProj);

		break;
	case MAX_RENDER_LEVEL:

		break;
	default:
		break;
	}
}

/*====/ スプライト関連 /====*/
//2Dスプライトの描画.
void StartEventScene::RenderSpriteProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 0:
		m_vpSprite[enSprite_BackGround]->Render();

		break;
	case 1:
		RenderCountDownMaskBuffer();
		m_pCountDownSprite->Render();

		break;
	case MAX_RENDER_LEVEL:
		m_pModelSprite->Render();

		break;
	default:
		break;
	}
}

//スプライトの作成.
void StartEventScene::CreateSprite()
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
		default:
			ERR_MSG("Clear::CreateSprite()", "error");

			break;
		}

		//配列を一つ増やす.
		m_vpSprite.push_back(new Sprite(SpriteData.vDivisionQuantity.x, SpriteData.vDivisionQuantity.y));
		m_vpSprite[i]->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, SpriteData.sPath);
	}

	m_pModelSprite = new DisplayBackBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pModelSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext);
	m_pModelSprite->SetSRV(m_pModelBuff->GetShaderResourceView());
}

//スプライトの解放.
void StartEventScene::ReleaseSprite()
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
void StartEventScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//スプライトの位置.
void StartEventScene::UpdateSpritePositio(int iSpriteNo)
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
	default:
		ERR_MSG("Clear::UpdateSpritePositio()", "error");

		break;
	}

	m_vpSprite[iSpriteNo]->SetPos(vPosition.x, vPosition.y);

	m_pModelSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
}

//スプライトのアニメーション.
void StartEventScene::UpdateSpriteAnimation(int iSpriteNo)
{
	switch (iSpriteNo)
	{
	case enSprite_BackGround:

		break;
	default:
		ERR_MSG("Clear::UpdateSpriteAnimation()", "error");

		break;
	}
}

//演出の段階ごとの描画.
void StartEventScene::PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
{
	//初期化.
	PhaseInit(iPhase);

	//カメラの操作.
	PhaseCameraControl(iPhase);

	switch (iPhase)
	{
	case 0:
		if (m_iTime / FPS <= COUNT_TIME / 2.0f)
		{
			if (m_pCountDownSprite->FadeOut(FADE_SPEED))
			{
				if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_Count))
				{
					Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Count);
				}
			}
		}
		else
		{
			m_pCountDownSprite->FadeIn(FADE_SPEED);
		}

		break;
	case 1:
		if (m_iTime / FPS <= COUNT_TIME / 2.0f)
		{
			if (m_pCountDownSprite->FadeOut(FADE_SPEED))
			{
				if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_Count))
				{
					Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Count);
				}
			}
		}
		else
		{
			m_pCountDownSprite->FadeIn(FADE_SPEED);
		}

		break;
	case 2:
		if (m_iTime / FPS <= COUNT_TIME / 2.0f)
		{
			if (m_pCountDownSprite->FadeOut(FADE_SPEED))
			{
				if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_Count))
				{
					Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Count);
				}
			}
		}
		else
		{
			m_pCountDownSprite->FadeIn(FADE_SPEED);
		}

		break;
	case 3:
		if (m_iTime / FPS <= COUNT_TIME / 2.0f)
		{
			if (m_pCountDownSprite->FadeOut(FADE_SPEED))
			{
				if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_Fire))
				{
					Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Fire);
				}

			}
		}
		else
		{
			m_pCountDownSprite->FadeIn(FADE_SPEED);
		}

		break;
	default:
		break;
	}

	//進行.
	PhaseProgress(iPhase);
}

//演出の段階ごとのカメラ操作.
void StartEventScene::PhaseCameraControl(const int iPhase)
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
	default:
		break;
	}
}

//演出の段階の進行.
void StartEventScene::PhaseProgress(const int iPhase)
{
	switch (iPhase)
	{
	case 0:
		if (m_iTime / FPS >= COUNT_TIME)
		{
			m_iPhase++;
			m_bWhenProgress = true;

			m_pCountDownSprite->AddPatternHeight(1.0f);
		}

		break;
	case 1:
		if (m_iTime / FPS >= COUNT_TIME)
		{
			m_iPhase++;
			m_bWhenProgress = true;

			m_pCountDownSprite->AddPatternHeight(1.0f);
		}

		break;
	case 2:
		if (m_iTime / FPS >= COUNT_TIME)
		{
			m_iPhase++;
			m_bWhenProgress = true;

			m_pCountDownSprite->AddPatternHeight(1.0f);
		}

		break;
	case 3:
		if (m_iTime / FPS >= COUNT_TIME)
		{
			m_iPhase++;
			m_bWhenProgress = true;

			//非表示にする.
			m_pCountDownSprite->SetDispFlg(false);
		}

		break;
	default:
		break;
	}
}

//演出の各段階の初期化.
void StartEventScene::PhaseInit(const int iPhase)
{
	if (!m_bWhenProgress)
	{
		return;
	}

	//注視位置をクリア.
	D3DXVECTOR3 vLookAt;
	CrearVECTOR3(vLookAt);

	//カメラの注視位置を設定する.
	vLookAt.y += 5.0f;
	m_pEventCamera->SetLookAt(vLookAt);

	//カメラの位置を設定する.
	m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 1.0f });

	m_iTime = 0;

	switch (iPhase)
	{
	case 0:
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(-45) });
		m_pPlayerModel->SetPos({ -4.0f, -4.0f, 0.0f });

		break;
	case 1:
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(180) });
		m_pPlayerModel->SetPos({ 0.0f, 4.0f, 0.0f });

		break;
	case 2:
		m_pPlayerModel->SetRot({ 0.0f, 0.0f, D3DXToRadian(45) });
		m_pPlayerModel->SetPos({ 4.0f, -4.0f, 0.0f });

		break;
	case 3:
		m_pPlayerModel->SetRot({ D3DXToRadian(60), D3DXToRadian(180), 0.0f });
		m_pPlayerModel->SetPos({ 0.0f, -1.0f, -8.0f });

		break;
	default:
		break;
	}

	m_bWhenProgress = false;
}

void StartEventScene::ModelControl(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
{
	//注視位置をクリア.
	D3DXVECTOR3 vLookAt;
	CrearVECTOR3(vLookAt);

	//カメラの注視位置を設定する.
	m_pModelSpriteCamera->SetLookAt(vLookAt);

	//カメラの位置を設定する.
	m_pModelSpriteCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 2.5f });

	float fMoveSpeed = 0.08f;
	switch (iPhase)
	{
	case 0:
		m_pPlayerModel->AddPos({ fMoveSpeed, fMoveSpeed, 0.0f });

		m_pPlayerModel->RenderModel(mView, mProj);

		break;
	case 1:
		m_pPlayerModel->AddPos({ 0.0f, -fMoveSpeed, 0.0f });

		m_pPlayerModel->RenderModel(mView, mProj);

		break;
	case 2:
		m_pPlayerModel->AddPos({ -fMoveSpeed, fMoveSpeed, 0.0f });

		m_pPlayerModel->RenderModel(mView, mProj);

		break;
	case 3:
		m_fAddY += 0.0008f;
		m_pPlayerModel->AddPos({ 0.0f, m_fAddY, fMoveSpeed * 4.0f });

		m_pPlayerModel->RenderModel(mView, mProj);

		//このフェーズ以降もキャラクターを表示したいのでbreakを消してます.
	default:
		break;
	}
}

//カウントダウン用のマスクの描画.
void StartEventScene::RenderCountDownMaskBuffer()
{
	//レンダーターゲットをフェード用画像に使うマスク用バッファに変える.
	ID3D11RenderTargetView* pRTV = m_pCountDownMaskBuffer->GetRenderTargetView();
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

	//画面のクリア.
	const float fClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_SceneNeedPointer.pContext->ClearRenderTargetView(m_pCountDownMaskBuffer->GetRenderTargetView(), fClearColor);
	m_SceneNeedPointer.pContext->ClearDepthStencilView(m_SceneNeedPointer.pBackBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//ルール画像を描画する.
	m_pCountDownMaskSprite->Render();

	//レンダーターゲットを元に戻す.
	pRTV = m_SceneNeedPointer.pBackBuffer_RTV;
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);
}

#if _DEBUG

//デバッグテキストの表示.
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

//デバッグ中のみの操作.
void StartEventScene::DebugKeyControl()
{
}

#endif //#if _DEBUG.