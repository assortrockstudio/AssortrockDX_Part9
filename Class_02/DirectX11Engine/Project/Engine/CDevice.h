#pragma once

#include "CTexture.h"

class CConstBuffer;

class CDevice
	: public CSingleton<CDevice>
{
	SINGLE(CDevice)
private:
	HWND							m_hMainWnd;
	Vec2							m_RenderResolution;

	ComPtr<ID3D11Device>			m_Device;		// GPU �޸� �Ҵ�, Dx11 ���� ��ü ����
	ComPtr<ID3D11DeviceContext>		m_Context;		// GPU ������ ���� ���,
	ComPtr<IDXGISwapChain>			m_SwapChain;	// ����Ÿ�� ���� ����, ȭ�鿡 ���� ����� �Խ�

	ComPtr<ID3D11SamplerState>		m_Sampler[2];
	ComPtr<ID3D11RasterizerState>	m_RS[(UINT)RS_TYPE::END];

	ComPtr<ID3D11BlendState>		m_BS[(UINT)BS_TYPE::END];
	ComPtr<ID3D11DepthStencilState>	m_DS[(UINT)DS_TYPE::END];
	CConstBuffer*					m_CB[(UINT)CB_TYPE::END];

private:
	int CreateSwapChain();
	int CreateView();
	int CreateConstBuffer();
	int CreateSamplerState();
	int CreateRasterizerState();
	int CreateDepthStencilState();
	int CreateBlendState();

public:
	int init(HWND _hWnd, Vec2 _Resolution);	
	void Present(){ m_SwapChain->Present(0, 0); }


	Vec2 GetRenderResolution() { return m_RenderResolution; }

	ID3D11Device* GetDevice() { return m_Device.Get(); }
	ID3D11DeviceContext* GetContext() { return m_Context.Get(); }

	CConstBuffer* GetConstBuffer(CB_TYPE _Type) { return m_CB[(UINT)_Type]; }
	ComPtr<ID3D11RasterizerState> GetRS(RS_TYPE _Type) { return m_RS[(UINT)_Type]; }
	ComPtr<ID3D11DepthStencilState> GetDS(DS_TYPE _Type) { return m_DS[(UINT)_Type]; }
	ComPtr<ID3D11BlendState> GetBS(BS_TYPE _Type) { return m_BS[(UINT)_Type]; }
};

