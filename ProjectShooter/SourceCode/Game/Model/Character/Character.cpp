#include "Character.h"

Character::Character()
	: m_pModel(nullptr)
	, m_ModelState()
{
	m_State.iHp = 0;
	m_State.bDead = false;

	m_Action = enAction::Wait;
}

Character::~Character()
{
}

//更新.
void Character::Update()
{
	UpdateProduct();

	switch (m_Action)
	{
	case enAction::Wait:
		Wait();

		break;
	case enAction::Move:
		Move();

		break;
	case enAction::Attack:
		Attack();

		break;
	case enAction::Hit:
		Hit();

		break;
	case enAction::Dead:
		Dead();

		break;
	default:
		break;
	}
}

//3Dモデルの描画.
void Character::RenderModel(const D3DXMATRIX &mView, const D3DXMATRIX &mProj)
{
	//位置,回転,サイズをモデルに適応.
	UpdateState();

	m_ModelState.dAnimationTime += m_ModelState.dAnimationSpeed;
	m_ModelState.AnimationController->AdvanceTime(m_ModelState.dAnimationSpeed, nullptr);

	m_pModel->Render(mView, mProj, m_ModelState.AnimationController);
}

//モデルのYawを使って前後に移動.
void Character::MoveFrontRear(const float fSpeed)
{
	D3DXVECTOR3 vMoveAxisZ = { 0.0f, 0.0f, 1.0f };

	D3DXMATRIX mMat;
	D3DXMatrixRotationY(&mMat, m_vRot.y);

	D3DXVec3TransformCoord(&vMoveAxisZ, &vMoveAxisZ, &mMat);

	m_vPos += vMoveAxisZ * fSpeed;
}

//モデルのYawを使って左右に移動.
void Character::MoveLeftRight(const float fSpeed)
{
	D3DXVECTOR3 vMoveAxisX = { 1.0f, 0.0f, 0.0f };

	D3DXMATRIX mMat;
	D3DXMatrixRotationY(&mMat, m_vRot.y);

	D3DXVec3TransformCoord(&vMoveAxisX, &vMoveAxisX, &mMat);

	m_vPos += vMoveAxisX * fSpeed;
}

//モデルデータの関連付け.
void Character::AttachModel(clsD3DXSKINMESH* const pModel)
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
void Character::DetatchModel()
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
void Character::ChangeAnimation(const int iIndex, const double dStartPos)
{
	//アニメーションの範囲内かチェック.
	if (iIndex < 0 || iIndex >= GetAnimationQuantityMax())return;

	m_pModel->ChangeAnimSet_StartPos(iIndex, dStartPos, m_ModelState.AnimationController);

	m_ModelState.iIndex = iIndex;

	m_ModelState.dAnimationTime = dStartPos;
}

//現在のアニメーションの終了を確認する関数.
bool Character::IsAnimationEnd()
{
	if (m_pModel->GetAnimPeriod(m_ModelState.iIndex) - m_ModelState.dAnimationSpeed < m_ModelState.dAnimationTime)
	{
		return true;
	}

	return false;
}

//現在のアニメーションが何割終了したかを確認する関数.
bool Character::IsAnimationRatioEnd(const float fRatio)
{
	if ((m_pModel->GetAnimPeriod(m_ModelState.iIndex) - m_ModelState.dAnimationSpeed) * fRatio < m_ModelState.dAnimationTime)
	{
		return true;
	}

	return false;
}

//アニメーションの再生速度を変える.
void Character::SetAnimationSpeed(const double dAnimationSpeed)
{
	m_ModelState.dAnimationSpeed = dAnimationSpeed;
	m_pModel->SetAnimSpeed(m_ModelState.dAnimationSpeed);
}

//アニメーション最大数を取得.
int Character::GetAnimationQuantityMax()
{
	if (m_ModelState.AnimationController != nullptr)
	{
		return static_cast<int>(m_ModelState.AnimationController->GetMaxNumAnimationSets());
	}
	return -1;
}

//位置,回転,サイズ等をモデルに適応.
void Character::UpdateState()
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

