#include "EventCamera.h"

//����̍L��.
const float SCOPE = 4.0f;

//�`�悷��ő�̋���.
const float LOOK_DISTANCE = 500.0f;

//�����̏������(�^��).
const D3DXVECTOR3 UP_VECTOR = { 0.0f, 1.0f, 0.0f };

EventCamera::EventCamera(const float fWindowWidth, const float fWindowHeight)
	: m_fWindowWidth(fWindowWidth)
	, m_fWindowHeight(fWindowHeight)
{
	CrearVECTOR3(m_vPos);
	CrearVECTOR3(m_vLookAt);
	CrearVECTOR3(m_vRot);
}

EventCamera::~EventCamera()
{
}

//�X�V.
void EventCamera::Update()
{
	//�r���[�ϊ�.
	View();

	//�v���W�F�N�V����(�ˉe�s��)�ϊ�.
	Projection();
}

//�r���[�ϊ�.
void EventCamera::View()
{
	D3DXVECTOR3 vUpVec = { 0.0f, 1.0f, 0.0f };
	
	D3DXMATRIX mLookAtMat;
	D3DXMatrixIdentity(&mLookAtMat);
	D3DXMatrixLookAtLH(&mLookAtMat, &m_vPos, &m_vLookAt, &vUpVec);
	D3DXMatrixInverse(&mLookAtMat, NULL, &mLookAtMat);


	D3DXMATRIX mRotMat;
	D3DXMatrixIdentity(&mRotMat);
	D3DXMatrixRotationYawPitchRoll(&mRotMat, m_vRot.y, m_vRot.x, m_vRot.z);

	mRotMat = mLookAtMat * mRotMat;

	//�r���[�s��X�V.
	memcpy(&m_mView, &mRotMat, sizeof(D3DXMATRIX));
	memcpy(&m_mView.m[3], &m_vPos, sizeof(D3DXVECTOR3));

	D3DXMatrixInverse(&m_mView, NULL, &m_mView);
}

//�v���W�F�N�V����(�ˉe�s��)�ϊ�.
void EventCamera::Projection()
{
	D3DXMatrixPerspectiveFovLH(&m_mProjection, (FLOAT)(D3DX_PI / SCOPE),
		m_fWindowWidth / m_fWindowHeight,//�A�X�y�N�g��(��������).
		0.1f, LOOK_DISTANCE);
}