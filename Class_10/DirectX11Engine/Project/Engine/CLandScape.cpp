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
	Transform()->Binding();

	GetMaterial()->Binding();

	GetMesh()->render();
}

