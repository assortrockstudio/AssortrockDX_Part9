#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"


CLayer::CLayer()
{
}

CLayer::~CLayer()
{
	Safe_Del_Vector(m_vecParent);
}

void CLayer::begin()
{
	for (size_t i = 0; i < m_vecParent.size(); ++i)
	{
		m_vecParent[i]->begin();
	}
}

void CLayer::tick()
{
	for (size_t i = 0; i < m_vecParent.size(); ++i)
	{
		m_vecParent[i]->tick();
	}
}

void CLayer::finaltick()
{
	vector<CGameObject*>::iterator iter = m_vecParent.begin();

	for (; iter != m_vecParent.end(); )
	{
		(*iter)->finaltick();

		if ((*iter)->IsDead())
		{
			iter = m_vecParent.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CLayer::AddObject(CGameObject* _Object, bool _ChildMove)
{
	// �ֻ��� �θ� ������Ʈ
	if (!_Object->GetParent())
	{
		m_vecParent.push_back(_Object);		
	}	
	
	// ������Ʈ�� �ڽ��� �����ϰ� �ִٸ�.
	static list<CGameObject*> queue;
	queue.clear();
	queue.push_back(_Object);

	while (!queue.empty())
	{
		CGameObject* pObject = queue.front();
		queue.pop_front();

		if (_ChildMove)
		{
			pObject->m_LayerIdx = m_LayerIdx;
		}
		else
		{
			// �ڽı��� ���̾� �̵��� �ƴϸ�, ���̾�� �ҼӵǴ� �θ� ������Ʈ�� ���̾� �ε��� üũ
			// �ڽ� ������Ʈ�� ���̾� �ε����� -1 �̸�(���Ҽ�), �̷� ���� ���������� ���̾� �ε��� üũ
			if (pObject == _Object || -1 == pObject->m_LayerIdx)
			{
				pObject->m_LayerIdx = m_LayerIdx;
			}		
		}

		vector<CGameObject*> vecChildren = pObject->GetChildren();
		for (size_t i = 0; i < vecChildren.size(); ++i)
		{
			queue.push_back(vecChildren[i]);
		}
	}
}



void CLayer::DeregisterParentObject(CGameObject* _Object)
{
	vector<CGameObject*>::iterator iter = m_vecParent.begin();

	for (iter; iter != m_vecParent.end(); ++iter)
	{
		if ((*iter) == _Object)
		{
			m_vecParent.erase(iter);
			return;
		}
	}

	assert(nullptr);
}