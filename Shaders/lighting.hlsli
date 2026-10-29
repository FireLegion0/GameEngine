#pragma once

#define LIGHT_DIRECTIONAL   0
#define LIGHT_POINT         1
#define LIGHT_SPOT          2

//#define MAX_LIGHTS          8
static const uint MAX_LIGHTS = 8;

struct Material
{
    float4 color;
    float2 tiling;
    float2 offset;
    float shininess;
    float3 pad;
};

struct Light
{
    float3 lightPos;
    uint type;
    
    float3 lightCol;
    float lightStr;
    
    float3 lightDir;
    float inCut;
    
    float3 ambient;
    float outCut;
    
    float3 pad;
    float range;
};

float calculateAttenuation(float dist, float range)
{
    return pow(1 - saturate(dist / range), 2);
}

float3 getLightVector(Light light, float3 pos, out float atten)
{
    float3 L = 0;
    atten = 1.0f;
    
    switch (light.type)
    {
        case LIGHT_DIRECTIONAL:
            L = normalize(-light.lightDir);
            break;
        case LIGHT_POINT:
        {
            float3 toLight = light.lightPos - pos;
            float dist = length(toLight);
            atten = calculateAttenuation(dist, light.range);
            L = toLight / dist;
        }
            break;
        case LIGHT_SPOT:
        {
            float3 toLight = light.lightPos - pos;
            float dist = length(toLight);
            L = toLight / dist;
            atten = calculateAttenuation(dist, light.range);
            atten *= smoothstep(light.outCut, light.inCut, dot(-L, light.lightDir));
        }
            break;
    }
    
    return L;
}

float3 calculateDiffuse(float3 rad, float3 N, float3 L)
{
    return rad * saturate(dot(N, L));;
}

float3 calculateSpecular(float3 rad, float3 N, float3 L, float3 V, Material material)
{
    float3 specular = float3(0, 0, 0);
    if (dot(N, L) > 0)
    {
        float3 H = normalize(V + L);
        float HdotN = saturate(dot(H, N));
        HdotN = pow(HdotN, material.shininess);
        
        specular = rad * HdotN;
    }
    
    return specular;
}

void calculateLighting(Light light, Material material, float3 position, float3 N, float3 V,
                       out float3 diffuse, out float3 specular)
{
    float attenuation;
    float3 L = getLightVector(light, position, attenuation);

    // light arriving at the surface
    float3 radiance = light.lightCol * light.lightStr * attenuation;

    diffuse += calculateDiffuse(radiance, N, L);
    specular += calculateSpecular(radiance, N, L, V, material);
}