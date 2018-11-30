#pragma once

#include "..\\Global.h"

#include "Direct3D.h"

#include "Scene\\Base\\SceneFactory.h"

#include "Scene\\Base\\BaseScene.h"

class Game
{
public:
	Game(const HWND hWnd);
	~Game();

	//メッセージ毎の処理.
	void MsgProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam);

	/*====/ 毎フレーム更新する関数 /====*/
	//更新.
	void Update();

private:
	Direct3D* m_pDirect3D;

	//Direct3Dクラスで作成されたポインタの格納用.
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;

	/*====/ シーン関連 /====*/
	SceneFactory m_SceneFactory;
	BaseScene* m_pScene;

	//シーンに必要なポインタの構造体.
	SCENE_NEED_POINTER m_SceneNeedPointer;

	//シーン切り替え時のフェード用画像.
	TransitionsSprite* m_pFadeSprite;
	Sprite*		m_pFadeMaskSprite;
	BackBuffer* m_pFadeMaskBuffer;

	bool m_bBlackOut;

	//シーンの変更先の一時保存用.
	enSwitchToNextScene m_NextSceneTemporary;

	//フェード.
	void Fade(const enSwitchToNextScene enNextScene);

	//シーン切り替え.
	void SwitchScene(const enSwitchToNextScene enNextScene);

	/*====/ 毎フレーム更新する関数 /====*/
	//描画.
	void Render();
};