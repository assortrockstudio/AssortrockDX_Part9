#pragma once
#include "CEntity.h"

class CComponent;
class CRenderComponent;
class CScript;


#define GET_COMPONENT(Type, TYPE) class C##Type* Type() { return ( C##Type*)m_arrCom[(UINT)COMPONENT_TYPE::TYPE]; }

class CGameObject :
    public CEntity
{
private:
    CComponent*             m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent*       m_RenderCom;
    vector<CScript*>        m_vecScripts;

    CGameObject*            m_Parent;
    vector<CGameObject*>    m_vecChild;

    int                     m_LayerIdx; // GameObject �� �ҼӵǾ��ִ� Layer
    bool                    m_Dead; // ������Ʈ Life üũ

public:
    void begin();               // ������ ���۵� ��, ������ �շ��� ��
    void tick();                // �� �����Ӹ��� ȣ��, DT ���� �� ���� ����
    virtual void finaltick();   // tick ���� �߻��� �ϵ��� ������ �۾� or ���ҽ� ���ε� �� ���� ������ ����
    void render();              // ȭ�鿡 �׷����� �Լ�

public:
    void AddComponent(CComponent* _Component);
    CComponent* GetComponent(COMPONENT_TYPE _Type) { return m_arrCom[(UINT)_Type]; }
    CRenderComponent* GetRenderComponent() { return m_RenderCom; }

    GET_COMPONENT(Transform, TRANSFORM);
    GET_COMPONENT(MeshRender, MESHRENDER);
    GET_COMPONENT(Camera, CAMERA);
    GET_COMPONENT(Collider2D, COLLIDER2D);
    GET_COMPONENT(Animator2D, ANIMATOR2D);
    GET_COMPONENT(Light2D, LIGHT2D);
    GET_COMPONENT(Light3D, LIGHT3D);
    GET_COMPONENT(TileMap, TILEMAP);
    GET_COMPONENT(ParticleSystem, PARTICLESYSTEM);
    GET_COMPONENT(SkyBox, SKYBOX);
    GET_COMPONENT(Decal, DECAL);
    GET_COMPONENT(BoundingBox, BOUNDINGBOX);
    GET_COMPONENT(LandScape, LANDSCAPE);

    void DisconnectWidthParent();
    void RegisterAsParentObject();
    void AddChild(CGameObject* _Object);
    const vector<CGameObject*>& GetChildren() { return m_vecChild; }
    CGameObject* GetParent() { return m_Parent; }
    const vector<CScript*>& GetScripts() { return m_vecScripts; }
    bool IsAncestor(CGameObject* _Object);

    template<typename T>
    T* GetScript();

    bool IsDead() { return m_Dead; }
    void Destroy();

    int GetLayerIdx() { return m_LayerIdx; }

private:
    void SetLayerIdx(int _Idx) { m_LayerIdx = _Idx; }

public:
    CLONE(CGameObject);
    CGameObject();
    CGameObject(const CGameObject& _Other);
    ~CGameObject();

    friend class CLayer;
    friend class CTaskMgr;
};

template<typename T>
inline T* CGameObject::GetScript()
{
    for (size_t i = 0; i < m_vecScripts.size(); ++i)
    {
        if (dynamic_cast<T*>(m_vecScripts[i]))
            return (T*)m_vecScripts[i];
    }

    return nullptr;
}
