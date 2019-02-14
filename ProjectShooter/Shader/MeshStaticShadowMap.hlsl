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
	//深度テクスチャのカメラ位置.
	float4 g_vLightEye;

	//現在のカメラ位置.
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
	float3 Norm : NORMAL,
	float2 Tex : TEXCOORD)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	//射影変換(ワールド,ビュー,プロジェクション).
	Out.Pos = mul(Pos, g_mWVP);

	//法線をモデルの姿勢に合わせる : モデルが回転すれば法線も回転させる必要があるため.
	Out.Normal = mul(Norm, (float3x3)g_mW);

	float4 PosWorld = mul(Pos, g_mW);
	Out.PosWorld = mul(Pos, g_mW);

	//テクスチャ座標.
	Out.Tex = Tex;

	//ライトビューを参照するとき、手がかりとなるテクスチャー座標.
	Out.LightTex = mul(Pos, g_mWLPT);

	Out.LighViewPos = mul(g_vLightEye, g_mWLP);

	return Out;
}

//ピクセルシェーダ.
float4 PS_Main(VS_OUTPUT Input) : SV_Target
{
	float4 OutColor;
	float4 TexColor;

	//テクスチャカラー.
	float4 Tex = g_Tex.Sample(g_samLinearTex, Input.Tex);
	TexColor = Tex;

	/*====/ シャドウマップ /====*/

	Input.LightTex /= Input.LightTex.w;
	float4 TexValue = g_DepthTex.Sample(g_samLinearDepth, float2( Input.LightTex.x, Input.LightTex.y));

	////float LightLength = Input.LighViewPos.z / Input.LighViewPos.w;
	//float LightLength = Input.Pos.z / Input.Pos.w;

	////深度の差.
	//const float fDepthDif = abs(TexValue - LightLength);
	////陰にする値.
	//const float fDepthHaveShade = 0.04;
	//if (fDepthDif > fDepthHaveShade)
	//{
	//	OutColor /= 4;//影.
	//}

	OutColor = TexColor;
	if (0 <= TexValue.x && TexValue.x < 1)
	{
		OutColor /= 4;
		OutColor.a = 1;
	}

	//カメラの位置と近いところを透過.
	//float Length = abs(length(Input.PosWorld - g_vEye));
	//if(Length <= 8)
	//{
	//	OutColor.a = 0;
	//}


	return OutColor;
}