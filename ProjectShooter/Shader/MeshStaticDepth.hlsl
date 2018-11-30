//グローバル.
Texture2D		g_texColor	: register(t0);
SamplerState	g_samLinear	: register(s0);

//コンスタントバッファ.
cbuffer per_mesh : register(b0)
{
	//ワールド.
	matrix g_mW;
	//[ ワールド×ビュー×プロジェクション ] の変換合成行列.
	matrix g_mWVP;
};

//バーテックスシェーダ出力構造体.
struct VS_OUTPUT
{
	float4 Pos			: SV_Position;
	float3 Normal		: TEXCOORD0;
	float2 Tex			: TEXCOORD1;
	float4 PosWorld		: TEXCOORD2;
	float4 Depth		: TEXCOORD3;
};

//バーテックスシェーダ.
VS_OUTPUT VS_Main(
	float4 Pos	: POSITION,
	float4 Norm : NORMAL,
	float2 Tex : TEXCOORD)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	//射影変換(ワールド,ビュー,プロジェクション).
	Out.Pos = mul(Pos, g_mWVP);

	//法線をモデルの姿勢に合わせる : モデルが回転すれば法線も回転させる必要があるため.
	Out.Normal = mul(Norm, (float3x3)g_mW);

	float3 PosWorld = mul(Pos, g_mW);
		Out.PosWorld = mul(Pos, g_mW);

	//テクスチャ座標.
	Out.Tex = Tex;

	Out.Depth = Pos;

	return Out;
}

//ピクセルシェーダ.
float4 PS_Main(VS_OUTPUT In) : SV_Target
{
	float4 OutColor;

	OutColor = In.Pos.z / In.Pos.w;

	return OutColor;
}