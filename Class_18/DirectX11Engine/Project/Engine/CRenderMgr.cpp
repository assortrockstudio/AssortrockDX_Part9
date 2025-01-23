#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

#include "CAssetMgr.h"

#include "CCamera.h"
#include "CLight2D.h"
#include "CLight3D.h"

#include "CLevelMgr.h"
#include "CMRT.h"

#include "CFontMgr.h"

CRenderMgr::CRenderMgr()
	: m_EditorCam(nullptr)
	, m_Light2DBuffer(nullptr)
	, m_Light3DBuffer(nullptr)
	, m_MRT{}
	, m_BoundingBox(true)
	, m_TotalDrawCall(0)
{
	m_Light2DBuffer = new CStructuredBuffer;
	m_Light3DBuffer = new CStructuredBuffer;

	Render_Func = &CRenderMgr::render_play;
}

CRenderMgr::~CRenderMgr()
{
	delete m_Light2DBuffer;
	delete m_Light3DBuffer;

	Safe_Del_Array(m_MRT);
}

void CRenderMgr::CopyRenderTarget()
{
	Ptr<CTexture> pRenderTargetTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	CONTEXT->CopyResource(m_RenderTargetCopyTex->GetTex2D().Get(), pRenderTargetTex->GetTex2D().Get());
}


void CRenderMgr::tick()
{
}

void CRenderMgr::render()
{
	if (!CLevelMgr::GetInst()->GetCurrentLevel())
		return;

	m_TotalDrawCall = 0;

	// MRT Clear
	ClearMRT();

	// �������� �ʿ��� ������ ���ε�
	DataBinding();	

	(this->*Render_Func)();

	// ������ Ŭ����
	DataClear();
}


void CRenderMgr::render_play()
{
	// �� ī�޶� �����ִ� ����� ����Ÿ�ٿ� �׸���.
	for (size_t i = 0; i < m_vecCam.size(); ++i)
	{
		if (nullptr == m_vecCam[i])
			continue;

		m_vecCam[i]->render();
	}
}

void CRenderMgr::render_editor()
{
	if (nullptr == m_EditorCam)
		return;

	g_Trans.matView		= m_EditorCam->GetViewMat();
	g_Trans.matViewInv	= m_EditorCam->GetViewInvMat();
	g_Trans.matProj		= m_EditorCam->GetProjMat();
	g_Trans.matProjInv	= m_EditorCam->GetProjInvMat();

	// Shader Domain �� ���� ��ü�� �з��۾�
	m_EditorCam->SortObject();

	// Deferred MRT �� ����
	m_MRT[(UINT)MRT_TYPE::DEFERRED]->OMSet();

	// Deferred ��ü ������
	m_EditorCam->render_deferred();

	// Decal MRT �� ����
	m_MRT[(UINT)MRT_TYPE::DECAL]->OMSet();
	m_EditorCam->render_decal();

	// Light MRT �� ����
	m_MRT[(UINT)MRT_TYPE::LIGHT]->OMSet();	

	// Light ������
	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		m_vecLight3D[i]->Lighting();
		CRenderMgr::GetInst()->AddDrawCall();
	}

	// Color ������ Light ������ ����
	// �������� SwapChain
	// Deferred ������Ʈ���� ������ ��ϵ� Color Ÿ�� + ���� ��ϵǾ��ִ� Diffuse, Specular, Emissive
	m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet();

	Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
	Ptr<CMaterial> pMergeMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"MergeMtrl");

	// Ư�� ����Ÿ���� ȭ�鿡 ����Ϸ��� �ϴ� ���
	if (m_OutputTarget)
	{
		pMergeMtrl->SetTexParam(TEX_0, nullptr);
		pMergeMtrl->SetTexParam(TEX_1, nullptr);
		pMergeMtrl->SetTexParam(TEX_2, nullptr);
		pMergeMtrl->SetTexParam(TEX_3, nullptr);
		pMergeMtrl->SetTexParam(TEX_4, nullptr);
		pMergeMtrl->SetTexParam(TEX_5, m_OutputTargetTex);

		pMergeMtrl->SetScalarParam(INT_0, 1);
	}

	else
	{
		pMergeMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"ColorTargetTex"));
		pMergeMtrl->SetTexParam(TEX_1, CAssetMgr::GetInst()->FindAsset<CTexture>(L"DiffuseTargetTex"));
		pMergeMtrl->SetTexParam(TEX_2, CAssetMgr::GetInst()->FindAsset<CTexture>(L"SpecularTargetTex"));
		pMergeMtrl->SetTexParam(TEX_3, CAssetMgr::GetInst()->FindAsset<CTexture>(L"EmissiveTargetTex"));
		pMergeMtrl->SetTexParam(TEX_4, CAssetMgr::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
		pMergeMtrl->SetTexParam(TEX_5, m_OutputTargetTex);

		pMergeMtrl->SetScalarParam(INT_0, 0);
	}
	
	pMergeMtrl->Binding();
	pRectMesh->render();
	CRenderMgr::GetInst()->AddDrawCall();


	// Forawrd Rendering
	m_EditorCam->render_opaque();
	m_EditorCam->render_masked();
	m_EditorCam->render_transparent();
	m_EditorCam->render_particle();

	// PostProcess
	m_EditorCam->render_postprocess();
}


void CRenderMgr::render_drawcall()
{
	wchar_t szDrawCall[255] = {};

	swprintf_s(szDrawCall, 255, L"Total DrawCall : %d", m_TotalDrawCall);

	// ��Ʈ ���
	CFontMgr::GetInst()->DrawFont(szDrawCall, 10.f, 55.f, 20, FONT_RGBA(255, 30, 30, 255));
}


void CRenderMgr::DataBinding()
{
	// GlobalData ���ε�
	g_GlobalData.Light2DCount = (int)m_vecLight2D.size();
	g_GlobalData.Light3DCount = (int)m_vecLight3D.size();

	static CConstBuffer* GlobalBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	GlobalBuffer->SetData(&g_GlobalData);
	GlobalBuffer->Binding();
	GlobalBuffer->Binding_CS();

	// =======
	// Light2D
	// =======
	// ���� �������� ����ȭ���� ���ũ�Ⱑ �� ������ Ȯ���Ѵ�.
	if (m_Light2DBuffer->GetElementCount() < m_vecLight2D.size())
	{
		m_Light2DBuffer->Create(sizeof(tLightInfo), (UINT)m_vecLight2D.size(), SB_TYPE::SRV_ONLY, true);
	}

	// m_vecLight2D �� ���� ������ ������ ����ȭ���۷� ������.
	static vector<tLightInfo> vecLightInfo;
	vecLightInfo.clear();

	for (size_t i = 0; i < m_vecLight2D.size(); ++i)
	{
		vecLightInfo.push_back(m_vecLight2D[i]->GetLightInfo());
	}

	// ������ ������ 1�� �̻� �ִٸ�
	if (!vecLightInfo.empty())
	{
		m_Light2DBuffer->SetData(vecLightInfo.data(), m_vecLight2D.size());
		m_Light2DBuffer->Binding(15);
	}

	// =======
	// Light3D
	// =======
	// ���� �������� ����ȭ���� ���ũ�Ⱑ �� ������ Ȯ���Ѵ�.
	if (m_Light3DBuffer->GetElementCount() < m_vecLight3D.size())
	{
		m_Light3DBuffer->Create(sizeof(tLightInfo), (UINT)m_vecLight3D.size(), SB_TYPE::SRV_ONLY, true);
	}

	// m_vecLight3D �� ���� ������ ������ ����ȭ���۷� ������.	
	vecLightInfo.clear();
	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		vecLightInfo.push_back(m_vecLight3D[i]->GetLightInfo());
	}

	// ������ ������ 1�� �̻� �ִٸ�
	if (!vecLightInfo.empty())
	{
		m_Light3DBuffer->SetData(vecLightInfo.data(), m_vecLight3D.size());
		m_Light3DBuffer->Binding(16);
	}
}

void CRenderMgr::DataClear()
{
	m_vecLight2D.clear();
	m_vecLight3D.clear();
}

void CRenderMgr::RegisterCamera(CCamera* _Cam, int _Priority)
{
	// ��ϵ� �ε��� ��ġ��ŭ ���Ͱ� Ȯ��� ��Ȳ�� �ƴϸ� ���͸� �ø���.
	if (m_vecCam.size() <= _Priority)
	{
		m_vecCam.resize(_Priority + 1);
	}

	if (m_vecCam[_Priority] && m_vecCam[_Priority] != _Cam)
		assert(nullptr);

	m_vecCam[_Priority] = _Cam;
}

CCamera* CRenderMgr::GetFOVCamera()
{	
	if (m_vecCam.empty())
		return m_EditorCam;
	else
		return m_vecCam[0];	
}