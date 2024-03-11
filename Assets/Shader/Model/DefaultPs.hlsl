#include "Default.hlsli"
#include "../Lighting.hlsli"

struct PsOutput
{
    float32_t4 color : SV_TARGET0;
};

struct Material
{
    float32_t4 baseColor;
    float32_t1 shininess;
    bool isShadowCast;
};

ConstantBuffer<Material> gMaterial : register(b2);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PsOutput main(VsOutput input)
{
    PsOutput output;

    // Calculate Light
    float32_t3 toEye = normalize(gCamera.wpos - input.wpos);
    float32_t3 lightColor = CalcDirectionalLight(
        gMaterial.baseColor.rgb, gMaterial.shininess, input.wnorm, toEye); // Directional Light
    lightColor += CalcPointLight(
        gMaterial.baseColor.rgb, gMaterial.shininess, input.wpos, input.wnorm, toEye); // Point Light
    lightColor += CalcSpotLight(
        gMaterial.baseColor.rgb, gMaterial.shininess, input.wpos, input.wnorm, toEye); // Spot Light
    if (gMaterial.isShadowCast)
    {
        lightColor += CalcCircleShadow(input.wpos, toEye); // Circle Shadow
    }

    output.color.rgb = lightColor;
    output.color.a = gMaterial.baseColor.a;
    output.color *= gTexture.Sample(gSampler, input.uv);

    return output;
}
