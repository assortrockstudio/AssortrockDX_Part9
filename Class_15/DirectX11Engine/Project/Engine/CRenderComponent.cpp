#include "pch.h"
#include "CRenderComponent.h"


CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
	: CComponent(_Type)
	, m_FrustumCheck(true)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _Other)
	: CComponent(_Other)
	, m_Mesh(_Other.m_Mesh)
	, m_CurMtrl(nullptr)
	, m_SharedMtrl(_Other.m_SharedMtrl)
	, m_DynamicMtrl(nullptr)
{
	// ���� ������Ʈ�� ���� ������ �����ϰ� �ְ�, �װ� ���� ��� ���̶��
	if (nullptr != _Other.m_DynamicMtrl && _Other.m_DynamicMtrl == _Other.m_CurMtrl)
	{
		// ����Ǵ� ������Ʈ�� �ڽŸ��� ���������� ������ �ϰ�,
		GetDynamicMaterial();

		// ���� ������Ʈ�� ���������� ���ð��� �Ȱ��� �޾ƿ;� �Ѵ�.
		m_DynamicMtrl = _Other.m_DynamicMtrl;
	}

	// ���� ������Ʈ�� ���� ��������� Shared ��, ���纻 ������Ʈ�� �Ȱ��� �Ѵ�.
	else if (nullptr != _Other.m_SharedMtrl && _Other.m_SharedMtrl == _Other.m_CurMtrl)
	{
		m_CurMtrl = _Other.m_SharedMtrl;
	}
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _Mtrl)
{
	// ���� ������ ���ù��� �� ����.
	assert(!_Mtrl->IsDynamic());

	m_CurMtrl = m_SharedMtrl = _Mtrl;	
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial()
{
	// ������ �÷��̸�忡���� ��� ����


	// ���� ��ü�� ���������� ���ٸ� nullptr �� �ش�.
	if (nullptr == m_SharedMtrl)
		return nullptr;

	// ���������� ������ ������ ���� ������ �װ� �ش�.
	if (nullptr != m_DynamicMtrl)
	{
		m_CurMtrl = m_DynamicMtrl;
		return m_DynamicMtrl;
	}
	
	m_CurMtrl = m_DynamicMtrl = m_SharedMtrl->GetDynamicMaterial();

	return m_DynamicMtrl;
}

void CRenderComponent::RestoreMaterial()
{
	m_CurMtrl = m_SharedMtrl;
	m_DynamicMtrl = nullptr;
}




void CRenderComponent::SaveToLevelFile(FILE* _File)
{
	SaveAssetRef(m_Mesh, _File);
	SaveAssetRef(m_SharedMtrl, _File);
}

void CRenderComponent::LoadFromLevelFile(FILE* _File)
{
	LoadAssetRef(m_Mesh, _File);
	LoadAssetRef(m_SharedMtrl, _File);

	SetMaterial(m_SharedMtrl);
}
