#pragma once
#include "CEntity.h"

#include "CGameObject.h"

#define GET_OTHER_COMPONENT(Type)  C##Type* Type() { return m_Owner->Type(); }

class CRenderComponent;

class CComponent :
    public CEntity
{
private:
    CGameObject*            m_Owner;    // �ش� ������Ʈ�� �����ϰ� �ִ� ������Ʈ �ּ�
    const COMPONENT_TYPE    m_Type;     // ������ ���� ������Ʈ������ ���� enum

public:
    virtual void begin() {}
    virtual void tick() {}
    virtual void finaltick() = 0;

public:
    COMPONENT_TYPE GetComponentType() { return m_Type; }
    CGameObject* GetOwner() { return m_Owner; }
    CRenderComponent* GetRenderComponent() { return m_Owner->GetRenderComponent(); }

    GET_OTHER_COMPONENT(Transform);
    GET_OTHER_COMPONENT(MeshRender);
    GET_OTHER_COMPONENT(Camera);
    GET_OTHER_COMPONENT(Collider2D);
    GET_OTHER_COMPONENT(Animator2D);
    GET_OTHER_COMPONENT(Light2D);
    GET_OTHER_COMPONENT(Light3D);
    GET_OTHER_COMPONENT(TileMap);
    GET_OTHER_COMPONENT(ParticleSystem);
    GET_OTHER_COMPONENT(SkyBox);
    GET_OTHER_COMPONENT(Decal);
    GET_OTHER_COMPONENT(BoundingBox);
    GET_OTHER_COMPONENT(LandScape);

    virtual void SaveToLevelFile(FILE* _File) = 0;
    virtual void LoadFromLevelFile(FILE* _File) = 0;

    virtual CComponent* Clone() = 0;
public:
    CComponent(COMPONENT_TYPE _Type);
    CComponent(const CComponent& _Other);
    ~CComponent();

    friend class CGameObject;
};

