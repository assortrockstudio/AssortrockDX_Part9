#pragma once
#include "CShader.h"

class CComputeShader :
    public CShader
{
private:
    ComPtr<ID3DBlob>                m_CSBlob;
    ComPtr<ID3D11ComputeShader>     m_CS;

protected:
    tMtrlConst                      m_Const;


protected:
    // 1���� �׷� �� ������ ����
    UINT                            m_ThreadPerGroupX;
    UINT                            m_ThreadPerGroupY;
    UINT                            m_ThreadPerGroupZ;

    // �׷� ����
    UINT                            m_GroupX;
    UINT                            m_GroupY;
    UINT                            m_GroupZ;

public:
    virtual int Binding() = 0;
    virtual void CalculateGroupNum() = 0;
    virtual void Clear() = 0;

public:
    int Execute();

public:    
    int CreateComputeShader(const wstring& _strFilePath, const string& _CSFuncName);

public:
    CComputeShader(UINT _ThreadPerGroupX, UINT _ThreadPerGroupY, UINT _ThreadPerGroupZ);
    ~CComputeShader();
};

