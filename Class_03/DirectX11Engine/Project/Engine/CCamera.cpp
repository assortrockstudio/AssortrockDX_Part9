#include "pch.h"
#include "CCamera.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CRenderComponent.h"

#include "CRenderMgr.h"
#include "CTransform.h"

#include "CDevice.h"

CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_ProjType(PROJ_TYPE::PERSPECTIVE)
	, m_CamPriority(-1)
	, m_FOV((XM_PI / 3.f))
	, m_Far(10000.f)
	, m_Width(0.f)
	, m_Scale(1.f)
	, m_LayerCheck(0)
{
	Vec2 vRenderResol = CDevice::GetInst()->GetRenderResolution();
	m_Width = vRenderResol.x;
	m_AspectRatio = vRenderResol.x / vRenderResol.y;
}

CCamera::CCamera(const CCamera& _Other)
	: CComponent(_Other)
	, m_ProjType(_Other.m_ProjType)
	, m_CamPriority(-1)
	, m_FOV(_Other.m_FOV)
	, m_Far(_Other.m_Far)
	, m_Width(_Other.m_Width)
	, m_AspectRatio(_Other.m_AspectRatio)
	, m_Scale(_Other.m_Scale)
	, m_LayerCheck(_Other.m_LayerCheck)
{
}

CCamera::~CCamera()
{
}

void CCamera::begin()
{
	// 레벨이 시작될 때 카메라를 RenderMgr 에 등록
	CRenderMgr::GetInst()->RegisterCamera(this, m_CamPriority);
}

void CCamera::finaltick()
{
	// View 행렬 계산
	Vec3 vCamWorldPos = Transform()->GetRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vCamWorldPos.x, -vCamWorldPos.y, -vCamWorldPos.z);

	// 카메라가 바라보는 방향을 z 축을 처다보도록 하는 회전량을 회전행렬로 구해야한다.
	// 카메라의 우, 상, 전 벡터
	Vec3 vR = Transform()->GetRelativeDir(DIR_TYPE::RIGHT);
	Vec3 vU = Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vF = Transform()->GetRelativeDir(DIR_TYPE::FRONT);

	Matrix matViewRot = XMMatrixIdentity();
	matViewRot._11 = vR.x;	matViewRot._12 = vU.x;	 matViewRot._13 = vF.x;
	matViewRot._21 = vR.y;	matViewRot._22 = vU.y;	 matViewRot._23 = vF.y;
	matViewRot._31 = vR.z;	matViewRot._32 = vU.z;	 matViewRot._33 = vF.z;

	m_matView = matViewTrans * matViewRot;
	m_matViewInv = XMMatrixInverse(nullptr, m_matView);



	// Proj 행렬 계산
	if (PROJ_TYPE::PERSPECTIVE == m_ProjType)
	{
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);
	}
	else
	{		
		m_matProj = XMMatrixOrthographicLH(m_Width * m_Scale, (m_Width / m_AspectRatio) * m_Scale, 1.f, m_Far);
	}

	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);
}

void CCamera::render()
{

}

void CCamera::SortClear()
{
	m_vecDeferred.clear();
	m_vecDecal.clear();
	m_vecOpaque.clear();
	m_vecMasked.clear();
	m_vecTransparent.clear();
	m_vecParticle.clear();
	m_vecPostProcess.clear();
}

void CCamera::render_deferred()
{
	for (size_t i = 0; i < m_vecDeferred.size(); ++i)
	{
		m_vecDeferred[i]->render();
	}
}

void CCamera::render_decal()
{
	for (size_t i = 0; i < m_vecDecal.size(); ++i)
	{
		m_vecDecal[i]->render();
	}
}

void CCamera::render_opaque()
{
	for (size_t i = 0; i < m_vecOpaque.size(); ++i)
	{
		m_vecOpaque[i]->render();
	}
}

void CCamera::render_masked()
{
	for (size_t i = 0; i < m_vecMasked.size(); ++i)
	{
		m_vecMasked[i]->render();
	}
}

void CCamera::render_transparent()
{
	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
	{
		m_vecTransparent[i]->render();
	}
}

void CCamera::render_particle()
{
	for (size_t i = 0; i < m_vecParticle.size(); ++i)
	{
		m_vecParticle[i]->render();
	}
}

void CCamera::render_postprocess()
{
	for (size_t i = 0; i < m_vecPostProcess.size(); ++i)
	{
		CRenderMgr::GetInst()->CopyRenderTarget();
		m_vecPostProcess[i]->render();
	}
}


void CCamera::SetCameraPriority(int _Priority)
{
	m_CamPriority = _Priority;	
}

void CCamera::LayerCheck(int _LayerIdx)
{
	if (m_LayerCheck & (1 << _LayerIdx))
	{
		m_LayerCheck &= ~(1 << _LayerIdx);
	}
	else
	{
		m_LayerCheck |= (1 << _LayerIdx);
	}
}

void CCamera::SortObject()
{
	SortClear();

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (m_LayerCheck & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObjects = pLayer->GetObjects();

			for (size_t j = 0; j < vecObjects.size(); ++j)
			{
				if( !vecObjects[j]->GetRenderComponent() 
					|| nullptr == vecObjects[j]->GetRenderComponent()->GetMesh()
					|| nullptr == vecObjects[j]->GetRenderComponent()->GetMaterial()
					|| nullptr == vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader())
					continue;

				SHADER_DOMAIN domain = vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader()->GetDomain();

				switch (domain)
				{
				case SHADER_DOMAIN::DOMAIN_DEFERRED:
					m_vecDeferred.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_DECAL:
					m_vecDecal.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_OPAQUE:
					m_vecOpaque.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_MASKED:
					m_vecMasked.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
					m_vecTransparent.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_PARTICLE:
					m_vecParticle.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
					m_vecPostProcess.push_back(vecObjects[j]);
					break;				
				}
			}
		}
	}
}




void CCamera::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fwrite(&m_CamPriority, sizeof(int), 1, _File);
	fwrite(&m_FOV, sizeof(float), 1, _File);
	fwrite(&m_Far, sizeof(float), 1, _File);
	fwrite(&m_Width, sizeof(float), 1, _File);
	fwrite(&m_AspectRatio, sizeof(float), 1, _File);
	fwrite(&m_Scale, sizeof(float), 1, _File);
	fwrite(&m_LayerCheck, sizeof(UINT), 1, _File);
}

void CCamera::LoadFromLevelFile(FILE* _File)
{
	fread(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fread(&m_CamPriority, sizeof(int), 1, _File);
	fread(&m_FOV, sizeof(float), 1, _File);
	fread(&m_Far, sizeof(float), 1, _File);
	fread(&m_Width, sizeof(float), 1, _File);
	fread(&m_AspectRatio, sizeof(float), 1, _File);
	fread(&m_Scale, sizeof(float), 1, _File);
	fread(&m_LayerCheck, sizeof(UINT), 1, _File);
}