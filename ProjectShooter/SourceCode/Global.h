#pragma once
//�x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`.
#pragma warning( disable:4005 )

/*========/ �C���N���[�h /========*/
#include "MyMacro.h"
#include <stdio.h>

#include <assert.h>

#include <D3DX9.h>
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>
#include <D3D10.h>

/*========/ ���C�u���� /========*/
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx10.lib" )//�uD3DX�`�v�̒�`�g�p���ɕK�v.
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

/*========/ �萔 /========*/
//�E�B���h�E��.
const float WINDOW_WIDTH = 1280.0f;
//�E�B���h�E����.
const float WINDOW_HEIGHT = 720.0f;

const float FPS = 60.0f;

/*========/ �񋓑� /========*/
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

/*========/ ���������[�N /========*/
//���[�N���Ă���|�C���^�����ꂽ�s�����m.
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#endif//#ifdef _DEBUG.
