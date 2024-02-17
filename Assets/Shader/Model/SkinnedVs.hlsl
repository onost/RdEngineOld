#include "Default.hlsli"

struct VsInput
{
    float32_t4 pos : POSITION0;
    float32_t4 norm : NORMAL0;
    float32_t2 uv : TEXCOORD0;
    uint32_t4 boneIdx : BONEINDICES0;
    float32_t4 boneWt : WEIGHTS0;
};

cbuffer cbuff0 : register(b0)
{
    float32_t4x4 gWorld;
};

cbuffer cbuff4 : register(b4)
{
    float32_t4x4 gBones[128];
}

VsOutput main(VsInput input)
{
    // Skin Pos
    float32_t4 skinPos = mul(input.pos, gBones[input.boneIdx.x]) * input.boneWt.x;
    skinPos += mul(input.pos, gBones[input.boneIdx.y]) * input.boneWt.y;
    skinPos += mul(input.pos, gBones[input.boneIdx.z]) * input.boneWt.z;
    skinPos += mul(input.pos, gBones[input.boneIdx.w]) * input.boneWt.w;

    // Skin Norm
    float32_t4 skinNorm = mul(input.norm, gBones[input.boneIdx.x]) * input.boneWt.x;
    skinNorm += mul(input.norm, gBones[input.boneIdx.y]) * input.boneWt.y;
    skinNorm += mul(input.norm, gBones[input.boneIdx.z]) * input.boneWt.z;
    skinNorm += mul(input.norm, gBones[input.boneIdx.w]) * input.boneWt.w;

    VsOutput output;
    // pos
    // skinPos.w = 0.0f;
    float32_t4 wpos = mul(skinPos, gWorld);
    output.wpos = wpos.xyz;
    output.svpos = mul(wpos, mul(gCamera.view, gCamera.proj));
    // norm
    skinNorm.w = 0.0f;
    output.wnorm = normalize(mul(skinNorm, gWorld)).xyz;
    output.vnorm = mul(float32_t4(output.wnorm, 0.0f), gCamera.view).xyz;
    // uv
    output.uv = input.uv;

    return output;
}
