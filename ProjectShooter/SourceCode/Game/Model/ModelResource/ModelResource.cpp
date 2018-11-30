#include"ModelResource.h"

/*/========/ �R���X�g���N�^ /========/*/
ModelResource::ModelResource()
	: m_hWnd(nullptr)
	, m_pDevice11(nullptr)
	, m_pContext11(nullptr)
	, m_ppStaticModels(nullptr)
	, m_ppSkinModels(nullptr)
{
}

/*/========/ �f�X�g���N�^ /========/*/
ModelResource::~ModelResource()
{
	//���f���̉��.
	ReleaceModelAll();

	//�|�C���^�̈���������.
	if (m_ppStaticModels)
	{
		delete[] m_ppStaticModels;
		m_ppStaticModels = nullptr;
	}

	if (m_ppSkinModels)
	{
		delete[] m_ppSkinModels;
		m_ppSkinModels = nullptr;
	}

	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
	m_hWnd = nullptr;
}

//�S�Ẵ��f���̍쐬.
void ModelResource::CreateModelAll(const HWND hWnd, ID3D11Device* const pDevice, ID3D11DeviceContext* const pContext)
{
	m_hWnd = hWnd;
	m_pDevice11 = pDevice;
	m_pContext11 = pContext;

	CreateStaticModelAll(m_hWnd, m_pDevice11, m_pContext11);
	CreateSkinModelAll(m_hWnd, m_pDevice11, m_pContext11);
}

//�S�Ẵ��f���̉��.
void ModelResource::ReleaceModelAll()
{
	for (int i = 0; i < enStaticModel_Max; i++)
	{
		ReleaseStaticModel(static_cast<enStaticModel>(i));
	}
	for (int i = 0; i < enSkinModel_Max; i++)
	{
		ReleaseSkinModel(static_cast<enSkinModel>(i));
	}
}

/*/========================================/ �X�^�e�B�b�N���f�� /========================================/*/
void ModelResource::CreateStaticModelAll(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	InitStaticModel(hWnd, pDevice, pContext);
	CreateStaticModel("Data\\Ground\\Stage.X", enStaticModel_Ground);
	CreateStaticModel("Data\\Collision\\Sphere.X", enStaticModel_Shpere);
}

HRESULT ModelResource::InitStaticModel(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_hWnd = hWnd;
	m_pDevice11 = pDevice;
	m_pContext11 = pContext;

	//�ÓI���b�V���̃|�C���^�̃|�C���^�̈���m��.
	m_ppStaticModels = new clsDX9Mesh*[enStaticModel_Max];
	for (int iStModelNo = 0; iStModelNo<enStaticModel_Max; iStModelNo++)
	{
		//nullptr�ŏ�����.
		m_ppStaticModels[iStModelNo] = nullptr;
	}

	return S_OK;
}

HRESULT ModelResource::CreateStaticModel(LPSTR fileName, enStaticModel enModel)
{
	//�񋓑͈͓̂̔���.
	if (!IsRengeStaticModel(enModel))
	{
		return E_FAIL;
	}

	//���Ƀf�[�^�����邩.
	if (m_ppStaticModels[enModel] != nullptr)
	{
		return E_FAIL;
	}

	//���f���Ǎ�.
	m_ppStaticModels[enModel] = new clsDX9Mesh;
	m_ppStaticModels[enModel]->Init(m_hWnd, m_pDevice11, m_pContext11, fileName);

	return S_OK;
}

HRESULT ModelResource::ReleaseStaticModel(enStaticModel enModel)
{
	//�񋓑͈͓̂̔���.
	if (!IsRengeStaticModel(enModel))
	{
		return E_FAIL;
	}

	//���Ƀf�[�^���Ȃ��Ȃ��Ă邩.
	if (m_ppStaticModels[enModel] == nullptr)
	{
		return E_FAIL;
	}

	//�폜����nullptr�Ŗ��߂�.)
	m_ppStaticModels[enModel]->Release();

	delete m_ppStaticModels[enModel];
	m_ppStaticModels[enModel] = nullptr;

	return S_OK;
}

clsDX9Mesh* ModelResource::GetStaticModels(enStaticModel enModel)
{
	//�͈͓����`�F�b�N.
	if (!IsRengeStaticModel(enModel))
	{
		return nullptr;
	}

	//���f�������邩�`�F�b�N.
	if (m_ppStaticModels[enModel] == nullptr)
	{
		return nullptr;
	}

	//���f���̃|�C���^��Ԃ�.
	return m_ppStaticModels[enModel];
}

//�͈͓����ǂ����`�F�b�N���� : �͈͓��Ɏ��܂��Ă���true.
bool ModelResource::IsRengeStaticModel(enStaticModel enModel)
{
	if (0 <= enModel && enModel < enStaticModel_Max)
	{
		return true;
	}
	return false;
}

/*/=========================================/ �X�L�����f�� /=========================================/*/
void ModelResource::CreateSkinModelAll(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	InitSkinModel(hWnd, pDevice, pContext);
	CreateSkinModel("Data\\Player\\Bozu.x", enSkinModel_Player);
	CreateSkinModel("Data\\Enemy\\Josin01.x", enSkinModel_Enemy);
}

HRESULT ModelResource::InitSkinModel(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//�ÓI���b�V���̃|�C���^�̃|�C���^�̈���m��.
	m_ppSkinModels = new clsD3DXSKINMESH*[enSkinModel_Max];
	for (int iStModelNo = 0; iStModelNo<enSkinModel_Max; iStModelNo++)
	{
		//nullptr�ŏ�����.
		m_ppSkinModels[iStModelNo] = nullptr;
	}

	return S_OK;
}

HRESULT ModelResource::CreateSkinModel(LPSTR fileName, enSkinModel enModel)
{
	//�񋓑͈͓̂̔���.
	if (!IsRengeSkinModel(enModel))
	{
		return E_FAIL;
	}

	//���Ƀf�[�^�����邩.
	if (m_ppSkinModels[enModel] != nullptr)
	{
		return E_FAIL;
	}

	CD3DXSKINMESH_INIT	si;
	si.hWnd = m_hWnd;
	si.pDevice = m_pDevice11;
	si.pDeviceContext = m_pContext11;

	//���f���Ǎ�.
	m_ppSkinModels[enModel] = new clsD3DXSKINMESH;
	m_ppSkinModels[enModel]->Init(&si);
	m_ppSkinModels[enModel]->CreateFromX(fileName);

	return S_OK;
}

HRESULT ModelResource::ReleaseSkinModel(enSkinModel enModel)
{
	//�񋓑͈͓̂̔���.
	if (!IsRengeSkinModel(enModel))
	{
		return E_FAIL;
	}

	//���Ƀf�[�^���Ȃ��Ȃ��Ă邩.
	if (m_ppSkinModels[enModel] == nullptr)
	{
		return E_FAIL;
	}

	//�폜����nullptr�Ŗ��߂�.
	delete m_ppSkinModels[enModel];
	m_ppSkinModels[enModel] = nullptr;

	return S_OK;
}

clsD3DXSKINMESH* ModelResource::GetSkinModels(enSkinModel enModel)
{
	//�͈͓����`�F�b�N.
	if (!IsRengeSkinModel(enModel))
	{
		return nullptr;
	}

	//���f�������邩�`�F�b�N.
	if (m_ppSkinModels[enModel] == nullptr)
	{
		return nullptr;
	}

	//���f���̃|�C���^��Ԃ�.
	return m_ppSkinModels[enModel];
}

//�͈͓����ǂ����`�F�b�N���� : �͈͓��Ɏ��܂��Ă���true.
bool ModelResource::IsRengeSkinModel(enSkinModel enModel)
{
	if (0 <= enModel && enModel < enSkinModel_Max)
	{
		return true;
	}
	return false;
}