//�O���[�o��.
Texture2D		g_texColor	: register(t0);
SamplerState	g_samLinear	: register(s0);

//�R���X�^���g�o�b�t�@.
cbuffer per_mesh : register(b0)
{
	//���[���h.
	matrix g_mW;
	//[ ���[���h�~�r���[�~�v���W�F�N�V���� ] �̕ϊ������s��.
	matrix g_mWVP;
};

//�o�[�e�b�N�X�V�F�[�_�o�͍\����.
struct VS_OUTPUT
{
	float4 Pos			: SV_Position;
	float3 Normal		: TEXCOORD0;
	float2 Tex			: TEXCOORD1;
	float4 PosWorld		: TEXCOORD2;
	float4 Depth		: TEXCOORD3;
};

//�o�[�e�b�N�X�V�F�[�_.
VS_OUTPUT VS_Main(
	float4 Pos	: POSITION,
	float4 Norm : NORMAL,
	float2 Tex : TEXCOORD)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	//�ˉe�ϊ�(���[���h,�r���[,�v���W�F�N�V����).
	Out.Pos = mul(Pos, g_mWVP);

	//�@�������f���̎p���ɍ��킹�� : ���f������]����Ζ@������]������K�v�����邽��.
	Out.Normal = mul(Norm, (float3x3)g_mW);

	float3 PosWorld = mul(Pos, g_mW);
		Out.PosWorld = mul(Pos, g_mW);

	//�e�N�X�`�����W.
	Out.Tex = Tex;

	Out.Depth = Pos;

	return Out;
}

//�s�N�Z���V�F�[�_.
float4 PS_Main(VS_OUTPUT In) : SV_Target
{
	float4 OutColor;

	OutColor = In.Pos.z / In.Pos.w;

	return OutColor;
}