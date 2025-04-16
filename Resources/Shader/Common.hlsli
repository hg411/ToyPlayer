#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

SamplerState g_pointWrapSampler : register(s0);
SamplerState g_pointClampSampler : register(s1);
SamplerState g_linearWrapSampler : register(s2);
SamplerState g_linearClampSampler : register(s3);

Texture2D iChannel0 : register(t0);
Texture2D iChannel1 : register(t1);
Texture2D iChannel2 : register(t2);
Texture2D iChannel3 : register(t3);

cbuffer ShadertoyParams : register(b0)
{
    float dx;
    float dy;
    float threshold;
    float strength;
    float iTime;
    float padding[3];
};

struct PS_IN
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

#endif // __COMMON_HLSLI__