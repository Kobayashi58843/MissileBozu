#pragma once

#include "Sprite.h"

//スプライトを何等分するか.
const D3DXVECTOR2 g_vDivisionQuantity = {1.0f, 10.0f};

class NumberSprite
{
public:
	NumberSprite(const int iNumber, ID3D11Device* const pDevice, ID3D11DeviceContext* const pContext);
	~NumberSprite();
	
	void Render(D3DXVECTOR2 vPosition);

private:
	Sprite** m_ppSprite;

	int m_iNumber;
	
	//桁数.
	int m_iDigitNumber;
};