#include "Player.h"

#include "..\\..\\..\\Sound\\SoundManager.h"

#include "..\\..\\..\\..\\Singleton.h"

//���f���̑傫��.
const float SCALE = 0.0005f;

//���f���̌����̒���.
const int ADJUST_MODEL_ORIENTATION = 180;

//���f���̃A�j���[�V�������x.
const double ANIMATION_SPEED = 0.01f;

//���f���̈ړ����x.
const float MOVE_SPEED = 0.08f;

//�����蔻��̑傫��.
const float RADIUS = 0.6f;

//�����ʒu.
const D3DXVECTOR3 POSITION = { 0.0f, 0.0f, -20.0f };

//�ړ��\�͈�.
const float MOVE_LIMIT = 30.0f;

Player::Player(clsD3DXSKINMESH* const pModel, BulletManager* const pBulletManager)
	: m_bPushedMoveButtonFlg(false)
	, m_fTargetDirection(0.0f)
{
	AttachModel(pModel);

	AddYaw(ADJUST_MODEL_ORIENTATION);

	//�����ʒu.
	m_vPos = POSITION;

	m_vOldPos = m_vPos;

	//�T�C�Y.
	m_fScale = SCALE;

	//�A�j���[�V�������x.
	SetAnimationSpeed(ANIMATION_SPEED);

	SetAction(enAction::Wait);

	m_Collision.fRadius = RADIUS;

	m_pBulletManager = pBulletManager;

	m_State.iHp = m_iMaxHp;
	m_State.bDead = false;

	m_pEffect = clsEffects::GetInstance();

	//�N����1��ڂ̃G�t�F�N�g���Đ�����Ȃ����߂��̑Ή��Ƃ���1��Đ����Ă���.
	m_HitHandle = m_pEffect->Play({0.0f,0.0f,0.0f}, clsEffects::enEfcType_PlayerHit);
	m_pEffect->Stop(m_HitHandle);
}

Player::~Player()
{
	DetatchModel();

	m_pEffect->StopAll();
}

//���C�ƃ��b�V���̏Փˎ�.
void Player::RayHitToMesh(clsDX9Mesh* const pTarget)
{
	if (IsRayHit(pTarget))
	{
		//��_�̍��W����y���W�����@��y���W�Ƃ��ăZ�b�g.
		m_vPos.y = GetRayIntersect().y;
	}
}

//Sphere�Ƃ̏Փˎ�.
void Player::HitToSphere(SPHERE const TargetSphere)
{
	if (SphereCollision(m_Collision, TargetSphere))
	{
		if (m_Action != enAction::Hit
			&& m_Action != enAction::Dead)
		{
			SetAction(enAction::Hit);
		}
	}
}

void Player::UpdateProduct()
{
}

void Player::Wait()
{
	//�ړ��̓��͂��󂯕t����.
	AcceptedMoveButton();

	//�U���̓��͂��󂯕t����.
	AcceptedAttackButton();
}

void Player::Move()
{
	const float fTurnSpeed = 0.1f;
	m_fTargetDirection += static_cast<float>D3DXToRadian(ADJUST_MODEL_ORIENTATION);
	SpinModel(m_fTargetDirection, fTurnSpeed);

	//�ړ��̓��͂��󂯕t����.
	AcceptedMoveButton();

	//�U���̓��͂��󂯕t����.
	AcceptedAttackButton();

	//�ړ��L�[��������Ă��Ȃ��̂őҋ@��.
	if (!m_bPushedMoveButtonFlg)
	{
		SetAction(enAction::Wait);
	}
}

void Player::Attack()
{
	const float fTurnSpeed = 0.4f;
	m_fTargetDirection += static_cast<float>D3DXToRadian(ADJUST_MODEL_ORIENTATION);
	SpinModel(m_fTargetDirection, fTurnSpeed);

	const float fRatio = 0.5f;
	if (IsAnimationRatioEnd(fRatio))
	{
		SetAction(enAction::Wait);
	}
}

void Player::Hit()
{
	if (0 >= m_State.iHp)
	{
		SetAction(enAction::Dead);
	}

	const float fRatio = 0.5f;

	if (IsAnimationRatioEnd(fRatio))
	{
		SetAction(enAction::Wait);
	}
}

void Player::Dead()
{
	const float fRatio = 0.5f;

	if (IsAnimationRatioEnd(fRatio))
	{
		m_State.bDead = true;
	}
}

//�A�N�V�����̐؂�ւ�.
void Player::SetAction(const enAction Action)
{
	if (m_Action == Action)return;

	m_Action = Action;

	//�A�j���[�V�����̑��x�������l�ɖ߂�.
	SetAnimationSpeed(ANIMATION_SPEED);

	//�G�t�F�N�g�̍Đ��ʒu�𒲐�����.
	const float fAdjustPosition = 1.0f;
	D3DXVECTOR3 vEffectPos = m_vPos;
	vEffectPos.y += fAdjustPosition;

	//�G�t�F�N�g�̍Đ����~�߂�.
	m_pEffect->Stop(m_HitHandle);
	m_pEffect->Stop(m_DeadHandle);

	switch (Action)
	{
	case enAction::Wait:
		ChangeAnimation(1);

		break;
	case enAction::Move:
		ChangeAnimation(0);
		m_bPushedMoveButtonFlg = true;

		break;
	case enAction::Attack:
		ChangeAnimation(7);

		//SE���Đ�.
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_PlayerAttack);
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_PlayerAttackVoice);

		break;
	case enAction::Hit:
		ChangeAnimation(3);
		//�A�j���[�V������x������.
		{
			const double dDeadAnimationSpeed = 0.006;
			SetAnimationSpeed(dDeadAnimationSpeed);
		}

		m_State.iHp--;

		//SE���Đ�.
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_PlayerDamage);
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_PlayerDamageVoice);

		//�G�t�F�N�g���Đ�.
		m_HitHandle = m_pEffect->Play(vEffectPos, clsEffects::enEfcType_PlayerHit);
		{
			const float fHitEffectScale = 0.5f;
			m_pEffect->SetScale(m_HitHandle, { fHitEffectScale, fHitEffectScale, fHitEffectScale });
		}
		break;
	case enAction::Dead:
		ChangeAnimation(5);
		//�A�j���[�V������x������.
		{
			const double dDeadAnimationSpeed = 0.005;
			SetAnimationSpeed(dDeadAnimationSpeed);
		}

		//SE���Đ�.
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_PlayerDead);
		Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_PlayerDeadVoice);
		
		//�G�t�F�N�g���Đ�.
		m_DeadHandle = m_pEffect->Play(vEffectPos, clsEffects::enEfcType_PlayerDead);
		{
			const float fDeadEffectScale = 0.2f;
			m_pEffect->SetScale(m_DeadHandle, { fDeadEffectScale, fDeadEffectScale, fDeadEffectScale });
		}
		break;
	default:
		break;
	}
}

//�ړ��̓��͂��󂯕t���邩�ǂ���.
void Player::AcceptedMoveButton()
{
	bool bNotPushedFrontRear = false;
	bool bNotPushedLeftRight = false;

	if (GetKeyState('W') & 0x8000)
	{
		SetAction(enAction::Move);
		MoveFrontRear(-MOVE_SPEED);
	}
	else if (GetKeyState('S') & 0x8000)
	{
		SetAction(enAction::Move);
		MoveFrontRear(MOVE_SPEED);
	}
	else
	{
		bNotPushedFrontRear = true;
	}

	if (GetKeyState('A') & 0x8000)
	{
		SetAction(enAction::Move);
		MoveLeftRight(MOVE_SPEED);
	}
	else if (GetKeyState('D') & 0x8000)
	{
		SetAction(enAction::Move);
		MoveLeftRight(-MOVE_SPEED);
	}
	else
	{
		bNotPushedLeftRight = true;
	}

	if (bNotPushedFrontRear && bNotPushedLeftRight)
	{
		m_bPushedMoveButtonFlg = false;
	}

	//�ړ�����.
	MoveLimit();
}

//�ړ�����.
void Player::MoveLimit()
{
	//const float fDistance = D3DXVec3Length(&m_vPos);

	//if (fabs(fDistance) < MOVE_LIMIT)
	//{
	//	m_vOldPos = m_vPos;
	//}
	//else
	//{
	//	m_vPos = m_vOldPos;
	//	if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_PlayerMoveLimit))
	//	{
	//		//SE���Đ�.
	//		Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_PlayerMoveLimit);

	//		//�G�t�F�N�g���Đ�.
	//		m_WallHitHandle = m_pEffect->Play(m_vPos, clsEffects::enEfcType_PlayerWallHit);

	//		const float fEffectScale = 0.5f;
	//		m_pEffect->SetScale(m_WallHitHandle, { fEffectScale, fEffectScale, fEffectScale });

	//		const float fYaw = m_vRot.y - static_cast<float>D3DXToRadian(90);
	//		m_pEffect->SetRotation(m_WallHitHandle, { 0.0f, fYaw, 0.0f });
	//	}
	//}

	if (fabs(m_vPos.x) < MOVE_LIMIT)
	{
		m_vOldPos.x = m_vPos.x;
	}
	else
	{
		m_vPos.x = m_vOldPos.x;

		if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_PlayerMoveLimit))
		{
			//SE���Đ�.
			Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_PlayerMoveLimit);

			//�G�t�F�N�g���Đ�.
			m_WallHitHandle = m_pEffect->Play(m_vPos, clsEffects::enEfcType_PlayerWallHit);

			float fEffectScale = 0.5f;
			m_pEffect->SetScale(m_WallHitHandle, { fEffectScale, fEffectScale, fEffectScale });

			float fYaw = static_cast<float>D3DXToRadian(0);
			m_pEffect->SetRotation(m_WallHitHandle, { 0.0f, fYaw, 0.0f });
		}
	}

	if (fabs(m_vPos.z) < MOVE_LIMIT)
	{
		m_vOldPos.z = m_vPos.z;
	}
	else
	{
		m_vPos.z = m_vOldPos.z;

		if (Singleton<SoundManager>().GetInstance().IsStoppedFirstSE(SoundManager::enSE_PlayerMoveLimit))
		{
			//SE���Đ�.
			Singleton<SoundManager>().GetInstance().PlayFirstSE(SoundManager::enSE_PlayerMoveLimit);

			//�G�t�F�N�g���Đ�.
			m_WallHitHandle = m_pEffect->Play(m_vPos, clsEffects::enEfcType_PlayerWallHit);

			float fEffectScale = 0.5f;
			m_pEffect->SetScale(m_WallHitHandle, { fEffectScale, fEffectScale, fEffectScale });

			float fYaw = static_cast<float>D3DXToRadian(90);
			m_pEffect->SetRotation(m_WallHitHandle, { 0.0f, fYaw, 0.0f });
		}
	}
}

//�U���̓��͂��󂯕t���邩�ǂ���.
void Player::AcceptedAttackButton()
{
	if (m_pBulletManager->IsShot())
	{
		SetAction(enAction::Attack);
	}
}