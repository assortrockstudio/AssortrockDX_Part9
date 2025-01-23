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

#define RayStart    g_vec4_0
#define RayDir      g_vec4_1

[numthreads(32, 32, 1)]
void CS_Raycast(int3 _ID : SV_DispatchThreadID)
{
    
}

#endif