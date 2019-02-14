//�O���[�o���ϐ�.
//�e�N�X�`���̓��W�X�^�[ t(n)
Texture2D	g_Division		:	register(t0);
Texture2D	g_DispTexMain	:	register(t1);
Texture2D	g_DispTexSub	:	register(t2);

//�T���v���[�̓��W�X�^�[ s(n)
SamplerState g_Sampler	:	register(s0);

//�R���X�^���g�o�b�t�@.
cbuffer global	:	register(b0)
{
	matrix	g_mW			: packoffset(c0);
	float	g_fViewPortW	: packoffset(c4);
	float	g_fViewPortH	: packoffset(c5);
	float	g_fAlpha		: packoffset(c6);
	float2	g_vUV			: packoffset(c7);
};

//�o�[�e�b�N�X�V�F�[�_�̏o�̓p�����[�^.
struct VS_OUT
{
	float4 Pos : SV_Position;
	float2 UV : TEXCOORD;
};

//�o�[�e�b�N�X�V�F�[�_.
VS_OUT VS(float4 Pos	:	POSITION,
	float2 UV : TEXCOORD)
{
	//������.
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(Pos, g_mW);

	//�X�N���[�����W�ɍ��킹��v�Z.
	output.Pos.x = (output.Pos.x / g_fViewPortW) * 2 - 1;
	output.Pos.y = 1 - (output.Pos.y / g_fViewPortH) * 2;

	//UV�ݒ�.
	output.UV = UV;

	//UV���W�����炷.
	output.UV.x += g_vUV.x;
	output.UV.y += g_vUV.y;

	return output;
}

//�s�N�Z���V�F�[�_.
float4 PS(VS_OUT input) : SV_Target
{
	float4 OutColor = g_DispTexMain.Sample(g_Sampler, input.UV);

	float4 Division = g_Division.Sample(g_Sampler, input.UV);
	
	float4 SubColor = g_DispTexSub.Sample(g_Sampler, input.UV);
	
	//���F������2���ڂ̂��̂��g��.
	if( Division.x == 0 )
	{
		OutColor = SubColor;
	}
	else if( Division.x < 1 )
	{
		//�O���f�[�V�����ɂȂ��Ă���ꍇ��2������������.
		float4 fMainColor = OutColor * Division.x;
		float4 fSubColor = SubColor * (1 - Division.x);
		OutColor = fMainColor + fSubColor;
	}

	return OutColor;
}