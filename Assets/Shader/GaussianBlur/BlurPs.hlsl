#include "Blur.hlsli"

struct PsOutput
{
	float32_t4 color : SV_TARGET0;
};

cbuffer cbuff1 : register(b1)
{
	float32_t4 gWeights[2];
};
SamplerState gSampler : register(s0);

PsOutput main(VsOutput input)
{
	PsOutput output;
	output.color = gWeights[0].x * gTexture.Sample(gSampler, input.uv0.xy);
	output.color += gWeights[0].y * gTexture.Sample(gSampler, input.uv1.xy);
	output.color += gWeights[0].z * gTexture.Sample(gSampler, input.uv2.xy);
	output.color += gWeights[0].w * gTexture.Sample(gSampler, input.uv3.xy);
	output.color += gWeights[1].x * gTexture.Sample(gSampler, input.uv4.xy);
	output.color += gWeights[1].y * gTexture.Sample(gSampler, input.uv5.xy);
	output.color += gWeights[1].z * gTexture.Sample(gSampler, input.uv6.xy);
	output.color += gWeights[1].w * gTexture.Sample(gSampler, input.uv7.xy);
	output.color += gWeights[0].x * gTexture.Sample(gSampler, input.uv0.zw);
	output.color += gWeights[0].y * gTexture.Sample(gSampler, input.uv1.zw);
	output.color += gWeights[0].z * gTexture.Sample(gSampler, input.uv2.zw);
	output.color += gWeights[0].w * gTexture.Sample(gSampler, input.uv3.zw);
	output.color += gWeights[1].x * gTexture.Sample(gSampler, input.uv4.zw);
	output.color += gWeights[1].y * gTexture.Sample(gSampler, input.uv5.zw);
	output.color += gWeights[1].z * gTexture.Sample(gSampler, input.uv6.zw);
	output.color += gWeights[1].w * gTexture.Sample(gSampler, input.uv7.zw);
	output.color.a = 1.0f;
	return output;
}
