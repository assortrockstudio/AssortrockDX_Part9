#pragma once
#include "CRenderComponent.h"

#include "CHeightMapCS.h"

class CLandScape :
    public CRenderComponent
{
private:
    UINT                    m_FaceX;
    UINT                    m_FaceZ;
    Vec2                    m_BrushScale;
    vector<Ptr<CTexture>>   m_vecBrush;
    UINT                    m_BrushIdx;
    Ptr<CTexture>           m_HeightMap;
    bool                    m_IsHeightMapCreated;
    Ptr<CHeightMapCS>       m_HeightMapCS;



public:
    void SetFace(UINT _X, UINT _Z);
    void SetHeightMap(Ptr<CTexture> _HeightMap) { m_HeightMap = _HeightMap; m_IsHeightMapCreated = false;}
    void AddBrushTexture(Ptr<CTexture> _BrushTex) { m_vecBrush.push_back(_BrushTex); }
    void CreateHeightMap(UINT _Width, UINT _Height);

public:
    void init();
    virtual void finaltick() override;
    virtual void render() override;

private:
    void CreateMesh();
    void CreateComputeShader();
    void Binding();

public:
    CLONE(CLandScape);

public:
    CLandScape();
    ~CLandScape();
};

