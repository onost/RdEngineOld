
struct VsOutput
{
    float32_t3 wpos : POSITION0;
    float32_t4 svpos : SV_POSITION0;
    float32_t3 wnorm : NORMAL0;
    float32_t3 vnorm : NORMAL1;
    float32_t2 uv : TEXCOORD0;
};

struct Camera
{
    float32_t4x4 view;
    float32_t4x4 proj;
    float32_t3 wpos;
};
ConstantBuffer<Camera> gCamera : register(b1);
