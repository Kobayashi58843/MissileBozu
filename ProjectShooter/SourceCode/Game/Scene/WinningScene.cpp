#include "WinningScene.h"

#include "..\\..\\Singleton.h"

//モデルの初期アニメーション速度.
const double ANIMETION_SPEED = 0.01;

//演出の段階の最大数.
const int MAX_PHASE = 5;

//フェードの速度.
const float FADE_SPEED = 0.05f;

//一フェーズの時間.
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
	//全サウンドを停止する.
	Singleton<SoundManager>().GetInstance().StopSound();

	m_pOneFrameBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	m_pEffect = clsEffects::GetInstance();

	//起動後1回目のエフェクトが再生されないためその対応として1回再生しておく.
	m_ExpHandle = m_pEffect->Play({ 0.0f, 0.0f, 0.0f }, clsEffects::enEfcType_Explosion);
	m_pEffect->Stop(m_ExpHandle);
}

WinningScene::~WinningScene()
{
	Release();
}

//作成.
void WinningScene::CreateProduct(const enSwitchToNextScene enNextScene)
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
	m_pSky->SetPos({ 0.0f, 0.0f, 0.0f });
	m_pSky->SetScale(32.0f);

	m_pStage = Singleton<ModelResource>().GetInstance().GetStaticModels(ModelResource::enStaticModel_Ground);
	m_pStage->SetPos({ 0.0f, -1.5f, 0.0f });
	m_pStage->SetScale(30.0f);
}

//解放.
void WinningScene::Release()
{
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

//更新.
void WinningScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
#if _DEBUG

	//デバッグ中のみの操作.
	DebugKeyControl();

#endif //#if _DEBUG.

	//BGMをループで再生.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Win);

	m_iTime++;

	//スプライト更新.
	UpdateSprite();

	//カメラ更新.
	m_pEventCamera->Update();

	//次のシーンへ.
	if (m_iPhase >= MAX_PHASE)
	{
		enNextScene = enSwitchToNextScene::Clear;
	}

	if (Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		enNextScene = enSwitchToNextScene::Clear;

		//シーン移動時のSE.
		if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_PushButton))
		{
			//SEを再生.
			Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_PushButton);
		}
	}
}

//3Dモデルの描画.
void WinningScene::RenderModelProduct(const int iRenderLevel)
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

		//エフェクトの描画.
		clsEffects::GetInstance()->Render(mView, mProj, m_pEventCamera->GetPos());

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
void WinningScene::RenderSpriteProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 0:
		break;
	case 1:
		m_pOneFrameSprite->Render();

		break;
	case MAX_RENDER_LEVEL:
		break;
	default:
		break;
	}
}

//スプライトの作成.
void WinningScene::CreateSprite()
{
	//キャパシティを確定させる.
	m_vpSprite.reserve(enSprite_Max);

	m_pOneFrameSprite = new DisplayBackBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pOneFrameSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext);
	m_pOneFrameSprite->SetSRV(m_pOneFrameBuff->GetShaderResourceView());
}

//スプライトの解放.
void WinningScene::ReleaseSprite()
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
void WinningScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}

	//ウインドウの中心.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;

	m_pOneFrameSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
}

//スプライトの位置.
void WinningScene::UpdateSpritePositio(int iSpriteNo)
{
}

//スプライトのアニメーション.
void WinningScene::UpdateSpriteAnimation(int iSpriteNo)
{
}

//演出の段階ごとの描画.
void WinningScene::PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
{
	//初期化.
	PhaseInit(iPhase);

	//カメラの操作.
	PhaseCameraControl(iPhase);

	switch (iPhase)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		m_pPlayerModel->RenderModel(mView, mProj);
		m_pEnemyModel->RenderModel(mView, mProj);

		m_pSky->Render(mView, mProj);
		m_pStage->Render(mView, mProj);

		break;
	default:
		break;
	}

	//進行.
	PhaseProgress(iPhase);
}

//演出の段階ごとのカメラ操作.
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

//演出の段階の進行.
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
		if (m_iTime / FPS >= COUNT_TIME)
		{
			m_iPhase++;
			m_bWhenProgress = true;
		}

		break;
	default:
		break;
	}
}

//演出の各段階の初期化.
void WinningScene::PhaseInit(const int iPhase)
{
	if (!m_bWhenProgress)
	{
		return;
	}

	//注視位置をクリア.
	D3DXVECTOR3 vLookAt;
	CrearVECTOR3(vLookAt);

	m_iTime = 0;

	switch (iPhase)
	{
	case 0:
		//モデルの位置を設定.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, -20.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//カメラの注視位置を設定する.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//エフェクトを再生.
		m_ExpHandle = m_pEffect->Play(vLookAt, clsEffects::enEfcType_Explosion);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x + 1.0f, vLookAt.y - 1.0f, vLookAt.z - 2.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	case 1:
		//モデルの位置を設定.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, -20.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//カメラの注視位置を設定する.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x - 2.0f, vLookAt.y + 4.0f, vLookAt.z - 6.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	case 2:
		//モデルの位置を設定.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, -20.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//カメラの注視位置を設定する.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x + 2.0f, vLookAt.y + 4.0f, vLookAt.z + 6.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	case 3:
		//モデルの位置を設定.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, 0.0f });
		m_pPlayerModel->SetRot({ D3DXToRadian(60), D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//カメラの注視位置を設定する.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x - 1.0f, vLookAt.y + 1.0f, vLookAt.z - 8.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	case 4:
		//モデルの位置を設定.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, -20.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//カメラの注視位置を設定する.
		vLookAt = m_pPlayerModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x + 1.0f, vLookAt.y - 1.0f, vLookAt.z + 4.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	default:
		break;
	}

	m_bWhenProgress = false;
}

#if _DEBUG

//デバッグテキストの表示.
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

//デバッグ中のみの操作.
void WinningScene::DebugKeyControl()
{
}

#endif //#if _DEBUG.