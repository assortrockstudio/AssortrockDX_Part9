#pragma once
#include "CRenderComponent.h"

#include "CParticleTickCS.h"


class CStructuredBuffer;

class CParticleSystem :
    public CRenderComponent
{
private:
    Ptr<CParticleTickCS>    m_TickCS;
    CStructuredBuffer*      m_ParticleBuffer;   // ParticleBuffer
    CStructuredBuffer*      m_SpawnCountBuffer; // SpawnCount ���� �뵵
    CStructuredBuffer*      m_ModuleBuffer;     // Module Data Buffer

    Ptr<CTexture>           m_ParticleTex;      // ��ƼŬ ���� �ؽ���
    float                   m_Time;             // ���� �ð�
    float                   m_BurstTime;        // SpawnBurst üũ�� ����

    // Particle Data
    UINT                    m_MaxParticle;      // ��ƼŬ �ִ� ����    

    tParticleModule         m_Module;           // ��ƼŬ�� ��� ����

public:
    void SetParticleTexture(Ptr<CTexture> _Tex) { m_ParticleTex = _Tex; }
    void SetMaxParticleCount(UINT _Max);

private:
    void CaculateSpawnCount();

public:
    virtual void finaltick() override;
    virtual void render() override;

    CLONE(CParticleSystem);
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
public:
    CParticleSystem();
    CParticleSystem(const CParticleSystem& _Other);
    ~CParticleSystem();
};

