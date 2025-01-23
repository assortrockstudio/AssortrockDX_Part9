#pragma once
#include "CComponent.h"

#include "assets.h"

class CRenderComponent :
    public CComponent
{
private:
    Ptr<CMesh>          m_Mesh;

    Ptr<CMaterial>      m_CurMtrl;      // ���� ������� ������ ����
    Ptr<CMaterial>      m_SharedMtrl;   // ���� �Ŵ����� ��ϵ� ������ ����
    Ptr<CMaterial>      m_DynamicMtrl;  // ���� ������ �纻

    bool                m_FrustumCheck; // Frustum Culling ����
    


public:
    virtual void render() = 0;

public:
    void SetMesh(Ptr<CMesh> _Mesh) { m_Mesh = _Mesh; }
    Ptr<CMesh> GetMesh() { return m_Mesh; }

    void SetMaterial(Ptr<CMaterial> _Mtrl);
    Ptr<CMaterial> GetMaterial() { return m_CurMtrl; }

    // ������ �÷��̸�忡���� ��� ����
    Ptr<CMaterial> GetDynamicMaterial();
    void RestoreMaterial();

    void SetFrustumCheck(bool _Check) { m_FrustumCheck = _Check; }
    bool IsFrustumCheck() { return m_FrustumCheck; }

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CRenderComponent(COMPONENT_TYPE _Type);
    CRenderComponent(const CRenderComponent& _Other);
    ~CRenderComponent();
};

