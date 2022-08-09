// 参考
// https://qiita.com/2dgames_jp/items/cbd7a10424ad819ee342

//
//	Textures
//
Texture2D		g_texture0 : register(t0);
SamplerState	g_sampler0 : register(s0);
Texture2D		g_maskTexture : register(t1);
SamplerState	g_maskSampler : register(s1);

namespace s3d
{
	//
	//	VS Output / PS Input
	//
	struct PSInput
	{
		float4 position	: SV_POSITION;
		float4 color	: COLOR0;
		float2 uv		: TEXCOORD0;
	};
}
//
//	Constant Buffer
//
cbuffer PSConstants2D : register(b0)
{
	float4 g_colorAdd;
	float4 g_sdfParam;
	float4 g_sdfOutlineColor;
	float4 g_sdfShadowColor;
	float4 g_internal;
}

cbuffer RuleFade : register(b1)
{
    float  g_softRange;
	float  g_fadeTime;
}

float4 PS(s3d::PSInput input) : SV_TARGET
{
    // 基準値を計算
    float rangedValue = g_fadeTime * (g_softRange * 2.0 + 1.0) - g_softRange;
    // 最小値と最大値を決める
    float minValue = rangedValue - g_softRange;
    float maxValue = rangedValue + g_softRange;
    // ルール画像の濃度を取得
    float maskValue = g_maskTexture.Sample(g_maskSampler, input.uv).r;
    // smoothstep(エルミート補完)で透過値を決定
    float alpha = smoothstep(minValue, maxValue, maskValue);
    
    // 割り当てたテクスチャを使う
    float4 col = g_texture0.Sample(g_sampler0, input.uv);
    return float4(col.rgb, alpha);
}