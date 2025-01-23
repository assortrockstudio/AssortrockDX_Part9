#include "pch.h"
#include "CLandScape.h"

#include "CAssetMgr.h"

void CLandScape::init()
{
	// LandScape 전용 Mesh 생성
	CreateMesh();

	// LandScape 전용 재질 참조
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"LandScapeMtrl");
	SetMaterial(pMtrl);
}

void CLandScape::SetFace(UINT _X, UINT _Z)
{
	m_FaceX = _X;
	m_FaceZ = _Z;

	// 변경된 Face 개수로 새로 제작하고, 세팅
	CreateMesh();

	// LandScape 전용 재질 참조
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"LandScapeMtrl");
	SetMaterial(pMtrl);
}

void CLandScape::CreateMesh()
{

}