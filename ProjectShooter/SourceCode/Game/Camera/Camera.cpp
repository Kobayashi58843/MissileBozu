#include"Camera.h"

//�����_����J�����܂ł̋���.
const float FOCUSING_SPACE_DISTANCE = 5.0f;

//��]���x.
const float DEGREE = 1.0f;

//����̍L��.
const float SCOPE = 4.0f;

//�`�悷��ő�̋���.
const float LOOK_DISTANCE = 500.0f;

//������̊p�x�̌��E.
const int MAX_UP_ANGLE_DEGREE = 60;

//�������̊p�x�̌��E.
const int MAX_DOWN_ANGLE_DEGREE = 45;

//�����̌��E.
const float DISTANCE_MAX = 50.0f;

//�����蔻��̑傫��.
const float RADIUS = 0.2f;

Camera::Camera(const float fWindowWidth, const float fWindowHeight)
	: m_fWindowWidth(fWindowWidth)
	, m_fWindowHeight(fWindowHeight)
	, m_fCameraAngleUnit(0.0f)
	, m_fOffsetZ(0.0f)
	, m_fDisplaceHorizontally(0.0f)
{
	CrearVECTOR3(m_vFocusingSpacePos);
	m_vFocusingSpacePos.z = -FOCUSING_SPACE_DISTANCE;

	CrearVECTOR3(m_vWorldPos);

	D3DXMatrixIdentity(&m_mView);
	D3DXMatrixIdentity(&m_mCameraPose);
	D3DXMatrixIdentity(&m_mProjection);

	CrearVECTOR3(m_vLookAt);
	CrearVECTOR3(m_vCameraMoveDirDif);
	CrearVECTOR3(m_vCamRotDif);
	SetRotPerFrame(DEGREE);

	CrearVECTOR3(m_vPos);
	CrearVECTOR3(m_vRot);
	m_Collision.fRadius = RADIUS;
	m_Collision.vCenter = m_vPos;

	m_vOldFocusingSpacePos = m_vPos + m_vFocusingSpacePos;

	m_vOldPos = m_vWorldPos;
}

Camera::~Camera()
{
}

void Camera::Update()
{
	//�J����Z����]�Ŏp�����X�V.
	D3DXMATRIX mRotMatV;
	D3DXMatrixRotationYawPitchRoll(&mRotMatV, m_vCamRotDif.y, m_vCamRotDif.x, m_vCamRotDif.z);
	m_mCameraPose = mRotMatV * m_mCameraPose;

	//�ܓx�o�x�̍������o��.
	D3DXVECTOR3 vDif;
	D3DXVec3TransformCoord(&vDif, &m_vCameraMoveDirDif, &m_mCameraPose);

	//���������鎞�݈̂ړ�������.
	if (vDif.x != 0.0f || vDif.y != 0.0f || vDif.z != 0.0f)
	{
		//�ړ�.
		Move(vDif);
	}

	//���s�������̃I�t�Z�b�g�X�V.
	OffsetZUpdate();

	//�ړ�����.
	MoveLimit();

	//�ړ���J�����p���X�V.
	PostureUpdate();

	//�J�����̃��[���h���W.
	m_vWorldPos = m_vFocusingSpacePos + m_vLookAt;

	//�ړ���̈ʒu�����C�p�̂��̂ɓ����.
	UpdateState();

	//�r���[�s��X�V.
	memcpy(&m_mView, &m_mCameraPose, sizeof(D3DXMATRIX));
	memcpy(&m_mView.m[3], &m_vWorldPos, sizeof(D3DXVECTOR3));
	m_mView._44 = 1.0f;
	D3DXMatrixInverse(&m_mView, 0, &m_mView);

	//���ɂ��炷.
	m_mView.m[3][0] += m_fDisplaceHorizontally;

	//�v���W�F�N�V����(�ˉe�s��)�ϊ�.
	Projection();

	//���������o�����Z�b�g.
	CrearVECTOR3(m_vCameraMoveDirDif);
	m_fOffsetZ = 0.0f;
	CrearVECTOR3(m_vCamRotDif);
}

//���C�ƃ��b�V���̏Փˎ�.
void Camera::RayHitToMesh(clsDX9Mesh* const pTarget)
{
}

//�v���W�F�N�V����(�ˉe�s��)�ϊ�.
void Camera::Projection()
{
	D3DXMatrixPerspectiveFovLH(&m_mProjection, (FLOAT)(D3DX_PI / SCOPE),
		m_fWindowWidth / m_fWindowHeight,//�A�X�y�N�g��(��������).
		0.1f, LOOK_DISTANCE);
}

//�ړ�.
void Camera::Move(const D3DXVECTOR3 vDif)
{
	D3DXVECTOR3 vRotAxis;
	D3DXVECTOR3* camZAxis = (D3DXVECTOR3*)m_mCameraPose.m[2];
	D3DXVec3Cross(&vRotAxis, &vDif, camZAxis);

	D3DXQUATERNION qTrans;
	D3DXQuaternionRotationAxis(&qTrans, &vRotAxis, m_fCameraAngleUnit);
	D3DXMATRIX mTransRot;
	D3DXMatrixRotationQuaternion(&mTransRot, &qTrans);
	D3DXVec3TransformCoord(&m_vFocusingSpacePos, &m_vFocusingSpacePos, &mTransRot);
}

//�ړ�����.
void Camera::MoveLimit()
{
	//�ύX�O�̋���.
	float fOldDistance = D3DXVec3Length(&m_vFocusingSpacePos);

	//�J�����̊p�x.
	float fCamAngleY = atanf(m_vFocusingSpacePos.y);

	if (fCamAngleY < D3DXToRadian(MAX_UP_ANGLE_DEGREE) &&
		fCamAngleY > D3DXToRadian(-MAX_DOWN_ANGLE_DEGREE))
	{
		//�����_��Ԃł̃J�����̈ʒu��ۑ�.
		m_vOldFocusingSpacePos = m_vFocusingSpacePos;
	}
	else
	{
		//�͈͊O�̏ꍇ�͑O�̈ʒu�ɖ߂�.
		m_vFocusingSpacePos.y = m_vOldFocusingSpacePos.y;
	}

	/*====/ �����ʒu�ƃJ�����̋����̒��� /====*/

	//�ύX��̋���.
	float fDistance = D3DXVec3Length(&m_vFocusingSpacePos);

	D3DXVECTOR3 vCameraPoseZ;
	vCameraPoseZ = -m_vFocusingSpacePos;
	D3DXVec3Normalize(&vCameraPoseZ, &vCameraPoseZ);

	//�ύX�O��̋����ɍ�������ꍇ�͒�������.
	if (fOldDistance != fDistance)
	{
		m_vFocusingSpacePos -= (fOldDistance - fDistance) * vCameraPoseZ;
	}
}

//�p���X�V.
void Camera::PostureUpdate()
{
	D3DXVECTOR3 vCameraPoseX, vCameraPoseY, vCameraPoseZ;

	vCameraPoseZ = -m_vFocusingSpacePos;
	D3DXVec3Normalize(&vCameraPoseZ, &vCameraPoseZ);

	vCameraPoseY = { 0.0f, 1.0f, 0.0f };
	D3DXVec3Cross(&vCameraPoseX, &vCameraPoseY, &vCameraPoseZ);
	D3DXVec3Normalize(&vCameraPoseX, &vCameraPoseX);

	D3DXVec3Cross(&vCameraPoseY, &vCameraPoseZ, &vCameraPoseX);
	D3DXVec3Normalize(&vCameraPoseY, &vCameraPoseY);

	D3DXMatrixIdentity(&m_mCameraPose);
	memcpy(m_mCameraPose.m[0], &vCameraPoseX, sizeof(D3DXVECTOR3));
	memcpy(m_mCameraPose.m[1], &vCameraPoseY, sizeof(D3DXVECTOR3));
	memcpy(m_mCameraPose.m[2], &vCameraPoseZ, sizeof(D3DXVECTOR3));
}

//���s�������̃I�t�Z�b�g�X�V.
void Camera::OffsetZUpdate()
{
	D3DXVECTOR3 vCameraPoseZ;
	vCameraPoseZ = -m_vFocusingSpacePos;
	D3DXVec3Normalize(&vCameraPoseZ, &vCameraPoseZ);

	float fDistance = D3DXVec3Length(&m_vFocusingSpacePos);

	if (fDistance - m_fOffsetZ <= DISTANCE_MAX &&
		fDistance - m_fOffsetZ > 0.0f)
	{
		m_vFocusingSpacePos += m_fOffsetZ * vCameraPoseZ;
	}
}

//�ʒu,��],�T�C�Y��K��.
void Camera::UpdateState()
{
	CrearVECTOR3(m_vRot);
	m_fScale = 0.0f;

	m_Collision.fRadius = 1.0f;

	m_vPos = m_vWorldPos;
	m_Collision.vCenter = m_vPos;
}