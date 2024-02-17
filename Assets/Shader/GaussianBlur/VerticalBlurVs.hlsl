#include "Blur.hlsli"

struct VsInput
{
	float32_t4 pos : POSITION0;
	float32_t2 uv : TEXCOORD0;
};

struct Constants
{
	float32_t4x4 mat;
	float32_t4 color;// Not used
};
ConstantBuffer<Constants> gConstants : register(b0);

VsOutput main(VsInput input)
{
	VsOutput output;
	output.svpos = mul(input.pos, gConstants.mat);

	// Get texture size
	float2 size;
	float numLevels;
    gTexture.GetDimensions(0, size.x, size.y, numLevels);

	// y
	output.uv0.xy = float32_t2(0.0f, 1.0f / size.y);
	output.uv1.xy = float32_t2(0.0f, 3.0f / size.y);
	output.uv2.xy = float32_t2(0.0f, 5.0f / size.y);
	output.uv3.xy = float32_t2(0.0f, 7.0f / size.y);
	output.uv4.xy = float32_t2(0.0f, 9.0f / size.y);
	output.uv5.xy = float32_t2(0.0f, 11.0f / size.y);
	output.uv6.xy = float32_t2(0.0f, 13.0f / size.y);
	output.uv7.xy = float32_t2(0.0f, 15.0f / size.y);
	output.uv0.zw = output.uv0.xy * -1.0f;
	output.uv1.zw = output.uv1.xy * -1.0f;
	output.uv2.zw = output.uv2.xy * -1.0f;
	output.uv3.zw = output.uv3.xy * -1.0f;
	output.uv4.zw = output.uv4.xy * -1.0f;
	output.uv5.zw = output.uv5.xy * -1.0f;
	output.uv6.zw = output.uv6.xy * -1.0f;
	output.uv7.zw = output.uv7.xy * -1.0f;

	output.uv0 += float32_t4(input.uv, input.uv);
	output.uv1 += float32_t4(input.uv, input.uv);
	output.uv2 += float32_t4(input.uv, input.uv);
	output.uv3 += float32_t4(input.uv, input.uv);
	output.uv4 += float32_t4(input.uv, input.uv);
	output.uv5 += float32_t4(input.uv, input.uv);
	output.uv6 += float32_t4(input.uv, input.uv);
	output.uv7 += float32_t4(input.uv, input.uv);

	return output;
}
