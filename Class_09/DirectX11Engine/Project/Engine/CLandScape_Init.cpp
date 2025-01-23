#include "pch.h"
#include "CLandScape.h"

#include "CAssetMgr.h"

void CLandScape::init()
{
	// LandScape ���� Mesh ����
	CreateMesh();

	// LandScape ���� ���� ����
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"LandScapeMtrl");
	SetMaterial(pMtrl);
}

void CLandScape::SetFace(UINT _X, UINT _Z)
{
	m_FaceX = _X;
	m_FaceZ = _Z;

	// ����� Face ������ ���� �����ϰ�, ����
	CreateMesh();

	// LandScape ���� ���� ����
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"LandScapeMtrl");
	SetMaterial(pMtrl);
}

void CLandScape::CreateMesh()
{

}