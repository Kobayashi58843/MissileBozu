#pragma once

#include "BaseSprite.h"

class TitleBackgroundSprite : public BaseSprite
{
public:
	TitleBackgroundSprite(const float fWidth, const float fHeight);
	virtual ~TitleBackgroundSprite();

	void SetPos(const float fX, const float fY)
	{
		m_vPos.x = fX;
		m_vPos.y = fY;
	}
	void AddPos(const float fXQuantity, const float fYQuantity)
	{
		m_vPos.x += fXQuantity;
		m_vPos.y += fYQuantity;
	}
	D3DXVECTOR3 GetPos() const
	{
		return m_vPos;
	}

	void SetRot(const D3DXVECTOR3 vRot)
	{
		m_vRot = vRot;
	}
	void AddPitch(const float fQuantity)
	{
		m_vRot.x += fQuantity;
		ClampRot(m_vRot.x);
	}
	void AddYaw(const float fQuantity)
	{
		m_vRot.y += fQuantity;
		ClampRot(m_vRot.y);
	}
	void AddRoll(const float fQuantity)
	{
		m_vRot.z += fQuantity;
		ClampRot(m_vRot.z);
	}
	D3DXVECTOR3 GetRot() const
	{
		return m_vRot;
	}

	void SetScale(const float fScale)
	{
		m_fScale = fScale;
	}
	void AddScale(const float fQuantity)
	{
		if (0.0f <= m_fScale + fQuantity)
		{
			m_fScale += fQuantity;
		}
	}
	float GetScale() const
	{
		return m_fScale;
	}
	D3DXVECTOR2 GetSize()
	{
		D3DXVECTOR2 vSize;
		vSize.x = m_vSize.x * m_fScale;
		vSize.y = m_vSize.y * m_fScale;
		return vSize;
	}

	void SetAlpha(const float fAlpha)
	{
		m_fAlpha = fAlpha;
	}
	void AddAlpha(const float fQuantity)
	{
		m_fAlpha += fQuantity;
		ClampSize(m_fAlpha, 0.0f, 1.0f);
	}
	float GetAlpha() const
	{
		return m_fAlpha;
	}

	void SetDispFlg(const bool bDispFlg)
	{
		m_bDispFlg = bDispFlg;
	}
	bool IsDispFlg() const
	{
		return m_bDispFlg;
	}

	void AddUVWidth(const float fQuantity)
	{
		m_vUV.x += fQuantity;
		ClampSize(m_vUV.x, 0.0f, m_vSize.x);
	}
	void AddUVHeight(const float fQuantity)
	{
		m_vUV.y += fQuantity;
		ClampSize(m_vUV.y, 0.0f, m_vSize.y);
	}

	void SetSRV(ID3D11ShaderResourceView* const pMainSRV, ID3D11ShaderResourceView* const pSubSRV)
	{
		m_pMainSRV = pMainSRV;
		m_pSubSRV = pSubSRV;
	}

private:
	//�X�v���C�g�̑傫��.
	D3DXVECTOR2 m_vSize;
	//���Z����UV.
	D3DXVECTOR2 m_vUV;

	//�\������V�F�[�_�[���\�[�X�r���[.
	ID3D11ShaderResourceView*	m_pMainSRV;
	ID3D11ShaderResourceView*	m_pSubSRV;

	//�V�F�[�_�쐬.
	HRESULT CreateShader();

	//�|��(�l�p�`)�̒��_���쐬.
	void CreateVertex();
	//���[���h�ϊ�.
	void ConversionWorld(D3DMATRIX &mWorld);
	//�V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	void InputConstantBuffer(const D3DMATRIX mWorld);

	//���l���ۂ߂�.
	void ClampSize(float &fOut, float fMin, float fMax)
	{
		if (fOut < fMin)
		{
			fOut = fMin;
		}
		if (fOut > fMax)
		{
			fOut = fMax;
		}
	}
	void ClampRot(float &fRot)
	{
		if (fRot > D3DXToRadian(360))
		{
			fRot -= static_cast<float>D3DXToRadian(360);
		}
		else if (fRot < 0.0f)
		{
			fRot += static_cast<float>D3DXToRadian(360);
		}
	}
	void ClampPattern(float &fPattern, float fMin, float fMax)
	{
		if (fPattern < fMin)
		{
			fPattern = fMax - 1.0f;
		}
		else if (fPattern >= fMax)
		{
			fPattern = fMin;
		}
	}
};