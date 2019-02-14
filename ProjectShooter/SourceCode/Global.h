#pragma once
//警告についてのコード分析を無効にする.4005:再定義.
#pragma warning( disable:4005 )

/*========/ インクルード /========*/
#include "MyMacro.h"
#include <stdio.h>

#include <assert.h>

#include <D3DX9.h>
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>
#include <D3D10.h>

/*========/ ライブラリ /========*/
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx10.lib" )//「D3DX～」の定義使用時に必要.
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

/*========/ 定数 /========*/
//ウィンドウ幅.
const float WINDOW_WIDTH = 1280.0f;
//ウィンドウ高さ.
const float WINDOW_HEIGHT = 720.0f;

const float FPS = 60.0f;

/*========/ 列挙体 /========*/
enum class enSwitchToNextScene : UCHAR
{
	Starting,
	Title,
	StartEvent,
	Action,
	Winning,
	Clear,
	Loser,
	Continue,
	Over,

	Nothing
};

/*========/ メモリリーク /========*/
//リークしているポインタが作られた行を検知.
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#endif//#ifdef _DEBUG.
