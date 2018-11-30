#include "NumberSprite.h"

NumberSprite::NumberSprite(const int iNumber, ID3D11Device* const pDevice, ID3D11DeviceContext* const pContext)
	: m_ppSprite(nullptr)
	, m_iNumber(iNumber)
	, m_iDigitNumber(0)
{
	//負の数は受け付けない.
	assert(m_iNumber >= 0);

	int iNum = m_iNumber;
	int iDig = 10;
	while (iNum > 0.0f)
	{
		iNum /= iDig;
		m_iDigitNumber++;
	}

	//スプライトの領域を確保.
	m_ppSprite = new Sprite*[m_iDigitNumber];
	for (int i = 0; i < m_iDigitNumber; i++)
	{
		m_ppSprite[i] = nullptr;
	}

	SPRITE_STATE SpriteData;
	SpriteData = { "Data\\Image\\Number.jpg", { 1.0f, 10.0f } };
	for (int i = 0; i < m_iDigitNumber; i++)
	{
		m_ppSprite[i] = new Sprite(SpriteData.vDivisionQuantity.x, SpriteData.vDivisionQuantity.y);
		m_ppSprite[i]->Create(pDevice, pContext, SpriteData.sPath);
	}
}

NumberSprite::~NumberSprite()
{
	for (int i = 0; i < m_iDigitNumber; i++)
	{
		SAFE_DELETE(m_ppSprite[i]);
	}
	SAFE_DELETE_ARRAY(m_ppSprite);
}

void NumberSprite::Render(D3DXVECTOR2 vPosition)
{
	int iNum = m_iNumber;
	int iDig = 10;

	for (int i = 0; i < m_iDigitNumber; i++)
	{
		m_ppSprite[i]->SetPatternHeight(static_cast<float>(iNum % iDig));
		iNum /= iDig;

		m_ppSprite[i]->SetPos(vPosition.x - (m_ppSprite[i]->GetSize().x * i), vPosition.y);
		m_ppSprite[i]->Render();
	}
}