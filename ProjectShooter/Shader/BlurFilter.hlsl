//グローバル変数.
//テクスチャはレジスター t(n)
Texture2D	g_Texture	:	register(t0);
Texture2D	g_DispTex	:	register(t1);

//サンプラーはレジスター s(n)
SamplerState g_Sampler	:	register(s0);

//コンスタントバッファ.
cbuffer global	:	register(b0)
{
	matrix	g_mW			: packoffset(c0);
	float	g_fViewPortW	: packoffset(c4);
	float	g_fViewPortH	: packoffset(c5);
	float	g_fAlpha		: packoffset(c6);
	float2	g_vUV			: packoffset(c7);
};

//バーテックスシェーダの出力パラメータ.
struct VS_OUT
{
	float4 Pos : SV_Position;
	float2 UV : TEXCOORD;
};

//バーテックスシェーダ.
VS_OUT VS(float4 Pos	:	POSITION,
	float2 UV : TEXCOORD)
{
	//初期化.
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(Pos, g_mW);

	//スクリーン座標に合わせる計算.
	output.Pos.x = (output.Pos.x / g_fViewPortW) * 2 - 1;
	output.Pos.y = 1 - (output.Pos.y / g_fViewPortH) * 2;

	//UV設定.
	output.UV = UV;

	//UV座標をずらす.
	output.UV.x += g_vUV.x;
	output.UV.y += g_vUV.y;

	return output;
}

//ピクセルシェーダ.
float4 PS(VS_OUT input) : SV_Target
{
	float2 UV = input.UV;
	float Isolating = 0.004;

	float4 OutColor = g_DispTex.Sample(g_Sampler, UV);	

	float4 UpColor = g_DispTex.Sample(g_Sampler, float2(UV.x ,UV.y + Isolating));
	float4 DownColor = g_DispTex.Sample(g_Sampler, float2(UV.x ,UV.y - Isolating));
	float4 LeftColor = g_DispTex.Sample(g_Sampler, float2(UV.x + Isolating ,UV.y));
	float4 RightColor = g_DispTex.Sample(g_Sampler, float2(UV.x - Isolating ,UV.y));

	OutColor = (OutColor + UpColor + DownColor + LeftColor + RightColor) / 5;
	
	OutColor.a = 1.0f;

	return OutColor;
}