#ifndef _RAYCAST
#define _RAYCAST

#include "value.fx"
#include "func.fx"


struct tRaycastOut
{
    float2  Location;
    float   Distance;
    int     Success;
};

RWStructuredBuffer<tRaycastOut> m_OutBuffer : register(u0);

#define FACE_X      g_int_0
#define FACE_Z      g_int_1

#define RayStart    g_vec4_0.xyz
#define RayDir      g_vec4_1.xyz

[numthreads(32, 32, 1)]
void CS_Raycast(int3 _ID : SV_DispatchThreadID)
{
    int2 ID = _ID.xy;
    
    // 스레드가 담당할 지역영역을 초과해서 배정된 스레드인 경우
    if (FACE_X * 2 <= ID.x || FACE_Z <= ID.y)
    {
        return;
    }
        
    float3 vTriPos[3] = { 0.f, 0.f, 0.f };
    
    if (0 == ID.x % 2)
    {
        // 아래쪽 삼각형 
        // 2
        // | \
        // 0--1
        vTriPos[0].x = ID.x / 2;
        vTriPos[0].z = ID.y;
        vTriPos[0].y = 0.f;
        
        vTriPos[1].x = (ID.x / 2) + 1;
        vTriPos[1].z = ID.y;
        vTriPos[1].y = 0.f;
        
        vTriPos[2].x = ID.x / 2;
        vTriPos[2].z = ID.y + 1;
        vTriPos[2].y = 0.f;        
    }
    
    else
    {
        // 위쪽 삼각형         
        // 1--0
        //  \ |
        //    2
        vTriPos[0].x = (ID.x / 2) + 1;
        vTriPos[0].z = ID.y + 1;
        vTriPos[0].y = 0.f;
        
        vTriPos[1].x = ID.x / 2;
        vTriPos[1].z = ID.y + 1;
        vTriPos[1].y = 0.f;
        
        vTriPos[2].x = (ID.x / 2) + 1;
        vTriPos[2].z = ID.y;
        vTriPos[2].y = 0.f;
    }
    
    float3 vCrossPos = (float3) 0.f;
    float Dist = 0.f;
    
    if (IntersectsRay(vTriPos, RayStart, RayDir, vCrossPos, Dist))
    {
        m_OutBuffer[0].Location = ID.xy;
        m_OutBuffer[0].Success = 1;
    }    
}

#endif