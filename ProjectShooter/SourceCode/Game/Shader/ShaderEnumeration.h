#pragma once

//シェーダ(スタティックメッシュ).
enum enStaticModelShader
{
	enStaticModelShader_NoTex = 0,
	enStaticModelShader_Simple,
	enStaticModelShader_Depth,
	enStaticModelShader_ShadowMap,

	enStaticModelShader_Max,
};

//コンスタントバッファ(スタティックメッシュ).
enum enStaticModelCBuffer
{
	enStaticModelCBuffer_Mesh = 0,
	enStaticModelCBuffer_ShadowMap_Mesh,
	enStaticModelCBuffer_ShadowMap_Frame,

	enStaticModelCBuffer_Max,
};

//シェーダ(スキンメッシュ).
enum enSkinModelShader
{
	enSkinModelShader_NoTex = 0,
	enSkinModelShader_Simple,
	enSkinModelShader_Depth,

	enSkinModelShader_Max,
};

//コンスタントバッファ(スキンメッシュ).
enum enSkinModelCBuffer
{
	enSkinModelCBuffer_Mesh = 0,

	enSkinModelCBuffer_Max,
};
