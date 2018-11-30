#pragma once

#include "..\\\..\\Global.h"
#include <vector>

#include "..\\..\\..\\Singleton.h"

#include "..\\..\\Camera\\Camera.h"

#include "..\\..\\Model\\ModelResource\\ModelResource.h"

#include "..\\..\\Model\\Character\\Player\\Player.h"
#include "..\\..\\Model\\Character\\Enemy\\Enemy.h"
#include "..\\..\\Model\\Character\\Event\\EventModel.h"

#include "..\\..\\Sprite\\BlurFilter.h"
#include "..\\..\\Sprite\\DisplayBackBuffer.h"
#include "..\\..\\Sprite\\MaskSprite.h"
#include "..\\..\\Sprite\\NumberSprite.h"
#include "..\\..\\Sprite\\Sprite.h"
#include "..\\..\\Sprite\\TransitionsSprite.h"

#include "..\\..\\BackBuffer.h"

#include "..\\..\\Sprite\\DebugText.h"
#include "..\\..\\RawInput.h"

#include "..\\..\\Sound\\SoundManager.h"

//描画順の最大数.
static const int MAX_RENDER_LEVEL = 10;

struct SCENE_NEED_POINTER
{
	//ウインドウハンドル.
	HWND hWnd;
	//デバイスオブジェクト.
	ID3D11Device* pDevice;
	//デバイスコンテキスト.
	ID3D11DeviceContext* pContext;

	//レンダーターゲットビュー.
	ID3D11RenderTargetView* pBackBuffer_RTV;

	//シェーダーリソースビュー.
	ID3D11ShaderResourceView* pBackBuffer_SRV;

	//デプスステンシルビュー.
	ID3D11DepthStencilView*	 pBackBuffer_DSV;

	//各ポインターとインスタンスをセット.
	void SetSceneNeedPointer(const HWND Wnd, ID3D11Device* const Device, ID3D11DeviceContext* const Context,
		ID3D11RenderTargetView* const BackBuffer_RTV, ID3D11ShaderResourceView* const BackBuffer_SRV, ID3D11DepthStencilView* const BackBuffer_DSV)
	{
		hWnd = Wnd;
		pDevice = Device;
		pContext = Context;

		pBackBuffer_RTV = BackBuffer_RTV;
		pBackBuffer_SRV = BackBuffer_SRV;
		pBackBuffer_DSV = BackBuffer_DSV;
	}
};

class BaseScene
{
public:
	BaseScene(SCENE_NEED_POINTER PointerGroup);
	virtual ~BaseScene();

	/*====/ どのシーンでも同じ処理をするもの /====*/
	//作成.
	void Create(const enSwitchToNextScene enNextScene);

	//解放.
	virtual void Release() = 0;

	//更新.
	void Update(enSwitchToNextScene &enNextScene);

	//3Dモデルの描画.
	void RenderModel(const int iRenderLevel);

	//2Dスプライトの描画.
	void RenderSprite(const int iRenderLevel);

protected:
	SCENE_NEED_POINTER m_SceneNeedPointer;

	//スプライト.
	std::vector<Sprite*> m_vpSprite;

	/*====/ 各シーンごとに違う処理をするもの /====*/
	//各シーンの作成.
	virtual void CreateProduct(const enSwitchToNextScene enNextScene) = 0;

	//各シーンの更新.
	virtual void UpdateProduct(enSwitchToNextScene &enNextScene) = 0;

	//各シーンの3Dモデルの描画.
	virtual void RenderModelProduct(const int iRenderLevel) = 0;

	//各シーンの2Dスプライトの描画.
	virtual void RenderSpriteProduct(const int iRenderLevel) = 0;

	//スプライトの作成.
	virtual void CreateSprite() = 0;

	//スプライトの解放.
	virtual void ReleaseSprite() = 0;

	//スプライトのフレームごとの更新.
	virtual void UpdateSprite() = 0;

	//スプライトの位置.
	virtual void UpdateSpritePositio(int iSpriteNo) = 0;

	//スプライトのアニメーション.
	virtual void UpdateSpriteAnimation(int iSpriteNo) = 0;

	//スプライト同士の接触.
	bool IsHittingOfSprite(int iSourceSpriteNo, int iTargetSpriteNo);

	/*====/ デバッグ用 /====*/
#if _DEBUG

	void InputUpdate(enSwitchToNextScene &enNextScene);

	DebugText* m_pDebugText;

	//デバッグテキストの表示.
	virtual void RenderDebugText() = 0;

#endif //#if _DEBUG.
};