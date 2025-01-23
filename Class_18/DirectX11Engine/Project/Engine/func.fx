#ifndef _FUNC
#define _FUNC

#include "value.fx"


float3 CalLight2D(int _LightIdx, float3 _vWorldPos)
{    
    tLightInfo info = g_Light2D[_LightIdx];
  
    // ���� ó��
    float3 vLightPow = (float3) 0.f;
        
    // Directional Light
    if (0 == info.LightType)
    {
        vLightPow = info.Light.vDiffuse.rgb + info.Light.vAmbient.rgb;
    }
    
    // PointLight
    else if (1 == info.LightType)
    {
        // �ȼ��� ���彺���̽�
        float fDist = distance(info.WorldPos.xy, _vWorldPos.xy);
        
        float fRatio = 0.f;
       
        //fRatio = 1.f - saturate(fDist / info.Range);
        fRatio = cos((fDist / info.Range) * (PI / 2.f));        
        
        if (fDist < info.Range)
        {
            vLightPow = info.Light.vDiffuse.xyz * fRatio;
        }
    }
    
    // SpotLight
    else
    {
        
    }
        
    return vLightPow;
}

void CalLight3D(int _LightIdx, float3 _ViewPos, float3 _ViewNormal, inout tLight _Light)
{    
    // ������ ����
    tLightInfo LightInfo = g_Light3D[_LightIdx];
        
    // ���� ����
    float LightPow = (float) 0.f;
    
    // ���� ����
    float3 vLight = (float3) 0.f;
    
    // ���� ������� �ȼ�(��ü) ������ ��������
    _ViewNormal = normalize(_ViewNormal);
    
    // �Ÿ��� ���� ���� ����
    float DistancePow = 1.f;
    
    // Directional
    if (0 == LightInfo.LightType)
    {
        // View �����̽����� ������ ����
        vLight = normalize(mul(float4(LightInfo.WorldDir, 0.f), g_matView).xyz);        
        
        // ����Ʈ �ڻ��ι�Ģ
        LightPow = saturate(dot(_ViewNormal, -vLight));
    }
    
    // Point
    else if(1 == LightInfo.LightType)
    {
        // View �����̽����� ������ ����
        // ��ü(�ȼ�) �� ��ġ - ������ ��ġ
        float3 vLightPos = mul(float4(LightInfo.WorldPos.xyz, 1.f), g_matView).xyz;       
        vLight = normalize(_ViewPos - vLightPos);
        
        // ����Ʈ �ڻ��ι�Ģ
        LightPow = saturate(dot(_ViewNormal, -vLight));
        
        // ������ ��ü ������ �Ÿ��� ���Ѵ�.
        float Distance = distance(_ViewPos, vLightPos);
        
        // �Ÿ��� ���� ���� ���� ������
        //DistancePow =  saturate(1.f - Distance / LightInfo.Range);
        DistancePow = saturate(cos(saturate(Distance / LightInfo.Range) * (PI / 2.f)));
    }
    
    // Spot
    else
    {
        
    } 
    
    // �ݻ�
    // Reflect
    float3 vReflect = normalize(vLight + dot(-vLight, _ViewNormal) * 2.f * _ViewNormal);
        
    // �ü�����
    float3 vEye = normalize(_ViewPos);
    
    // �ݻ�� ���� ī�޶�� ������ ������ cos0 �� ȯ��
    float ReflectPow = saturate(dot(-vEye, vReflect));
    ReflectPow = pow(ReflectPow, 20);
         
    // ���� ũ�⸦ ���
    _Light.vDiffuse.rgb     += LightInfo.Light.vDiffuse.rgb * LightPow * DistancePow;
    _Light.vAmbient.rgb     += LightInfo.Light.vAmbient.rgb;
    _Light.vMaxSpecular.rgb += LightInfo.Light.vDiffuse.rgb * LightInfo.Light.vMaxSpecular.rgb * ReflectPow * DistancePow;
}


int IsBinding(in Texture2D _tex)
{
    uint width = 0;
    uint height = 0;
    _tex.GetDimensions(width, height);
    
    if (width && height)
        return 1;
    else
        return 0;
}



float3 GetRandom(in Texture2D _Noise, float _NormalizedThreadID)
{
    // ��ƼŬ�� �������� ������ ��ġ�� �����ϱ�
    float2 vUV = (float2) 0.f;      
    
    vUV.x = _NormalizedThreadID + (Time * 0.1f);
    vUV.y = (sin((vUV.x - Time) * PI * 20.f) * 0.5f) + (Time * 0.2f);
                
    float3 vNoise = _Noise.SampleLevel(g_sam_0, vUV, 0).xyz;
    
    return vNoise;
}

int IntersectsRay(float3 _Pos[3], float3 _vStart, float3 _vDir, out float3 _CrossPos, out float _Dist)
{
        
    
    return 0;
}

#endif