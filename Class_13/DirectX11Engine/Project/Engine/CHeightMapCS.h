#pragma once
#include "CComputeShader.h"
class CHeightMapCS :
    public CComputeShader
{
private:
    Ptr<CTexture>       m_HeightMapTex;

public:
    void SetHeightMap(Ptr<CTexture> _HeightMap) { m_HeightMapTex = _HeightMap; }

public:
    virtual int Binding() override;
    virtual void CalculateGroupNum()override;
    virtual void Clear() override;

public:
    CHeightMapCS();
    ~CHeightMapCS();
};

