
// Max light count
static const uint32_t kDirectionalLightCount = 5;
static const uint32_t kPointLightCount = 5;
static const uint32_t kSpotLightCount = 5;
static const uint32_t kCircleShadowCount = 10;

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

struct PointLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
    float radius;
    float decay;
};

struct SpotLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
    float32_t3 position;
    float radius;
    float decay;
    float cosInner;
    float cosOuter;
};

struct CircleShadow
{
    float32_t3 direction;
    float intensity;
    float32_t3 position;
    float radius;
    float decay;
    float cosInner;
    float cosOuter;
};

cbuffer cbuff3 : register(b3)
{
    DirectionalLight gDirectionalLights[kDirectionalLightCount];
    PointLight gPointLights[kPointLightCount];
    SpotLight gSpotLights[kSpotLightCount];
    CircleShadow gCircleShadows[kCircleShadowCount];
}



// Directional Light
float32_t3 CalcDirectionalLight(float32_t3 baseColor, float shininess, float32_t3 wnorm, float32_t3 toEye)
{
    float32_t3 outColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (uint32_t i = 0; i < kDirectionalLightCount; ++i)
    {
        float NdotL = dot(wnorm, -gDirectionalLights[i].direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f); // Half Lambert
        float32_t3 diffuse = baseColor * gDirectionalLights[i].color.rgb * cos * gDirectionalLights[i].intensity;

        // Phong
        float32_t3 R = reflect(gDirectionalLights[i].direction, wnorm);
        float RdotE = dot(R, toEye);
        float specularPow = pow(saturate(RdotE), shininess);
        float32_t3 specular = float32_t3(1.0f, 1.0f, 1.0f) * gDirectionalLights[i].color.rgb * specularPow * gDirectionalLights[i].intensity;

        // Rim Light
        /*float rimPow1 = 1.0f - max(0.0f, NdotL);
        float rimPow2 = 1.0f - max(0.0f, vnorm.z * -1.0f);
        float rimPow = rimPow1 * rimPow2;
        outColor += baseColor * pow(rimPow, 5.0f);*/

        outColor += diffuse + specular;
    }
    return outColor;
}

// Point Light
float32_t3 CalcPointLight(float32_t3 baseColor, float shininess, float32_t3 wpos, float32_t3 wnorm, float32_t3 toEye)
{
    float32_t3 outColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (uint32_t i = 0; i < kPointLightCount; ++i)
    {
        float32_t3 direction = normalize(wpos - gPointLights[i].position);
        float distance = length(gPointLights[i].position - wpos);
        float factor = pow(saturate(-distance / gPointLights[i].radius + 1.0f), gPointLights[i].decay);

        float NdotL = dot(wnorm, -direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float32_t3 diffuse = baseColor * gPointLights[i].color.rgb * cos * factor * gPointLights[i].intensity;

        float32_t3 R = reflect(direction, wnorm);
        float RdotE = dot(R, toEye);
        float specularPow = pow(saturate(RdotE), shininess);
        float32_t3 specular = float32_t3(1.0f, 1.0f, 1.0f) * gPointLights[i].color.rgb * specularPow * factor * gPointLights[i].intensity;

        outColor += diffuse + specular;
    }
    return outColor;
}

// Spot Light
float32_t3 CalcSpotLight(float32_t3 baseColor, float shininess, float32_t3 wpos, float32_t3 wnorm, float32_t3 toEye)
{
    float32_t3 outColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (uint32_t i = 0; i < kSpotLightCount; ++i)
    {
        float32_t3 direction = normalize(wpos - gSpotLights[i].position);
        float distance = length(gSpotLights[i].position - wpos);
        float factor = pow(saturate(-distance / gSpotLights[i].radius + 1.0f), gSpotLights[i].decay);
        float angle = dot(direction, gSpotLights[i].direction);
        factor *= saturate((angle - gSpotLights[i].cosOuter) / (gSpotLights[i].cosInner - gSpotLights[i].cosOuter));

        float NdotL = dot(wnorm, -direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float32_t3 diffuse = baseColor * gSpotLights[i].color.rgb * cos * factor * gSpotLights[i].intensity;

        float32_t3 R = reflect(direction, wnorm);
        float RdotE = dot(R, toEye);
        float specularPow = pow(saturate(RdotE), shininess);
        float32_t3 specular = float32_t3(1.0f, 1.0f, 1.0f) * gSpotLights[i].color.rgb * specularPow * factor * gSpotLights[i].intensity;

        outColor += diffuse + specular;
    }
    return outColor;
}

// Circle Shadow
float32_t3 CalcCircleShadow(float32_t3 wpos, float32_t3 toEye)
{
    float32_t3 outColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (uint32_t i = 0; i < kCircleShadowCount; ++i)
    {
        float32_t3 direction = normalize(wpos - gCircleShadows[i].position);
        float distance = length(gCircleShadows[i].position - wpos);
        float factor = pow(saturate(-distance / gCircleShadows[i].radius + 1.0f), gCircleShadows[i].decay);
        float angle = dot(direction, gCircleShadows[i].direction);
        factor *= saturate((angle - gCircleShadows[i].cosOuter) / (gCircleShadows[i].cosInner - gCircleShadows[i].cosOuter));

        outColor -= factor * gCircleShadows[i].intensity;
    }
    return outColor;
}
