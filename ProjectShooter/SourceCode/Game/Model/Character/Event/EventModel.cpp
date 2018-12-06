#include "EventModel.h"

//初期位置.
const D3DXVECTOR3 POSITION = { 0.0f, 0.0f, 0.0f };

EventModel::EventModel(clsD3DXSKINMESH* const pModel, const float fScale, const double dAnimSpeed)
	: m_ModelState()
{
	AttachModel(pModel);

	//位置.
	m_vPos = POSITION;

	m_vRot = { 0.0f, 0.0f, 0.0f };

	//サイズ.
	m_fScale = fScale;

	//アニメーション速度.
	SetAnimationSpeed(dAnimSpeed);
}

EventModel::~EventModel()
{
	DetatchModel();
}

//3Dモデルの描画.
void EventModel::RenderModel(const D3DXMATRIX &mView, const D3DXMATRIX &mProj)
{
	//位置,回転,サイズをモデルに適応.
	UpdateState();

	m_ModelState.dAnimationTime += m_ModelState.dAnimationSpeed;
	m_ModelState.AnimationController->AdvanceTime(m_ModelState.dAnimationSpeed, nullptr);

	m_pModel->Render(mView, mProj, m_ModelState.AnimationController);
}

//モデルデータの関連付け.
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

//モデルデータ関連付け解除.
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

//アニメーション切り替え関数.
void EventModel::ChangeAnimation(const int iIndex, const double dStartPos)
{
	//アニメーションの範囲内かチェック.
	if (iIndex < 0 || iIndex >= GetAnimationQuantityMax())return;

	m_pModel->ChangeAnimSet_StartPos(iIndex, dStartPos, m_ModelState.AnimationController);

	m_ModelState.iIndex = iIndex;

	m_ModelState.dAnimationTime = dStartPos;
}

//現在のアニメーションの終了を確認する関数.
bool EventModel::IsAnimationEnd()
{
	if (m_pModel->GetAnimPeriod(m_ModelState.iIndex) - m_ModelState.dAnimationSpeed < m_ModelState.dAnimationTime)
	{
		return true;
	}

	return false;
}

//現在のアニメーションが何割終了したかを確認する関数.
bool EventModel::IsAnimationRatioEnd(const float fRatio)
{
	if ((m_pModel->GetAnimPeriod(m_ModelState.iIndex) - m_ModelState.dAnimationSpeed) * fRatio < m_ModelState.dAnimationTime)
	{
		return true;
	}

	return false;
}

//アニメーションの再生速度を変える.
void EventModel::SetAnimationSpeed(const double dAnimationSpeed)
{
	m_ModelState.dAnimationSpeed = dAnimationSpeed;
	m_pModel->SetAnimSpeed(m_ModelState.dAnimationSpeed);
}

//アニメーション最大数を取得.
int EventModel::GetAnimationQuantityMax()
{
	if (m_ModelState.AnimationController != nullptr)
	{
		return static_cast<int>(m_ModelState.AnimationController->GetMaxNumAnimationSets());
	}
	return -1;
}

//位置,回転,サイズ等をモデルに適応.
void EventModel::UpdateState()
{
	m_pModel->m_Trans.vPos = m_vPos;

	m_pModel->m_Trans.fPitch = m_vRot.x;
	m_pModel->m_Trans.fYaw = m_vRot.y;
	m_pModel->m_Trans.fRoll = m_vRot.z;

	m_pModel->m_Trans.vScale = { m_fScale, m_fScale, m_fScale };

	//当たり判定用Sphereの位置を合わせる.
	D3DXVECTOR3 vSpherePos = m_vPos;
	//Sphereの場所を上にずらす.
	const float fAdjustmentPositionY = 1.0f;
	vSpherePos.y += m_Collision.fRadius + fAdjustmentPositionY;
	m_Collision.vCenter = vSpherePos;
}

