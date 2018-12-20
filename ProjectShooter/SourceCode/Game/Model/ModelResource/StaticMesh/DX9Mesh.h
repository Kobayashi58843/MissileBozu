#pragma once

#include "..\\..\\..\\Shader\\ShaderGathering.h"

#include "..\\..\\..\\Shader\\NoTexture.h"
#include "..\\..\\..\\Shader\\Simple.h"
#include "..\\..\\..\\Shader\\DepthTexture.h"
#include "..\\..\\..\\Shader\\ShadowMap.h"

//���b�V���f�[�^���t�@�C��������o���ׂ����ɁADirectX9���g�p����.
//�������_�����O(�\��)�́ADirectX11�ōs��.
class clsDX9Mesh
	: private NoTexture
	, private Simple
	, private DepthTexture
	, private ShadowMap
{
public:
	clsDX9Mesh();
	~clsDX9Mesh();

	LPD3DXMESH GetMesh() const
	{
		return m_pMesh;
	}

	LPD3DXMESH GetMeshForRay() const
	{
		return m_pMeshForRay;
	}

	//�傫��.
	void SetScale(const float fScale)
	{
		m_fScale = fScale;
	}
	float GetScale() const
	{
		return m_fScale;
	}
	void AddScale(const float fAddScale)
	{
		m_fScale += fAddScale;
	}

	//�ʒu.
	void SetPos(const D3DXVECTOR3 vPos)
	{
		m_vPos = vPos;
	}
	D3DXVECTOR3 GetPos() const
	{
		return m_vPos;
	}
	void AddPos(const D3DXVECTOR3 fAddPos)
	{
		m_vPos += fAddPos;
	}

	//��].
	void SetRot(const D3DXVECTOR3 vRot)
	{
		m_vRot = vRot;
	}
	D3DXVECTOR3 GetRot() const
	{
		return m_vRot;
	}
	void AddRot(const D3DXVECTOR3 fAddRot)
	{
		m_vRot += fAddRot;
	}

	HRESULT Init(HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPSTR fileName);

	void Release();

	//�����_�����O�p.
	void Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj);

	//�V�F�[�_�����Z�b�g.
	HRESULT SetShader(const enStaticModelShader enShader);

	enStaticModelShader GetShader() const
	{
		return m_enShader;
	}

	//�V���h�E�}�b�v�ɕK�v�ȃ|�C���^��n��.
	void SetShadowMapPointer(const SHADOWMAP_REQUIRED_POINTER Pointers)
	{
		m_ShadowMapPointers = Pointers;
	}

private:
	//�u�����h�X�e�[�g.
	struct BLEND_STATE
	{
		//���߂���.
		ID3D11BlendState*	m_pAlpha;
		//���ߖ���.
		ID3D11BlendState*	m_pNonAlpha;
	};

	HWND					m_hWnd;		//�E�B���h�E�n���h��.

	//Dx9.
	LPDIRECT3D9				m_pD3d;		//Dx9�I�u�W�F�N�g.
	LPDIRECT3DDEVICE9		m_pDevice9;	//Dx9�f�o�C�X�I�u�W�F�N�g.

	SHADER_POINTER			m_Shader;
	enStaticModelShader		m_enShader;

	DWORD					m_dwNumMaterials;	//�}�e���A����.

	//Dx11.
	ID3D11Device*			m_pDevice11;		//�f�o�C�X�I�u�W�F�N�g.
	ID3D11DeviceContext*	m_pDeviceContext11;	//�f�o�C�X�R���e�L�X�g.

	ID3D11Buffer*			m_pCBuffPerMesh;	//�R���X�^���g�o�b�t�@(���b�V��).
	ID3D11Buffer*			m_pCBuffPerMaterial;//�R���X�^���g�o�b�t�@(�}�e���A��).
	ID3D11Buffer*			m_pCBuffPerFrame;	//�R���X�^���g�o�b�t�@(�t���[��).

	ID3D11Buffer*			m_pVertexBuffer;	//���_(�o�[�e�b�N�X)�o�b�t�@.
	ID3D11Buffer**			m_ppIndexBuffer;	//�C���f�b�N�X�o�b�t�@.

	ID3D11SamplerState*		m_pSampleLinear;		//�e�N�X�`���̃T���v��-:�e�N�X�`���[�Ɋe��t�B���^��������.
	ID3D11SamplerState*		m_pSampleLinearDepth;	//�e�N�X�`���̃T���v��-:�e�N�X�`���[�Ɋe��t�B���^��������.

	MY_MATERIAL*			m_pMaterials;		//�}�e���A���\����.
	DWORD					m_NumAttr;			//������.
	DWORD					m_AttrID[300];		//����ID ��300�����܂�.

	//�u�����h�X�e�[�g.
	BLEND_STATE				m_BlendState;

	bool m_bTexture; //�e�N�X�`���̗L��.

	SHADOWMAP_REQUIRED_POINTER m_ShadowMapPointers;

	LPD3DXMESH	m_pMesh;		//���b�V���I�u�W�F�N�g.
	LPD3DXMESH	m_pMeshForRay;	//���C�ƃ��b�V���p.

	//�ʒu.
	D3DXVECTOR3 m_vPos;

	//���f���̉�].
	D3DXVECTOR3 m_vRot;

	//�T�C�Y.
	float m_fScale;

	//Dx9������.
	HRESULT InitDx9(HWND hWnd);
	//���b�V���Ǎ�.
	HRESULT LoadXMesh(LPSTR fileName);

	//���b�V���̃����_�����O.
	void RenderMesh(const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj);

	//����(�A���t�@�u�����h)�ݒ�̐؂�ւ�.
	void SetBlend(bool flg);
	HRESULT CreateBlendStateList();
	HRESULT ReleaseBlendStateList();
};