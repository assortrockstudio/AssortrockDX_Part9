#ifndef _TESS
#define _TESS

#include "value.fx"

struct VS_IN
{
    float3 vPos : POSITION;   
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;   
    float2 vUV : TEXCOORD;    
};

VS_OUT VS_Tess(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    // ����� ���Ҷ� 3���� ��ǥ�� 4�������� Ȯ��(������ǥ)    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);  
    output.vUV = _in.vUV;
    
    return output;
}


float4 PS_Tess(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    vColor = float4(1.f, 0.f, 1.f, 1.f);
    
    return vColor;
}




#endif