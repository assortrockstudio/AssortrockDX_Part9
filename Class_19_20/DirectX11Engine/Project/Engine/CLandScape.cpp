#include "pch.h"
#include "CLandScape.h"

#include "CStructuredBuffer.h"

#include "CRenderMgr.h"
#include "CKeyMgr.h"

#include "CTransform.h"
#include "CCamera.h"

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

	// Raycasting 결과를 받는 용도의 구조화버퍼
	m_RaycastOut = new CStructuredBuffer;
	m_RaycastOut->Create(sizeof(tRaycastOut), 1, SB_TYPE::SRV_UAV, true);
}

CLandScape::~CLandScape()
{
	delete m_RaycastOut;
}

void CLandScape::finaltick()
{
	if (m_IsHeightMapCreated && KEY_PRESSED(KEY::LBTN))
	{
		// RayCasting
		if (Raycasting())
		{
			// 높이맵 설정
			m_HeightMapCS->SetBrushPos(m_RaycastOut);
			m_HeightMapCS->SetBrushScale(m_BrushScale);
			m_HeightMapCS->SetHeightMap(m_HeightMap);
			m_HeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
			m_HeightMapCS->Execute();
		}
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

	GetMaterial()->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);

	// 재질 정보
	GetMaterial()->SetScalarParam(INT_0, m_FaceX);
	GetMaterial()->SetScalarParam(INT_1, m_FaceZ);
	GetMaterial()->SetTexParam(TEX_0, m_HeightMap);

	GetMaterial()->Binding();
}

int CLandScape::Raycasting()
{
	// 현재 시점 카메라 가져오기
	CCamera* pCam = CRenderMgr::GetInst()->GetFOVCamera();
	if (nullptr == pCam)
		return false;

	// 구조화버퍼 클리어
	tRaycastOut output = {};
	m_RaycastOut->SetData(&output, 1);

	// 카메라가 시점에서 마우스를 향하는 Ray 정보를 가져옴
	tRay ray = pCam->GetRay();

	// LandScape 의 WorldInv 행렬 가져옴
	const Matrix& matWorldInv = Transform()->GetWorldMatInv();

	// 월드 기준 Ray 정보를 LandScape 의 Local 공간으로 데려감
	ray.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	ray.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	ray.vDir.Normalize();

	// Raycast 컴퓨트 쉐이더에 필요한 데이터 전달
	m_RaycastCS->SetRayInfo(ray);
	m_RaycastCS->SetFace(m_FaceX, m_FaceZ);
	m_RaycastCS->SetOutBuffer(m_RaycastOut);

	// 컴퓨트쉐이더 실행
	m_RaycastCS->Execute();

	// 결과 확인
	m_RaycastOut->GetData(&output, 1);

	return output.Success;
}

