#pragma once
#include "CEntity.h"

class CMRT :
    public CEntity
{
private:
    Ptr<CTexture>   m_RTTex[8];    
    Vec4            m_ClearColor[8];
    UINT            m_RTCount;


    Ptr<CTexture>   m_DSTex;

public:
    void OMSet();
    void ClearTarget();
    void ClearDepthStencil();
    void Create(Ptr<CTexture>* _RTTex, UINT _RTCount, Ptr<CTexture> _DSTex);
    void SetClearColor(UINT _Idx, Vec4 _ClearColor) { m_ClearColor[_Idx] = _ClearColor; }

    CLONE_DISABLE(CMRT);
public:   
    CMRT();
    ~CMRT();
};

