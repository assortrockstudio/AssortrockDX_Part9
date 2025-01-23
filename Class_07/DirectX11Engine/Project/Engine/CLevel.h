#pragma once
#include "CEntity.h"

class CLayer;
class CGameObject;

class CLevel :
    public CEntity
{
private:
    CLayer*         m_arrLayer[MAX_LAYER];
    LEVEL_STATE     m_State;


public:
    void begin();
    void tick();
    void finaltick();
    void RegisterClear();

public:
    void AddObject(UINT _LayerIdx, CGameObject* _Object, bool _bChildMove = false);
    CLayer* GetLayer(int _iLayerIdx) { return m_arrLayer[_iLayerIdx]; }
    LEVEL_STATE GetState() { return m_State; }

    // Ư�� �̸��� ���� GameObject �� �˻�(���� ���� ����� ������Ʈ ��ȯ)
    CGameObject* FindObjectByName(const wstring& _Name);

    // Ư�� �̸��� ���� GameObject �� ��� ã�Ƽ� ��ȯ
    void FindObjectByName(const wstring& _Name, vector<CGameObject*>& _vecObj);

    // Ư�� ������Ʈ�� ������ GameObject �� �˻�(���� ���� ����� ������Ʈ ��ȯ)
    CGameObject* FindObjectByComponent(COMPONENT_TYPE _Type, const wstring& _Name);

    // Ư�� ������Ʈ�� ���� GameObject �� ��� ã�Ƽ� ��ȯ
    void FindObjectByComponent(COMPONENT_TYPE _Type, vector<CGameObject*>& _vecObj);


private:
    void ChangeState(LEVEL_STATE _NextState);

public:
    CLONE_DISABLE(CLevel);

public:
    CLevel();
    ~CLevel();

    friend class CTaskMgr;
};

