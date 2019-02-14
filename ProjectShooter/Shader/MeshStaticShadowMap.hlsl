//�O���[�o��.
Texture2D		g_Tex		: register(t0);
Texture2D		g_DepthTex	: register(t1);

SamplerState	g_samLinearTex		: register(s0);
SamplerState	g_samLinearDepth	: register(s1);

cbuffer per_mesh : register(b0)
{
	//���[���h�s��.
	matrix g_mW;

	//( W * V * P ) �s��.
	matrix g_mWVP;

	//( W * L * P  * T ) �s��.
	matrix g_mWLPT;

	//( W * L * P ) �s��.
	matrix g_mWLP;
};

cbuffer per_frame : register(b1)
{
	//�[�x�e�N�X�`���̃J�����ʒu.
	float4 g_vLightEye;

	//���݂̃J�����ʒu.
	float4 g_vEye;
};

//�o�[�e�b�N�X�V�F�[�_�o�͍\����.
struct VS_OUTPUT
{
	float4 Pos			: SV_Position;
	float3 Normal		: TEXCOORD0;
	float2 Tex			: TEXCOORD1;
	float4 PosWorld		: TEXCOORD2;
	float4 LightTex		: TEXCOORD3;
	float4 LighViewPos	: TEXCOORD4;
};

//�o�[�e�b�N�X�V�F�[�_.
VS_OUTPUT VS_Main(
	float4 Pos	: POSITION,
	float3 Norm : NORMAL,
	float2 Tex : TEXCOORD)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	//�ˉe�ϊ�(���[���h,�r���[,�v���W�F�N�V����).
	Out.Pos = mul(Pos, g_mWVP);

	//�@�������f���̎p���ɍ��킹�� : ���f������]����Ζ@������]������K�v�����邽��.
	Out.Normal = mul(Norm, (float3x3)g_mW);

	float4 PosWorld = mul(Pos, g_mW);
	Out.PosWorld = mul(Pos, g_mW);

	//�e�N�X�`�����W.
	Out.Tex = Tex;

	//���C�g�r���[���Q�Ƃ���Ƃ��A�肪����ƂȂ�e�N�X�`���[���W.
	Out.LightTex = mul(Pos, g_mWLPT);

	Out.LighViewPos = mul(g_vLightEye, g_mWLP);

	return Out;
}

//�s�N�Z���V�F�[�_.
float4 PS_Main(VS_OUTPUT Input) : SV_Target
{
	float4 OutColor;
	float4 TexColor;

	//�e�N�X�`���J���[.
	float4 Tex = g_Tex.Sample(g_samLinearTex, Input.Tex);
	TexColor = Tex;

	/*====/ �V���h�E�}�b�v /====*/

	Input.LightTex /= Input.LightTex.w;
	float4 TexValue = g_DepthTex.Sample(g_samLinearDepth, float2( Input.LightTex.x, Input.LightTex.y));

	////float LightLength = Input.LighViewPos.z / Input.LighViewPos.w;
	//float LightLength = Input.Pos.z / Input.Pos.w;

	////�[�x�̍�.
	//const float fDepthDif = abs(TexValue - LightLength);
	////�A�ɂ���l.
	//const float fDepthHaveShade = 0.04;
	//if (fDepthDif > fDepthHaveShade)
	//{
	//	OutColor /= 4;//�e.
	//}

	OutColor = TexColor;
	if (0 <= TexValue.x && TexValue.x < 1)
	{
		OutColor /= 4;
		OutColor.a = 1;
	}

	//�J�����̈ʒu�Ƌ߂��Ƃ���𓧉�.
	//float Length = abs(length(Input.PosWorld - g_vEye));
	//if(Length <= 8)
	//{
	//	OutColor.a = 0;
	//}


	return OutColor;
}