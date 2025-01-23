#pragma once
#include "CRenderComponent.h"

class CLandScape :
    public CRenderComponent
{
private:
    UINT                m_FaceX;
    UINT                m_FaceZ;

    Ptr<CTexture>       m_HeightMap;


public:
    void SetFace(UINT _X, UINT _Z);
    void SetHeightMap(Ptr<CTexture> _HeightMap) { m_HeightMap = _HeightMap; }

public:
    void init();
    virtual void finaltick() override;
    virtual void render() override;

private:
    void CreateMesh();
    void Binding();

public:
    CLONE(CLandScape);

public:
    CLandScape();
    ~CLandScape();
};

