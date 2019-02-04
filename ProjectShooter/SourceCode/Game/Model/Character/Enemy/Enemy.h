#pragma once

#include "..\\Character.h"

#include "..\\..\\..\\Effekseer\\CEffects.h"

class Enemy : public Character
{
public:
	Enemy(clsD3DXSKINMESH* const pModel);
	virtual ~Enemy();

	//�w�肵���ꏊ�̕����Ƀ��f������������.
	void DecideTargetDirection(const D3DXVECTOR3 &vPos)
	{
		m_vPlayerPosition = vPos;
		m_fTargetDirection = atan2f(m_vPos.x - vPos.x, m_vPos.z - vPos.z);
	}

	SPHERE GetCollisionSphere() const
	{
		return m_Collision;
	}

	bool IsDeadFlg() const
	{
		return m_State.bDead;
	}

	int GetHpMax() const
	{
		return m_iMaxHp;
	}
	int GetHp() const
	{
		return m_State.iHp;
	}

	//���C�ƃ��b�V���̏Փˎ�.
	void RayHitToMesh(clsDX9Mesh* const pTarget);

	//Sphere�Ƃ̏Փˎ�.
	void HitToSphere();

private:
	//Hp�̍ő�l.
	const int m_iMaxHp = 10;

	//���f���Ɍ�������������.
	float m_fTargetDirection;

	//�ړ��O�̈ʒu.
	D3DXVECTOR3 m_vOldPos;

	//�v���C���[�̈ʒu.
	D3DXVECTOR3 m_vPlayerPosition;

	float m_fAttackDistance;

	//�G�t�F�N�g�֘A.
	clsEffects* m_pEffect;
	//�n���h��.
	::Effekseer::Handle m_HitHandle;
	::Effekseer::Handle m_DeadHandle;

	void UpdateProduct();

	void Wait();

	void Move();

	void Attack();

	void Hit();

	void Dead();

	//�A�N�V�����̐؂�ւ�.
	void SetAction(const enAction Action);
};