#pragma once

#include <Windows.h>

#include "Game\\Game.h"

//定数.
#define WND_TITLE	"MissileBozu"
#define APR_NAME	"MissileBozu"

/*====/ メインクラス /====*/
class Main
{
private:
	HWND	m_hWnd;

public:
	Main();
	~Main();

	//ウィンドウ作成関数.
	HRESULT MakeWindow(HINSTANCE hInstance, INT x, INT y, INT Width, INT Height, LPSTR WindowName);

	//ウィンドウ関数(メッセージ毎の処理).
	LRESULT MsgProc(HWND hWnd, UINT uMsg,WPARAM wParam, LPARAM lParam );

	//メッセージループ.
	void Loop();

private:
	//ゲーム.
	Game* m_pGame;

public:
};