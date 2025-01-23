#pragma once
#include "singleton.h"


class CTimeMgr :
    public CSingleton<CTimeMgr>
{
    SINGLE(CTimeMgr)
private:
    LARGE_INTEGER   m_llFrequency;  // �ʴ� ī��Ʈ ��
    LARGE_INTEGER   m_llCurCount;   // ���� ī��Ʈ ��
    LARGE_INTEGER   m_llPrevCount;  // ���� ������ ī��Ʈ ��

    float           m_DT;           // Delta Time : 1������ �����ϴµ� �ɸ� �ð�, LevelContent �뵵
    float           m_EngineDT;     // Delta Time : 1������ �����ϴµ� �ɸ� �ð�

    float           m_Time;         // ���μ��� ���� ������ ���� �ð�
    float           m_EngineTime;   // ���μ��� ���� ������ ���� �ð� (���� ���¿� ������� ����)
    float           m_AccTime;      // 1�ʸ� üũ�ϱ� ���� �ð� ��������

    UINT            m_FrmCount;
    wchar_t			m_szText[255];

public:
    void init();
    void tick();
    void render();

public:
    float GetDeltaTime() { return m_DT; }
    float GetDeltaTime_Engine() { return m_EngineDT; }
};

