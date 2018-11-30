#pragma once

#include "..\\..\\..\\Shader\\ShaderGathering.h"

#include "..\\..\\..\\Shader\\NoTexture.h"
#include "..\\..\\..\\Shader\\Simple.h"
#include "..\\..\\..\\Shader\\DepthTexture.h"

// �ő�A�j���[�V�����Z�b�g��.
#define MAX_ANIM_SET	100

//==================================================================================================
//
//	�p�[�T�[�N���X.
//
//==================================================================================================
class D3DXPARSER
{
public:
	MY_HIERARCHY cHierarchy;
	MY_HIERARCHY* m_pHierarchy;
	LPD3DXFRAME m_pFrameRoot;

	LPD3DXANIMATIONCONTROLLER m_pAnimController;//�f�t�H���g�ň��.
	LPD3DXANIMATIONSET m_pAnimSet[MAX_ANIM_SET];

	HRESULT LoadMeshFromX(LPDIRECT3DDEVICE9, LPSTR);
	HRESULT AllocateBoneMatrix(LPD3DXMESHCONTAINER);
	HRESULT AllocateAllBoneMatrices(LPD3DXFRAME);
	VOID UpdateFrameMatrices(LPD3DXFRAME, LPD3DXMATRIX);


	int GetNumVertices(MYMESHCONTAINER* pContainer);
	int GetNumFaces(MYMESHCONTAINER* pContainer);
	int GetNumMaterials(MYMESHCONTAINER* pContainer);
	int GetNumUVs(MYMESHCONTAINER* pContainer);
	int GetNumBones(MYMESHCONTAINER* pContainer);
	int GetNumBoneVertices(MYMESHCONTAINER* pContainer, int iBoneIndex);
	DWORD GetBoneVerticesIndices(MYMESHCONTAINER* pContainer, int iBoneIndex, int iIndexInGroup);
	double GetBoneVerticesWeights(MYMESHCONTAINER* pContainer, int iBoneIndex, int iIndexInGroup);
	D3DXVECTOR3 GetVertexCoord(MYMESHCONTAINER* pContainer, DWORD iIndex);
	D3DXVECTOR3 GetNormal(MYMESHCONTAINER* pContainer, DWORD iIndex);
	D3DXVECTOR2 GetUV(MYMESHCONTAINER* pContainer, DWORD iIndex);
	int GetIndex(MYMESHCONTAINER* pContainer, DWORD iIndex);
	D3DXVECTOR4 GetAmbient(MYMESHCONTAINER* pContainer, int iIndex);
	D3DXVECTOR4 GetDiffuse(MYMESHCONTAINER* pContainer, int iIndex);
	D3DXVECTOR4 GetSpecular(MYMESHCONTAINER* pContainer, int iIndex);
	CHAR* GetTexturePath(MYMESHCONTAINER* pContainer, int index);
	float GetSpecularPower(MYMESHCONTAINER* pContainer, int iIndex);
	int GeFaceMaterialIndex(MYMESHCONTAINER* pContainer, int iFaceIndex);
	int GetFaceVertexIndex(MYMESHCONTAINER* pContainer, int iFaceIndex, int iIndexInFace);
	D3DXMATRIX GetBindPose(MYMESHCONTAINER* pContainer, int iBoneIndex);
	D3DXMATRIX GetNewPose(MYMESHCONTAINER* pContainer, int iBoneIndex);
	CHAR* GetBoneName(MYMESHCONTAINER* pContainer, int iBoneIndex);

	// ���b�V���R���e�i���擾����.
	LPD3DXMESHCONTAINER GetMeshContainer(LPD3DXFRAME pFrame);

	// �A�j���[�V�����Z�b�g�̐؂�ւ�.
	void ChangeAnimSet(int index, LPD3DXANIMATIONCONTROLLER pAC = NULL);
	// �A�j���[�V�����Z�b�g�̐؂�ւ�(�J�n�t���[���w��\��).
	void ChangeAnimSet_StartPos(int index, double dStartFramePos, LPD3DXANIMATIONCONTROLLER pAC = NULL);

	// �A�j���[�V������~���Ԃ��擾.
	double GetAnimPeriod(int index);
	// �A�j���[�V���������擾.
	int GetAnimMax(LPD3DXANIMATIONCONTROLLER pAC = NULL);

	// �w�肵���{�[�����(���W�E�s��)���擾����֐�.
	bool GetMatrixFromBone(char* sBoneName, D3DXMATRIX* pOutMat);
	bool GetPosFromBone(char* sBoneName, D3DXVECTOR3* pOutPos);

	// ���b�V�����.
	HRESULT ReleaseMesh(LPD3DXFRAME pFrame);

	// �ꊇ�������.
	HRESULT Release();
};

//==================================================================================================
//
//	�X�L�����b�V���N���X.
//
//==================================================================================================
class clsD3DXSKINMESH
	: private NoTexture
	, private Simple
	, private DepthTexture
{
public:
	TRANSFORM m_Trans;

	//�p�[�T�[�N���X����A�j���[�V�����R���g���[���[���擾����.
	LPD3DXANIMATIONCONTROLLER GetAnimController()
	{
		return m_pD3dxMesh->m_pAnimController;
	}

	D3DXMATRIX m_mWorld;
	D3DXMATRIX m_mRotation;

	D3DXMATRIX m_mView;
	D3DXMATRIX m_mProj;

	//�A�j���[�V�������x.
	double m_dAnimSpeed;

	// ���\�b�h.
	clsD3DXSKINMESH();
	~clsD3DXSKINMESH();

	HRESULT Init(CD3DXSKINMESH_INIT* si);
	// X�t�@�C������X�L�����b�V�����쐬����.
	HRESULT CreateFromX(CHAR* szFileName);
	// �`��֐�.
	void Render(const D3DXMATRIX& mView, const D3DXMATRIX& mProj, LPD3DXANIMATIONCONTROLLER pAC = NULL);
	// ����֐�.
	HRESULT Release();

	double GetAnimSpeed()				{ return m_dAnimSpeed; }
	void SetAnimSpeed(double dSpeed)	{ m_dAnimSpeed = dSpeed; }

	// �A�j���[�V�����Z�b�g�̐؂�ւ�.
	void ChangeAnimSet(int index, LPD3DXANIMATIONCONTROLLER pAC = NULL);
	// �A�j���[�V�����Z�b�g�̐؂�ւ�(�J�n�t���[���w��\��).
	void ChangeAnimSet_StartPos(int index, double dStartFramePos, LPD3DXANIMATIONCONTROLLER pAC = NULL);

	// �A�j���[�V������~���Ԃ��擾.
	double GetAnimPeriod(int index);
	// �A�j���[�V���������擾.
	int GetAnimMax(LPD3DXANIMATIONCONTROLLER pAC = NULL);

	// �w�肵���{�[�����(���W�E�s��)���擾����֐�.
	bool GetMatrixFromBone(char* sBoneName, D3DXMATRIX* pOutMat);
	bool GetPosFromBone(char* sBoneName, D3DXVECTOR3* pOutPos);
	bool GetDeviaPosFromBone(char* sBoneName, D3DXVECTOR3* pOutPos, D3DXVECTOR3 vDeviation = D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//�V�F�[�_���Z�b�g.
	HRESULT SetShader(const enSkinModelShader enShader);

	enSkinModelShader GetShader() const
	{
		return m_enShader;
	}

private:
	HWND m_hWnd;
	// Dx9.
	LPDIRECT3D9 m_pD3d9;
	LPDIRECT3DDEVICE9 m_pDevice9;
	// Dx11.
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	ID3D11SamplerState*		m_pSampleLinear;

	SHADER_POINTER			m_Shader;
	enSkinModelShader		m_enShader;

	ID3D11Buffer* m_pConstantBufferBone;

	ID3D11Buffer* m_pConstantBuffer;

	// ���b�V��.
	D3DXPARSER* m_pD3dxMesh;

	// XFile�̃p�X.
	CHAR	m_FilePath[256];

	// �A�j���[�V�����t���[��.
	int		m_iFrame;

	HRESULT CreateDeviceDx9(HWND hWnd);
	HRESULT CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);
	void RecursiveSetNewPoseMatrices(BONE* pBone, D3DXMATRIX* pmParent);

	// �S�Ẵ��b�V�����쐬����.
	void BuildAllMesh(D3DXFRAME* pFrame);

	// ���b�V�����쐬����.
	HRESULT CreateAppMeshFromD3DXMesh(LPD3DXFRAME pFrame);

	// X�t�@�C������X�L���֘A�̏���ǂݏo���֐�.
	HRESULT ReadSkinInfo(MYMESHCONTAINER* pContainer, MY_SKINVERTEX* pvVB, SKIN_PARTS_MESH* pParts);

	// �{�[�������̃|�[�Y�ʒu�ɃZ�b�g����֐�.
	void SetNewPoseMatrices(SKIN_PARTS_MESH* pParts, int frame, MYMESHCONTAINER* pContainer);
	// ����(���݂�)�|�[�Y�s���Ԃ��֐�.
	D3DXMATRIX GetCurrentPoseMatrix(SKIN_PARTS_MESH* pParts, int index);

	// �t���[���`��.
	void DrawFrame(LPD3DXFRAME pFrame);
	// �p�[�c�`��.
	void DrawPartsMesh(SKIN_PARTS_MESH* p, D3DXMATRIX World, MYMESHCONTAINER* pContainer);
	void DrawPartsMeshStatic(SKIN_PARTS_MESH* pMesh, D3DXMATRIX World, MYMESHCONTAINER* pContainer);

	// �S�Ẵ��b�V�����폜.
	void DestroyAllMesh(D3DXFRAME* pFrame);
	HRESULT DestroyAppMeshFromD3DXMesh(LPD3DXFRAME p);
};