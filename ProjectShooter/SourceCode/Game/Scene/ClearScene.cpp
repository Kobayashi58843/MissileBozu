#include "ClearScene.h"

#include "..\\..\\Singleton.h"

//モデルの初期アニメーション速度.
const double ANIMETION_SPEED = 0.01;

//演出の段階の最大数.
const int MAX_PHASE = 8;

//一フェーズの時間.
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
	//全サウンドを停止する.
	Singleton<SoundManager>().GetInstance().StopSound();

	m_pModelBuff = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	m_pEffect = clsEffects::GetInstance();

	//起動後1回目のエフェクトが再生されないためその対応として1回再生しておく.
	m_MissileHandle = m_pEffect->Play({ 0.0f, 0.0f, 0.0f }, clsEffects::enEfcType_Missile);
	m_pEffect->Stop(m_MissileHandle);
}

ClearScene::~ClearScene()
{
	m_pEffect->Stop(m_MissileHandle);

	Release();
}

//作成.
void ClearScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//スプライトの作成.
	CreateSprite();

	//カメラを作成.
	m_pModelSpriteCamera = new EventCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	//スキンモデルの作成.
	m_pPlayerModel = new EventModel(Singleton<ModelResource>().GetInstance().GetSkinModels(ModelResource::enSkinModel_Player), 0.0005f, ANIMETION_SPEED);
	m_pPlayerModel->ChangeAnimation(1);
}

//解放.
void ClearScene::Release()
{
	SAFE_DELETE(m_pPlayerModel);

	SAFE_DELETE(m_pModelSpriteCamera);

	SAFE_DELETE(m_pModelBuff);
	SAFE_DELETE(m_pModelSprite);

	ReleaseSprite();
}

//更新.
void ClearScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
	//BGMをループで再生.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Clear);

	m_iTime++;

	//スプライト更新.
	UpdateSprite();

	//カメラ更新.
	m_pModelSpriteCamera->Update();

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
void ClearScene::RenderModelProduct(const int iRenderLevel)
{
	D3DXMATRIX mView = m_pModelSpriteCamera->GetView();
	D3DXMATRIX mProj = m_pModelSpriteCamera->GetProjection();

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

		ModelControl(m_pModelSpriteCamera->GetView(), m_pModelSpriteCamera->GetProjection(), m_iPhase);

		//エフェクトの描画.
		clsEffects::GetInstance()->Render(mView, mProj, m_pModelSpriteCamera->GetPos());

		//レンダーターゲットを元に戻す.
		m_SceneNeedPointer.pContext->OMSetRenderTargets(1, &m_SceneNeedPointer.pBackBuffer_RTV, m_SceneNeedPointer.pBackBuffer_DSV);
	}
	break;
	case 1:
		//演出の段階ごとの描画.
		PhaseDrawing(mView, mProj, m_iPhase);

		break;
	case MAX_RENDER_LEVEL:

		break;
	default:
		break;
	}
}

/*====/ スプライト関連 /====*/
//2Dスプライトの描画.
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

//スプライトの作成.
void ClearScene::CreateSprite()
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

		//配列を一つ増やす.
		m_vpSprite.push_back(new Sprite(SpriteData.vDivisionQuantity.x, SpriteData.vDivisionQuantity.y));
		m_vpSprite[i]->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, SpriteData.sPath);
	}

	m_pModelSprite = new DisplayBackBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pModelSprite->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext);
	m_pModelSprite->SetSRV(m_pModelBuff->GetShaderResourceView());
}

//スプライトの解放.
void ClearScene::ReleaseSprite()
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
void ClearScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//スプライトの位置.
void ClearScene::UpdateSpritePositio(int iSpriteNo)
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
	default:
		ERR_MSG("Clear::UpdateSpritePositio()", "error");

		break;
	}

	m_vpSprite[iSpriteNo]->SetPos(vPosition.x, vPosition.y);

	m_pModelSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
}

//スプライトのアニメーション.
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
	default:
		ERR_MSG("Clear::UpdateSpriteAnimation()", "error");

		break;
	}
}

//演出の段階ごとの描画.
void ClearScene::PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
{
	//初期化.
	PhaseInit(iPhase);

	//カメラの操作.
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

	//進行.
	PhaseProgress(iPhase);
}

//演出の段階ごとのカメラ操作.
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

//演出の段階の進行.
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

//演出の各段階の初期化.
void ClearScene::PhaseInit(const int iPhase)
{
	if (!m_bWhenProgress)
	{
		return;
	}

	m_iTime = 0;

	//注視位置をクリア.
	D3DXVECTOR3 vLookAt;
	CrearVECTOR3(vLookAt);

	//カメラの注視位置を設定する.
	m_pModelSpriteCamera->SetLookAt(vLookAt);

	//カメラの位置を設定する.
	m_pModelSpriteCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z - 5.0f });

	//エフェクトを再生.
	if (!m_pEffect->PlayCheck(m_MissileHandle))
	{
		m_MissileHandle = m_pEffect->Play(vLookAt, clsEffects::enEfcType_Missile);
		//エフェクトの大きさ.
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

//デバッグテキストの表示.
void ClearScene::RenderDebugText()
{
	char cStrDbgTxt[512];

	sprintf_s(cStrDbgTxt, "Scene : Clear");
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 0));
}

#endif //#if _DEBUG.