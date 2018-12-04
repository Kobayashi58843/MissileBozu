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
	, m_vUpVector(UP_VECTOR)
	, m_fRoll(1.0f)
{
	CrearVECTOR3(m_vPos);
	CrearVECTOR3(m_vLookAt);
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
	m_vUpVector.y = sinf(m_fRoll);

	D3DXMatrixLookAtLH(&m_mView, &m_vPos, &m_vLookAt, &m_vUpVector);
}

//�v���W�F�N�V����(�ˉe�s��)�ϊ�.
void EventCamera::Projection()
{
	D3DXMatrixPerspectiveFovLH(&m_mProjection, (FLOAT)(D3DX_PI / SCOPE),
		m_fWindowWidth / m_fWindowHeight,//�A�X�y�N�g��(��������).
		0.1f, LOOK_DISTANCE);
}