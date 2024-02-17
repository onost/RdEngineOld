#include "Particle.hlsli"

struct VsInput
{
    float32_t4 pos : POSITION0;
    float32_t4 norm : NORMAL0;
    float32_t2 uv : TEXCOORD0;
};

struct Particle
{
    float32_t4x4 world;
    float32_t4 color;
};

StructuredBuffer<Particle> gParticles : register(t0);

VsOutput main(VsInput input, uint32_t instanceId : SV_InstanceID)
{
    VsOutput output;

    // pos
    // input.pos.w = 0.0f;
    float32_t4 wpos = mul(input.pos, gParticles[instanceId].world);
    output.wpos = wpos.xyz;
    output.svpos = mul(wpos, mul(gCamera.view, gCamera.proj));
    // norm
    input.norm.w = 0.0f;
    output.wnorm = normalize(mul(input.norm, gParticles[instanceId].world)).xyz;
    output.vnorm = mul(float32_t4(output.wnorm, 0.0f), gCamera.view).xyz;
    // uv
    output.uv = input.uv;
    // color
    output.color = gParticles[instanceId].color;

    return output;
}
