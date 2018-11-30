#pragma once

#include "..\\..\\Global.h"

#define ALIGN16 _declspec(align(16))

struct SPRITE_STATE
{
	//�t�@�C�����Ƃ����܂ł̃p�X.
	char sPath[MAX_PATH];
	//���摜�����������邩.
	D3DXVECTOR2 vDivisionQuantity;
};

class BaseSprite
{
public:
	const int m_iVerticesMax = 4;

	//���_�̍\����.
	struct VERTEX
	{
		D3DXVECTOR3	vPos;	//�ʒu.
		D3DXVECTOR2	vTex;	//�e�N�X�`�����W.
	};

	//�R���X�^���g�o�b�t�@�̃A�v�����̒�`(Sprite2D.hlsl).
	struct CONSTANT_BUFFER
	{
		ALIGN16 D3DXMATRIX	mW;				//���[���h�s��.
		ALIGN16 float		fViewPortWidth;	//�r���[�|�[�g��.
		ALIGN16 float		fViewPortHeight;//�r���[�|�[�g����.
		ALIGN16 float		fAlpha;			//�A���t�@�l(���߂Ŏg�p).
		ALIGN16 D3DXVECTOR2	vUV;			//UV���W.
	};

	struct SPRITE_INFO
	{
		float Width;
		float Height;
	};

	BaseSprite();			//�R���X�g���N�^.
	virtual ~BaseSprite();	//�f�X�g���N�^.

	//�쐬.
	HRESULT Create(ID3D11Device* const pDevice11, ID3D11DeviceContext* const pContext11, const LPSTR FileName = nullptr);

	//���.
	HRESULT Release();

	//�`��(�����_�����O)
	void Render();

protected:
	//�u�����h�X�e�[�g.
	struct BLEND_STATE
	{
		//���߂���.
		ID3D11BlendState*	m_pAlpha;
		//���ߖ���.
		ID3D11BlendState*	m_pNonAlpha;
	};

	ID3D11Device*				m_pDevice11;		//�f�o�C�X�I�u�W�F�N�g.
	ID3D11DeviceContext*		m_pDeviceContext11;	//�f�o�C�X�R���e�L�X�g.

	ID3D11VertexShader*			m_pVertexShader;	//���_�V�F�[�_.
	ID3D11InputLayout*			m_pVertexLayout;	//���_���C�A�E�g.
	ID3D11PixelShader*			m_pPixelShader;		//�s�N�Z���V�F�[�_.
	ID3D11Buffer*				m_pConstantBuffer;	//�R���X�^���g�o�b�t�@.

	ID3D11Buffer*				m_pVertexBuffer;	//���_�o�b�t�@.

	ID3D11ShaderResourceView*	m_pTexture;			//�e�N�X�`��.
	ID3D11SamplerState*			m_pSampleLinear;	//�e�N�X�`���̃T���v��-:�e�N�X�`���[�Ɋe��t�B���^��������.

	BLEND_STATE					m_BlendState;

	SPRITE_INFO					m_BaseSpriteState;	//���摜�̏��.

	D3DXVECTOR3		m_vPos;			//�ʒu.
	D3DXVECTOR3		m_vRot;			//��].
	float			m_fScale;		//�T�C�Y.
	float			m_fAlpha;		//�A���t�@�l.
	bool			m_bDispFlg;		//�\���t���O.

	UINT			m_uVerMax;		//���_�̍ő吔.
	VERTEX			m_Vertices[4];

	//�V�F�[�_�쐬.
	virtual HRESULT CreateShader() = 0;
	//���f���쐬.
	HRESULT CreateModel();
	//�e�N�X�`���쐬.
	HRESULT CreateTexture(const LPSTR FileName, ID3D11ShaderResourceView** const pTex);

	//����(�A���t�@�u�����h)�ݒ�̐؂�ւ�.
	void SetBlend(bool flg);
	HRESULT CreateBlendStateList();
	HRESULT RelaseBlendStateList();

	//�|��(�l�p�`)�̒��_���쐬.
	virtual void CreateVertex() = 0;
	//���[���h�ϊ�.
	virtual void ConversionWorld(D3DMATRIX &mWorld) = 0;
	//�V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	virtual void InputConstantBuffer(const D3DMATRIX mWorld) = 0;
};
