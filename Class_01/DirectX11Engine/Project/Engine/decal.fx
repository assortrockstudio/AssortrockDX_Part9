#ifndef _DECAL
#define _DECAL

// ==================
// Decal Shader
// MRT      : Deferred
// DOMAIN   : DOMAIN_DECAL
// Parameter
#define PositionTarget g_tex_0
#define OutputTarget   g_tex_1
// ====================
struct VS_IN
{
    
    
};

struct VS_OUT
{
    
};


VS_OUT VS_Decal(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    return output;
}


struct PS_OUT
{
    float4 vColor : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
    float4 vEmissive : SV_Target3;
    float4 vData : SV_Target4;
};


PS_OUT PS_Decal(VS_OUT _in)
{
    PS_OUT output = (PS_OUT)0.f;
        
    
    
    return output;
}











#endif