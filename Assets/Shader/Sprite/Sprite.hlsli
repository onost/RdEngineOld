
struct VsOutput
{
    float32_t4 svpos : SV_POSITION0;
    float32_t2 uv : TEXCOORD0;
};

struct Constants
{
    float32_t4x4 mat;
    float32_t4 color;
};
ConstantBuffer<Constants> gConstants : register(b0);
