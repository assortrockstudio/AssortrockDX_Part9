#pragma once
#include "CRenderComponent.h"

class CLandScape :
    public CRenderComponent
{
private:
    UINT    m_FaceX;
    UINT    m_FaceZ;


public:
    void SetFace(UINT _X, UINT _Z);

public:
    void init();
    virtual void finaltick() override;
    virtual void render() override;

private:
    void CreateMesh();


public:
    CLONE(CLandScape);

public:
    CLandScape();
    ~CLandScape();
};

