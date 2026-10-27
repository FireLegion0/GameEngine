#pragma once

#define LIGHT_DIRECTIONAL   0
#define LIGHT_POINT         1
#define LIGHT_SPOT          2
struct Material
{
    float2 tiling;
    float2 offset;
    float shininess;
};

struct Light
{
    float3 pos;
    uint type;
    
    float3 dir;
    float pad2;
    
    float3 col;
    float str;
    
    float3 ambient;
    float pad3;
};
