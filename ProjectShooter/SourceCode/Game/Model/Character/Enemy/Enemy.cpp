#include "Enemy.h"

#include "..\\..\\..\\Sound\\SoundManager.h"

#include "..\\..\\..\\..\\Singleton.h"

const float SCALE = 0.08f;

//���f���̌����̒���.
const int ADJUST_MODEL_ORIENTATION = 0;

//���f���̃A�j���[�V�������x.
const double ANIMATION_SPEED = 0.01f;

//���f���̈ړ����x.
const float MOVE_SPEED = 0.12f;

//�U�����̈ړ����x.
const float ATTACK_MOVE_SPEED = 0.24f;

//�����蔻��̑傫��.
const float RADIUS = 1.0f;

//�U���ɂ��鋗��.
const float IN_ATTACK_RANGE = 10.0f;

//�����ʒu.
const D3DXVECTOR3 POSITION = { 0.0f, 0.0f, 20.0f };

Enemy::Enemy(clsD3DXSKINMESH* const pModel)
	: m_fTargetDirection(0.0f)
	, m_fAttackDistance(0.0f)
{
	AttachModel(pModel);

	CrearVECTOR3(m_vPlayerPosition);

	AddYaw(ADJUST_MODEL_ORIENTATION);

	//�����ʒu.
	m_vPos = POSITION;

	m_vOldPos = m_vPos;

	CrearVECTOR3(m_vRot);

	//�T�C�Y.
	m_fScale = SCALE;

	//�A�j���[�V�������x.
	SetAnimationSpeed(ANIMATION_SPEED);

	ChangeAnimation(5);

	m_Collision.fRadius = RADIUS;

	m_State.iHp = m_iMaxHp;
	m_State.bDead = false;

	m_pEffect = clsEffects::GetInstance();
}

Enemy::~Enemy()
{
	DetatchModel();

	m_pEffect->StopAll();
}

//���C�ƃ��b�V���̏Փˎ�.
void Enemy::RayHitToMesh(clsDX9Mesh* const pTarget)
{
	if (fabs(m_vPos.x) < MOVE_LIMIT &&
		fabs(m_vPos.z) < MOVE_LIMIT)
	{
		m_vOldPos = m_vPos;
		m_vPos.y = m_vOldPos.y;
	}
	else
	{
		if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_EnemyFall) &&
			Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_EnemyFallVoice))
		{
			//���ʂ𒲐�.
			Singleton<SoundManager>().GetInstance().SetSEVolume(SoundManager::enSE_EnemyFallVoice, 1000);
			Singleton<SoundManager>().GetInstance().SetSEVolume(SoundManager::enSE_EnemyFall, 600);

			Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_EnemyFallVoice);
			Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_EnemyFall);
		}

		m_vPos.y -= 0.05f;

		if (m_vPos.y < -2.0f)
		{
			m_State.bDead = true;
		}
	}
}

//Sphere�Ƃ̏Փˎ�.
void Enemy::HitToSphere()
{
	if (m_Action != enAction::Attack
		&& m_Action != enAction::Hit
		&& m_Action != enAction::Dead)
	{
		m_State.iHp--;
		SetAction(enAction::Hit);
	}
	else if (m_Action == enAction::Attack || m_Action == enAction::Hit)
	{
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_EnemyGuard);
	}
}

void Enemy::UpdateProduct()
{
}

void Enemy::Wait()
{
	const float fTurnSpeed = 0.015f;
	m_fTargetDirection += static_cast<float>D3DXToRadian(ADJUST_MODEL_ORIENTATION);
	SpinModel(m_fTargetDirection, fTurnSpeed);

	D3DXVECTOR3 vVec = m_vPlayerPosition - m_vPos;
	float fLength = D3DXVec3Length(&vVec);

	if (fabs(m_vRot.y - m_fTargetDirection) <= D3DXToRadian(2))
	{
		if (fLength <= IN_ATTACK_RANGE)
		{
			SetAction(enAction::Attack);
		}
		else
		{
			SetAction(enAction::Move);
		}
	}
}

void Enemy::Move()
{
	const float fTurnSpeed = 0.1f;
	m_fTargetDirection += static_cast<float>D3DXToRadian(ADJUST_MODEL_ORIENTATION);
	SpinModel(m_fTargetDirection, fTurnSpeed);

	MoveFrontRear(-MOVE_SPEED);

	D3DXVECTOR3 vVec = m_vPlayerPosition - m_vPos;
	float fLength = D3DXVec3Length(&vVec);

	if (fabs(m_vRot.y - m_fTargetDirection) <= D3DXToRadian(2))
	{
		if (fLength <= IN_ATTACK_RANGE)
		{
			SetAction(enAction::Attack);
		}
	}
}

void Enemy::Attack()
{
	MoveFrontRear(-ATTACK_MOVE_SPEED);
	m_fAttackDistance += ATTACK_MOVE_SPEED;

	const float fAttackMaxDistance = 15.0f;
	if (m_fAttackDistance >= fAttackMaxDistance)
	{
		SetAction(enAction::Wait);
	}
}

void Enemy::Hit()
{
	if (m_State.iHp <= 0)
	{
		SetAction(enAction::Dead);
	}

	const float fRatio = 0.5f;

	if (IsAnimationRatioEnd(fRatio))
	{
		SetAction(enAction::Wait);
	}
}

void Enemy::Dead()
{
	const float fRatio = 0.5f;

	if (IsAnimationRatioEnd(fRatio))
	{
		//�A�j���[�V�������x.
		SetAnimationSpeed(0);

		m_State.bDead = true;
	}
}

//�A�N�V�����̐؂�ւ�.
void Enemy::SetAction(const enAction Action)
{
	if (m_Action == Action)return;

	m_Action = Action;

	//�A�j���[�V�����̑��x�������l�ɖ߂�.
	SetAnimationSpeed(ANIMATION_SPEED);

	//�G�t�F�N�g�̍Đ��ʒu�𒲐�����.
	const float fAdjustPosition = 1.5f;
	D3DXVECTOR3 vEffectPos = m_vPos;
	vEffectPos.y += fAdjustPosition;

	//�G�t�F�N�g�̍Đ����~�߂�.
	m_pEffect->Stop(m_HitHandle);
	m_pEffect->Stop(m_DeadHandle);

	switch (Action)
	{
	case enAction::Wait:
		ChangeAnimation(5);
		m_fAttackDistance = 0.0f;

		break;
	case enAction::Move:
		ChangeAnimation(3);

		break;
	case enAction::Attack:
		ChangeAnimation(7);

		{
			const int iAttackVolume = 400;
			Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_EnemyAttack, iAttackVolume);
			Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_EnemyAttackVoice);
		}

		break;
	case enAction::Hit:
		ChangeAnimation(2);
		//�A�j���[�V�����𑁂�����.
		{
			const double dHitAnimationSpeed = 0.02;
			SetAnimationSpeed(dHitAnimationSpeed);
		}

		//SE���Đ�.
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_EnemyDamage);
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_EnemyDamageVoice);

		//�G�t�F�N�g���Đ�.
		m_HitHandle = m_pEffect->Play(vEffectPos, clsEffects::enEfcType_PlayerHit);
		{
			const float fHitEffectScale = 1.0f;
			m_pEffect->SetScale(m_HitHandle, { fHitEffectScale, fHitEffectScale, fHitEffectScale });
		}
		break;
	case enAction::Dead:
		ChangeAnimation(4);
		//�A�j���[�V������x������.
		{
			const double dDeadAnimationSpeed = 0.005;
			SetAnimationSpeed(dDeadAnimationSpeed);
		}

		//SE���Đ�.
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_EnemyDead);
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_EnemyDeadVoice);

		//�G�t�F�N�g���Đ�.
		//m_DeadHandle = m_pEffect->Play(vEffectPos, clsEffects::enEfcType_EnemyDead);
		//{
		//	const float fDeadEffectScale = 0.01f;
		//	m_pEffect->SetScale(m_DeadHandle, { fDeadEffectScale, fDeadEffectScale, fDeadEffectScale });
		//}

		break;
	default:
		break;
	}
}