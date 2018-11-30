#include "ShaderGathering.h"

ShaderGathering::ShaderGathering()
{
	m_pDevice11 = nullptr;
	m_pDeviceContext11 = nullptr;

	for (unsigned int i = 0; i < enStaticModelShader_Max; i++)
	{
		m_StaticMeshShader[i].pVertexShader = nullptr;
		m_StaticMeshShader[i].pPixelShader = nullptr;
		m_StaticMeshShader[i].pVertexLayout = nullptr;
	}

	for (unsigned int i = 0; i < enStaticModelCBuffer_Max; i++)
	{
		m_pStaticCBuffers[i] = nullptr;
	}

	for (unsigned int i = 0; i < enSkinModelShader_Max; i++)
	{
		m_SkinMeshShader[i].pVertexShader = nullptr;
		m_SkinMeshShader[i].pPixelShader = nullptr;
		m_SkinMeshShader[i].pVertexLayout = nullptr;
	}

	for (unsigned int i = 0; i < enSkinModelCBuffer_Max; i++)
	{
		m_pSkinCBuffers[i] = nullptr;
	}
}

ShaderGathering::~ShaderGathering()
{
	for (unsigned int i = 0; i < enStaticModelShader_Max; i++)
	{
		SAFE_RELEASE(m_StaticMeshShader[i].pVertexShader);
		SAFE_RELEASE(m_StaticMeshShader[i].pPixelShader);
		SAFE_RELEASE(m_StaticMeshShader[i].pVertexLayout);
	}

	for (unsigned int i = 0; i < enStaticModelCBuffer_Max; i++)
	{
		SAFE_RELEASE(m_pStaticCBuffers[i]);
	}

	for (unsigned int i = 0; i < enSkinModelShader_Max; i++)
	{
		SAFE_RELEASE(m_SkinMeshShader[i].pVertexShader);
		SAFE_RELEASE(m_SkinMeshShader[i].pPixelShader);
		SAFE_RELEASE(m_SkinMeshShader[i].pVertexLayout);
	}

	for (unsigned int i = 0; i < enSkinModelCBuffer_Max; i++)
	{
		SAFE_RELEASE(m_pSkinCBuffers[i]);
	}

	m_pDeviceContext11 = nullptr;
	m_pDevice11 = nullptr;
}

//�V�F�[�_�̏����ݒ�.
void ShaderGathering::InitShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	m_pDevice11 = pDevice;
	m_pDeviceContext11 = pDeviceContext;

	//�X�^�e�B�b�N���b�V���p�V�F�[�_�쐬.
	CreateStaticShader();

	//�X�L�����b�V���p�V�F�[�_�쐬.
	CreateSkinShader();
}

//�R���X�^���g�o�b�t�@�쐬.
void ShaderGathering::MakeConstantBuffer(ID3D11Buffer* &pConstantBuffer, UINT CBufferSize)
{
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	//�R���X�^���g�o�b�t�@���w��.
	cb.ByteWidth = CBufferSize;					//�R���X�^���g�o�b�t�@�̃T�C�Y.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//�������݂ŃA�N�Z�X.
	cb.MiscFlags = 0;							//���̑��̃t���O(���g�p)
	cb.StructureByteStride = 0;					//�\���̂̃T�C�Y(���g�p)
	cb.Usage = D3D11_USAGE_DYNAMIC;				//�g�p���@:���ڏ�������.

	//�R���X�^���g�o�b�t�@�쐬.
	if (FAILED(m_pDevice11->CreateBuffer(&cb, NULL, &pConstantBuffer)))
	{
		MessageBox(NULL, "�R���X�^���g�o�b�t�@�쐬���s", "�G���[", MB_OK);
	}
}

//�X�^�e�B�b�N���b�V���p�V�F�[�_�쐬.
void ShaderGathering::CreateStaticShader()
{
	/*================/�V�F�[�_�쐬 /================*/

	char* ShaderName = nullptr;
	SHADER_ENTRY_POINT EntryPoint;

	for (int i = 0; i < enStaticModelShader_Max; i++)
	{
		switch (i)
		{
		case enStaticModelShader_NoTex:
			//�e�N�X�`�������̃V�F�[�_.
			ShaderName = "Shader\\MeshStatic.hlsl";
			EntryPoint.pVertexShaderEntryPointName = "VS_NoTex";
			EntryPoint.pPixelShaderEntryPointName = "PS_NoTex";
			
			break;
		case enStaticModelShader_Simple:
			//�G�t�F�N�g�������Ȃ��V�F�[�_.
			ShaderName = "Shader\\MeshStatic.hlsl";
			EntryPoint.pVertexShaderEntryPointName = "VS_Main";
			EntryPoint.pPixelShaderEntryPointName = "PS_Main";

			break;
		case enStaticModelShader_Depth:
			//�[�x�e�N�X�`���쐬�p�̃V�F�[�_.
			ShaderName = "Shader\\MeshStaticDepth.hlsl";
			EntryPoint.pVertexShaderEntryPointName = "VS_Main";
			EntryPoint.pPixelShaderEntryPointName = "PS_Main";

			break;
		case enStaticModelShader_ShadowMap:
			//�V���h�E�}�b�v.
			ShaderName = "Shader\\MeshStaticShadowMap.hlsl";
			EntryPoint.pVertexShaderEntryPointName = "VS_Main";
			EntryPoint.pPixelShaderEntryPointName = "PS_Main";

			break;
		default:
			break;
		}

		MakeStaticMeshShader(ShaderName, EntryPoint, m_StaticMeshShader[i]);
	}

	/*================/�R���X�^���g�o�b�t�@�쐬 /================*/
	
	for (int i = 0; i < enStaticModelCBuffer_Max; i++)
	{
		UINT uiBufferSize;

		switch (i)
		{
		case enStaticModelCBuffer_Mesh:
			uiBufferSize = sizeof(CBUFF_STATICMESH_FOR_SIMPLE);

			break;
		case enStaticModelCBuffer_ShadowMap_Mesh:
			uiBufferSize = sizeof(CBUFF_STATICMESH_FOR_SHADOWMAP);

			break;
		case enStaticModelCBuffer_ShadowMap_Frame:
			uiBufferSize = sizeof(SUB_CBUFF_STATICMESH_FOR_SHADOWMAP);

			break;
		default:
			break;
		}
		
		MakeConstantBuffer(m_pStaticCBuffers[i], uiBufferSize);
	}
}

//�V�F�[�_�쐬(�X�^�e�B�b�N���b�V��).
void ShaderGathering::MakeStaticMeshShader(char* ShaderName, SHADER_ENTRY_POINT EntryPoint, SHADER_POINTER& SHADER_POINTER)
{
	ID3DBlob* pCompiledShader = NULL;
	ID3DBlob* pErrors = NULL;
	UINT uCompileFlag = 0;

#ifdef _DEBUG
	uCompileFlag
		= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif//#ifdef _DEBUG

	//HLSL����o�[�e�b�N�X�V�F�[�_�̃u���u���쐬.
	if (FAILED(D3DX11CompileFromFile(ShaderName, NULL, NULL, EntryPoint.pVertexShaderEntryPointName,
		"vs_5_0", uCompileFlag, 0, NULL, &pCompiledShader, &pErrors, NULL)))
	{
		MessageBox(NULL, ShaderName, "hlsl�Ǎ����s", MB_OK);
	}
	SAFE_RELEASE(pErrors);

	//��L�ō쐬�����u���u����u�o�[�e�b�N�X�V�F�[�_�v���쐬.
	if (FAILED(m_pDevice11->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL,
		&SHADER_POINTER.pVertexShader)))//(out)�o�[�e�b�N�X�V�F�[�_.
	{
		MessageBox(NULL, ShaderName, "vs�쐬���s", MB_OK);
	}

	//���_�C���v�b�g���C�A�E�g���`.
	D3D11_INPUT_ELEMENT_DESC layout[3];
	//���_�C���v�b�g���C�A�E�g�̔z��v�f��.
	UINT numElements = 0;

	D3D11_INPUT_ELEMENT_DESC tmp[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	numElements = sizeof(tmp) / sizeof(tmp[0]);//�v�f���Z�o.
	memcpy_s(layout, sizeof(layout), tmp, sizeof(D3D11_INPUT_ELEMENT_DESC) * numElements);

	//���_�C���v�b�g���C�A�E�g�̍쐬.
	if (FAILED(m_pDevice11->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
		&SHADER_POINTER.pVertexLayout)))//(out)���_�C���v�b�g���C�A�E�g.
	{
		MessageBox(NULL, "���_�C���v�b�g���C�A�E�g�쐬���s", "�G���[", MB_OK);
	}
	SAFE_RELEASE(pCompiledShader);

	//HLSL����s�N�Z���V�F�[�_�̃u���u���쐬.
	if (FAILED(D3DX11CompileFromFile(ShaderName, NULL, NULL, EntryPoint.pPixelShaderEntryPointName,
		"ps_5_0", uCompileFlag, 0, NULL, &pCompiledShader, &pErrors, NULL)))
	{
		MessageBox(NULL, ShaderName, "hlsl�Ǎ����s", MB_OK);
	}

	SAFE_RELEASE(pErrors);

	//��L�ō쐬�����u���u����u�s�N�Z���V�F�[�_�v���쐬.
	if (FAILED(m_pDevice11->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL,
		&SHADER_POINTER.pPixelShader)))//(out)�s�N�Z���V�F�[�_.
	{
		MessageBox(NULL, ShaderName, "ps�쐬���s", MB_OK);
	}

	SAFE_RELEASE(pCompiledShader);//�u���u���.
}

//�X�L�����b�V���p�̃V�F�[�_���쐬.
void ShaderGathering::CreateSkinShader()
{
	/*================/�V�F�[�_�쐬 /================*/

	char* ShaderName = nullptr;
	SHADER_ENTRY_POINT EntryPoint;

	for (int i = 0; i < enSkinModelShader_Max; i++)
	{
		switch (i)
		{
		case enSkinModelShader_NoTex:
			//�e�N�X�`�������̃V�F�[�_.
			ShaderName = "Shader\\MeshSkinNoTexture.hlsl";
			EntryPoint.pVertexShaderEntryPointName = "VS";
			EntryPoint.pPixelShaderEntryPointName = "PS";

			break;
		case enSkinModelShader_Simple:
			//�G�t�F�N�g�������Ȃ��V�F�[�_.
			ShaderName = "Shader\\MeshSkinSimple.hlsl";
			EntryPoint.pVertexShaderEntryPointName = "VS";
			EntryPoint.pPixelShaderEntryPointName = "PS";

			break;
		case enSkinModelShader_Depth:
			//�[�x�e�N�X�`���쐬�p�̃V�F�[�_.
			ShaderName = "Shader\\MeshSkinDepth.hlsl";
			EntryPoint.pVertexShaderEntryPointName = "VS";
			EntryPoint.pPixelShaderEntryPointName = "PS";

			break;
		default:
			break;
		}

		MakeSkinMeshShader(ShaderName, EntryPoint, m_SkinMeshShader[i]);
	}

	/*================/�R���X�^���g�o�b�t�@�쐬 /================*/

	for (int i = 0; i < enSkinModelCBuffer_Max; i++)
	{
		UINT uiBufferSize;

		switch (i)
		{
		case enSkinModelCBuffer_Mesh:
			uiBufferSize = sizeof(CBUFF_SKINMESH_FOR_MESH);

			break;
		default:
			break;
		}

		MakeConstantBuffer(m_pSkinCBuffers[i], uiBufferSize);
	}
}

//�V�F�[�_�쐬(�X�L�����b�V��).
void ShaderGathering::MakeSkinMeshShader(char* ShaderName, SHADER_ENTRY_POINT EntryPoint, SHADER_POINTER& SHADER_POINTER)
{
	//D3D11�֘A�̏�����.
	ID3DBlob *pCompiledShader = NULL;
	ID3DBlob *pErrors = NULL;
	UINT	uCompileFlag = 0;

#ifdef _DEBUG
	uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;

#endif//#ifdef _DEBUG.
	
	//�u���u����o�[�e�b�N�X�V�F�[�_�[�쐬.
	if (FAILED(D3DX11CompileFromFile(ShaderName, NULL, NULL, EntryPoint.pVertexShaderEntryPointName, "vs_5_0", uCompileFlag, 0, NULL, &pCompiledShader, &pErrors, NULL)))
	{
		int size = pErrors->GetBufferSize();
		char* ch = (char*)pErrors->GetBufferPointer();
		MessageBox(0, "hlsl�ǂݍ��ݎ��s", NULL, MB_OK);
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(m_pDevice11->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &SHADER_POINTER.pVertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "�o�[�e�b�N�X�V�F�[�_�[�쐬���s", NULL, MB_OK);
	}
	//���_�C���v�b�g���C�A�E�g���`.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//���_�C���v�b�g���C�A�E�g���쐬.
	if (FAILED(m_pDevice11->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &SHADER_POINTER.pVertexLayout)))
	{
	}
	//���_�C���v�b�g���C�A�E�g���Z�b�g.
	m_pDeviceContext11->IASetInputLayout(SHADER_POINTER.pVertexLayout);

	//�u���u����s�N�Z���V�F�[�_�[�쐬.
	if (FAILED(D3DX11CompileFromFile(ShaderName, NULL, NULL, EntryPoint.pPixelShaderEntryPointName, "ps_5_0", uCompileFlag, 0, NULL, &pCompiledShader, &pErrors, NULL)))
	{
		MessageBox(0, "hlsl�ǂݍ��ݎ��s", NULL, MB_OK);
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(m_pDevice11->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &SHADER_POINTER.pPixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "�s�N�Z���V�F�[�_�[�쐬���s", NULL, MB_OK);
	}
	SAFE_RELEASE(pCompiledShader);
}
