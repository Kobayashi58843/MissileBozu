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

//スプライトの作成.
void WinningScene::CreateSprite()
{
	//キャパシティを確定させる.
	m_vpSprite.reserve(enSprite_Max);

	SPRITE_STATE SpriteData;

	//各スプライトの設定.
	for (int i = 0; i < enSprite_Max; i++)
	{
		switch (i)
		{
		case enSprite_Text:
			SpriteData =
			{ "Data\\Image\\Win.png"	//ファイルまでのパス.
			, { 1.0f, 1.0f } };			//元画像を何分割するか.

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
}

//スプライトの位置.
void WinningScene::UpdateSpritePositio(int iSpriteNo)
{
	//ウインドウの中心.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;

	//スプライト位置.
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

//スプライトのアニメーション.
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

//演出の段階ごとの描画.
void WinningScene::PhaseDrawing(const D3DXMATRIX mView, const D3DXMATRIX mProj, const int iPhase)
{
	//初期化.
	PhaseInit(iPhase);

	//カメラの操作.
	PhaseCameraControl(iPhase);

	D3DXVECTOR3 vEfcPos = m_pEnemyModel->GetPos();
	vEfcPos.y += 1.5f;

	const float fRatio = 0.5f;
	if (m_pEnemyModel->IsAnimationRatioEnd(fRatio))
	{
		//アニメーション速度.
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
			//エフェクトを再生.
			vEfcPos.x -= 1.0f;
			vEfcPos.y -= 1.0f;
			m_ExpHandle = m_pEffect->Play(vEfcPos, clsEffects::enEfcType_Explosion);
			Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_Explosion);
		}

		if (m_iTime / FPS == COUNT_TIME - (COUNT_TIME / 3.0f))
		{
			//エフェクトを再生.
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

				//カメラの注視位置を設定する.
				D3DXVECTOR3 vLookAt;
				vLookAt = m_pPlayerModel->GetPos();
				vLookAt.y += 1.5f;
				m_pEventCamera->SetLookAt(vLookAt);

				m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

				//エフェクトを再生.
				if (!m_pEffect->PlayCheck(m_MissileHandle))
				{
					m_MissileHandle = m_pEffect->Play(m_pPlayerModel->GetPos(), clsEffects::enEfcType_Missile);
					//エフェクトの大きさ.
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

		//アニメーションを設定.
		{
			m_pEnemyModel->ChangeAnimation(4);
			const double dHitAnimationSpeed = 0.005;
			m_pEnemyModel->SetAnimationSpeed(dHitAnimationSpeed);
		}

		//カメラの注視位置を設定する.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//エフェクトを再生.
		m_ExpHandle = m_pEffect->Play(vLookAt, clsEffects::enEfcType_Explosion);
		Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Explosion);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x + 1.0f, vLookAt.y - 1.0f, vLookAt.z - 2.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	case 1:
		//モデルの位置を設定.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, -20.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//アニメーションを設定.
		{
			m_pEnemyModel->ChangeAnimation(4);
			const double dHitAnimationSpeed = 0.005;
			m_pEnemyModel->SetAnimationSpeed(dHitAnimationSpeed);
		}

		//カメラの注視位置を設定する.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//エフェクトを再生.
		m_ExpHandle = m_pEffect->Play(vLookAt, clsEffects::enEfcType_Explosion);
		Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Explosion);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x - 2.0f, vLookAt.y + 4.0f, vLookAt.z - 6.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	case 2:
		//モデルの位置を設定.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, -20.0f });
		m_pPlayerModel->SetRot({ 0.0f, D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//アニメーションを設定.
		{
			m_pEnemyModel->ChangeAnimation(4);
			const double dHitAnimationSpeed = 0.005;
			m_pEnemyModel->SetAnimationSpeed(dHitAnimationSpeed);
		}

		//カメラの注視位置を設定する.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//エフェクトを再生.
		m_ExpHandle = m_pEffect->Play(vLookAt, clsEffects::enEfcType_Explosion);
		Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_Explosion);

		//カメラの位置を設定する.
		m_pEventCamera->SetPos({ vLookAt.x + 2.0f, vLookAt.y + 4.0f, vLookAt.z + 6.0f });

		m_pEventCamera->SetRot({ 0.0f, 0.0f, 0.0f });

		break;
	case 3:
		//モデルの位置を設定.
		m_pPlayerModel->SetPos({ 0.0f, 0.0f, 0.0f });
		m_pPlayerModel->SetRot({ D3DXToRadian(60), D3DXToRadian(180), 0.0f });
		m_pEnemyModel->SetPos({ 0.0f, 0.0f, 20.0f });

		//アニメーションを設定.
		{
			m_pEnemyModel->ChangeAnimation(4);
			const double dHitAnimationSpeed = 0.005;
			m_pEnemyModel->SetAnimationSpeed(dHitAnimationSpeed);
		}

		//カメラの注視位置を設定する.
		vLookAt = m_pEnemyModel->GetPos();
		vLookAt.y += 1.5f;
		m_pEventCamera->SetLookAt(vLookAt);

		//エフェクトを再生.
		m_ExpHandle = m_pEffect->Play(vLookAt, clsEffects::enEfcType_Explosion);
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_Explosion);

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
		m_pEventCamera->SetPos({ vLookAt.x, vLookAt.y, vLookAt.z + 4.0f });

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