#include "pch.h"
#include "CMRT.h"

#include "CDevice.h"
#include "CTexture.h"

CMRT::CMRT()
	: m_RTTex{}
	, m_ClearColor{}
	, m_RTCount(0)
{
}

CMRT::~CMRT()
{
}

void CMRT::Create(Ptr<CTexture>* _RTTex, UINT _RTCount, Ptr<CTexture> _DSTex)
{
	for (UINT i = 0; i < _RTCount; ++i)
	{
		m_RTTex[i] = _RTTex[i];		
	}

	m_RTCount = _RTCount;

	m_DSTex = _DSTex;
}

void CMRT::OMSet()
{
	ID3D11RenderTargetView* m_RTView[8] = {};
	for (UINT i = 0; i < m_RTCount; ++i)
	{
		m_RTView[i] = m_RTTex[i]->GetRTV().Get();
	}
	
	ID3D11DepthStencilView* DSView = nullptr;
	if (m_DSTex.Get())
		DSView = m_DSTex->GetDSV().Get();

	CONTEXT->OMSetRenderTargets(m_RTCount, m_RTView, DSView);
}

void CMRT::ClearTarget()
{
	for (UINT i = 0; i < m_RTCount; ++i)
	{
		CONTEXT->ClearRenderTargetView(m_RTTex[i]->GetRTV().Get(), m_ClearColor[i]);
	}
}

void CMRT::ClearDepthStencil()
{
	if (nullptr == m_DSTex)
		return;

	CONTEXT->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
