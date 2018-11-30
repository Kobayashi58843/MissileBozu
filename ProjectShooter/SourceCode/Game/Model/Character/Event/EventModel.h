#pragma once

#include "..\\..\\..\\Object.h"
#include "..\\..\\ModelResource\\SkinMesh\\CD3DXSKINMESH.h"

class EventModel : public Object
{
public:
	EventModel(clsD3DXSKINMESH* const pModel, const float fScale, const double dAnimSpeed);
	~EventModel();

	//3D���f���̕`��.
	void RenderModel(const D3DXMATRIX &mView, const D3DXMATRIX &mProj);

	//�A�j���[�V�����؂�ւ��֐�.
	void ChangeAnimation(const int iIndex, const double dStartPos = 0.0f);

	//���݂̃A�j���[�V�����̏I�����m�F����֐�.
	bool IsAnimationEnd();

	//���݂̃A�j���[�V�����������I�����������m�F����֐�.
	bool IsAnimationRatioEnd(const float fRatio);

	//�A�j���[�V�����̍Đ����x��ς���.
	void SetAnimationSpeed(const double dAnimationSpeed);

	//�A�j���[�V�����̍ő吔���擾.
	int GetAnimationQuantityMax();

private:
	//�X�L�����b�V���̃��f���ɕK�v�ȏ��.
	struct MESH_STATE
	{
		//�Đ����Ă���A�j���[�V�����̔ԍ�.
		int iIndex;

		//�A�j���[�V�����R���g���[��.
		LPD3DXANIMATIONCONTROLLER AnimationController;

		//���f���̃A�j���[�V�����̍Đ����x.
		double dAnimationSpeed;

		//���f���̃A�j���[�V�����̌��݂̎���.
		double dAnimationTime;
	};

	//�g���������f���̃|�C���^.
	clsD3DXSKINMESH* m_pModel;

	MESH_STATE m_ModelState;

	void AttachModel(clsD3DXSKINMESH* const pModel);
	void DetatchModel();

	//�ʒu,��],�T�C�Y�����f���ɓK��.
	void UpdateState();
};