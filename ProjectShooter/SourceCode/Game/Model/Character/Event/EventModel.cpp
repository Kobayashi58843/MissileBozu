#include "EventModel.h"

//�����ʒu.
const D3DXVECTOR3 POSITION = { 0.0f, 0.0f, 0.0f };

EventModel::EventModel(clsD3DXSKINMESH* const pModel, const float fScale, const double dAnimSpeed)
	: m_ModelState()
{
	AttachModel(pModel);

	//�ʒu.
	m_vPos = POSITION;

	m_vRot = { 0.0f, 0.0f, 0.0f };

	//�T�C�Y.
	m_fScale = fScale;

	//�A�j���[�V�������x.
	SetAnimationSpeed(dAnimSpeed);
}

EventModel::~EventModel()
{
	DetatchModel();
}

//3D���f���̕`��.
void EventModel::RenderModel(const D3DXMATRIX &mView, const D3DXMATRIX &mProj)
{
	//�ʒu,��],�T�C�Y�����f���ɓK��.
	UpdateState();

	m_ModelState.dAnimationTime += m_ModelState.dAnimationSpeed;
	m_ModelState.AnimationController->AdvanceTime(m_ModelState.dAnimationSpeed, nullptr);

	m_pModel->Render(mView, mProj, m_ModelState.AnimationController);
}

//���f���f�[�^�̊֘A�t��.
void EventModel::AttachModel(clsD3DXSKINMESH* const pModel)
{
	assert(pModel != nullptr);

	m_pModel = pModel;

	LPD3DXANIMATIONCONTROLLER pAC = m_pModel->GetAnimController();

	pAC->CloneAnimationController(
		pAC->GetMaxNumAnimationOutputs(),
		pAC->GetMaxNumAnimationSets(),
		pAC->GetMaxNumTracks(),
		pAC->GetMaxNumEvents(),
		&m_ModelState.AnimationController);

	m_pModel->SetAnimSpeed(1);
}

//���f���f�[�^�֘A�t������.
void EventModel::DetatchModel()
{
	if (m_pModel != nullptr)
	{
		m_pModel = nullptr;

		if (m_ModelState.AnimationController != nullptr)
		{
			m_ModelState.AnimationController->Release();
			m_ModelState.AnimationController = nullptr;
		}
	}
}

//�A�j���[�V�����؂�ւ��֐�.
void EventModel::ChangeAnimation(const int iIndex, const double dStartPos)
{
	//�A�j���[�V�����͈͓̔����`�F�b�N.
	if (iIndex < 0 || iIndex >= GetAnimationQuantityMax())return;

	m_pModel->ChangeAnimSet_StartPos(iIndex, dStartPos, m_ModelState.AnimationController);

	m_ModelState.iIndex = iIndex;

	m_ModelState.dAnimationTime = dStartPos;
}

//���݂̃A�j���[�V�����̏I�����m�F����֐�.
bool EventModel::IsAnimationEnd()
{
	if (m_pModel->GetAnimPeriod(m_ModelState.iIndex) - m_ModelState.dAnimationSpeed < m_ModelState.dAnimationTime)
	{
		return true;
	}

	return false;
}

//���݂̃A�j���[�V�����������I�����������m�F����֐�.
bool EventModel::IsAnimationRatioEnd(const float fRatio)
{
	if ((m_pModel->GetAnimPeriod(m_ModelState.iIndex) - m_ModelState.dAnimationSpeed) * fRatio < m_ModelState.dAnimationTime)
	{
		return true;
	}

	return false;
}

//�A�j���[�V�����̍Đ����x��ς���.
void EventModel::SetAnimationSpeed(const double dAnimationSpeed)
{
	m_ModelState.dAnimationSpeed = dAnimationSpeed;
	m_pModel->SetAnimSpeed(m_ModelState.dAnimationSpeed);
}

//�A�j���[�V�����ő吔���擾.
int EventModel::GetAnimationQuantityMax()
{
	if (m_ModelState.AnimationController != nullptr)
	{
		return static_cast<int>(m_ModelState.AnimationController->GetMaxNumAnimationSets());
	}
	return -1;
}

//�ʒu,��],�T�C�Y�������f���ɓK��.
void EventModel::UpdateState()
{
	m_pModel->m_Trans.vPos = m_vPos;

	m_pModel->m_Trans.fPitch = m_vRot.x;
	m_pModel->m_Trans.fYaw = m_vRot.y;
	m_pModel->m_Trans.fRoll = m_vRot.z;

	m_pModel->m_Trans.vScale = { m_fScale, m_fScale, m_fScale };

	//�����蔻��pSphere�̈ʒu�����킹��.
	D3DXVECTOR3 vSpherePos = m_vPos;
	//Sphere�̏ꏊ����ɂ��炷.
	const float fAdjustmentPositionY = 1.0f;
	vSpherePos.y += m_Collision.fRadius + fAdjustmentPositionY;
	m_Collision.vCenter = vSpherePos;
}

