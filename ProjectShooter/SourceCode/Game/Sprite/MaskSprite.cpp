#include "MaskSprite.h"

#include "..\\Direct3D.h"

//�V�F�[�_�t�@�C����(�f�B���N�g�����܂�)
const char SHADER_NAME[] = "Shader\\MaskSprite.hlsl";

MaskSprite::MaskSprite(const float fDivisionQuantityWidth, const float fDivisionQuantityHeight)
	: m_vDivisionQuantity({ fDivisionQuantityWidth, fDivisionQuantityHeight })
	, m_pMaskTexture_SRV(nullptr)
{
	CrearVECTOR2(m_vPattern);
	CrearVECTOR2(m_vUV);
}

MaskSprite::~MaskSprite()
{
	m_pMaskTexture_SRV = nullptr;
}

//�V�F�[�_���쐬����.
HRESULT MaskSprite::CreateShader()
{
	ID3DBlob* pCompiledShader = NULL;
	ID3DBlob* pErrors = NULL;
	UINT uCompileFlag = 0;

#ifdef _DEBUG
	uCompileFlag
		= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif//#ifdef _DEBUG

	//HLSL����o�[�e�b�N�X�V�F�[�_�̃u���u���쐬.
	if (FAILED(
		D3DX11CompileFromFile(
		SHADER_NAME,		//�V�F�[�_�t�@�C����(HLSL�t�@�C��)
		NULL,				//�}�N����`�̔z��ւ̃|�C���^(���g�p)
		NULL,				//�C���N���[�h�t�@�C���������C���^�[�t�F�C�X�ւ̃|�C���^(���g�p)
		"VS",				//�V�F�[�_�G���g���[�|�C���g�֐��̖��O.
		"vs_5_0",			//�V�F�[�_�̃��f�����w�肷�镶����(�v���t�@�C��)
		uCompileFlag,		//�V�F�[�_�R���p�C���t���O.
		0,					//�G�t�F�N�g�R���p�C���t���O(���g�p)
		NULL,				//�X���b�h�|���v�C���^�[�t�F�C�X�ւ̃|�C���^(���g�p)
		&pCompiledShader,	//�u���u���i�[���郁�����ւ̃|�C���^.
		&pErrors,			//�G���[�ƌx���ꗗ���i�[���郁�����ւ̃|�C���^.
		NULL)))				//�߂�l�ւ̃|�C���^(���g�p)
	{
		MessageBox(NULL, "hlsl�Ǎ����s", "�G���[", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//��L�ō쐬�����u���u����u�o�[�e�b�N�X�V�F�[�_�v���쐬.
	if (FAILED(m_pDevice11->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
		NULL, &m_pVertexShader)))//(out)�o�[�e�b�N�X�V�F�[�_.
	{
		MessageBox(NULL, "vs�쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}

	//���_�C���v�b�g���C�A�E�g���`.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",				//�ʒu.
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,//DXGI�̃t�H�[�}�b�g(32bit float�^*3)
			0,
			0,							//�f�[�^�̊J�n�ʒu.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD",		//�e�N�X�`���ʒu.
			0,
			DXGI_FORMAT_R32G32_FLOAT,	//DXGI�̃t�H�[�}�b�g(32bit float�^*2)
			0,
			12,							//�e�N�X�`���f�[�^�̊J�n�ʒu.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	//���_�C���v�b�g���C�A�E�g�̔z��v�f�����Z�o.
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//���_�C���v�b�g���C�A�E�g�̍쐬.
	if (FAILED(m_pDevice11->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
		&m_pVertexLayout)))//(out)���_�C���v�b�g���C�A�E�g.
	{
		MessageBox(NULL, "���_�C���v�b�g���C�A�E�g�쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	//HLSL����s�N�Z���V�F�[�_�̃u���u���쐬.
	if (FAILED(
		D3DX11CompileFromFile(
		SHADER_NAME,	//�V�F�[�_�t�@�C����(HLSL�t�@�C��)
		NULL, NULL,
		"PS_Main",		//�V�F�[�_�G���g���[�|�C���g�֐��̖��O.
		"ps_5_0",		//�V�F�[�_�̃��f�����w�肷�镶����(�v���t�@�C��)
		uCompileFlag,	//�V�F�[�_�R���p�C���t���O.
		0, NULL,
		&pCompiledShader,//�u���u���i�[���郁�����ւ̃|�C���^.
		&pErrors,
		NULL)))
	{
		MessageBox(NULL, "hlsl(ps)�Ǎ����s", "�G���[", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//��L�ō쐬�����u���u����u�s�N�Z���V�F�[�_�v���쐬.
	if (FAILED(m_pDevice11->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
		NULL, &m_pPixelShader)))//(out)�s�N�Z���V�F�[�_.
	{
		MessageBox(NULL, "ps�쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);//�u���u���.

	//�R���X�^���g(�萔)�o�b�t�@�쐬 �V�F�[�_�ɓ���̐��l�𑗂�o�b�t�@.
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//�R���X�^���g�o�b�t�@���w��.
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);//�R���X�^���g�o�b�t�@�̃T�C�Y.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//�������݂ŃA�N�Z�X.
	cb.MiscFlags = 0;//���̑��̃t���O(���g�p)
	cb.StructureByteStride = 0;//�\���̂̃T�C�Y(���g�p)
	cb.Usage = D3D11_USAGE_DYNAMIC;//�g�p���@:���ڏ�������.

	//�R���X�^���g�o�b�t�@�쐬.
	if (FAILED(m_pDevice11->CreateBuffer(&cb, NULL, &m_pConstantBuffer)))
	{
		MessageBox(NULL, "�R���X�^���g�o�b�t�@�쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//�|��(�l�p�`)�̒��_���쐬.
void MaskSprite::CreateVertex()
{
	assert(m_vDivisionQuantity.x != 0.0f);
	assert(m_vDivisionQuantity.y != 0.0f);

	//�\���X�v���C�g���A����.
	float fW = (m_BaseSpriteState.Width / m_vDivisionQuantity.x) * m_fScale;
	float fH = (m_BaseSpriteState.Height / m_vDivisionQuantity.y) * m_fScale;

	float fU = (m_BaseSpriteState.Width / m_vDivisionQuantity.x) / m_BaseSpriteState.Width;
	float fV = (m_BaseSpriteState.Height / m_vDivisionQuantity.y) / m_BaseSpriteState.Height;

	//�|��(�l�p�`)�̒��_���쐬.
	VERTEX vertices[] =
	{
		D3DXVECTOR3(-fW / 2, fH / 2, 0.0f), D3DXVECTOR2(0.0f, fV),
		D3DXVECTOR3(-fW / 2, -fH / 2, 0.0f), D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR3(fW / 2, fH / 2, 0.0f), D3DXVECTOR2(fU, fV),
		D3DXVECTOR3(fW / 2, -fH / 2, 0.0f), D3DXVECTOR2(fU, 0.0f)
	};
	//�ő�v�f�����Z�o����.
	m_uVerMax = sizeof(vertices) / sizeof(vertices[0]);

	for (unsigned int i = 0; i < m_uVerMax; i++)
	{
		m_Vertices[i] = vertices[i];
	}
}

//���[���h�ϊ�.
void MaskSprite::ConversionWorld(D3DMATRIX &mWorld)
{
	D3DXMATRIX mTrans;
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMATRIX mScale;

	//�T�C�Y.
	D3DXMatrixScaling(&mScale, m_fScale, m_fScale, m_fScale);

	//��].
	D3DXMatrixRotationX(&mPitch, m_vRot.x);
	D3DXMatrixRotationY(&mYaw, m_vRot.y);
	D3DXMatrixRotationZ(&mRoll, m_vRot.z);

	//���s�ړ�.
	D3DXMatrixTranslation(&mTrans, m_vPos.x, m_vPos.y, m_vPos.z);

	mWorld = mScale * mPitch * mYaw * mRoll * mTrans;
}

//�V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
void MaskSprite::InputConstantBuffer(const D3DMATRIX mWorld)
{
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER cd;	//�R���X�^���g�o�b�t�@.
	//�o�b�t�@���̃f�[�^�̏������J�n����map.
	if (SUCCEEDED(m_pDeviceContext11->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//���[���h�s���n��.
		D3DXMATRIX m = mWorld;
		D3DXMatrixTranspose(&m, &m);//�s���]�u����.					
		//�s��̌v�Z���@��DirectX��GPU�ňقȂ��.
		cd.mW = m;

		//�r���[�|�[�g�̕��A������n��.
		cd.fViewPortWidth = WINDOW_WIDTH;
		cd.fViewPortHeight = WINDOW_HEIGHT;

		//�A���t�@�l��n��.
		cd.fAlpha = m_fAlpha;

		//UV���W��n��.
		cd.vUV.x = (((m_BaseSpriteState.Width / m_vDivisionQuantity.x) / m_BaseSpriteState.Width) * m_vPattern.x)
			+ (m_vUV.x / m_BaseSpriteState.Width);

		cd.vUV.y = (((m_BaseSpriteState.Height / m_vDivisionQuantity.y) / m_BaseSpriteState.Height) * m_vPattern.y)
			+ (m_vUV.y / m_BaseSpriteState.Height);

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cd), sizeof(cd));

		m_pDeviceContext11->Unmap(m_pConstantBuffer, 0);
	}

	//���̃R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�����H.
	m_pDeviceContext11->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext11->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	//�}�X�N�����Ɏg���e�N�X�`�����V�F�[�_�ɓn��.
	m_pDeviceContext11->PSSetShaderResources(1, 1, &m_pMaskTexture_SRV);
}
