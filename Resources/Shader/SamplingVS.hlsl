#include "Common.hlsli"

struct VS_IN
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    
    output.position = float4(input.position, 1.0);
    output.texcoord = input.texcoord;

    return output;
}
