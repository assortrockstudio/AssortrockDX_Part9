#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CAssetMgr.h"
#include "assets.h"

#include "CMRT.h"


void CRenderMgr::init()
{
	// RenderTarget �� ������� �� �ִ� �ؽ��ĸ� ����
	Vec2 vRenderResol = CDevice::GetInst()->GetRenderResolution();
	m_RenderTargetCopyTex = CAssetMgr::GetInst()->CreateTexture(L"RenderTargetCopyTex"
		, (UINT)vRenderResol.x, (UINT)vRenderResol.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);


	// MRT ����
	CreateMRT();
}

void CRenderMgr::CreateMRT()
{
	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
	CMRT* pMRT = nullptr;

	// =============
	// SwapChain MRT
	// =============	
	pMRT = m_MRT[(UINT)MRT_TYPE::SWAPCHAIN] = new CMRT;

	Ptr<CTexture> RTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");

	// DepthStencil Texture �� �����Ѵ�.
	Ptr<CTexture> DSTex = CAssetMgr::GetInst()->CreateTexture(L"DepthStencilTex"
								, (UINT)vResolution.x, (UINT)vResolution.y
								, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);

	pMRT->Create(&RTTex, 1, DSTex);

	
	// ============
	// Deferred MRT
	// ============
	{
		pMRT = m_MRT[(UINT)MRT_TYPE::DEFERRED] = new CMRT;

		Ptr<CTexture> arrRTTex[8] =
		{
			// 0. ColorTarget ( ��ü�� ����)
			CAssetMgr::GetInst()->CreateTexture(L"ColorTargetTex"
								, (UINT)vResolution.x, (UINT)vResolution.y
								, DXGI_FORMAT_R8G8B8A8_UNORM
								, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 1. NormalTarget ( ��ü ǥ���� �븻����)
			CAssetMgr::GetInst()->CreateTexture(L"NormalTargetTex"
								, (UINT)vResolution.x, (UINT)vResolution.y
								, DXGI_FORMAT_R32G32B32A32_FLOAT
								, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 2. PositionTarget ( ��ü�� ��ġ����)
			CAssetMgr::GetInst()->CreateTexture(L"PositionTargetTex"
								, (UINT)vResolution.x, (UINT)vResolution.y
								, DXGI_FORMAT_R32G32B32A32_FLOAT
								, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 3. EmissiveTarget ( �߱� ���� )
			CAssetMgr::GetInst()->CreateTexture(L"EmissiveTargetTex"
								, (UINT)vResolution.x, (UINT)vResolution.y
								, DXGI_FORMAT_R32G32B32A32_FLOAT
								, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
	
			// 4. DataTarget ( Ŀ���� ������ )
			CAssetMgr::GetInst()->CreateTexture(L"DataTargetTex"
								, (UINT)vResolution.x, (UINT)vResolution.y
								, DXGI_FORMAT_R32G32B32A32_FLOAT
								, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};
		
		pMRT->Create(arrRTTex, 5, DSTex);		
	}

	// ==========
	// Decal MRT
	// ==========
	{
		pMRT = m_MRT[(UINT)MRT_TYPE::DECAL] = new CMRT;

		Ptr<CTexture> arrRTTex[8] =
		{
			// 0. ColorTarget ( ��ü�� ����)
			CAssetMgr::GetInst()->FindAsset<CTexture>(L"ColorTargetTex"),
			
			// 1. NormalTarget ( ��ü ǥ���� �븻����)
			CAssetMgr::GetInst()->FindAsset<CTexture>(L"EmissiveTargetTex"),
		};

		pMRT->Create(arrRTTex, 2, nullptr);
	}

	// =========
	// Light MRT
	// =========
	{
		pMRT = m_MRT[(UINT)MRT_TYPE::LIGHT] = new CMRT;

		Ptr<CTexture> arrRTTex[8] =
		{
			// 1. DiffuseTarget ( ��ü�� ���� �ε��� ���� (���ݻ� ����) )
			CAssetMgr::GetInst()->CreateTexture(L"DiffuseTargetTex"
								, (UINT)vResolution.x, (UINT)vResolution.y
								, DXGI_FORMAT_R32G32B32A32_FLOAT
								, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 2. SpecularTarget ( ��ü���� �ݻ�� ���� ī�޶�� ������ ���� )
			CAssetMgr::GetInst()->CreateTexture(L"SpecularTargetTex"
								, (UINT)vResolution.x, (UINT)vResolution.y
								, DXGI_FORMAT_R32G32B32A32_FLOAT
								, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),			
		};

		pMRT->Create(arrRTTex, 2, nullptr);
	}
}

void CRenderMgr::ClearMRT()
{
	m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->ClearTarget();
	m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->ClearDepthStencil();

	m_MRT[(UINT)MRT_TYPE::DEFERRED]->ClearTarget();
	m_MRT[(UINT)MRT_TYPE::LIGHT]->ClearTarget();
}