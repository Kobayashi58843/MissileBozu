//グローバル変数.
//テクスチャはレジスター t(n)
Texture2D	g_Texture	:	register(t0);
Texture2D	g_Mask		:	register(t1);
//サンプラーはレジスター s(n)
SamplerState g_Sampler	:	register(s0);

//コンスタントバッファ.
cbuffer global	:	register(b0)
{
	matrix	g_mW			: packoffset(c0);
	float	g_fViewPortW : packoffset(c4);
	float	g_fViewPortH : packoffset(c5);
	float	g_fAlpha : packoffset(c6);
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
float4 PS_Main(VS_OUT input) : SV_Target
{
	float4 OutColor = g_Texture.Sample(g_Sampler, input.UV);

	float4 MaskColor = g_Mask.Sample(g_Sampler, input.UV);
	float fGrayScale = (((MaskColor.r + MaskColor.g + MaskColor.b) / 3) * MaskColor.a) * 3;

	OutColor.a = (((fGrayScale * g_fAlpha) + g_fAlpha) * OutColor.a) * MaskColor.a;

	return OutColor;
}