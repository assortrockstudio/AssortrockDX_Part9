#include "pch.h"
#include "CSkyBox.h"

#include "CTransform.h"

CSkyBox::CSkyBox()
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_Type(SKYBOX_TYPE::SPHERE)
{
	SetSkyBoxType(m_Type);

	SetFrustumCheck(false);
}

CSkyBox::~CSkyBox()
{

}

void CSkyBox::finaltick()
{
}

void CSkyBox::render()
{
	if (nullptr == GetMesh() || nullptr == GetMaterial())
	{
		return;
	}

	// ������Ʈ�� ��ġ���� ������۸� ���ؼ� ���ε�
	Transform()->Binding();

	// ����� ���̴� ���ε�
	GetMaterial()->SetScalarParam(INT_0, m_Type);

	if (SKYBOX_TYPE::SPHERE == m_Type)
		GetMaterial()->SetTexParam(TEX_0, m_SkyBoxTex);
	else if(SKYBOX_TYPE::CUBE == m_Type)
		GetMaterial()->SetTexParam(TEX_CUBE_0, m_SkyBoxTex);
	
	GetMaterial()->Binding();

	// �޽� ���ε� �� ������
	GetMesh()->render();
}

void CSkyBox::SetSkyBoxType(SKYBOX_TYPE _Type)
{
	if (m_Type != _Type)
		m_SkyBoxTex = nullptr;

	m_Type = _Type;

	if (SKYBOX_TYPE::SPHERE == m_Type)
	{
		SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
	}	
	else if (SKYBOX_TYPE::CUBE == m_Type)
	{
		SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CubeMesh"));
	}

	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"SkyBoxMtrl"));
}