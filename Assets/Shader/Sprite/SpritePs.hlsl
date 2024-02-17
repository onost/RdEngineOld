#include "Sprite.hlsli"

struct PsOutput
{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PsOutput main(VsOutput input)
{
    PsOutput output;
    float32_t4 texColor = gTexture.Sample(gSampler, input.uv);
    output.color = texColor * gConstants.color;
    return output;
}
