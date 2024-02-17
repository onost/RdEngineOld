#include "Sprite.hlsli"

struct VsInput
{
    float32_t4 pos : POSITION0;
    float32_t2 uv : TEXCOORD0;
};

VsOutput main(VsInput input)
{
    VsOutput output;
    output.svpos = mul(input.pos, gConstants.mat);
    output.uv = input.uv;
    return output;
}
