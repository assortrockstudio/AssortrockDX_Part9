#include "pch.h"
#include "CCamera.h"

#include "CKeyMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CRenderComponent.h"

#include "CRenderMgr.h"
#include "CMRT.h"
#include "components.h"

#include "CDevice.h"

CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_Frustum(this)
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
	, m_Frustum(_Other.m_Frustum)
	, m_ProjType(_Other.m_ProjType)
	, m_CamPriority(-1)
	, m_FOV(_Other.m_FOV)
	, m_Far(_Other.m_Far)
	, m_Width(_Other.m_Width)
	, m_AspectRatio(_Other.m_AspectRatio)
	, m_Scale(_Other.m_Scale)
	, m_LayerCheck(_Other.m_LayerCheck)
{
	m_Frustum.SetOwner(this);
}

CCamera::~CCamera()
{
}

void CCamera::begin()
{
	// ������ ���۵� �� ī�޶� RenderMgr �� ���
	CRenderMgr::GetInst()->RegisterCamera(this, m_CamPriority);
}

void CCamera::finaltick()
{
	// View ��� ���
	Vec3 vCamWorldPos = Transform()->GetRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vCamWorldPos.x, -vCamWorldPos.y, -vCamWorldPos.z);

	// ī�޶� �ٶ󺸴� ������ z ���� ó�ٺ����� �ϴ� ȸ������ ȸ����ķ� ���ؾ��Ѵ�.
	// ī�޶��� ��, ��, �� ����
	Vec3 vR = Transform()->GetRelativeDir(DIR_TYPE::RIGHT);
	Vec3 vU = Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vF = Transform()->GetRelativeDir(DIR_TYPE::FRONT);

	Matrix matViewRot = XMMatrixIdentity();
	matViewRot._11 = vR.x;	matViewRot._12 = vU.x;	 matViewRot._13 = vF.x;
	matViewRot._21 = vR.y;	matViewRot._22 = vU.y;	 matViewRot._23 = vF.y;
	matViewRot._31 = vR.z;	matViewRot._32 = vU.z;	 matViewRot._33 = vF.z;

	m_matView = matViewTrans * matViewRot;
	m_matViewInv = XMMatrixInverse(nullptr, m_matView);

	// Proj ��� ���
	if (PROJ_TYPE::PERSPECTIVE == m_ProjType)
	{
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);
	}
	else
	{		
		m_matProj = XMMatrixOrthographicLH(m_Width * m_Scale, (m_Width / m_AspectRatio) * m_Scale, 1.f, m_Far);
	}

	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);

	// ���콺���� Ray ���
	CalcRay();

	// ����ü ���
	m_Frustum.finaltick();
}

void CCamera::render()
{

}

void CCamera::CalcRay()
{
	// ViewPort ����
	CMRT* pSwapChainMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
	const D3D11_VIEWPORT& VP = pSwapChainMRT->GetViewPort();

	// ���� ���콺 ��ǥ
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

	// ���콺�� ���ϴ� ������ ī�޶� ��ġ�� ������.
	m_Ray.vStart = Transform()->GetWorldPos();

	// View ���� �󿡼� ī�޶󿡼� ���콺 ������ ���ϴ� ���⺤�͸� ���Ѵ�.
	//  - ���콺�� �ִ� ��ǥ�� -1 ~ 1 ������ ����ȭ�� ��ǥ�� �ٲ۴�.
	//  - ��������� _11, _22 �� �ִ� ���� Near ���󿡼� Near ���� ���� ���� ���̷� ������
	//  - ���� ViewSpace �󿡼��� Near ���󿡼� ���콺�� �ִ� ������ ���ϴ� ��ġ�� ���ϱ� ���ؼ� ������ ����� 
	//  - ���� Near ���󿡼� ���콺�� ���ϴ� ��ġ�� ��ǥ�� ����
	m_Ray.vDir.x = (((vMousePos.x - VP.TopLeftX) * 2.f / VP.Width) - 1.f) / m_matProj._11;
	m_Ray.vDir.y = -(((vMousePos.y - VP.TopLeftY) * 2.f / VP.Height) - 1.f) / m_matProj._22;
	m_Ray.vDir.z = 1.f;

	// ���� ���Ϳ� ViewMatInv �� ����, ����󿡼��� ������ �˾Ƴ���.
	m_Ray.vDir = XMVector3TransformNormal(m_Ray.vDir, m_matViewInv);
	m_Ray.vDir.Normalize();
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
		CRenderMgr::GetInst()->AddDrawCall();
	}
}

void CCamera::render_decal()
{
	for (size_t i = 0; i < m_vecDecal.size(); ++i)
	{
		m_vecDecal[i]->render();
		CRenderMgr::GetInst()->AddDrawCall();
	}	
}

void CCamera::render_opaque()
{
	for (size_t i = 0; i < m_vecOpaque.size(); ++i)
	{
		m_vecOpaque[i]->render();
		CRenderMgr::GetInst()->AddDrawCall();
	}
}

void CCamera::render_masked()
{
	for (size_t i = 0; i < m_vecMasked.size(); ++i)
	{
		m_vecMasked[i]->render();
		CRenderMgr::GetInst()->AddDrawCall();
	}
}

void CCamera::render_transparent()
{
	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
	{
		m_vecTransparent[i]->render();
		CRenderMgr::GetInst()->AddDrawCall();
	}
}

void CCamera::render_particle()
{
	for (size_t i = 0; i < m_vecParticle.size(); ++i)
	{
		m_vecParticle[i]->render();
		CRenderMgr::GetInst()->AddDrawCall();
	}	
}

void CCamera::render_postprocess()
{
	for (size_t i = 0; i < m_vecPostProcess.size(); ++i)
	{
		CRenderMgr::GetInst()->CopyRenderTarget();
		m_vecPostProcess[i]->render();
		CRenderMgr::GetInst()->AddDrawCall();
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

				// FrustumCheck ����� ����ϴ���, ����Ѵٸ� Frustum ���ο� �������� üũ
				if (vecObjects[j]->GetRenderComponent()->IsFrustumCheck())
				{
					// vecObjects[j] �� BoundingBox �� Ȯ��
					if (vecObjects[j]->BoundingBox())
					{
						Vec3 vWorldPos = vecObjects[j]->BoundingBox()->GetWorldPos();
						float Radius = vecObjects[j]->BoundingBox()->GetRadius();

						if (false == m_Frustum.FrustumSphereCheck(vWorldPos, Radius))
						{
							continue;
						}
					}

					else
					{
						Vec3 vWorldPos = vecObjects[j]->Transform()->GetWorldPos();
						Vec3 vWorldScale = vecObjects[j]->Transform()->GetWorldScale();

						float Radius = vWorldScale.x;
						if (Radius < vWorldScale.y) Radius = vWorldScale.y;
						if (Radius < vWorldScale.z) Radius = vWorldScale.z;

						if (false == m_Frustum.FrustumSphereCheck(vWorldPos, Radius))
						{
							continue;
						}
					}				
				}


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