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
	Vtx v;
	vector<Vtx> vecVtx;

	for (int Row = 0; Row < m_FaceZ + 1; ++Row)
	{
		for (int Col = 0; Col < m_FaceX + 1; ++Col)
		{
			v.vPos = Vec3((float)Col, 0.f, (float)Row);
			v.vUV = Vec2((float)Col, (float)m_FaceZ - Row);

			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, -1.f);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

			vecVtx.push_back(v);
		}
	}


	vector<UINT> vecIdx;

	for (int Row = 0; Row < m_FaceZ; ++Row)
	{
		for (int Col = 0; Col < m_FaceX; ++Col)
		{
			// 0
			// | \
			// 2--1
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col);
			vecIdx.push_back((m_FaceX + 1) * (Row)     + Col + 1);
			vecIdx.push_back((m_FaceX + 1) * (Row)	   + Col);

			// 1--2
			//  \ |
			//    0
			vecIdx.push_back((m_FaceX + 1) * (Row)     + Col + 1);
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col);
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col + 1);
		}
	}

	Ptr<CMesh> pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	SetMesh(pMesh);
}