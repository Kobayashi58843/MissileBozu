#pragma once

#include <d3dx9.h>

/*================/ ���ʂ��ĕK�v�ȍ\���� /================*/

struct SHADER_POINTER
{
	ID3D11VertexShader*	pVertexShader;	//���_�V�F�[�_.
	ID3D11PixelShader*	pPixelShader;	//�s�N�Z���V�F�[�_.
	ID3D11InputLayout*	pVertexLayout;	//���_���C�A�E�g.
};

struct SHADER_ENTRY_POINT
{
	char* pVertexShaderEntryPointName;
	char* pPixelShaderEntryPointName;
};

/*================/ �X�^�e�B�b�N���b�V���ɕK�v�ȍ\���� /================*/

struct CBUFF_STATICMESH_FOR_SIMPLE
{
	D3DXMATRIX mW;		//���[���h(�ʒu)���W�s��.
	D3DXMATRIX mWVP;	//���[���h,�r���[,�ˉe�̍����ϊ��s��.
};

struct CBUFF_STATICMESH_FOR_SHADOWMAP
{
	//���[���h(�ʒu)���W�s��.
	D3DXMATRIX mW;

	//( W * V * P ) �s��.
	D3DXMATRIX mWVP;

	//( W * L * P  * T ) �s��.
	D3DXMATRIX mWLPT;

	//( W * L * P ) �s��.
	D3DXMATRIX mWLP;
};

struct SUB_CBUFF_STATICMESH_FOR_SHADOWMAP
{
	//�J�����ʒu(���_�ʒu).
	D3DXVECTOR4 vEye;
};

//�V���h�E�}�b�v�ɕK�v�ȃ|�C���^.
struct SHADOWMAP_REQUIRED_POINTER
{
	//�[�x�e�N�X�`��.
	ID3D11ShaderResourceView* pDepthTexture;
	//�[�x�e�N�X�`���̃r���[�s��.
	D3DXMATRIX mLightView;
	//�[�x�e�N�X�`���̃J�����̈ʒu.
	D3DXVECTOR3 vLightCameraPosition;
};

//���_�̍\����.
struct MESH_VERTEX
{
	D3DXVECTOR3	vPos;	//�ʒu.
	D3DXVECTOR3	vNormal;//�@��(�A�e�v�Z�ɕK�{)
	D3DXVECTOR2	vTex;	//�e�N�X�`�����W.
};

//�}�e���A���\����.
struct MY_MATERIAL
{
	D3DXVECTOR4	Ambient;	//�A���r�G���g.
	D3DXVECTOR4	Diffuse;	//�f�B�t���[�Y.
	D3DXVECTOR4	Specular;	//�X�y�L����.
	CHAR szTextureName[128];//�e�N�X�`���t�@�C����.
	ID3D11ShaderResourceView* pTexture;//�e�N�X�`��.
	DWORD dwNumFace;	//���̃}�e���A���̃|���S����.

	//�R���X�g���N�^.
	MY_MATERIAL()
	{
		ZeroMemory(this, sizeof(MY_MATERIAL));
	}
	//�f�X�g���N�^.
	~MY_MATERIAL()
	{
		SAFE_RELEASE(pTexture);
	}
};

/*================/ �X�L�����b�V���ɕK�v�ȍ\���� /================*/

// �ő�{�[����.
#define MAX_BONES	255

struct CBUFF_SKINMESH_FOR_BONES
{
	D3DXMATRIX mBone[MAX_BONES];
	CBUFF_SKINMESH_FOR_BONES()
	{
		for (int i = 0; i<MAX_BONES; i++)
		{
			D3DXMatrixIdentity(&mBone[i]);
		}
	}
};

struct CBUFF_SKINMESH_FOR_MESH
{
	D3DXMATRIX	mW;			// ���[���h�s��.
	D3DXMATRIX	mWVP;		// ���[���h����ˉe�܂ł̕ϊ��s��.
	D3DXVECTOR4 vAmbient;	// �A���r�G���g��.
	D3DXVECTOR4 vDiffuse;	// �f�B�t���[�Y�F.
	D3DXVECTOR4 vSpecular;	// ���ʔ���.
};

//�ʒu�\����.
struct TRANSFORM
{
	D3DXVECTOR3 vScale;
	float		fYaw;	//��](Y��)
	float		fPitch;	//��](X��)
	float		fRoll;	//��](Z��)
	D3DXVECTOR3	vPos;	//�ʒu(X,Y,Z)
};

// �I���W�i���@�}�e���A���\����.
struct MY_SKINMATERIAL
{
	CHAR szName[110];
	D3DXVECTOR4	Ka;	// �A���r�G���g.
	D3DXVECTOR4	Kd;	// �f�B�t���[�Y.
	D3DXVECTOR4	Ks;	// �X�y�L�����[.
	CHAR szTextureName[512];	// �e�N�X�`���[�t�@�C����.
	ID3D11ShaderResourceView* pTexture;
	DWORD dwNumFace;	// ���̃}�e���A���ł���|���S����.
	MY_SKINMATERIAL()
	{
		ZeroMemory(this, sizeof(MY_SKINMATERIAL));
	}
	~MY_SKINMATERIAL()
	{
		SAFE_RELEASE(pTexture);
	}
};

// ���_�\����.
struct MY_SKINVERTEX
{
	D3DXVECTOR3	vPos;		// ���_�ʒu.
	D3DXVECTOR3	vNorm;		// ���_�@��.
	D3DXVECTOR2	vTex;		// UV���W.
	UINT bBoneIndex[4];		// �{�[�� �ԍ�.
	float bBoneWeight[4];	// �{�[�� �d��.
	MY_SKINVERTEX()
	{
		ZeroMemory(this, sizeof(MY_SKINVERTEX));
	}
};
// �{�[���\����.
struct BONE
{
	D3DXMATRIX mBindPose;	// �����|�[�Y�i�����ƕς��Ȃ��j.
	D3DXMATRIX mNewPose;	// ���݂̃|�[�Y�i���̓s�x�ς��j.
	DWORD dwNumChild;		// �q�̐�.
	int iChildIndex[50];	// �����̎q�́g�C���f�b�N�X�h50�܂�.
	CHAR Name[256];

	BONE()
	{
		ZeroMemory(this, sizeof(BONE));
		D3DXMatrixIdentity(&mBindPose);
		D3DXMatrixIdentity(&mNewPose);
	}
};

// �p�[�c���b�V���\����.
struct SKIN_PARTS_MESH
{
	DWORD	dwNumVert;
	DWORD	dwNumFace;
	DWORD	dwNumUV;
	DWORD			dwNumMaterial;
	MY_SKINMATERIAL*	pMaterial;
	char				TextureFileName[8][256];	// �e�N�X�`���[�t�@�C����(8���܂�).
	bool				bTex;

	ID3D11Buffer*	pVertexBuffer;
	ID3D11Buffer**	ppIndexBuffer;

	// �{�[��.
	int		iNumBone;
	BONE*	pBoneArray;

	bool	bEnableBones;	// �{�[���̗L���t���O.

	SKIN_PARTS_MESH()
	{
		ZeroMemory(this, sizeof(SKIN_PARTS_MESH));
		pVertexBuffer = NULL;
		ppIndexBuffer = NULL;
		pBoneArray = NULL;
	}
};

// �f�o�C�X�֌W���p�\����.
struct CD3DXSKINMESH_INIT
{
	HWND hWnd;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
};

// �h���t���[���\����.
//	���ꂼ��̃��b�V���p�̍ŏI���[���h�s���ǉ�����.
struct MYFRAME : public D3DXFRAME
{
	D3DXMATRIX CombinedTransformationMatrix;
	SKIN_PARTS_MESH* pPartsMesh;

	MYFRAME()
	{
		ZeroMemory(this, sizeof(MYFRAME));
	}
};

// �h�����b�V���R���e�i�[�\����.
//	�R���e�i�[���e�N�X�`���𕡐����Ă�悤�Ƀ|�C���^�[�̃|�C���^�[��ǉ�����
struct MYMESHCONTAINER : public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9*  ppTextures;
	DWORD dwWeight;				// �d�݂̌��i�d�݂Ƃ͒��_�ւ̉e���B�j.
	DWORD dwBoneNum;			// �{�[���̐�.
	LPD3DXBUFFER pBoneBuffer;	// �{�[���E�e�[�u��.
	D3DXMATRIX** ppBoneMatrix;	// �S�Ẵ{�[���̃��[���h�s��̐擪�|�C���^.
	D3DXMATRIX* pBoneOffsetMatrices;// �t���[���Ƃ��Ẵ{�[���̃��[���h�s��̃|�C���^.
};
// X�t�@�C�����̃A�j���[�V�����K�w��ǂ݉����Ă����N���X��h��������.
//	ID3DXAllocateHierarchy�͔h�����邱�Ƒz�肵�Đ݌v����Ă���.
class MY_HIERARCHY : public ID3DXAllocateHierarchy
{
public:
	MY_HIERARCHY(){}
	STDMETHOD(CreateFrame)(THIS_ LPCSTR, LPD3DXFRAME *);
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR, CONST D3DXMESHDATA*, CONST D3DXMATERIAL*,
		CONST D3DXEFFECTINSTANCE*, DWORD, CONST DWORD *, LPD3DXSKININFO, LPD3DXMESHCONTAINER *);
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER);
};