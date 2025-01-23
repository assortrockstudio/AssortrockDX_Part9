#pragma once

#include "CRenderComponent.h"

class CDecal :
    public CRenderComponent
{
private:
    Ptr<CTexture>   m_OutputTex;
    float           m_Alpha;
    bool            m_AsEmissive;

public:
    void SetOutputTexture(Ptr<CTexture> _Tex) { m_OutputTex = _Tex; }


public:
    virtual void finaltick() override;
    virtual void render() override;

public:
    CLONE(CDecal);

public:
    CDecal();
    ~CDecal();
};

