#ifndef _DECAL
#define _DECAL

#include "value.fx"

// ==================
// Decal Shader
// MRT      : DECAL
// DOMAIN   : DOMAIN_DECAL
// Mesh     : CubeMesh
// Parameter
#define PositionTarget g_tex_0
#define OutputTarget   g_tex_1
// ====================
struct VS_IN
{
    float3 vPos : POSITION;    
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
};


VS_OUT VS_Decal(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
        
    return output;
}


struct PS_OUT
{
    float4 vColor : SV_Target0;  
    float4 vEmissive : SV_Target3;    
};

PS_OUT PS_Decal(VS_OUT _in)
{
    PS_OUT output = (PS_OUT)0.f;
    
    float2 vScreenUV = _in.vPosition.xy / vResolution;    
    float4 vViewPos = PositionTarget.Sample(g_sam_0, vScreenUV);
    
    if (0.f == vViewPos.a)
        discard;
        
    float4 vLocalPos = mul(mul(float4(vViewPos.xyz, 1.f), g_matViewInv), g_matWorldInv);
    
    if (   0.5f < abs(vLocalPos.x) 
        || 0.5f < abs(vLocalPos.y)
        || 0.5f < abs(vLocalPos.z) )
    {
        discard;
    }
    
    float4 vOutputColor = float4(0.2f, 0.8f, 0.2f, 1.f);   
    
    if(g_btex_1)
    {
        float2 vUV = float2(vLocalPos.x + 0.5f, 1.f - (vLocalPos.y + 0.5f));        
        vOutputColor = g_tex_1.Sample(g_sam_0, vUV) * g_float_0;
    }
    
    
    output.vColor = vOutputColor;
    output.vEmissive = float4(0.f, 0.f, 0.f, 1.f);
    
    return output;
}











#endif