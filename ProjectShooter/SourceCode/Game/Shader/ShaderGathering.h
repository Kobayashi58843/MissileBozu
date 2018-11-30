#pragma once

#include "..\\..\\Global.h"

#include "ShaderEnumeration.h"
#include "ShaderStructure.h"

class ShaderGathering
{
public:
	ShaderGathering();
	~ShaderGathering();

	//シェーダの初期設定.
	void InitShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	/*================/ スタティックメッシュ /================*/
	
	//指定したシェーダのポインタが入った構造体を返す.
	SHADER_POINTER GetStaticMeshShader(const enStaticModelShader Index) const
	{
		return m_StaticMeshShader[Index];
	}
	//指定したコンスタントバッファのポインタを返す.
	ID3D11Buffer* GetStaticCBuffer(const enStaticModelCBuffer Index) const
	{
		return m_pStaticCBuffers[Index];
	}

	/*================/ スキンメッシュ /================*/

	//指定したシェーダのポインタが入った構造体を返す.
	SHADER_POINTER GetSkinMeshShader(const enSkinModelShader Index) const
	{
		return m_SkinMeshShader[Index];
	}
	//指定したコンスタントバッファのポインタを返す.
	ID3D11Buffer* GetSkinCBuffer(const enSkinModelCBuffer Index) const
	{
		return m_pSkinCBuffers[Index];
	}

private:
	//Dx11.
	ID3D11Device*			m_pDevice11;		//デバイスオブジェクト.
	ID3D11DeviceContext*	m_pDeviceContext11;	//デバイスコンテキスト.

	//コンスタントバッファ作成.
	void MakeConstantBuffer(ID3D11Buffer* &pConstantBuffer, UINT CBufferSize);

	/*================/ スタティックメッシュ /================*/

	//スタティックメッシュのシェーダ.
	SHADER_POINTER m_StaticMeshShader[enStaticModelShader_Max];

	//スタティックメッシュのコンスタントバッファ.
	ID3D11Buffer* m_pStaticCBuffers[enStaticModelCBuffer_Max];

	//スタティックメッシュ用のシェーダを作成.
	void CreateStaticShader();

	//シェーダ作成(スタティックメッシュ).
	void MakeStaticMeshShader(char* ShaderName, SHADER_ENTRY_POINT EntryPoint, SHADER_POINTER& SHADER_POINTER);

	/*================/ スキンメッシュ /================*/

	//スキンメッシュのシェーダ.
	SHADER_POINTER m_SkinMeshShader[enSkinModelShader_Max];

	//スキンメッシュのコンスタントバッファ.
	ID3D11Buffer* m_pSkinCBuffers[enSkinModelCBuffer_Max];

	//スキンメッシュ用のシェーダを作成.
	void CreateSkinShader();

	//シェーダ作成(スキンメッシュ).
	void MakeSkinMeshShader(char* ShaderName, SHADER_ENTRY_POINT EntryPoint, SHADER_POINTER& SHADER_POINTER);
};