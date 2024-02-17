#include "Prim.hlsli"

struct VsInput
{
    float32_t4 pos : POSITION0;
    float32_t4 color : COLOR0;
};

cbuffer cbuff0 : register(b0)
{
    float32_t4x4 gViewProj;
};

VsOutput main(VsInput input)
{
    VsOutput output;
    output.svpos = mul(input.pos, gViewProj);
    output.color = input.color;
    return output;
}
