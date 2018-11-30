#include "Main.h"
#include "Global.h"
#include <stdio.h>

#include "Singleton.h"

//グローバル変数.
Main*	g_pClsMain = nullptr;

//メイン関数.
INT WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,INT nCmdShow )
{
	//メモリリークの検知.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	g_pClsMain = new Main;//初期化&メインクラスの宣言.

	//クラスが存在してるかチェック.
	assert(g_pClsMain != nullptr);

#if _DEBUG

	if (SUCCEEDED(g_pClsMain->MakeWindow(hInstance, 0, 0, static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT), WND_TITLE)))
	{
		g_pClsMain->Loop();
	}

#else

	if (SUCCEEDED(g_pClsMain->MakeWindow(hInstance, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), WND_TITLE)))
	{
		g_pClsMain->Loop();
	}

#endif //#if _DEBUG.

	/*====/ 終了 /====*/
	//メインクラスの破棄.
	delete g_pClsMain;

	//シングルトンクラスの破棄.
	SingletonFinalizer::Finalize();
	
	return 0;
}

//ウィンドウプロシージャ.
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg,WPARAM wParam, LPARAM lParam )
{
	//プロシージャ.
	return g_pClsMain->MsgProc( hWnd, uMsg, wParam, lParam );
}

/*====/ メインクラス /====*/
Main::Main()
	: m_hWnd(nullptr)
	, m_pGame(nullptr)
{
}

Main::~Main()
{
	SAFE_DELETE(m_pGame);

	m_hWnd = nullptr;
}

//ウィンドウ作成関数.
HRESULT Main::MakeWindow(HINSTANCE hInstance, INT x, INT y, INT Width, INT Height, LPSTR WindowName)
{
	//ウィンドウの定義.
	WNDCLASSEX wc;
	ZeroMemory( &wc, sizeof( wc ) );//初期化(0設定)

	wc.cbSize		= sizeof( wc );
	wc.style		= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	= WndProc;
	wc.hInstance	= hInstance;
	wc.hIcon		= LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor		= LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground= (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszClassName= APR_NAME;
	wc.hIconSm		= LoadIcon( NULL, IDI_APPLICATION );

	//ウィンドウクラスをWindowsに登録.
	if( !RegisterClassEx( &wc ) )
	{
		MessageBox( NULL,"ウィンドウクラスの登録に失敗", "error", MB_OK );
		return E_FAIL;
	}

	//ウィンドウの作成.
#if _DEBUG
	m_hWnd = CreateWindow(
		APR_NAME,			//アプリ名.
		WindowName,			//ウィンドウタイトル.
		WS_OVERLAPPEDWINDOW,//ウィンドウ種別(普通).
		0, 0,				//表示位置座標.
		Width, Height,		//ウィンドウの幅,高さ.
		NULL,				//親ウィンドウハンドル.
		NULL,				//メニュー設定.
		hInstance,			//インスタンス番号.
		NULL);				//ウィンドウ作成時に発生するイベントに渡すデータ.

#else	//#if _DEBUG.
	m_hWnd = CreateWindow(APR_NAME,WindowName,WS_VISIBLE | WS_POPUP,0, 0,Width, Height,NULL,NULL,hInstance,NULL);

#endif	//#if _DEBUG.

	assert(m_hWnd != NULL);

	//ウィンドウの表示.
	ShowWindow( m_hWnd, SW_SHOW );
	UpdateWindow( m_hWnd );

	return S_OK;
}

//ウィンドウ関数(メッセージ毎の処理).
LRESULT Main::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
	case WM_DESTROY://ウィンドウが破棄された時.

		//アプリケーションの終了をWindowsに通知する.
		PostQuitMessage( 0 );
		break;
	}

	m_pGame->MsgProc(hWnd, uMsg, wParam, lParam);
	
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//メッセージループとアプリケーション処理の入り口.
void Main::Loop()
{
	m_pGame = new Game(m_hWnd);

	//フレームレート調節準備.
	float fRate		= 0.0f;	//レート.
	DWORD sync_old	= timeGetTime();//過去時間.
	DWORD sync_now;					//現在時間.

	//時間処理の為、最小単位を1ミリ秒に変更.
	timeBeginPeriod( 1 );
	
	//メッセージループ.
	MSG msg = { 0 };
	ZeroMemory( &msg, sizeof( msg ) );

	while( msg.message != WM_QUIT )
	{
		Sleep( 1 );
		//現在時間を取得.
		sync_now = timeGetTime();

		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

#ifndef _DEBUG

		//デバッグモード以外の時はマウスカーソルを見えなくする.
		ShowCursor(false);

#endif //#ifndef _DEBUG.
		
		//理想時間を算出.
		fRate = 1000.0f / FPS;
		if( sync_now - sync_old >= fRate )
		{
			//現在時間に置き換え.
			sync_old = sync_now;

			//ゲームループ.
			m_pGame->Update();
		}
	}
	//アプリケーションの終了.
	timeEndPeriod( 1 );//解除.
}