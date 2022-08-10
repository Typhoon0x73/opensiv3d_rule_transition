
# version 410

//
//	Textures
//
uniform sampler2D Texture0;
uniform sampler2D Texture1;

//
//	PSInput
//
layout(location = 0) in vec4 Color;
layout(location = 1) in vec2 UV;

//
//	PSOutput
//
layout(location = 0) out vec4 FragColor;

//
//	Constant Buffer
//
layout(std140) uniform PSConstants2D
{
	vec4 g_colorAdd;
	vec4 g_sdfParam;
	vec4 g_sdfOutlineColor;
	vec4 g_sdfShadowColor;
	vec4 g_internal;
};

layout(std140) uniform RuleFade
{
	float  softRange;
	float  fadeTime;
};

//
//	Functions
//
void main()
{
    // 基準値を計算
    float rangedValue = fadeTime * (softRange * 2.0f + 1.0f) - softRange;
    // 最小値と最大値を決める
    float minValue = rangedValue - softRange;
    float maxValue = rangedValue + softRange;
    // ルール画像の濃度を取得
    float maskValue = texture(Texture1, UV).r;
    // smoothstep(エルミート補完)で透過値を決定
    float alpha = smoothstep(minValue, maxValue, maskValue);
    vec4 texCol = texture(Texture0, UV) * Color + g_colorAdd;
    FragColor = vec4(texCol.rgb, alpha);
}