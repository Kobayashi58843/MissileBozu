#include "ContinueScene.h"

#include "..\\..\\Singleton.h"

ContinueScene::ContinueScene(SCENE_NEED_POINTER PointerGroup)
	: BaseScene(PointerGroup)
	, m_iCursorAnimationCount(0)
{
	//全サウンドを停止する.
	Singleton<SoundManager>().GetInstance().StopSound();
}

ContinueScene::~ContinueScene()
{
	Release();
}

//作成.
void ContinueScene::CreateProduct(const enSwitchToNextScene enNextScene)
{
	//スプライトの作成.
	CreateSprite();
}

//解放.
void ContinueScene::Release()
{
	ReleaseSprite();
}

//更新.
void ContinueScene::UpdateProduct(enSwitchToNextScene &enNextScene)
{
	//BGMをループで再生.
	Singleton<SoundManager>().GetInstance().PlayBGM(SoundManager::enBGM_Continue);

	//スプライト更新.
	UpdateSprite();

	//左クリックされた時.
	if (Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		//カーソルがボタンの上にあるか.
		if (IsHittingOfSprite(enSprite_Cursor, enSprite_Yes))
		{
			enNextScene = enSwitchToNextScene::Action;
		}
		else if(IsHittingOfSprite(enSprite_Cursor, enSprite_No))
		{
			enNextScene = enSwitchToNextScene::Over;
		}
	}
}

//3Dモデルの描画.
void ContinueScene::RenderModelProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 0:
		break;
	default:
		break;
	}
}

/*====/ スプライト関連 /====*/
//2Dスプライトの描画.
void ContinueScene::RenderSpriteProduct(const int iRenderLevel)
{
	switch (iRenderLevel)
	{
	case 0:
		for (unsigned int i = 0; i < m_vpSprite.size(); i++)
		{
			m_vpSprite[i]->Render();
		}

		break;
	default:
		break;
	}
}

//スプライトの作成.
void ContinueScene::CreateSprite()
{
	//キャパシティを確定させる.
	m_vpSprite.reserve(enSprite_Max);

	SPRITE_STATE SpriteData;

	//各スプライトの設定.
	for (int i = 0; i < enSprite_Max; i++)
	{
		switch (i)
		{
		case enSprite_BackGround:
			SpriteData =
			{ "Data\\Image\\BackGround.jpg"	//ファイルまでのパス.
			, { 1.0f, 1.0f } };				//元画像を何分割するか.

			break;
		case enSprite_Logo:
			SpriteData = { "Data\\Image\\Continue.png", { 1.0f, 1.0f } };

			break;
		case enSprite_Yes:
			SpriteData = { "Data\\Image\\Push.jpg", { 1.0f, 2.0f } };

			break;
		case enSprite_No:
			SpriteData = { "Data\\Image\\End.jpg", { 1.0f, 2.0f } };

			break;
		case enSprite_YesText:
			SpriteData = { "Data\\Image\\Yes.png", { 1.0f, 1.0f } };

			break;
		case enSprite_NoText:
			SpriteData = { "Data\\Image\\No.png", { 1.0f, 1.0f } };

			break;
		case enSprite_Cursor:
			SpriteData = { "Data\\Image\\Cursor.png", { 8.0f, 1.0f } };

			break;
		default:
			ERR_MSG("Clear::CreateSprite()", "error");

			break;
		}

		//配列を一つ増やす.
		m_vpSprite.push_back(new Sprite(SpriteData.vDivisionQuantity.x, SpriteData.vDivisionQuantity.y));
		m_vpSprite[i]->Create(m_SceneNeedPointer.pDevice, m_SceneNeedPointer.pContext, SpriteData.sPath);
	}
}

//スプライトの解放.
void ContinueScene::ReleaseSprite()
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
void ContinueScene::UpdateSprite()
{
	for (int i = 0; i < enSprite_Max; i++)
	{
		UpdateSpritePositio(i);
		UpdateSpriteAnimation(i);
	}
}

//スプライトの位置.
void ContinueScene::UpdateSpritePositio(int iSpriteNo)
{
	//ウインドウの中心.
	float fWindowWidthCenter = WINDOW_WIDTH / 2.0f;
	float fWindowHeightCenter = WINDOW_HEIGHT / 2.0f;

	//スプライト位置.
	D3DXVECTOR2 vPosition;

	switch (iSpriteNo)
	{
	case enSprite_BackGround:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter;

		break;
	case enSprite_Logo:
		vPosition.x = fWindowWidthCenter;
		vPosition.y = fWindowHeightCenter / 2.0f;

		break;
	case enSprite_Yes:
		vPosition.x = fWindowWidthCenter - (fWindowWidthCenter / 2.0f);
		vPosition.y = fWindowHeightCenter + (fWindowHeightCenter / 2.0f);

		break;
	case enSprite_No:
		vPosition.x = fWindowWidthCenter + (fWindowWidthCenter / 2.0f);
		vPosition.y = fWindowHeightCenter + (fWindowHeightCenter / 2.0f);

		break;
	case enSprite_YesText:
		vPosition.x = fWindowWidthCenter - (fWindowWidthCenter / 2.0f);
		vPosition.y = fWindowHeightCenter + (fWindowHeightCenter / 2.0f);

		break;
	case enSprite_NoText:
		vPosition.x = fWindowWidthCenter + (fWindowWidthCenter / 2.0f);
		vPosition.y = fWindowHeightCenter + (fWindowHeightCenter / 2.0f);

		break;
	case enSprite_Cursor:
	{
		//マウスの座標.
		POINT MousePosition;
		GetCursorPos(&MousePosition);
		D3DXVECTOR2 vMousePos = { static_cast<float>(MousePosition.x), static_cast<float>(MousePosition.y) };

		//ウィンドウの座標.
		//ウィンドウ左上端の座標 : ( left , top).
		//ウィンドウ右下端の座標 : ( right, bottom).
		RECT WindowPosition;
		GetWindowRect(m_SceneNeedPointer.hWnd, &WindowPosition);
		D3DXVECTOR2 vWindowPos = { static_cast<float>(WindowPosition.left), static_cast<float>(WindowPosition.top) };

		vPosition.x = vMousePos.x - vWindowPos.x;
		vPosition.y = vMousePos.y - vWindowPos.y;
	}
	break;
	default:
		ERR_MSG("Clear::UpdateSpritePositio()", "error");

		break;
	}

	m_vpSprite[iSpriteNo]->SetPos(vPosition.x, vPosition.y);
}

//スプライトのアニメーション.
void ContinueScene::UpdateSpriteAnimation(int iSpriteNo)
{
	switch (iSpriteNo)
	{
	case enSprite_BackGround:

		break;
	case enSprite_Logo:

		break;
	case enSprite_Yes:
		//カーソルとボタンが接触していた時.
		if (IsHittingOfSprite(enSprite_Cursor, iSpriteNo))
		{
			m_vpSprite[iSpriteNo]->SetPatternHeight(1.0f);
		}
		else
		{
			m_vpSprite[iSpriteNo]->SetPatternHeight(0.0f);
		}

		break;
	case enSprite_No:
		//カーソルとボタンが接触していた時.
		if (IsHittingOfSprite(enSprite_Cursor, iSpriteNo))
		{
			m_vpSprite[iSpriteNo]->SetPatternHeight(1.0f);
		}
		else
		{
			m_vpSprite[iSpriteNo]->SetPatternHeight(0.0f);
		}

		break;
	case enSprite_YesText:

		break;
	case enSprite_NoText:

		break;
	case enSprite_Cursor:
		if (IsHittingOfSprite(enSprite_Cursor, enSprite_Yes) ||
			IsHittingOfSprite(enSprite_Cursor, enSprite_No))
		{
			m_iCursorAnimationCount++;
			float fCursorAnimationWaitTime = 0.2f;
			if (m_iCursorAnimationCount / FPS >= fCursorAnimationWaitTime)
			{
				m_vpSprite[iSpriteNo]->AddPatternWidth(1.0f);
				m_iCursorAnimationCount = 0;

				int iPattern = static_cast<int>(m_vpSprite[iSpriteNo]->GetPattern().x);
				if (iPattern % 2 == 0)
				{
					Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_Cursor);
				}
			}
		}

		break;
	default:
		ERR_MSG("Clear::UpdateSpriteAnimation()", "error");

		break;
	}
}

#if _DEBUG

//デバッグテキストの表示.
void ContinueScene::RenderDebugText()
{
	char cStrDbgTxt[512];

	sprintf_s(cStrDbgTxt, "Scene : Continue");
	m_pDebugText->Render(cStrDbgTxt, 0, 50 + (50 * 0));
}

#endif //#if _DEBUG.