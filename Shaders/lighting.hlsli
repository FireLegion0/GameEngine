#pragma once

#define LIGHT_DIRECTIONAL   0
#define LIGHT_POINT         1
#define LIGHT_SPOT          2
struct Material
{
    float4 color;
    float2 tiling;
    float2 offset;
    float shininess;
};

struct Light
{
    float3 pos;
    uint type;
    
    float3 dir;
    float inCut;
    
    float3 col;
    float str;
    
    float3 ambient;
    float outCut;
};

float3 getLightVector(Light light, float3 pos, out float atten)
{
    float3 L;
    atten = 1.0f;
    
    switch (light.type)
    {
        case LIGHT_DIRECTIONAL:
            L = normalize(-light.dir);
            break;
        case LIGHT_POINT:
            L = normalize(light.dir - pos);
            break;
        case LIGHT_SPOT:
            L = normalize(light.dir - pos);
            atten = smoothstep(light.outCut, light.inCut, dot(-L, light.dir));
            break;
    }
    
    return L;
}
