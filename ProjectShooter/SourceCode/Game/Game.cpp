#include "Game.h"

//スレッド.
#include <thread>

#include "Sound\\SoundManager.h"
#include "Effekseer\\CEffects.h"
#include "Model\\ModelResource\\ModelResource.h"

#include "..\\Singleton.h"

//ゲーム開始時のシーン.
const enSwitchToNextScene START_SCENE = enSwitchToNextScene::Action;

//フェードの速度.
const float FADE_SPEED = 0.025f;

//ロード画面のフェード.
bool LOAD_BLACKOUT_FLG = true;

//ロードの終了時の効果音.
bool LOAD_END_SE_FLG = false;

//ロード画面.
void Load(const HWND hWnd, Direct3D* const pDirect3D, bool* const bEnd)
{
	Sound* m_pRoll = nullptr;
	m_pRoll = new Sound;
	m_pRoll->Open("Data\\Sound\\SE\\System\\Roll.mp3", "Roll", hWnd);

	Sound* m_pRollEnd = nullptr;
	m_pRollEnd = new Sound;
	m_pRollEnd->Open("Data\\Sound\\SE\\System\\RollEnd.mp3", "RollEnd", hWnd);

	BackBuffer* pFadeMaskBuffer = nullptr;
	pFadeMaskBuffer = new BackBuffer(pDirect3D->GetDevice(), static_cast<UINT>(WINDOW_WIDTH), static_cast<UINT>(WINDOW_HEIGHT));
	TransitionsSprite* pFadeSprite = nullptr;

	D3DXVECTOR2 vDivisionQuantity = { 1.0f, 1.0f };
	pFadeSprite = new TransitionsSprite(vDivisionQuantity.x, vDivisionQuantity.y);
	pFadeSprite->Create(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext(), "Data\\Image\\LoadText.png");
	pFadeSprite->SetMaskTexture(pFadeMaskBuffer->GetShaderResourceView());

	Sprite* pFadeMaskSprite = nullptr;
	pFadeMaskSprite = new Sprite(vDivisionQuantity.x, vDivisionQuantity.y);
	pFadeMaskSprite->Create(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext(), "Data\\Image\\LoadMask.png");

	Sprite* pBackSprite = nullptr;
	pBackSprite = new Sprite(vDivisionQuantity.x, vDivisionQuantity.y);
	pBackSprite->Create(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext(), "Data\\Image\\BackGround2.jpg");

	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;
	pFadeSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
	pFadeMaskSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);
	pBackSprite->SetPos(fWindowWidthCenter, fWindowHeightCenter);

	pFadeSprite->SetDispFlg(true);
	pFadeSprite->SetAlpha(1.0f);

	LOAD_BLACKOUT_FLG = true;

	float fClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	//フレームレート調節準備.
	float fRate = 0.0f;	//レート.
	DWORD sync_old = timeGetTime();//過去時間.
	DWORD sync_now;					//現在時間.

	//時間処理の為、最小単位を1ミリ秒に変更.
	timeBeginPeriod(1);

	while (1)
	{
		Sleep(1);
		//現在時間を取得.
		sync_now = timeGetTime();

		//理想時間を算出.
		fRate = 1000.0f / FPS;
		if (sync_now - sync_old >= fRate)
		{
			//現在時間に置き換え.
			sync_old = sync_now;

			//データのロード終了時スレッドを抜ける.
			if (*bEnd)
			{
				if (m_pRollEnd->IsStopped() && !LOAD_END_SE_FLG)
				{
					m_pRoll->Stop();

					m_pRollEnd->SeekToStart();
					m_pRollEnd->Play();

					LOAD_END_SE_FLG = true;
				}

				if (m_pRollEnd->IsStopped() && LOAD_END_SE_FLG)
				{
					SAFE_DELETE(pBackSprite);
					SAFE_DELETE(pFadeSprite);
					SAFE_DELETE(pFadeMaskSprite);
					SAFE_DELETE(pFadeMaskBuffer);

					m_pRollEnd->Stop();
					m_pRollEnd->Close();
					SAFE_DELETE(m_pRollEnd);

					m_pRoll->Stop();
					m_pRoll->Close();
					SAFE_DELETE(m_pRoll);

					return;
				}
			}
			else
			{
				if (m_pRoll->IsStopped())
				{
					m_pRoll->SeekToStart();
					m_pRoll->Play();
				}

				pDirect3D->SetDepth(false);

				//レンダーターゲットをフェード用画像に使うマスク用バッファに変える.
				ID3D11RenderTargetView* pRTV = pFadeMaskBuffer->GetRenderTargetView();
				pDirect3D->GetDeviceContext()->OMSetRenderTargets(1, &pRTV, pDirect3D->GetDepthStencilView());

				//画面のクリア.
				pDirect3D->GetDeviceContext()->ClearRenderTargetView(pFadeMaskBuffer->GetRenderTargetView(), fClearColor);
				pDirect3D->GetDeviceContext()->ClearDepthStencilView(pDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

				pFadeMaskSprite->Render();

				//レンダーターゲットを元に戻す.
				pRTV = pDirect3D->GetRenderTargetView();
				pDirect3D->GetDeviceContext()->OMSetRenderTargets(1, &pRTV, pDirect3D->GetDepthStencilView());

				if (LOAD_BLACKOUT_FLG)
				{
					if (1.0f > pFadeSprite->GetAlpha())
					{
						pFadeSprite->AddAlpha(FADE_SPEED);
					}
					else
					{
						pFadeSprite->SetAlpha(1.0f);
						LOAD_BLACKOUT_FLG = false;
					}
				}
				else
				{
					if (0.0f < pFadeSprite->GetAlpha())
					{
						pFadeSprite->AddAlpha(-FADE_SPEED);
					}
					else
					{
						pFadeSprite->SetAlpha(0.0f);
						LOAD_BLACKOUT_FLG = true;
					}
				}

				//画面のクリア.
				pDirect3D->GetDeviceContext()->ClearRenderTargetView(pDirect3D->GetRenderTargetView(), fClearColor);
				pDirect3D->GetDeviceContext()->ClearDepthStencilView(pDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

				pBackSprite->Render();

				pFadeSprite->Render();

				pDirect3D->RenderSwapChain();
			}
		}
	}
}

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

	//別スレッド.
	bool bReadEnd = false;
	std::thread LoadThread(Load, hWnd, m_pDirect3D, &bReadEnd);

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
	m_pFadeMaskSprite->Create(m_pDevice, m_pDeviceContext, "Data\\Image\\Transitions3.png");

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

	//ロード終了.
	bReadEnd = true;
	LoadThread.join();
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