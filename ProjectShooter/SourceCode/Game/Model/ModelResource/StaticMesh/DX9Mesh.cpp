#include "DX9Mesh.h"

#include "..\\..\\..\\..\\Singleton.h"

//�����̃V�F�[�_.
const enStaticModelShader STARTING_SHADER = enStaticModelShader_Simple;

//================================================
//	�R���X�g���N�^.
//================================================
clsDX9Mesh::clsDX9Mesh()
{
	ZeroMemory(this, sizeof(clsDX9Mesh));
	m_fScale = 1.0f;
}

//================================================
//	�f�X�g���N�^.
//================================================
clsDX9Mesh::~clsDX9Mesh()
{
	Release();

	m_pDeviceContext11 = nullptr;
	m_pDevice11 = nullptr;

	m_hWnd = nullptr;
}

//================================================
//	������.
//================================================
HRESULT clsDX9Mesh::Init(HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPSTR fileName)
{
	m_hWnd = hWnd;
	m_pDevice11 = pDevice11;
	m_pDeviceContext11 = pContext11;

	if (FAILED(InitDx9(m_hWnd)))
	{
		return E_FAIL;
	}
	if (FAILED(LoadXMesh(fileName)))
	{
		return E_FAIL;
	}
	if (FAILED(SetShader(STARTING_SHADER)))
	{
		return E_FAIL;
	}

	//�u�����h�X�e�[�g�쐬.
	CreateBlendStateList();

	return S_OK;
}

void clsDX9Mesh::Release()
{
	//�u�����h�X�e�[�g���.
	ReleaseBlendStateList();

	SAFE_DELETE_ARRAY(m_ppIndexBuffer);
	SAFE_DELETE_ARRAY(m_pMaterials);

	m_pCBuffPerMesh = nullptr;
	m_pCBuffPerMaterial = nullptr;
	m_pCBuffPerFrame = nullptr;

	SAFE_RELEASE(m_pMeshForRay);
	SAFE_RELEASE(m_pMesh);

	//�I�u�W�F�N�g�̃����[�X.
	SAFE_RELEASE(m_pDevice9);
	SAFE_RELEASE(m_pD3d);
}

//================================================
//	Dx9������.
//================================================
HRESULT clsDX9Mesh::InitDx9(HWND hWnd)
{
	m_hWnd = hWnd;

	//�uDirect3D�v�I�u�W�F�N�g�̍쐬.
	m_pD3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3d == NULL)
	{
		MessageBox(NULL, "Dx9�I�u�W�F�N�g�쐬���s", "InitDx9", MB_OK);
		return E_FAIL;
	}

	//Direct3D�f�o�C�X �I�u�W�F�N�g�̍쐬.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	//�o�b�N�o�b�t�@�̃t�H�[�}�b�g(�f�t�H���g)
	d3dpp.BackBufferCount = 1;					//�o�b�N�o�b�t�@�̐�.
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	//�X���b�v�G�t�F�N�g(�f�t�H���g)
	d3dpp.Windowed = true;						//�E�B���h�E���[�h.
	d3dpp.EnableAutoDepthStencil = true;		//�X�e���V���L��.
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	//�X�e���V���̃t�H�[�}�b�g(16�r�b�g)

	//�f�o�C�X�쐬(HAL���[�h:�`��ƒ��_������GPU�ōs��)
	if (FAILED(m_pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice9)))
	{
		//�f�o�C�X�쐬(HAL���[�h:�`���GPU�A���_������CPU�ōs��)
		if (FAILED(m_pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice9)))
		{
			MessageBox(NULL, "HAL���[�h�Ńf�o�C�X���쐬�ł��܂���\nREF���[�h�ōĎ��s���܂�", "�x��", MB_OK);

			//�f�o�C�X�쐬(REF���[�h:�`���CPU�A���_������GPU�ōs��)
			if (FAILED(m_pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice9)))
			{
				//�f�o�C�X�쐬(REF���[�h:�`��ƒ��_������CPU�ōs��)
				if (FAILED(m_pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice9)))
				{
					MessageBox(NULL, "Direct3D�f�o�C�X�쐬���s", NULL, MB_OK);

					return E_FAIL;
				}
			}
		}
	}


	return S_OK;
}

//================================================
//X�t�@�C�����烁�b�V�������[�h����.
//================================================
HRESULT clsDX9Mesh::LoadXMesh(LPSTR fileName)
{
	//�}�e���A���o�b�t�@.
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	//���C�ƃ��b�V���̔���̂��߂ɒǉ�.
	if (FAILED(D3DXLoadMeshFromXA(
		fileName,			//�t�@�C����.
		D3DXMESH_SYSTEMMEM,	//�V�X�e���������ɓǍ�.
		m_pDevice9, NULL,
		&pD3DXMtrlBuffer,	//(out)�}�e���A�����.
		NULL,
		&m_dwNumMaterials,	//(out)�}�e���A����.
		&m_pMeshForRay)))	//(out)���b�V���I�u�W�F�N�g.
	{
		MessageBox(NULL, fileName, "X�t�@�C���Ǎ����s", MB_OK);
		return E_FAIL;
	}

	//X�t�@�C���̃��[�h.
	if (FAILED(D3DXLoadMeshFromXA(
		fileName,			//�t�@�C����.
		D3DXMESH_SYSTEMMEM	//�V�X�e���������ɓǍ�.
		| D3DXMESH_32BIT,
		m_pDevice9, NULL,
		&pD3DXMtrlBuffer,	//(out)�}�e���A�����.
		NULL,
		&m_dwNumMaterials,	//(out)�}�e���A����.
		&m_pMesh)))			//(out)���b�V���I�u�W�F�N�g.
	{
		MessageBox(NULL, fileName, "X�t�@�C���Ǎ����s", MB_OK);
		return E_FAIL;
	}

	D3D11_BUFFER_DESC		bd;	//Dx11�o�b�t�@�\����.
	D3D11_SUBRESOURCE_DATA	InitData;//�������f�[�^.

	//�Ǎ��񂾏�񂩂�K�v�ȏ��𔲂��o��.
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	//�}�e���A�������̗̈���l��.
	m_pMaterials = new MY_MATERIAL[m_dwNumMaterials];
	m_ppIndexBuffer = new ID3D11Buffer*[m_dwNumMaterials];
	//�}�e���A�������J��Ԃ�.
	for (DWORD i = 0; i<m_dwNumMaterials; i++)
	{
		//�}�e���A�����̃R�s�[.
		//�A���r�G���g.
		m_pMaterials[i].Ambient.x = d3dxMaterials[i].MatD3D.Ambient.r;
		m_pMaterials[i].Ambient.y = d3dxMaterials[i].MatD3D.Ambient.g;
		m_pMaterials[i].Ambient.z = d3dxMaterials[i].MatD3D.Ambient.b;
		m_pMaterials[i].Ambient.w = d3dxMaterials[i].MatD3D.Ambient.a;
		//�f�B�t���[�Y.
		m_pMaterials[i].Diffuse.x = d3dxMaterials[i].MatD3D.Diffuse.r;
		m_pMaterials[i].Diffuse.y = d3dxMaterials[i].MatD3D.Diffuse.g;
		m_pMaterials[i].Diffuse.z = d3dxMaterials[i].MatD3D.Diffuse.b;
		m_pMaterials[i].Diffuse.w = d3dxMaterials[i].MatD3D.Diffuse.a;
		//�X�y�L����.
		m_pMaterials[i].Specular.x = d3dxMaterials[i].MatD3D.Specular.r;
		m_pMaterials[i].Specular.y = d3dxMaterials[i].MatD3D.Specular.g;
		m_pMaterials[i].Specular.z = d3dxMaterials[i].MatD3D.Specular.b;
		m_pMaterials[i].Specular.w = d3dxMaterials[i].MatD3D.Specular.a;

		//(���̖ʂ�)�e�N�X�`�����\���Ă��邩�H.
		if (d3dxMaterials[i].pTextureFilename != NULL && lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
		{
			m_bTexture = true;//�e�N�X�`���t���O�𗧂Ă�.

			char path[64] = "";
			int  path_count = lstrlen(fileName);

			for (int k = path_count; k >= 0; k--)
			{
				if (fileName[k] == '\\'){
					for (int j = 0; j <= k; j++)
					{
						path[j] = fileName[j];
					}
					path[k + 1] = '\0';
					break;
				}
			}
			//�p�X�ƃe�N�X�`���t�@�C������A��.
			strcat_s(path, sizeof(path), d3dxMaterials[i].pTextureFilename);

			//�e�N�X�`���t�@�C�������R�s�[.
			strcpy_s(m_pMaterials[i].szTextureName, sizeof(m_pMaterials[i].szTextureName), path);

			//�e�N�X�`���쐬.
			if (FAILED(D3DX11CreateShaderResourceViewFromFileA(
				m_pDevice11,
				m_pMaterials[i].szTextureName,//�e�N�X�`���t�@�C����.
				NULL, NULL,
				&m_pMaterials[i].pTexture,//(out)�e�N�X�`���I�u�W�F�N�g.
				NULL)))
			{
				MessageBox(NULL, m_pMaterials[i].szTextureName, "�e�N�X�`���쐬���s", MB_OK);
				return E_FAIL;
			}
		}
	}

	//--------------------------------------------
	//	�C���f�b�N�X�o�b�t�@���쐬.
	//--------------------------------------------
	//���b�V���̑������𓾂�.
	//�������ŃC���f�b�N�X�o�b�t�@����ׂ����}�e���A�����Ƃ̃C���f�b�N�X�o�b�t�@�𕪗��ł���.
	D3DXATTRIBUTERANGE* pAttrTable = NULL;

	//���b�V���̖ʂ���ђ��_�̏��ԕύX�𐧌䂵�A�p�t�H�[�}���X���œK������.
	// D3DXMESHOPT_COMPACT :�ʂ̏��Ԃ�ύX���A
	//						�g�p����Ă��Ȃ����_�Ɩʂ��폜����.
	// D3DXMESHOPT_ATTRSORT:�p�t�H�[�}���X���グ��ׁA
	//						�ʂ̏��Ԃ�ύX���čœK�����s��.
	m_pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT, 0, 0, 0, 0);
	//�����e�[�u���̎擾.
	m_pMesh->GetAttributeTable(NULL, &m_NumAttr);
	pAttrTable = new D3DXATTRIBUTERANGE[m_NumAttr];
	if (FAILED(m_pMesh->GetAttributeTable(pAttrTable, &m_NumAttr)))
	{
		MessageBox(NULL, "�����e�[�u���擾���s",
			"LoadXMesh", MB_OK);
		return E_FAIL;
	}

	//�������A���b�N���Ȃ��Ǝ��o���Ȃ�.
	int* pIndex = NULL;
	m_pMesh->LockIndexBuffer(D3DLOCK_READONLY, (void**)&pIndex);

	//�������Ƃ̃C���f�b�N�X�o�b�t�@���쐬.
	for (DWORD i = 0; i<m_NumAttr; i++)
	{
		m_AttrID[i] = pAttrTable[i].AttribId;

		//Dx9�̃C���f�b�N�X�o�b�t�@����̏���Dx11�̃C���f�b�N�X�o�b�t�@���쐬.
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(int) * pAttrTable[i].FaceCount * 3; //�ʐ��~3�Œ��_��.
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		//�傫���C���f�b�N�X�o�b�t�@���̃I�t�Z�b�g(�~3����)
		InitData.pSysMem = &pIndex[pAttrTable[i].FaceStart * 3];

		if (FAILED(m_pDevice11->CreateBuffer(&bd, &InitData, &m_ppIndexBuffer[i])))
		{
			MessageBox(NULL, "�C���f�b�N�X�o�b�t�@�쐬���s", "LoadXMesh()", MB_OK);
			return E_FAIL;
		}
		//�ʂ̐����R�s�[.
		m_pMaterials[m_AttrID[i]].dwNumFace = pAttrTable[i].FaceCount;
	}
	delete[] pAttrTable;//�����e�[�u���̍폜.

	m_pMesh->UnlockIndexBuffer();

	//�s�v�ɂȂ����}�e���A���o�b�t�@�����.
	SAFE_RELEASE(pD3DXMtrlBuffer);

	//-----------------------------------------------
	//	���_(�o�[�e�b�N�X)�o�b�t�@�̍쐬.
	//-----------------------------------------------
	//Dx9�̏ꍇ�Amap�ł͂Ȃ����b�N�Ńo�[�e�b�N�X�o�b�t�@����f�[�^�����o��.
	LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
	m_pMesh->GetVertexBuffer(&pVB);
	DWORD dwStride = m_pMesh->GetNumBytesPerVertex();
	BYTE *pVertices = NULL;
	MESH_VERTEX* pVertex = NULL;
	if (SUCCEEDED(pVB->Lock(0, 0, (VOID**)&pVertices, 0)))
	{
		pVertex = (MESH_VERTEX*)pVertices;
		//Dx9�̃o�[�e�b�N�X�o�b�t�@������ŁADx11�̃o�[�e�b�N�X�o�b�t�@���쐬.
		bd.Usage = D3D11_USAGE_DEFAULT;
		//���_���i�[����̂ɕK�v�ȃo�C�g��.
		bd.ByteWidth = m_pMesh->GetNumBytesPerVertex() * m_pMesh->GetNumVertices(); //���_��.
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		InitData.pSysMem = pVertex;

		//�e�N�X�`�����W���}�C�i�X�΍�.
		if (m_bTexture)
		{
			//���e�N�X�`��������Ώ�������.
			//���_�����J��Ԃ�.
			for (unsigned int i = 0; i<m_pMesh->GetNumVertices(); i++)
			{
				if (pVertex[i].vTex.x < 0.0f)
				{
					pVertex[i].vTex.x += 1.0f;
				}
				if (pVertex[i].vTex.y < 0.0f)
				{
					pVertex[i].vTex.y += 1.0f;
				}
			}
		}

		if (FAILED(m_pDevice11->CreateBuffer(&bd, &InitData, &m_pVertexBuffer)))
		{
			MessageBox(NULL, "���_(�o�[�e�b�N�X)�o�b�t�@�쐬���s", "LoadXMesh()", MB_OK);
			return E_FAIL;
		}
		pVB->Unlock();
	}
	SAFE_RELEASE(pVB);//�o�[�e�b�N�X�o�b�t�@���.


	//-----------------------------------------------
	//	�T���v��-�쐬.
	//-----------------------------------------------
	//�e�N�X�`���p�̃T���v���[�\����.
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(SamDesc));
	//���j�A�t�B���^-(���`���)
	//	POINT:���������e��.
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//���b�s���O���[�h.
	//	WRAP  :�J��Ԃ�
	//	MIRROR:���]�J��Ԃ�.
	//	CLAMP :�[�̖͗l�������L�΂�.
	//	BORDER:�ʓr���E�F�����߂�.
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	//�T���v���[�쐬.
	if (FAILED(m_pDevice11->CreateSamplerState(&SamDesc, &m_pSampleLinear)))//(out)�T���v���[.
	{
		MessageBox(NULL, "�T���v���쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�[�x�e�N�X�`���p�T���v���[�쐬.
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	if (FAILED(m_pDevice11->CreateSamplerState(&SamDesc, &m_pSampleLinearDepth)))
	{
		MessageBox(NULL, "�T���v���쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT clsDX9Mesh::SetShader(const enStaticModelShader enShader)
{
	//�V�F�[�_���Z�b�g����.
	m_Shader = Singleton<ShaderGathering>().GetInstance().GetStaticMeshShader(enShader);
	m_enShader = enShader;

	//�V�F�[�_���ƂɕK�v�ȃR���X�^���g�o�b�t�@�̃|�C���^���Z�b�g����.
	switch (enShader)
	{
	case enStaticModelShader_NoTex:
		m_pCBuffPerMesh = Singleton<ShaderGathering>().GetInstance().GetStaticCBuffer(enStaticModelCBuffer_Mesh);
		m_pCBuffPerMaterial = nullptr;
		m_pCBuffPerFrame = nullptr;

		break;
	case enStaticModelShader_Simple:
		m_pCBuffPerMesh = Singleton<ShaderGathering>().GetInstance().GetStaticCBuffer(enStaticModelCBuffer_Mesh);
		m_pCBuffPerMaterial = nullptr;
		m_pCBuffPerFrame = nullptr;

		break;
	case enStaticModelShader_Depth:
		m_pCBuffPerMesh = Singleton<ShaderGathering>().GetInstance().GetStaticCBuffer(enStaticModelCBuffer_Mesh);
		m_pCBuffPerMaterial = nullptr;
		m_pCBuffPerFrame = nullptr;

		break;
	case enStaticModelShader_ShadowMap:
		m_pCBuffPerMesh = Singleton<ShaderGathering>().GetInstance().GetStaticCBuffer(enStaticModelCBuffer_ShadowMap_Mesh);
		m_pCBuffPerMaterial = nullptr;
		m_pCBuffPerFrame = Singleton<ShaderGathering>().GetInstance().GetStaticCBuffer(enStaticModelCBuffer_ShadowMap_Frame);

		break;
	default:
		break;
	}

	return S_OK;
}

//�����_�����O�p.
void clsDX9Mesh::Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj)
{
	D3DXMATRIX	mWorld;	//���[���h�s��.
	D3DXMATRIX	mScale, mYaw, mPitch, mRoll, mTrans;

	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixIdentity(&mScale);
	D3DXMatrixIdentity(&mYaw);
	D3DXMatrixIdentity(&mPitch);
	D3DXMatrixIdentity(&mRoll);
	D3DXMatrixIdentity(&mTrans);

	//���[���h�ϊ�(�\���ʒu��ݒ肷��)
	//�g�k.
	D3DXMatrixScaling(&mScale, m_fScale, m_fScale, m_fScale);
	//��].
	D3DXMatrixRotationY(&mYaw, m_vRot.y);	//Y����].
	D3DXMatrixRotationX(&mPitch, m_vRot.x);	//X����].
	D3DXMatrixRotationZ(&mRoll, m_vRot.z);	//Z����].

	//�ړ�.
	D3DXMatrixTranslation(&mTrans, m_vPos.x, m_vPos.y, m_vPos.z);

	//����(�g�k�~��]�~�ړ�)
	mWorld = mScale * mYaw * mPitch * mRoll * mTrans;

	//�g�p����V�F�[�_���Z�b�g.
	m_pDeviceContext11->VSSetShader(m_Shader.pVertexShader, NULL, 0);	//���_�V�F�[�_.
	m_pDeviceContext11->PSSetShader(m_Shader.pPixelShader, NULL, 0);	//�s�N�Z���V�F�[�_.

	switch (m_enShader)
	{
	case enStaticModelShader_NoTex:
		NoTexture::RenderStaticModel(
			m_pDeviceContext11,
			m_pVertexBuffer,
			m_Shader.pVertexLayout,
			m_pMesh,
			m_pCBuffPerMesh,
			mWorld, mView, mProj);

		break;
	case enStaticModelShader_Simple:
		Simple::RenderStaticModel(
			m_pDeviceContext11,
			m_pVertexBuffer,
			m_Shader.pVertexLayout,
			m_pMesh,
			m_pCBuffPerMesh,
			mWorld, mView, mProj);

		break;
	case enStaticModelShader_Depth:
		Simple::RenderStaticModel(
			m_pDeviceContext11,
			m_pVertexBuffer,
			m_Shader.pVertexLayout,
			m_pMesh,
			m_pCBuffPerMesh,
			mWorld, mView, mProj);

		break;
	case enStaticModelShader_ShadowMap:
		ShadowMap::RenderStaticModel(
			m_pDeviceContext11,
			m_pVertexBuffer,
			m_Shader.pVertexLayout,
			m_pMesh,
			m_pCBuffPerMesh, m_pCBuffPerFrame,
			mWorld, mView, mProj,
			m_ShadowMapPointers.mLightView,
			m_ShadowMapPointers.vLightCameraPosition);

		break;
	default:
		break;
	}

	//���b�V���̃����_�����O.
	RenderMesh(mWorld, mView, mProj);
}

//���b�V���̃����_�����O.
void clsDX9Mesh::RenderMesh(const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj)
{
	//�A���t�@�u�����h�p�u�����h�X�e�[�g�쐬.
	SetBlend(false);

	switch (m_enShader)
	{
	case enStaticModelShader_NoTex:
	{
		NoTexture::RenderStaticModelMesh(
			m_pDeviceContext11,
			m_pMaterials, m_NumAttr, m_AttrID,
			m_ppIndexBuffer, m_pSampleLinear);
	}
	break;
	case enStaticModelShader_Simple:
	{
		Simple::RenderStaticModelMesh(
			m_pDeviceContext11,
			m_pMaterials, m_NumAttr, m_AttrID,
			m_ppIndexBuffer, m_pSampleLinear);
	}
	break;
	case enStaticModelShader_Depth:
	{
		DepthTexture::RenderStaticModelMesh(
			m_pDeviceContext11,
			m_pMaterials, m_NumAttr, m_AttrID,
			m_ppIndexBuffer, m_pSampleLinear);
	}
	break;
	case enStaticModelShader_ShadowMap:
	{
		ShadowMap::RenderStaticModelMesh(
			m_pDeviceContext11,
			m_pMaterials, m_NumAttr, m_AttrID,
			m_ShadowMapPointers.pDepthTexture,
			m_ppIndexBuffer,
			m_pSampleLinear, m_pSampleLinearDepth);
	}
	break;
	default:
		break;
	}
}

//����(�A���t�@�u�����h)�ݒ�̐؂�ւ�.
void clsDX9Mesh::SetBlend(bool flg)
{
	ID3D11BlendState *pBlendState = nullptr;
	if (flg == true)
	{
		pBlendState = m_BlendState.m_pAlpha;
	}
	else
	{
		pBlendState = m_BlendState.m_pNonAlpha;
	}

	//�u�����h�X�e�[�g�̐ݒ�.
	UINT mask = 0xffffffff;	//�}�X�N�l.
	m_pDeviceContext11->OMSetBlendState(pBlendState, NULL, mask);
}

HRESULT clsDX9Mesh::CreateBlendStateList()
{
	//�A���t�@�u�����h�p�u�����h�X�e�[�g�\����.
	//png�t�@�C�����ɃA���t�@��񂪂���̂ŁA
	//���߂���悤�Ƀu�����h�X�e�[�g��ݒ肷��.
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));//������.

	blendDesc.IndependentBlendEnable
		= false;//false:RenderTarget[0]�̃����o�[�݂̂��g�p����.
	//true :RenderTarget[0�`7]���g�p�ł���.
	//      (�����_�[�^�[�Q�b�g���ɓƗ������u�����h����)
	blendDesc.AlphaToCoverageEnable
		= true;//true :�A���t�@�g�D�J�o���b�W���g�p����.
	blendDesc.RenderTarget[0].BlendEnable
		= false;	//true :�A���t�@�u�����h���g�p����.
	blendDesc.RenderTarget[0].SrcBlend	//���f�ނɑ΂���ݒ�.
		= D3D11_BLEND_SRC_ALPHA;		//	�A���t�@�u�����h���w��.
	blendDesc.RenderTarget[0].DestBlend	//�d�˂�f�ނɑ΂���ݒ�.
		= D3D11_BLEND_INV_SRC_ALPHA;	//	�A���t�@�u�����h�̔��]���w��.

	blendDesc.RenderTarget[0].BlendOp	//�u�����h�I�v�V����.
		= D3D11_BLEND_OP_ADD;			//	ADD:���Z����.

	blendDesc.RenderTarget[0].SrcBlendAlpha	//���f�ނ̃A���t�@�ɑ΂���ݒ�.
		= D3D11_BLEND_ONE;					//	���̂܂܎g�p.
	blendDesc.RenderTarget[0].DestBlendAlpha//�d�˂�f�ނ̃A���t�@�ɑ΂���ݒ�.
		= D3D11_BLEND_ZERO;					//	�������Ȃ�.

	blendDesc.RenderTarget[0].BlendOpAlpha	//�A���t�@�̃u�����h�I�v�V����.
		= D3D11_BLEND_OP_ADD;				//	ADD:���Z����.

	blendDesc.RenderTarget[0].RenderTargetWriteMask	//�s�N�Z�����̏������݃}�X�N.
		= D3D11_COLOR_WRITE_ENABLE_ALL;				//	�S�Ă̐���(RGBA)�ւ̃f�[�^�̊i�[��������.

	//�u�����h�X�e�[�g�쐬.���߂���.
	if (FAILED(m_pDevice11->CreateBlendState(&blendDesc, &m_BlendState.m_pAlpha)))
	{
		MessageBox(NULL, "�u�����h�X�e�[�g�쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}

	//���ߖ���.
	blendDesc.RenderTarget[0].BlendEnable = false;
	if (FAILED(m_pDevice11->CreateBlendState(&blendDesc, &m_BlendState.m_pNonAlpha)))
	{
		MessageBox(NULL, "�u�����h�X�e�[�g�쐬���s", "�G���[", MB_OK);

		//�쐬���s�����ꍇ�쐬���ꂽ���߂���̃u�����h�X�e�[�g������.
		if (m_BlendState.m_pAlpha != nullptr)
		{
			SAFE_RELEASE(m_BlendState.m_pAlpha);
		}

		return E_FAIL;
	}

	return S_OK;
}

HRESULT clsDX9Mesh::ReleaseBlendStateList()
{
	SAFE_RELEASE(m_BlendState.m_pNonAlpha);
	SAFE_RELEASE(m_BlendState.m_pAlpha);

	return S_OK;
}