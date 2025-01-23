#pragma once
#include "CShader.h"

class CGraphicShader :
    public CShader
{
private:
    ComPtr<ID3DBlob>                m_VSBlob;
    ComPtr<ID3DBlob>                m_HSBlob;
    ComPtr<ID3DBlob>                m_DSBlob;
    ComPtr<ID3DBlob>                m_GSBlob;
    ComPtr<ID3DBlob>                m_PSBlob;

    ComPtr<ID3D11VertexShader>      m_VS;
    ComPtr<ID3D11HullShader>        m_HS;
    ComPtr<ID3D11DomainShader>      m_DS;
    ComPtr<ID3D11GeometryShader>    m_GS;
    ComPtr<ID3D11PixelShader>       m_PS;

    ComPtr<ID3D11InputLayout>       m_Layout;
    D3D11_PRIMITIVE_TOPOLOGY        m_Topology;

    RS_TYPE                         m_RSType;
    DS_TYPE                         m_DSType;
    BS_TYPE                         m_BSType;

    SHADER_DOMAIN                   m_Domain;


public:
    int CreateVertexShader(const wstring& _strFilePath, const string& _VSFuncName);
    int CreateHullShader(const wstring& _strFilePath, const string& _HSFuncName);
    int CreateDomainShader(const wstring& _strFilePath, const string& _DSFuncName);
    int CreateGeometryShader(const wstring& _strFilePath, const string& _GSFuncName);
    int CreatePixelShader(const wstring& _strFilePath, const string& _PSFuncName);

    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY _Topology) { m_Topology = _Topology; }
    void SetRSType(RS_TYPE _Type) { m_RSType = _Type; }
    void SetDSType(DS_TYPE _Type) { m_DSType = _Type; }
    void SetBSType(BS_TYPE _Type) { m_BSType = _Type; }

    virtual int Binding() override;

    void SetDomain(SHADER_DOMAIN _Domain) { m_Domain = _Domain; }
    SHADER_DOMAIN GetDomain() { return m_Domain; }


public:
    CGraphicShader();
    ~CGraphicShader();
};

