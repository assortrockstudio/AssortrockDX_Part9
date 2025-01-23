#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CRaycastCS :
    public CComputeShader
{
private:
    UINT                m_FaceX;
    UINT                m_FaceZ;
    tRay                m_Ray;
    CStructuredBuffer*  m_OutBuffer;
    

public:
    void SetRayInfo(const tRay& _ray) { m_Ray = _ray; }
    void SetFace(UINT _FaceX, UINT _FaceZ) { m_FaceX = _FaceX, m_FaceZ = _FaceZ; }
    void SetOutBuffer(CStructuredBuffer* _Buffer) { m_OutBuffer = _Buffer; }


public:
    virtual int Binding() override;
    virtual void CalculateGroupNum()override;
    virtual void Clear() override;

public:
    CRaycastCS();
    ~CRaycastCS();
};

