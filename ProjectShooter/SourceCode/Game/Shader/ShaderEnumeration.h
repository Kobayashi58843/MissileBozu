#pragma once

//�V�F�[�_(�X�^�e�B�b�N���b�V��).
enum enStaticModelShader
{
	enStaticModelShader_NoTex = 0,
	enStaticModelShader_Simple,
	enStaticModelShader_Depth,
	enStaticModelShader_ShadowMap,

	enStaticModelShader_Max,
};

//�R���X�^���g�o�b�t�@(�X�^�e�B�b�N���b�V��).
enum enStaticModelCBuffer
{
	enStaticModelCBuffer_Mesh = 0,
	enStaticModelCBuffer_ShadowMap_Mesh,
	enStaticModelCBuffer_ShadowMap_Frame,

	enStaticModelCBuffer_Max,
};

//�V�F�[�_(�X�L�����b�V��).
enum enSkinModelShader
{
	enSkinModelShader_NoTex = 0,
	enSkinModelShader_Simple,
	enSkinModelShader_Depth,

	enSkinModelShader_Max,
};

//�R���X�^���g�o�b�t�@(�X�L�����b�V��).
enum enSkinModelCBuffer
{
	enSkinModelCBuffer_Mesh = 0,

	enSkinModelCBuffer_Max,
};
