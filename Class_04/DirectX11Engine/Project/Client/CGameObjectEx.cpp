#include "pch.h"
#include "CGameObjectEx.h"

#include <Engine/CComponent.h>

void CGameObjectEx::finaltick()
{
	// Component finaltick ȣ��
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != GetComponent(COMPONENT_TYPE(i)))
			GetComponent(COMPONENT_TYPE(i))->finaltick();
	}

	// �ڽ� ������Ʈ finaltick ȣ��
	const vector<CGameObject*>& vecChild = GetChildren();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		vecChild[i]->finaltick();
	}
}