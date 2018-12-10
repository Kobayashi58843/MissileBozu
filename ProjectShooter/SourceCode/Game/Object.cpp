#include "Object.h"

Object::Object()
	: m_fScale(1.0f)
{
	m_Collision.fRadius = 0.0f;
	CrearVECTOR3(m_Collision.vCenter);

	CrearVECTOR3(m_vPos);
	CrearVECTOR3(m_vRot);
	CrearVECTOR3(m_vUpperIntersect);
	CrearVECTOR3(m_vUnderIntersect);
}

Object::~Object()
{
}

//���C�ƑΏۂ̃��b�V�����Փ˂��Ă��邩.
bool Object::IsRayHit(clsDX9Mesh* const pTarget)
{
	////���x�N�g���͐����ŉ�����.
	//D3DXVECTOR3 vAxis = { 0.0f, -1.0f, 0.0f };

	//CrearVECTOR3(m_vIntersect);

	////����.
	//float fDistance = 0.0f;

	////���C�ƃ��b�V���̓����蔻��.
	//bool bHit = Intersect(vAxis, pTarget, &fDistance, &m_vIntersect);

	//if (bHit)
	//{
	//	D3DXVECTOR3 vLength = m_Collision.vCenter - m_vIntersect;
	//	float fLength = D3DXVec3Length(&vLength);

	//	if (fabs(fLength) > m_Collision.fRadius)
	//	{
	//		bHit = false;
	//	}
	//}

	//�������̌�_���W.
	D3DXVECTOR3 vAxis = { 0.0f, -1.0f, 0.0f };
	float fDistance = 0.0f;
	Intersect(vAxis, pTarget, &fDistance, &m_vUnderIntersect);

	//������̌�_���W.
	vAxis = { 0.0f, 1.0f, 0.0f };
	fDistance = 0.0f;
	Intersect(vAxis, pTarget, &fDistance, &m_vUpperIntersect);

	bool bHit = false;

	//�����蔻��.
	HitCheckMeshByRay(pTarget, &bHit);

	//�Փ�.
	return bHit;
}

//���C�ƃ��b�V���̓����蔻��.
bool Object::Intersect(const D3DXVECTOR3 vAxis,
	clsDX9Mesh* const pTarget,	//�Ώۂ̕���.
	float* pDistance,			//(out)����.
	D3DXVECTOR3* pIntersect)	//(out)�������W.
{
	D3DXMATRIXA16 matRot;//��]�s��.

	//��]�s����v�Z.
	D3DXMatrixRotationY(&matRot, m_vRot.y);

	//���x�N�g����p��.
	D3DXVECTOR3 vecAxis = vAxis;
	//Z���x�N�g�����̂��̂����݂̉�]��Ԃɂ��ϊ�����.
	D3DXVec3TransformCoord(&vecAxis, &vecAxis, &matRot);

	D3DXVECTOR3 vecStart, vecEnd, vecDistance;
	//���C�̊J�n�I���ʒu��Attacker�ƍ��킹��.
	vecStart = vecEnd = m_Collision.vCenter;
	//Attacker�̍��W�ɉ�]�s�����������.
	vecEnd += vecAxis * 1.0f;

	//�Ώۂ������Ă镨�̂ł��A�Ώۂ�world�s��́A�t�����p����ΐ��������C��������.
	
	//�g�k.
	D3DXMATRIX	mScale;
	D3DXMatrixIdentity(&mScale);
	
	float fScale = pTarget->GetScale();
	D3DXMatrixScaling(&mScale, fScale, fScale, fScale);
	
	//��].
	D3DXMATRIX	mYaw, mPitch, mRoll;
	D3DXMatrixIdentity(&mYaw);
	D3DXMatrixIdentity(&mPitch);
	D3DXMatrixIdentity(&mRoll);

	D3DXVECTOR3 vRot = pTarget->GetRot();
	D3DXMatrixRotationY(&mYaw, vRot.y);		//Y����].
	D3DXMatrixRotationX(&mPitch, vRot.x);	//X����].
	D3DXMatrixRotationZ(&mRoll, vRot.z);	//Z����].

	//�ړ�.
	D3DXMATRIX mTrans;
	D3DXMatrixIdentity(&mTrans);

	D3DXVECTOR3 vPos = pTarget->GetPos();
	D3DXMatrixTranslation(&mTrans, vPos.x, vPos.y, vPos.z);

	//����(�g�k�~��]�~�ړ�)
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);

	mWorld = mScale * mYaw * mPitch * mRoll * mTrans;

	//�t��������߂�.
	D3DXMatrixInverse(&mWorld, NULL, &mWorld);

	D3DXVec3TransformCoord(&vecStart, &vecStart, &mWorld);
	D3DXVec3TransformCoord(&vecEnd, &vecEnd, &mWorld);

	//���������߂�.
	vecDistance = vecEnd - vecStart;

	BOOL bHit = false;

	DWORD dwIndex = 0;		//�C���f�b�N�X�ԍ�.
	D3DXVECTOR3 vVertex[3];	//���_���W.
	FLOAT U = 0, V = 0;		//(out)�d�S�q�b�g���W.

	//���b�V���ƃ��C�̌����𒲂ׂ�.
	D3DXIntersect(
		pTarget->GetMesh(),	//�Ώۃ��b�V��.
		&vecStart,			//�J�n�ʒu.
		&vecDistance,		//���C�̋���.
		&bHit,				//(out)���茋��.
		&dwIndex,			//(out)bHit��True�̎��A���C�̎n�_�ɁB�ł��߂��̖ʂ̃C���f�b�N�X�l�ւ̃|�C���^.
		&U, &V,				//(out)�d�S�q�b�g���W.
		pDistance,			//�^�[�Q�b�g�Ƃ̋���.
		NULL, NULL);

	if (bHit)
	{
		//���������Ƃ�.
		FindVecticesOnPoly(pTarget->GetMeshForRay(), dwIndex, vVertex);

		//���S���W��������_���Z�o.
		//���[�J����_p�́Av0 + U*(v1-v0) + V*(v2-v0)�ŋ��܂�.
		*pIntersect =
			vVertex[0]
			+ U * (vVertex[1] - vVertex[0])
			+ V * (vVertex[2] - vVertex[0]);

		return true;//�������Ă���.
	}

	return false;//�������Ă��Ȃ�.
}

//�����ʒu�̃|���S���̒��_��������.
//�����_���W�̓��[�J�����W.
HRESULT Object::FindVecticesOnPoly(const LPD3DXMESH pTarget, const DWORD dwPolyIndex, D3DXVECTOR3* pVecVertices)
{
	//���_���Ƃ̃o�C�g�����擾.
	DWORD dwStride = pTarget->GetNumBytesPerVertex();
	//���_�����擾.
	DWORD dwVertexAm = pTarget->GetNumVertices();
	//�ʐ����擾.
	DWORD dwPolyAmt = pTarget->GetNumFaces();

	WORD* pwPoly = NULL;

	//�C���f�b�N�X�o�b�t�@�����b�N(�ǂݍ��݃��[�h)
	pTarget->LockIndexBuffer(D3DLOCK_READONLY, (VOID**)&pwPoly);
	BYTE* pbVertices = NULL;			//���_(�o�C�g��)
	FLOAT* pfVertices = NULL;			//���_(float�^)
	LPDIRECT3DVERTEXBUFFER9 VB = NULL;	//���_�o�b�t�@.
	pTarget->GetVertexBuffer(&VB);		//���_���̎擾.

	//���_�o�b�t�@�̃��b�N.
	if (SUCCEEDED(VB->Lock(0, 0, (VOID**)&pbVertices, 0)))
	{
		for (int i = 0; i < 3; i++)
		{
			//�|���S���̒��_��[i]�ڂ��擾.
			pfVertices
			= (FLOAT*)&pbVertices[dwStride*pwPoly[dwPolyIndex * 3 + i]];
			pVecVertices[i].x = pfVertices[0];
			pVecVertices[i].y = pfVertices[1];
			pVecVertices[i].z = pfVertices[2];
		}

		pTarget->UnlockIndexBuffer();//���b�N����.
		VB->Unlock();//���b�N����.
	}
	VB->Release();

	return S_OK;
}

//�����蔻��ԘA.
void Object::HitCheckMeshByRay(clsDX9Mesh* const pWall, bool* bHit)
{
	const int iDirectionMax = 10;
	D3DXVECTOR3 vAxis[iDirectionMax];

	FLOAT fDistance;//����.
	float fDis, fYaw;//�����Ɖ�].

	//���C�̌����ɂ�铖����ǂ܂ł̋��������߂�.
	vAxis[0] = { 0.0f, 0.0f, 1.0f };	//���x�N�g���O.
	vAxis[1] = { 1.0f, 0.0f, 1.0f };	//���x�N�g���E�O.
	vAxis[2] = { 1.0f, 0.0f, 0.0f };	//���x�N�g���E.
	vAxis[3] = { 1.0f, 0.0f, -1.0f };	//���x�N�g���E���.
	vAxis[4] = { 0.0f, 0.0f, -1.0f };	//���x�N�g�����.
	vAxis[5] = { -1.0f, 0.0f, -1.0f };	//���x�N�g�������.
	vAxis[6] = { -1.0f, 0.0f, 0.0f };	//���x�N�g����.
	vAxis[7] = { -1.0f, 0.0f, 1.0f };	//���x�N�g�����O.

	vAxis[8] = { 0.0f, 1.0f, 0.0f };
	vAxis[9] = { 0.0f, -1.0f, 0.0f };

	fYaw = fabs(m_vRot.y);//fabs:��Βl(float��)
	dirOverGuard(&fYaw);//0�`2�΂̊ԂɎ��߂�.

	D3DXMATRIX mYaw;
	D3DXMatrixRotationY(&mYaw, m_vRot.y);//Y����].

	for (int i = 0; i < iDirectionMax; i++)
	{
		D3DXVECTOR3 vIntersect;
		Intersect(vAxis[i], pWall, &fDistance, &vIntersect);
		fDis = fDistance;

		if (fDis < m_Collision.fRadius && fDis > 0.01f)
		{
			D3DXVec3TransformCoord(&vAxis[i], &vAxis[i], &mYaw);
			m_vPos -= vAxis[i] * (m_Collision.fRadius - fDis);
			*bHit = true;
			return;
		}
	}
}

//��]�l����.
void Object::dirOverGuard(float* fYaw)
{
	if (*fYaw > D3DX_PI * 2.0f)
	{
		//1���ȏサ�Ă���.
		*fYaw -= static_cast<float>D3DX_PI * 2.0f;//2��(360��)��������.
	
		//�ċA�֐�.
		if (*fYaw > D3DX_PI*2.0f)
		{
			dirOverGuard(fYaw);
		}
	}
}

//Sphere��Sphere�̓����蔻��.
bool Object::SphereCollision(SPHERE const pAttacker, SPHERE const pTarget)
{
	//2�̕��̂̒��S�Ԃ̋��������߂�.
	D3DXVECTOR3 vLength = pTarget.vCenter - pAttacker.vCenter;
	//�����ɕϊ�����.
	float fLength = D3DXVec3Length(&vLength);

	//2���̊Ԃ̋������A2���̂̔��a�𑫂�������.
	//�������Ƃ������Ƃ́A�X�t�B�A���m���d�Ȃ��Ă���.
	//(�Փ˂��Ă���)�Ƃ�������.
	if (fLength <= pAttacker.fRadius + pTarget.fRadius)
	{
		//�Փ�.
		return true;
	}

	//�Փ˂��Ă��Ȃ�.
	return false;
}

//���f�������X�ɖړI�̕����։�].
void Object::SpinModel(double dDestinationYaw, const float fTurnSpeed)
{
	if (dDestinationYaw - m_vRot.y > D3DXToRadian(180))
	{
		dDestinationYaw -= D3DXToRadian(360);
	}
	else if (dDestinationYaw - m_vRot.y < -D3DXToRadian(180))
	{
		dDestinationYaw += D3DXToRadian(360);
	}

	if (m_vRot.y > D3DXToRadian(180))
	{
		m_vRot.y -= static_cast<float>D3DXToRadian(360);
	}
	else if (m_vRot.y < -D3DXToRadian(180))
	{
		m_vRot.y += static_cast<float>D3DXToRadian(360);
	}

	//�p�x���߂Â�.
	if (fabs(dDestinationYaw - m_vRot.y) > fTurnSpeed)
	{
		if (m_vRot.y < dDestinationYaw)
		{
			m_vRot.y += fTurnSpeed;
		}
		else if (m_vRot.y > dDestinationYaw)
		{
			m_vRot.y -= fTurnSpeed;
		}
	}
	else
	{
		m_vRot.y = static_cast<float>(dDestinationYaw);
	}
}