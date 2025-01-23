#include "pch.h"
#include "CLandScape.h"

#include "CKeyMgr.h"

#include "CTransform.h"
#include "CMaterial.h"
#include "CMesh.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_FaceX(1)
	, m_FaceZ(1)
	, m_BrushIdx(-1)
	, m_BrushScale(Vec2(0.2f, 0.2f))
	, m_IsHeightMapCreated(false)
{
	init();
	SetFrustumCheck(false);
}

CLandScape::~CLandScape()
{
}

void CLandScape::finaltick()
{
	if (m_IsHeightMapCreated && KEY_PRESSED(KEY::LBTN))
	{
		m_HeightMapCS->SetBrushPos(Vec2(0.25f, 0.75f));
		m_HeightMapCS->SetBrushScale(m_BrushScale);
		m_HeightMapCS->SetHeightMap(m_HeightMap);
		m_HeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);

		m_HeightMapCS->Execute();
	}

	if (KEY_TAP(KEY::NUM8))
	{
		++m_BrushIdx;
		if (m_vecBrush.size() <= m_BrushIdx)
			m_BrushIdx = 0;
	}
}

void CLandScape::render()
{
	Binding();
	
	GetMesh()->render();
}

void CLandScape::Binding()
{
	// 위치 정보
	Transform()->Binding();

	// 재질 정보
	GetMaterial()->SetScalarParam(INT_0, m_FaceX);
	GetMaterial()->SetScalarParam(INT_1, m_FaceZ);
	GetMaterial()->SetTexParam(TEX_0, m_HeightMap);

	GetMaterial()->Binding();
}

