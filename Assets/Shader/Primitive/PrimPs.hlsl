#include "Prim.hlsli"

struct PsOutput
{
    float32_t4 color : SV_TARGET0;
};

PsOutput main(VsOutput input)
{
    PsOutput output;
    output.color = input.color;
    return output;
}
