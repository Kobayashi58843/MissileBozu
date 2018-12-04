#pragma once

#include "..\\Object.h"

class EventCamera
{
public:
	EventCamera(const float fWindowWidth, const float fWindowHeight);
	~EventCamera();

	//�r���[.
	D3DXMATRIX GetView() const
	{
		return m_mView;
	}

	//�v���W�F�N�V����.
	D3DXMATRIX GetProjection() const
	{
		return m_mProjection;
	}

	//�ʒu.
	void SetPos(const D3DXVECTOR3 vPos)
	{
		m_vPos = vPos;
	}
	void AddPos(const D3DXVECTOR3 vAdd)
	{
		m_vPos += vAdd;
	}
	D3DXVECTOR3 GetPos() const
	{
		return m_vPos;
	}

	//�����ʒu.
	void SetLookAt(const D3DXVECTOR3 vLookAt)
	{
		m_vLookAt = vLookAt;
	}
	void AddLookAt(const D3DXVECTOR3 vAdd)
	{
		m_vLookAt += vAdd;
	}
	D3DXVECTOR3 GetLookAt() const
	{
		return m_vLookAt;
	}

	void SetUpVector(const D3DXVECTOR3 vUpVec)
	{
		m_vUpVector = vUpVec;
	}
	D3DXVECTOR3 GetUpVector() const
	{
		return m_vUpVector;
	}

	void AddRoll(const float fAdd)
	{
		m_fRoll += fAdd;
	}
	float GetRoll() const
	{
		return m_fRoll;
	}

	//�X�V.
	void Update();

private:
	//�E�B���h�E��.
	float m_fWindowWidth;
	//�E�B���h�E����.
	float m_fWindowHeight;

	//�r���[�s��.
	D3DXMATRIX	m_mView;
	//�v���W�F�N�V�����s��.
	D3DXMATRIX m_mProjection;
	
	//�ʒu.
	D3DXVECTOR3 m_vPos;
	//�����ʒu.
	D3DXVECTOR3 m_vLookAt;
	//�������.
	D3DXVECTOR3 m_vUpVector;

	float m_fRoll;

	//�r���[�ϊ�.
	void View();
	//�v���W�F�N�V����(�ˉe�s��)�ϊ�.
	void Projection();
};