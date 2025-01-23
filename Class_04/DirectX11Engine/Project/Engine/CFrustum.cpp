#include "pch.h"
#include "CFrustum.h"

#include "CCamera.h"


CFrustum::CFrustum(CCamera* _Owner)
	: m_Camera(_Owner)
{
	// Frustum 초기 값 세팅
	//    4 --- 5
	//   /|  \  |  Far
	//  / 7 --/ 6     
	// 0 -- 1/ /
	// | \  | / 
	// 3 -- 2  Near
	m_arrProjPos[0] = Vec3(-1.f, 1.f, 0.f);
	m_arrProjPos[1] = Vec3(1.f, 1.f, 0.f);
	m_arrProjPos[2] = Vec3(1.f, -1.f, 0.f);
	m_arrProjPos[3] = Vec3(-1.f, -1.f, 0.f);

	m_arrProjPos[4] = Vec3(-1.f, 1.f, 1.f);
	m_arrProjPos[5] = Vec3(1.f, 1.f, 1.f);
	m_arrProjPos[6] = Vec3(1.f, -1.f, 1.f);
	m_arrProjPos[7] = Vec3(-1.f, -1.f, 1.f);
}

CFrustum::~CFrustum()
{
}

void CFrustum::finaltick()
{
	const Matrix& matViewInv = m_Camera->GetViewInvMat();
	const Matrix& matProjInv = m_Camera->GetProjInvMat();
	Matrix matInv = matProjInv * matViewInv;
		
	// 투영공간 끝좌표들 -> ViewSpace -> WorldSpcae
	Vec3 arrWorld[8] = {};
	
	for (int i = 0; i < 8; ++i)
	{
		arrWorld[i] = XMVector2TransformCoord(m_arrProjPos[i], matInv);
	}

	// 6개의 카메라 시야범위 평면을 월드공간 기준으로 만들어낸다.


}