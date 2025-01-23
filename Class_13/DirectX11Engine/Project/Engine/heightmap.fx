#ifndef _HEIGHTMAP
#define _HEIGHTMAP

#include "value.fx"

RWTexture2D<float> HeightMapTex : register(u0);

#define WIDTH       g_int_0
#define HEIGHT      g_int_1

[numthreads(32, 32, 1)]
void CS_HeightMap(int3 _ID : SV_DispatchThreadID)
{
    if (WIDTH <= _ID.x || HEIGHT <= _ID.y)
        return;
    
    HeightMapTex[_ID.xy].r += 0.01f;
}

#endif