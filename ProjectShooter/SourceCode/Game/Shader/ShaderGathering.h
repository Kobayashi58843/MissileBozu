#pragma once

#include "..\\..\\Global.h"

#include "ShaderEnumeration.h"
#include "ShaderStructure.h"

class ShaderGathering
{
public:
	ShaderGathering();
	~ShaderGathering();

	//�V�F�[�_�̏����ݒ�.
	void InitShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	/*================/ �X�^�e�B�b�N���b�V�� /================*/
	
	//�w�肵���V�F�[�_�̃|�C���^���������\���̂�Ԃ�.
	SHADER_POINTER GetStaticMeshShader(const enStaticModelShader Index) const
	{
		return m_StaticMeshShader[Index];
	}
	//�w�肵���R���X�^���g�o�b�t�@�̃|�C���^��Ԃ�.
	ID3D11Buffer* GetStaticCBuffer(const enStaticModelCBuffer Index) const
	{
		return m_pStaticCBuffers[Index];
	}

	/*================/ �X�L�����b�V�� /================*/

	//�w�肵���V�F�[�_�̃|�C���^���������\���̂�Ԃ�.
	SHADER_POINTER GetSkinMeshShader(const enSkinModelShader Index) const
	{
		return m_SkinMeshShader[Index];
	}
	//�w�肵���R���X�^���g�o�b�t�@�̃|�C���^��Ԃ�.
	ID3D11Buffer* GetSkinCBuffer(const enSkinModelCBuffer Index) const
	{
		return m_pSkinCBuffers[Index];
	}

private:
	//Dx11.
	ID3D11Device*			m_pDevice11;		//�f�o�C�X�I�u�W�F�N�g.
	ID3D11DeviceContext*	m_pDeviceContext11;	//�f�o�C�X�R���e�L�X�g.

	//�R���X�^���g�o�b�t�@�쐬.
	void MakeConstantBuffer(ID3D11Buffer* &pConstantBuffer, UINT CBufferSize);

	/*================/ �X�^�e�B�b�N���b�V�� /================*/

	//�X�^�e�B�b�N���b�V���̃V�F�[�_.
	SHADER_POINTER m_StaticMeshShader[enStaticModelShader_Max];

	//�X�^�e�B�b�N���b�V���̃R���X�^���g�o�b�t�@.
	ID3D11Buffer* m_pStaticCBuffers[enStaticModelCBuffer_Max];

	//�X�^�e�B�b�N���b�V���p�̃V�F�[�_���쐬.
	void CreateStaticShader();

	//�V�F�[�_�쐬(�X�^�e�B�b�N���b�V��).
	void MakeStaticMeshShader(char* ShaderName, SHADER_ENTRY_POINT EntryPoint, SHADER_POINTER& SHADER_POINTER);

	/*================/ �X�L�����b�V�� /================*/

	//�X�L�����b�V���̃V�F�[�_.
	SHADER_POINTER m_SkinMeshShader[enSkinModelShader_Max];

	//�X�L�����b�V���̃R���X�^���g�o�b�t�@.
	ID3D11Buffer* m_pSkinCBuffers[enSkinModelCBuffer_Max];

	//�X�L�����b�V���p�̃V�F�[�_���쐬.
	void CreateSkinShader();

	//�V�F�[�_�쐬(�X�L�����b�V��).
	void MakeSkinMeshShader(char* ShaderName, SHADER_ENTRY_POINT EntryPoint, SHADER_POINTER& SHADER_POINTER);
};