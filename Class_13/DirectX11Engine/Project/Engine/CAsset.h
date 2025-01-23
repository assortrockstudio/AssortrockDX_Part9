#pragma once
#include "CEntity.h"
#include "ptr.h"

class CAsset :
    public CEntity
{
private:
    wstring             m_Key;          // �ε��� Ű��
    wstring             m_RelativePath; // ����� 
    const ASSET_TYPE    m_Type;         // Asset �� Ÿ��
    int                 m_RefCount;     // ���� ī��Ʈ
    bool                m_bEngine;      // �������� ���� ������Ų ���� ����

public:
    const wstring& GetKey() { return m_Key; }
    const wstring& GetRelativePath() { return m_RelativePath; }
    ASSET_TYPE GetAssetType() { return m_Type; }
    bool IsEngineAsset() { return m_bEngine; }
    int GetRefCount() { return m_RefCount; }


protected:
    void SetRelativePath(const wstring& _strPath) { m_RelativePath = _strPath; }


private:
    virtual int Load(const wstring& _FilePath) = 0;
    virtual int Save(const wstring& _FilePath) = 0;

private:
    void AddRef() { m_RefCount++; }
    void Release() 
    { 
        m_RefCount--; 
        if (m_RefCount <= 0)
        {
            delete this;
        }
    }

public:
    virtual CAsset* Clone() = 0;
public:
    CAsset(ASSET_TYPE _Type, bool _bEngine);
    CAsset(const CAsset& _Origin);
    ~CAsset();

    template<typename T>
    friend class Ptr;

    friend class CAssetMgr;
};