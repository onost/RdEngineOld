
struct VsOutput
{
	float32_t4 svpos : SV_POSITION0;
	float32_t4 uv0 : TEXCOORD0;
	float32_t4 uv1 : TEXCOORD1;
	float32_t4 uv2 : TEXCOORD2;
	float32_t4 uv3 : TEXCOORD3;
	float32_t4 uv4 : TEXCOORD4;
	float32_t4 uv5 : TEXCOORD5;
	float32_t4 uv6 : TEXCOORD6;
	float32_t4 uv7 : TEXCOORD7;
};

Texture2D<float32_t4> gTexture : register(t0);
