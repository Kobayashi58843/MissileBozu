//グローバル.
Texture2D		g_Tex		: register(t0);
Texture2D		g_DepthTex	: register(t1);

SamplerState	g_samLinearTex		: register(s0);
SamplerState	g_samLinearDepth	: register(s1);

cbuffer per_mesh : register(b0)
{
	//ワールド行列.
	matrix g_mW;

	//( W * V * P ) 行列.
	matrix g_mWVP;

	//( W * L * P  * T ) 行列.
	matrix g_mWLPT;

	//( W * L * P ) 行列.
	matrix g_mWLP;
};

cbuffer per_frame : register(b1)
{
	//カメラ位置.
	float4 g_vEye;
};

//バーテックスシェーダ出力構造体.
struct VS_OUTPUT
{
	float4 Pos			: SV_Position;
	float3 Normal		: TEXCOORD0;
	float2 Tex			: TEXCOORD1;
	float4 PosWorld		: TEXCOORD2;
	float4 LightTex		: TEXCOORD3;
	float4 LighViewPos	: TEXCOORD4;
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

	//ライトビューを参照するとき、手がかりとなるテクスチャー座標.
	Out.LightTex = mul(Pos, g_mWLPT);

	Out.LighViewPos = mul(g_vEye, g_mWLP);

	return Out;
}

//ピクセルシェーダ.
float4 PS_Main(VS_OUTPUT Input) : SV_Target
{
	float4 OutColor;

	//テクスチャカラー.
	float4 Tex = g_Tex.Sample(g_samLinearTex, Input.Tex);
	OutColor = Tex;

	/*====/ シャドウマップ /====*/

	Input.LightTex /= Input.LightTex.w;
	float TexValue = g_DepthTex.Sample(g_samLinearDepth, Input.LightTex);

	////float LightLength = Input.LighViewPos.z / Input.LighViewPos.w;
	//float LightLength = Input.Pos.z / Input.Pos.w;

	////深度の差.
	//const float fDepthDif = abs(TexValue - LightLength);
	////陰にする値.
	//const float fDepthHaveShade = 0.04f;
	//if (fDepthDif > fDepthHaveShade)
	//{
	//	OutColor /= 4;//影.
	//}

	if (0.0f < TexValue && TexValue < 1.0f)
	{
		OutColor /= 4.0f;
	}

	return OutColor;
}