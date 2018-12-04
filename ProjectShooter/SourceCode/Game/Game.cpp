#include "Game.h"

#include "Sound\\SoundManager.h"
#include "Effekseer\\CEffects.h"
#include "Model\\ModelResource\\ModelResource.h"

#include "..\\Singleton.h"

//ゲーム開始時のシーン.
const enSwitchToNextScene START_SCENE = enSwitchToNextScene::StartEvent;

//フェードの速度.
const float FADE_SPEED = 0.02f;

Game::Game(const HWND hWnd)
	: m_pScene(nullptr)
	, m_bBlackOut(false)
{
	//Direct3D作成.
	m_pDirect3D = new Direct3D;
	m_pDirect3D->Create(hWnd);

	//作成されたDirect3Dからデバイスなどを取ってくる.
	m_pDevice = m_pDirect3D->GetDevice();
	m_pDeviceContext = m_pDirect3D->GetDeviceContext();

	//シェーダの作成.
	Singleton<ShaderGathering>().GetInstance().InitShader(m_pDevice, m_pDeviceContext);

	//リソースの作成.
	Singleton<ModelResource>().GetInstance().CreateModelAll(hWnd, m_pDevice, m_pDeviceContext);

	//BGMとSEを作成.
	Singleton<SoundManager>().GetInstance().LoadSound(hWnd);

	//エフェクトの作成.
	clsEffects::GetInstance()->Create(m_pDevice, m_pDeviceContext);
	clsEffects::GetInstance()->LoadData();

	//RawInput初期化.
	Singleton<RawInput>().GetInstance().InitRawInput(hWnd);

	//シーンに必要なポインタをセット.
	m_SceneNeedPointer.SetSceneNeedPointer(hWnd, m_pDevice, m_pDeviceContext,
		m_pDirect3D->GetRenderTargetView(), nullptr, m_pDirect3D->GetDepthStencilView());

	SwitchScene(START_SCENE);

	/*====/ フェード用スプライト関連 /====*/
	m_pFadeMaskBuffer = new BackBuffer(m_SceneNeedPointer.pDevice, static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));

	D3DXVECTOR2 vDivisionQuantity = { 1.0f, 1.0f };
	m_pFadeSprite = new TransitionsSprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pFadeSprite->Create(m_pDevice, m_pDeviceContext, "Data\\Image\\Fade.jpg");
	m_pFadeSprite->SetMaskTexture(m_pFadeMaskBuffer->GetShaderResourceView());

	m_pFadeMaskSprite = new Sprite(vDivisionQuantity.x, vDivisionQuantity.y);
	m_pFadeMaskSprite->Create(m_pDevice, m_pDeviceContext, "Data\\Image\\Transitions.png");

	//位置をウインドウの中心に設定.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;
	m_pFadeSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
	m_pFadeMaskSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);

	//はじめは非表示に設定しておく.
	m_pFadeSprite->SetDispFlg(false);

	//透過値を0にする.
	m_pFadeSprite->SetAlpha(0.0f);

	m_NextSceneTemporary = enSwitchToNextScene::Nothing;
}

Game::~Game()
{
	SAFE_DELETE(m_pFadeSprite);

	SAFE_DELETE(m_pFadeMaskSprite);

	SAFE_DELETE(m_pFadeMaskBuffer);

	SAFE_DELETE(m_pScene);

	clsEffects::GetInstance()->Destroy();

	SAFE_DELETE(m_pDirect3D);

	m_pDeviceContext = nullptr;
	m_pDevice = nullptr;
}

//メッセージ毎の処理.
void Game::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		//キーボードが押されたときのキー別の処理.
		switch ((char)wParam)
		{
		case VK_ESCAPE:
			//ウィンドウを破棄する.
			DestroyWindow(hWnd);

			break;
		}
		break;
	}

	Singleton<RawInput>().GetInstance().MsgProc(uMsg, lParam);
}

//更新.
void Game::Update()
{
	enSwitchToNextScene enSwitchNextScene = enSwitchToNextScene::Nothing;

	assert(m_pScene != nullptr);
	m_pScene->Update(enSwitchNextScene);

	//シーンの変更があればフェードさせながらシーンを切り替える.
	Fade(enSwitchNextScene);

	//シーンの描画.
	Render();

	//マウスの入力情報をクリア.
	Singleton<RawInput>().GetInstance().ClearPerFrame();
}

//フェード.
void Game::Fade(const enSwitchToNextScene enNextScene)
{
	//シーンの変更があったとき.
	if (enNextScene != enSwitchToNextScene::Nothing)
	{
		//フェード用の画像が表示可能かどうかでフェード中を判断する.
		//フェード中でない場合.
		if (!m_pFadeSprite->IsDispFlg())
		{
			//フェード用の画像を表示可能にする.
			m_pFadeSprite->SetDispFlg(true);
			m_bBlackOut = true;
			m_NextSceneTemporary = enNextScene;
		}
	}

	if (m_pFadeSprite->IsDispFlg())
	{
		//レンダーターゲットをフェード用画像に使うマスク用バッファに変える.
		ID3D11RenderTargetView* pRTV = m_pFadeMaskBuffer->GetRenderTargetView();
		m_pDeviceContext->OMSetRenderTargets(1, &pRTV, m_pDirect3D->GetDepthStencilView());

		//画面のクリア.
		const float fClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_pDeviceContext->ClearRenderTargetView(m_pFadeMaskBuffer->GetRenderTargetView(), fClearColor);
		m_pDeviceContext->ClearDepthStencilView(m_pDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//ルール画像を描画する.
		m_pFadeMaskSprite->Render();

		//レンダーターゲットを元に戻す.
		pRTV = m_pDirect3D->GetRenderTargetView();
		m_pDeviceContext->OMSetRenderTargets(1, &pRTV, m_pDirect3D->GetDepthStencilView());

		if (m_bBlackOut)
		{
			if (1.0f > m_pFadeSprite->GetAlpha())
			{
				m_pFadeSprite->AddAlpha(FADE_SPEED);
			}
			else
			{
				//透過値が最大になった時にシーンを切り替える.
				SwitchScene(m_NextSceneTemporary);
				m_bBlackOut = false;
			}
		}
		else
		{
			if (0.0f < m_pFadeSprite->GetAlpha())
			{
				m_pFadeSprite->AddAlpha(-FADE_SPEED);
			}
			else
			{
				//フェード用の画像を非表示にする.
				m_pFadeSprite->SetDispFlg(false);
				m_pFadeSprite->SetAlpha(0.0f);
			}
		}
	}
}

//シーン切り替え.
void Game::SwitchScene(const enSwitchToNextScene enNextScene)
{
	//前のシーンを消す.
	SAFE_DELETE(m_pScene);

	//次のシーンに切り替える.
	m_pScene = m_SceneFactory.Create(enNextScene, m_SceneNeedPointer);
	m_pScene->Create(enNextScene);
}

//描画.
void Game::Render()
{
	m_pDirect3D->ClearBackBuffer();

	assert(m_pScene != nullptr);

	for (unsigned int i = 0; i <= MAX_RENDER_LEVEL; i++)
	{
		//レンダーターゲットを元に戻す.
		ID3D11RenderTargetView* pRTV = m_pDirect3D->GetRenderTargetView();
		m_pDeviceContext->OMSetRenderTargets(1, &pRTV, m_pDirect3D->GetDepthStencilView());
		m_pDeviceContext->ClearDepthStencilView(m_pDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//深度テストを有効.
		m_pDirect3D->SetDepth(true);

		//3Dモデルの描画.
		m_pScene->RenderModel(i);

		//深度テストを無効.
		m_pDirect3D->SetDepth(false);

		//2Dスプライトの描画.
		m_pScene->RenderSprite(i);
	}

	//フェード用画像の描画.
	m_pFadeSprite->Render();

	m_pDirect3D->RenderSwapChain();
}