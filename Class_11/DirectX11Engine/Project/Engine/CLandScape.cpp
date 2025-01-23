#include "pch.h"
#include "CLandScape.h"

#include "CTransform.h"
#include "CMaterial.h"
#include "CMesh.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_FaceX(1)
	, m_FaceZ(1)
{
	init();
	SetFrustumCheck(false);
}

CLandScape::~CLandScape()
{
}

void CLandScape::finaltick()
{
}

void CLandScape::render()
{
	Binding();
	
	GetMesh()->render();
}



void CLandScape::Binding()
{
	// ��ġ ����
	Transform()->Binding();

	// ���� ����
	GetMaterial()->SetScalarParam(INT_0, m_FaceX);
	GetMaterial()->SetScalarParam(INT_1, m_FaceZ);
	GetMaterial()->SetTexParam(TEX_0, m_HeightMap);

	GetMaterial()->Binding();
}

