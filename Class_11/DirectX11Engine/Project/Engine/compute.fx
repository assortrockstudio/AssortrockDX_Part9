#ifndef _COMPUTE
#define _COMPUTE

#include "value.fx"

// hlsl 5.0 ���� �ִ� �׷�� ���� ������ ������ ������ 1024���̴�.

// SV_GroupThreadID     : �׷� �������� �������� 3���� ��ǥ
// SV_GroupIndex        : �׷� ������ �������� 1���� ��ǥ
// SV_GroupID           : �����尡 ���� �׷��� 3���� ��ǥ
// SV_DispatchThreadID  : ��� �׷��� ������ ��ü ���� 3���� ��ǥ

RWTexture2D<float4> TargetTex : register(u0); // unordered register

#define Width   g_int_0
#define Height  g_int_1
#define Color   g_vec4_0

[numthreads(32, 32, 1)]
void CS_Test(int3 _ThreadID : SV_DispatchThreadID)
{        
    if (Width <= _ThreadID.x || Height <= _ThreadID.y)
    {
        return;
    }    
    
    TargetTex[_ThreadID.xy] = Color;
}

#endif