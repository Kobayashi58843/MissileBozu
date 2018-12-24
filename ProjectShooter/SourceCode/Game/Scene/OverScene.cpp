#include "OverScene.h"

#include "..\\..\\Singleton.h"

//モデルの初期アニメーション速度.
const double ANIMETION_SPEED = 0.01;

//演出の段階の最大数.
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
	//全サウンドを停止する.
	Singleton<SoundManager>().GetInstance().StopSound();

	m_pOneFrameBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	/*====/ フェード用スプライト関連 /====*/
	m_pLightMaskBuffer = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	D3DXVECTOR2 vDivisionQuantity = { 1.0f, 1.0f };
	m_pLightSprite = new TransitionsSprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pLightSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\White.jpg");
	m_pLightSprite->SetMaskTexture(m_pLightMaskBuffer->GetShaderResourceView());

	m_pLightMaskSprite = new Sprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pLightMaskSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, "Data\\Image\\OverSceneLight.png");

	//位置をウインドウの中心に設定.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;
	m_pLightSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
	m_pLightMaskSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);

	//はじめは非表示に設定しておく.
	m_pLightSprite->SetDispFlg(true);

	//透過値を0にする.
	m_pLightSprite->SetAlpha(0.25f);
}

OverScene::~OverScene()
{
	SAFE_DELETE(m_pLightSprite);

	SAFE_DELETE(m_pLightMaskSprite);

	SAFE_DELETE(m_pLightMaskBuffer);

	Release();
}

//作成.
void OverScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//スプライトの作成.
	CreateSprite();

	//カメラを作成.
	m_pEventCamera = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	//スキンモデルの作成.
	m_pPlayerModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Player), 0.0005f, ANIMETION_SPEED);
	m_pPlayerModel->ChangeAnimation(1);
}

//解放.
void OverScene::Release()
{
	SAFE_DELETE(m_pPlayerModel);

	SAFE_DELETE(m_pEventCamera);

	SAFE_DELETE(m_pOneFrameBuff);
	SAFE_DELETE(m_pOneFrameSprite);

	ReleaseSprite();
}

//更新.
void OverScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
	//BGMをループで再生.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Over);

	//スプライト更新.
	UpdateSprite();

	//カメラ更新.
	m_pEventCamera->Update();

	//左クリックされた時.
	if (Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		//カーソルがボタンの上にあるか.
		if (IsHittingOfSprite(enSprite_Cursor, enSprite_ReturnButton))
		{
			enNextScene = enSwitchToNextScene::Title;

			//シーン移動時のSE.
			if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_PushButton))
			{
				//SEを再生.
				Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_PushButton);
			}
		}
	}
}

//3Dモデルの描画.
void OverScene::RenderModelProduct(const int iRenderLevel)
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

//スプライトの作成.
void OverScene::CreateSprite()
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
			{ "Data\\Image\\Fade.jpg"	//ファイルまでのパス.
			, { 1.0f, 1.0f } };				//元画像を何分割するか.

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

		//配列を一つ増やす.
		m_vpSprite.push_back(new Sprite(SpriteData.vDivisionQuantity.x, SpriteData.vDivisionQuantity.y));
		m_vpSprite[i]->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, SpriteData.sPath);
	}

	m_pOneFrameSprite = new DisplayBackBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pOneFrameSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext);
	m_pOneFrameSprite->SetSRV(m_pOneFrameBuff->GetShaderResourceView());
}

//スプライトの解放.
void OverScene::ReleaseSprite()
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
void OverScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//スプライトの位置.
void OverScene::UpdateSpritePositio(int iSpriteNo)
{
	//ウインドウの中心.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;

	//スプライト位置.
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
		//マウスの座標.
		POINT MousePosition;
		GetCursorPos(&MousePosition);
		D3DXVECTOR2 vMousePos = { static_cast<float>(MousePosition.x), static_cast<float>(MousePosition.y) };

		//ウィンドウの座標.
		//ウィンドウ左上端の座標 : ( left , top).
		//ウィンドウ右下端の座標 : ( right, bottom).
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

//スプライトのアニメーション.
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
		//カーソルとボタンが接触していた時.
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

//演出の段階ごとの描画.
void OverScene::PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
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
	default:
		break;
	}

	//進行.
	PhaseProgress(iPhase);
}

//演出の段階ごとのカメラ操作.
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

//演出の段階の進行.
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

//演出の各段階の初期化.
void OverScene::PhaseInit(const int iPhase)
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
		//モデルの位置を設定.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, 0.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(135), D3DXToRadian(90) });

		//アニメーションを設定.
		{
			double dHitAnimationSpeed = 0;

			m_pPlayerModel->ChangeAnimation(1);
			m_pPlayerModel->SetAnimationSpeed(dHitAnimationSpeed);
		}

		//カメラの注視位置を設定する.
		vLookAt = m_pPlayerModel->GetPos();
		vLookAt.x -= 0.8f;
		vLookAt.y += 0.2f;
		m_pEventCamera->SetLookAt(vLookAt);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 3.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	default:
		break;
	}

	m_bWhenProgress = false;
}


//スプライトのマスクの描画.
void OverScene::RenderLightMaskBuffer()
{
	//レンダーターゲットをフェード用画像に使うマスク用バッファに変える.
	ID3D11RenderTargetView* pRTV = m_pLightMaskBuffer->GetRenderTargetView();
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);

	//画面のクリア.
	const float fClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_SceneNeedPointer.pContext->ClearRenderTargetView(m_pLightMaskBuffer->GetRenderTargetView(), fClearColor);
	m_SceneNeedPointer.pContext->ClearDepthStencilView(m_SceneNeedPointer.pBackBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//ルール画像を描画する.
	m_pLightMaskSprite->Render();

	//レンダーターゲットを元に戻す.
	pRTV = m_SceneNeedPointer.pBackBuffer_RTV;
	m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &pRTV, m_SceneNeedPointer.pBackBuffer_DSV);
}

#if _DEBUG

//デバッグテキストの表示.
void OverScene::RenderDebugText()
{
	char cStrDbgTxt[512];

	sprintf_s(cStrDbgTxt, "Scene : Over");
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 0));
}

#endif //#if _DEBUG.