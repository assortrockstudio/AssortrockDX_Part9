#ifndef _LANDSCAPE
#define _LANDSCAPE

#include "value.fx"


// =====================
// LandScpae Shader
// MRT : Deferred
// DOMAIN : Deferred
// Mesh : 자체 Mesh 
// =====================
struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};

VS_OUT VS_LandScape(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    // 정점쉐이더는 어떠한 역할도 수행하지 않는다.
    // 입력값을 HullShader 로 그대로 넘긴다.
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    
    output.vTangent = _in.vTangent;
    output.vNormal = _in.vNormal;
    output.vBinormal = _in.vBinormal;    
    
    return output;
}

// ===========
// Hull Shader
// ===========
struct TessLevel
{
    float arrEdge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

// 패치상수함수 - 패치 당 호출되는 함수
TessLevel PatchConstFunc(InputPatch<VS_OUT, 3> _Patch, uint _PatchID : SV_PrimitiveID)
{
    TessLevel Level = (TessLevel) 0.f;
        
    Level.arrEdge[0] = 32.f;
    Level.arrEdge[1] = 32.f;
    Level.arrEdge[2] = 32.f;
    Level.Inside = 32.f;
    
    return Level;
}

struct HS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};

// 정점 당 호출되는 함수
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[maxtessfactor(64)]
[patchconstantfunc("PatchConstFunc")]
HS_OUT HS_LandScape(InputPatch<VS_OUT, 3> _Patch
                , uint _VtxIdx : SV_OutputControlPointID
                , uint _PatchID : SV_PrimitiveID)
{
    HS_OUT output = (HS_OUT) 0.f;
    
    
    output.vPos = _Patch[_VtxIdx].vPos;
    output.vUV = _Patch[_VtxIdx].vUV;
    
    output.vTangent = _Patch[_VtxIdx].vTangent;
    output.vNormal = _Patch[_VtxIdx].vNormal;
    output.vBinormal = _Patch[_VtxIdx].vBinormal;
    
    return output;
}


// =============
// Domain Shader
// =============
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;
    float3 vViewBinormal : BINORMAL;
};

[domain("tri")]
DS_OUT DS_LandScape(const OutputPatch<HS_OUT, 3> _Patch
             , float3 _Weight : SV_DomainLocation
             , TessLevel _TessLevel)
{
    DS_OUT output = (DS_OUT) 0.f;
    
    float3 vLocalPos = (float3) 0.f;
    float2 vUV = (float2) 0.f;
    
    float3 vTangent = (float3) 0.f;
    float3 vNormal = (float3) 0.f;
    float3 vBinormal = (float3) 0.f;    
    
    for (int i = 0; i < 3; ++i)
    {
        vLocalPos += _Patch[i].vPos * _Weight[i];
        vUV += _Patch[i].vUV * _Weight[i];
        
        vTangent += _Patch[i].vTangent * _Weight[i];
        vNormal += _Patch[i].vNormal * _Weight[i];
        vBinormal += _Patch[i].vBinormal * _Weight[i];
    }
    
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;
        
    output.vViewPos = mul(float4(vLocalPos, 1.f), g_matWV);
    output.vViewTangent = normalize(mul(float4(vTangent, 0.f), g_matWV));
    output.vViewBinormal = normalize(mul(float4(vBinormal, 0.f), g_matWV));
    output.vViewNormal = normalize(mul(float4(vNormal, 0.f), g_matWV));
    
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


PS_OUT PS_LandScape(DS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    output.vColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    output.vNormal = _in.vViewNormal;    
    output.vPosition = _in.vViewPos;    
    
    return output;
}





#endif