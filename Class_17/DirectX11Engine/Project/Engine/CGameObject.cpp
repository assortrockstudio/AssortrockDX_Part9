#include "pch.h"
#include "CGameObject.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"

#include "CComponent.h"
#include "CRenderComponent.h"
#include "CScript.h"

#include "CTaskMgr.h"

CGameObject::CGameObject()
	: m_arrCom{}
	, m_RenderCom(nullptr)
	, m_Parent(nullptr)
	, m_LayerIdx(-1)
	, m_Dead(false)
{
}

CGameObject::CGameObject(const CGameObject& _Other)
	: CEntity(_Other)
	, m_arrCom{}
	, m_RenderCom(nullptr)
	, m_Parent(nullptr)
	, m_LayerIdx(-1)
	, m_Dead(false) // ������Ʈ Life üũ	
{
	// ���� ������Ʈ�� ������Ʈ���� �����ؼ� �޾ƿ�
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != _Other.m_arrCom[i])
		{
			AddComponent(_Other.m_arrCom[i]->Clone());			
		}
	}
		
	for (size_t i = 0; i < _Other.m_vecScripts.size(); ++i)
	{
		AddComponent(_Other.m_vecScripts[i]->Clone());
	}
	
	for (size_t i = 0; i < _Other.m_vecChild.size(); ++i)
	{
		AddChild(_Other.m_vecChild[i]->Clone());
	}
}

CGameObject::~CGameObject()
{
	Safe_Del_Array(m_arrCom);
	Safe_Del_Vector(m_vecScripts);
	Safe_Del_Vector(m_vecChild);
}

void CGameObject::begin()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if(nullptr != m_arrCom[i])
			m_arrCom[i]->begin();
	}

	for (size_t i = 0; i < m_vecScripts.size(); ++i)
	{
		m_vecScripts[i]->begin();
	}


	// �ڽ� ������Ʈ begin ȣ��
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->begin();
	}
}

void CGameObject::tick()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->tick();
	}

	for (size_t i = 0; i < m_vecScripts.size(); ++i)
	{
		m_vecScripts[i]->tick();
	}

	// �ڽ� ������Ʈ tick ȣ��
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->tick();
	}
}

void CGameObject::finaltick()
{
	// Component finaltick ȣ��
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->finaltick();
	}

	// �Ҽ� Layer�� �ڽ��� ���
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	CLayer* pLayer = pCurLevel->GetLayer(m_LayerIdx); 
	pLayer->RegisterObject(this);


	// �ڽ� ������Ʈ finaltick ȣ��
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->finaltick();
	}
}

void CGameObject::render()
{
	if (m_RenderCom)
	{
		m_RenderCom->render();
	}
}

void CGameObject::AddComponent(CComponent* _Component)
{
	// �Է����� ������ ������Ʈ�� Ÿ���� Ȯ���Ѵ�.
	COMPONENT_TYPE type = _Component->GetComponentType();

	// �Էµ� ������Ʈ�� Script ������ ���
	if (COMPONENT_TYPE::SCRIPT == type)
	{
		m_vecScripts.push_back((CScript*)_Component);
	}

	else
	{
		// �Է����� ���� ������Ʈ�� �̹� ������ �ִ� ���
		assert(!m_arrCom[(UINT)type]);

		// �Էµ� ������Ʈ�� ������ ���� ������Ʈ���� Ȯ��
		CRenderComponent* pRenderCom = dynamic_cast<CRenderComponent*>(_Component);
		if (nullptr != pRenderCom)
		{
			// �̹� ������ ���� ������Ʈ�� ������ ���
			assert(!m_RenderCom);

			m_RenderCom = pRenderCom;
		}

		// �Էµ� ������Ʈ�� �迭�� �˸��� �ε��� �ڸ��� �ּҰ��� ����Ѵ�.
		m_arrCom[(UINT)type] = _Component;
	}

	// ������Ʈ�� ���� ������Ʈ�� �������� �˸�
	_Component->m_Owner = this;
}


void CGameObject::DisconnectWidthParent()
{
	if (nullptr == m_Parent)
		return;
	
	vector<CGameObject*>::iterator iter = m_Parent->m_vecChild.begin();
	for (; iter != m_Parent->m_vecChild.end(); ++iter)
	{
		if (this == (*iter))
		{
			m_Parent->m_vecChild.erase(iter);
			m_Parent = nullptr;
			return;
		}
	}

	// �ڽ� ������Ʈ�� �θ� �����ߴµ�, �θ� ������Ʈ�� �ڽ��� �𸣴� ���
	assert(nullptr);
}

void CGameObject::RegisterAsParentObject()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	CLayer* pCurLayer = pCurLevel->GetLayer(m_LayerIdx);
	pCurLayer->AddObject(this, false);
}

void CGameObject::AddChild(CGameObject* _ChildObj)
{
	// �Է����� ������ ������Ʈ�� �̹� �ٸ� �θ��� �ڽ��� ���
	if (_ChildObj->m_Parent)
	{
		_ChildObj->DisconnectWidthParent();
	}

	// �Է����� ������ ������Ʈ�� �ֻ��� �θ� ������Ʈ, ���� ���� �ҼӵǾ��ִ� ���
	else if(-1 != _ChildObj->m_LayerIdx)
	{
		CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
		CLayer* pCurLayer = pCurLevel->GetLayer(_ChildObj->m_LayerIdx);
		pCurLayer->DeregisterParentObject(_ChildObj);
	}
	
	_ChildObj->m_Parent = this;
	m_vecChild.push_back(_ChildObj);
}

bool CGameObject::IsAncestor(CGameObject* _Object)
{
	CGameObject* pAncestor = m_Parent;

	while (pAncestor)
	{
		if (pAncestor == _Object)
			return true;

		pAncestor = pAncestor->GetParent();
	}

	return false;
}

void CGameObject::Destroy()
{
	tTask task = {};

	task.Type = TASK_TYPE::DESTROY_OBJECT;
	task.dwParam_0 = (DWORD_PTR)this;

	CTaskMgr::GetInst()->AddTask(task);
}