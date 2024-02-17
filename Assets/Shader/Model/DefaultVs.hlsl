#include "Default.hlsli"

struct VsInput
{
    float32_t4 pos : POSITION0;
    float32_t4 norm : NORMAL0;
    float32_t2 uv : TEXCOORD0;
};

cbuffer cbuff0 : register(b0)
{
    float32_t4x4 gWorld;
};

VsOutput main(VsInput input)
{
    VsOutput output;

    // pos
    // input.pos.w = 0.0f;
    float32_t4 wpos = mul(input.pos, gWorld);
    output.wpos = wpos.xyz;
    output.svpos = mul(wpos, mul(gCamera.view, gCamera.proj));
    // norm
    input.norm.w = 0.0f;
    output.wnorm = normalize(mul(input.norm, gWorld)).xyz;
    output.vnorm = mul(float32_t4(output.wnorm, 0.0f), gCamera.view).xyz;
    // uv
    output.uv = input.uv;

    return output;
}
