#include "Direct3D.h"

Direct3D::Direct3D()
	: m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
	, m_pSwapChain(nullptr)
	, m_pBackBuffer_RTV(nullptr)
	, m_pBackBuffer_Tex(nullptr)
	, m_pBackBuffer_DSV(nullptr)
	, m_pDepthStencilState(nullptr)
{
}

Direct3D::~Direct3D()
{
	Release();
}

//Direct3D初期化.
void Direct3D::Create(HWND hWnd)
{
	/*====/ デバイスとスワップチェーン関係 /====*/

	//スワップチェーン構造体.
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;	//バックバッファの数.
	sd.BufferDesc.Width = (UINT)WINDOW_WIDTH;//バックバッファの幅.
	sd.BufferDesc.Height = (UINT)WINDOW_HEIGHT;//バックバッファの高さ.
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//フォーマット(32ビットカラー)
	sd.BufferDesc.RefreshRate.Numerator = 60;
	//リフレッシュレート(分母) ※FPS:60
	sd.BufferDesc.RefreshRate.Denominator = 1;
	//リフレッシュレート(分子)
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//使い方(表示先)
	sd.OutputWindow = hWnd;	//ウィンドウハンドル.
	sd.SampleDesc.Count = 1;	//マルチサンプルの数.
	sd.SampleDesc.Quality = 0;	//マルチサンプルのクオリティ.
	sd.Windowed = TRUE;	//ウィンドウモード(フルスク時はFALSE)

	//作成を試みる機能レベルの優先を指定.
	//	(GPUがサポートする機能セットの定義)
	//	D3D_FEATURE_LEVEL列挙体の配列.
	//	D3D_FEATURE_LEVEL_10_1:Direct3D 10.1のGPUレベル.
	D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL* pFeatureLevel = NULL;//配列の要素数.

	//デバイスとスワップチェーンの作成.
	//	ハードウェア(GPU)デバイスで作成.
	if (FAILED(D3D11CreateDeviceAndSwapChain(
		NULL,			//ビデオアダプタへのポインタ.
		D3D_DRIVER_TYPE_HARDWARE,//作成するデバイスの種類.
		NULL,			//ソフトウェア ラスタライザーを実装するDLLのハンドル.
		0,				//有効にするランタイムレイヤー.
		&pFeatureLevels,//作成を試みる機能レベルの順序を指定する配列へのポインタ.
		1,				//↑の要素数.
		D3D11_SDK_VERSION,//SDKのバージョン.
		&sd,			//スワップチェーンの初期化パラメータのポインタ.
		&m_pSwapChain,	//(out)レンダリングに使用するスワップチェーン.
		&m_pDevice,		//(out)作成されたデバイス.
		pFeatureLevel,	//機能レベルの配列にある最初の要素を表すポインタ.
		&m_pDeviceContext)))//(out)デバイス コンテキスト.
	{
		//WARPデバイスの作成.
		//	D3D_FEATURE_LEVEL_9_1 ～ D3D_FEATURE_LEVEL_10_1
		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP,NULL, 0, &pFeatureLevels,
			1,D3D11_SDK_VERSION, &sd, &m_pSwapChain,&m_pDevice, pFeatureLevel,&m_pDeviceContext)))
		{
			//リファレンスデバイスの作成.
			//	DirectX SDKがインストールされていないと使えない.
			if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_REFERENCE,NULL, 0, &pFeatureLevels,
				1,D3D11_SDK_VERSION, &sd, &m_pSwapChain,&m_pDevice, pFeatureLevel,&m_pDeviceContext)))
			{
				MessageBox(NULL,"デバイスとスワップチェーンの作成に失敗","error(main.cpp)", MB_OK);
			}
		}
	}

	//各種テクスチャ-と、それに付帯する各種ビュー(画面)を作成.

	//-----------------------------------------------
	//	バックバッファ準備:カラーバッファ設定.
	//-----------------------------------------------

	//バックバッファテクスチャ-を取得(既にあるので作成ではない)
	ID3D11Texture2D *pBackBuffer_Tex;
	m_pSwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),	//__uuidof:式に関連付けされたGUIDを取得.
		//         Texture2Dの唯一の物として扱う.
		(LPVOID*)&pBackBuffer_Tex);	//(out)バックバッファテクスチャ-.

	//そのテクスチャに対しレンダーターゲットビュー(RTV)を作成.
	m_pDevice->CreateRenderTargetView(pBackBuffer_Tex,NULL,&m_pBackBuffer_RTV);
	//バックバッファテクスチャ-を解放.
	SAFE_RELEASE(pBackBuffer_Tex);

	//-----------------------------------------------
	//	バックバッファ準備:デプス(深度)ステンシル関係.
	//-----------------------------------------------

	//デプス(深さor深度)ステンシルビュー用のテクスチャ-を作成.
	D3D11_TEXTURE2D_DESC descDepth;

	descDepth.Width = (UINT)WINDOW_WIDTH;//幅.
	descDepth.Height = (UINT)WINDOW_HEIGHT;//高さ.
	descDepth.MipLevels = 1;//ミップマップレベル:1.
	descDepth.ArraySize = 1;//配列数:1.
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;//32ビットフォーマット.
	descDepth.SampleDesc.Count = 1;//マルチサンプルの数.
	descDepth.SampleDesc.Quality = 0;//マルチサンプルのクオリティ.
	descDepth.Usage = D3D11_USAGE_DEFAULT;//使用方法:デフォルト.
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;//深度(ステンシルとして使用)
	descDepth.CPUAccessFlags = 0;//CPUからはアクセスしない.
	descDepth.MiscFlags = 0;//その他の設定なし.

	//(out)デプスステンシル用テクスチャ.
	m_pDevice->CreateTexture2D(&descDepth,NULL,&m_pBackBuffer_Tex);

	//そのテクスチャに対しデプスステンシルビュー(DSV)を作成.
	m_pDevice->CreateDepthStencilView(m_pBackBuffer_Tex,NULL,&m_pBackBuffer_DSV);//(out)DSV.

	//レンダーターゲットビューとデプスステンシルビューをパイプラインにセット.
	m_pDeviceContext->OMSetRenderTargets(1,&m_pBackBuffer_RTV,m_pBackBuffer_DSV);

	//深度テスト(Zテスト)を有効にする.
	D3D11_DEPTH_STENCIL_DESC	depthStencilDesc;
	ZeroMemory(&depthStencilDesc,sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true;

	if (SUCCEEDED(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState)))
	{
		m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
	}

	//ビューポートの設定.
	D3D11_VIEWPORT vp;
	vp.Width = WINDOW_WIDTH;		//幅.
	vp.Height = WINDOW_HEIGHT;	//高さ.
	vp.MinDepth = 0.0f;				//最小深度(手前)
	vp.MaxDepth = 1.0f;				//最大深度(奥)
	vp.TopLeftX = 0.0f;				//左上位置x.
	vp.TopLeftY = 0.0f;				//左上位置y.
	m_pDeviceContext->RSSetViewports(1, &vp);

	//ラスタライズ(面の塗りつぶし方)設定.
	//D3D11_CULL_NONE :カリングを切る(正背面を描画する)
	//D3D11_CULL_BACK :背面を描画しない.
	//D3D11_CULL_FRONT:正面を描画しない.
	SetRasterize(D3D11_CULL_NONE);
}

//解放.
void Direct3D::Release()
{
	SAFE_RELEASE(m_pDepthStencilState);
	SAFE_RELEASE(m_pBackBuffer_DSV);
	SAFE_RELEASE(m_pBackBuffer_Tex);
	SAFE_RELEASE(m_pBackBuffer_RTV);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pDevice);
}

//ラスタライズ設定.
void Direct3D::SetRasterize(D3D11_CULL_MODE CullMode)
{
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc, sizeof(rdc));
	rdc.FillMode = D3D11_FILL_SOLID;//塗りつぶし(ソリッド)
	rdc.CullMode = CullMode;
	rdc.FrontCounterClockwise = FALSE;
	//ポリゴンの表裏を決定するフラグ. 
	//TRUE :左回りなら前向き.右回りなら後ろ向き.
	//FALSE:↑の逆になる.
	rdc.DepthClipEnable = FALSE;	//距離についてのクリッピング有効.

	ID3D11RasterizerState* pIr = NULL;
	m_pDevice->CreateRasterizerState(&rdc, &pIr);
	m_pDeviceContext->RSSetState(pIr);
	SAFE_RELEASE(pIr);
}

//深度テスト(Zテスト) ON/OFF切替.
void Direct3D::SetDepth(bool bFlg)
{
	D3D11_DEPTH_STENCIL_DESC	depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = bFlg;

	m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
}

//バックバッファのクリア.
void Direct3D::ClearBackBuffer(D3DXVECTOR4 vClearColor)
{
	//カラーバックバッファ.
	m_pDeviceContext->ClearRenderTargetView(m_pBackBuffer_RTV, vClearColor);
	//デプスステンシルバックバッファ.
	m_pDeviceContext->ClearDepthStencilView(m_pBackBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

//レンダリングされたイメージを表示.
void Direct3D::RenderSwapChain()
{
	m_pSwapChain->Present(0, 0);
}