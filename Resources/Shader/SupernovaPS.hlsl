#include "Common.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

#define pi 3.14159265

// iq's noise
float noise(in float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f * f * (3.0 - 2.0 * f);
    float2 uv = (p.xy + float2(37.0, 17.0) * p.z) + f.xy;
    float2 rg = g_texture0.SampleLevel(g_sampler, (uv + 0.5) / 256.0, 0.0).yx;
    return 1.0 - 0.82 * lerp(rg.x, rg.y, f.z);
}

float fbm(float3 p)
{
    return noise(p * .06125) * .5 + noise(p * .125) * .25 + noise(p * .25) * .125 + noise(p * .4) * .2;
}

float length2(float2 p)
{
    return sqrt(p.x * p.x + p.y * p.y);
}

float length8(float2 p)
{
    p = p * p;
    p = p * p;
    p = p * p;
    return pow(p.x + p.y, 1.0 / 8.0);
}


float Disk(float3 p, float3 t)
{
    float2 q = float2(length2(p.xy) - t.x, p.z * 0.5);
    return max(length8(q) - t.y, abs(p.z) - t.z);
}

static const float nudge = 0.9; // size of perpendicular vector
static const float normalizer = 1.0 / sqrt(1.0 + nudge * nudge); // pythagorean theorem on that perpendicular to maintain scale
float SpiralNoiseC(float3 p)
{
    float n = 0.0; // noise amount
    float iter = 2.0;
    for (int i = 0; i < 8; i++)
    {
        // add sin and cos scaled inverse with the frequency
        n += -abs(sin(p.y * iter) + cos(p.x * iter)) / iter; // abs for a ridged look
        // rotate by adding perpendicular and scaling down
        p.xy += float2(p.y, -p.x) * nudge;
        p.xy *= normalizer;
        // rotate on other axis
        p.xz += float2(p.z, -p.x) * nudge;
        p.xz *= normalizer;
        // increase the frequency
        iter *= 1.733733;
    }
    return n;
}

float NebulaNoise(float3 p)
{
    float final = Disk(p.xzy, float3(2.0, 1.8, 1.25)); // t1: 수평 반경, t2: 성운 두께, t3: 수직확산
    final += fbm(p * 90.);
    final += SpiralNoiseC(p.zxy * 0.5123 + 100.0) * 3.0;

    return final;
}

float map(float3 p)
{
    float MouseX = 1.0f;
   
    float angle = MouseX * 0.008 * pi + iTime * 0.1;
    float c = cos(angle);
    float s = sin(angle);
    p.xz = float2(c * p.x + s * p.z, c * p.z - s * p.x);
    float NebNoise = abs(NebulaNoise(p / 0.5) * 0.5);
    
    return NebNoise + 0.07;
}

float3 computeColor(float density, float radius)
{
	// color based on density alone, gives impression of occlusion within
	// the media
    //float3 result = lerp(float3(1.0, 0.9, 0.8), float3(0.4, 0.15, 0.1), density); // 변화
    float3 result = lerp(float3(1.0, 1.0, 1.0), float3(0.5, 0.5, 0.5), density); // 변화
    
	
	// color added to the media
    float3 colCenter = 7.0 * float3(0.8, 1.0, 1.0);
    float3 colEdge = 1.5 * float3(0.48, 0.53, 0.5);
    result *= lerp(colCenter, colEdge, min((radius + 0.05) / 0.9, 1.15));
	
    return result;
}

float3 RaySphereIntersect(float3 org, float3 dir)
{
    float b = dot(dir, org);
    float c = dot(org, org) - 8.0; // radius2
    float delta = b * b - c;

    if (delta < 0.0)
        return float3(-1.0, -1.0, -1.0);
    else
    {
        float deltasqrt = sqrt(delta);
        return float3(-b - deltasqrt, -b + deltasqrt, deltasqrt);
    }
}

float3 ToneMapFilmicALU(float3 _color)
{
    _color = max(float3(0, 0, 0), _color - float3(0.004, 0.004, 0.004));
    _color = (_color * (6.2 * _color + float3(0.5, 0.5, 0.5))) / (_color * (6.2 * _color + float3(1.7, 1.7, 1.7)) + float3(0.06, 0.06, 0.06));
    return _color;
}

static const float2 iResolution = float2(2560.0, 1440.0);

float4 main(PS_IN input) : SV_Target
{
	// ro: ray origin
	// rd: direction of the ray
    //float2 fragCoord = input.texcoord * iResolution.xy;
    float2 fragCoord = float2(input.texcoord.x * iResolution.x, (1.0 - input.texcoord.y) * iResolution.y);
    float3 rd = normalize(float3((fragCoord.xy - 0.5 * iResolution.xy) / iResolution.y, 1.0));
    float3 ro = float3(0., 0., -6.0); // 카메라 위치

	// ld, td: local, total density 
	// w: weighting factor
    float ld = 0.0, td = 0.0, w = 0.0;

	// t: length of the ray
	// d: distance function
    float d = 1.0, t = 0.0;
    
    const float h = 0.1;
   
    float4 sum = float4(0, 0, 0, 0);
   
    float min_dist = 0.0, max_dist = 0.0;

    float3 hitInfo = RaySphereIntersect(ro, rd);
     
    if (hitInfo.y > 0.0 && hitInfo.z > 0)
    {
        min_dist = hitInfo.x;
        max_dist = hitInfo.y;
        t = min_dist * step(t, min_dist);
   
	    // raymarch loop
        [loop]
        for (int i = 0; i < 1024; i++)
        {
            float3 pos = ro + t * rd;
            
            if (td > 0.9 || sum.a > 0.99 || t > max_dist) // || d < 0.1 * t || t > 10.0
                break;

            // evaluate distance function
            d = map(pos);
		       
		    // change this string to control density 
            d = max(d, 0.0);
        
            // point light calculations
            float3 ldst = float3(0, 0, 0) - pos;
            float lDist = max(length(ldst), 0.001);

            // the color of light 
            //float3 lightColor = float3(1.0, 0.5, 0.25);
            
            float _T = lDist * 2.3 + 2.6; // <-v endless tweaking, 변화
            float3 lightColor = 0.4 + 0.5 * cos(_T + pi * 0.5 * float3(-0.5, 0.15, 0.5));
        
            sum.rgb += (float3(0.67, 0.75, 1.00) / (lDist * lDist * 10.0) / 80.0); // star itself
            sum.rgb += (lightColor / exp(lDist * lDist * lDist * 0.08) / 30.0); // bloom
        
            if (d < h)
            {
			// compute local density 
                ld = h - d;
            
            // compute weighting factor 
                w = (1.0 - td) * ld;
     
			// accumulate density
                td += w + 1.0 / 200.0;
		
                float4 col = float4(computeColor(td, lDist), td);
            
            // emission
                sum += sum.a * float4(sum.rgb, 0.0) * 0.2;
            
			// uniform scale density
                col.a *= 0.2; // 0.2
			// colour by alpha
                col.rgb *= col.a;
			// alpha blend in contribution
                sum += col * (1.0 - sum.a);
       
            }
      
            td += 1.0 / 70.0;

            //float2 uv = fragCoord.xy / iResolution.xy; // 변화
            float2 uv = fragCoord.xy / iResolution.xy;
            uv.y*=120.0;
            uv.x*=280.0;
            d = abs(d) * (0.8 + 0.08 * g_texture0.SampleLevel(g_sampler,float2(uv.y,-uv.x+0.5*sin(4.*iTime+uv.y*4.0)), 0.0).r);

            // trying to optimize step size near the camera and near the light source
            t += max(d * 0.1 * max(min(length(ldst), length(ro)), 1.0), 0.01);
        }
    
    // simple scattering
        sum *= 1.0 / exp(ld * 0.2) * 0.6;
        
        sum = clamp(sum, 0.0, 1.0);
   
        sum.xyz = sum.xyz * sum.xyz * (3.0 - 2.0 * sum.xyz);
    }


    // stars background
    if (td< 0.8)
    {
        float freq = 500.0;
        float3 stars = float3(noise(rd * freq) * 0.5 + 0.5, noise(rd * freq) * 0.5 + 0.5, noise(rd * freq) * 0.5 + 0.5);
        float3 starbg = float3(0, 0, 0);
        starbg = lerp(starbg, float3(0.8,0.9,1.0), smoothstep(0.99, 1.0, stars)*clamp(dot(float3(0, 0, 0),rd)+0.75,0.0,1.0));
        starbg = clamp(starbg, 0.0, 1.0);
        sum.xyz += starbg; 
    }

    
    //return float4(sum.xyz, 1.0);
    return float4(ToneMapFilmicALU(sum.xyz*2.2), 1.0); // 톤 매핑
}