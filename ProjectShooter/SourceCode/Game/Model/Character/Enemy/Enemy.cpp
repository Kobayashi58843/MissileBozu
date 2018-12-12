#include "Enemy.h"

#include "..\\..\\..\\Sound\\SoundManager.h"

#include "..\\..\\..\\..\\Singleton.h"

const float SCALE = 0.08f;

//モデルの向きの調整.
const int ADJUST_MODEL_ORIENTATION = 0;

//モデルのアニメーション速度.
const double ANIMATION_SPEED = 0.01f;

//モデルの移動速度.
const float MOVE_SPEED = 0.12f;

//攻撃時の移動速度.
const float ATTACK_MOVE_SPEED = 0.24f;

//当たり判定の大きさ.
const float RADIUS = 1.0f;

//攻撃にうつる距離.
const float IN_ATTACK_RANGE = 10.0f;

//初期位置.
const D3DXVECTOR3 POSITION = { 0.0f, 0.0f, 20.0f };

Enemy::Enemy(clsD3DXSKINMESH* const pModel)
	: m_fTargetDirection(0.0f)
	, m_fAttackDistance(0.0f)
{
	AttachModel(pModel);

	CrearVECTOR3(m_vPlayerPosition);

	AddYaw(ADJUST_MODEL_ORIENTATION);

	//初期位置.
	m_vPos = POSITION;

	m_vOldPos = m_vPos;

	CrearVECTOR3(m_vRot);

	//サイズ.
	m_fScale = SCALE;

	//アニメーション速度.
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

//レイとメッシュの衝突時.
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
			//音量を調整.
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

//Sphereとの衝突時.
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
		//アニメーション速度.
		SetAnimationSpeed(0);

		m_State.bDead = true;
	}
}

//アクションの切り替え.
void Enemy::SetAction(const enAction Action)
{
	if (m_Action == Action)return;

	m_Action = Action;

	//アニメーションの速度を初期値に戻す.
	SetAnimationSpeed(ANIMATION_SPEED);

	//エフェクトの再生位置を調整する.
	const float fAdjustPosition = 1.5f;
	D3DXVECTOR3 vEffectPos = m_vPos;
	vEffectPos.y += fAdjustPosition;

	//エフェクトの再生を止める.
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
		//アニメーションを早くする.
		{
			const double dHitAnimationSpeed = 0.02;
			SetAnimationSpeed(dHitAnimationSpeed);
		}

		//SEを再生.
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_EnemyDamage);
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_EnemyDamageVoice);

		//エフェクトを再生.
		m_HitHandle = m_pEffect->Play(vEffectPos, clsEffects::enEfcType_PlayerHit);
		{
			const float fHitEffectScale = 1.0f;
			m_pEffect->SetScale(m_HitHandle, { fHitEffectScale, fHitEffectScale, fHitEffectScale });
		}
		break;
	case enAction::Dead:
		ChangeAnimation(4);
		//アニメーションを遅くする.
		{
			const double dDeadAnimationSpeed = 0.005;
			SetAnimationSpeed(dDeadAnimationSpeed);
		}

		//SEを再生.
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_EnemyDead);
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_EnemyDeadVoice);

		//エフェクトを再生.
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