#include "pch.h"
#include "CConstBuffer.h"

#include "CDevice.h"

CConstBuffer::CConstBuffer()
	: m_Type(CB_TYPE::END)
	, m_Desc{}
{
}

CConstBuffer::~CConstBuffer()
{
}

int CConstBuffer::Create(size_t _bufferSize, CB_TYPE _Type)
{
	m_Type = _Type;

	m_Desc.ByteWidth = _bufferSize;
	m_Desc.MiscFlags = 0;

	// ������ �뵵 ����
	m_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// �ѹ� ������ ���Ŀ� �б�, ���� ����
	m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_Desc.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(DEVICE->CreateBuffer(&m_Desc, nullptr, m_CB.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CConstBuffer::SetData(void* _pData)
{
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	
	memcpy(tSub.pData, _pData, m_Desc.ByteWidth);

	CONTEXT->Unmap(m_CB.Get(), 0);
}

void CConstBuffer::Binding()
{
	CONTEXT->VSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->DSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->HSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->GSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->PSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
}

void CConstBuffer::Binding_CS()
{
	CONTEXT->CSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
}

void CConstBuffer::Clear()
{
	ID3D11Buffer* pBuffer = nullptr;
	CONTEXT->VSSetConstantBuffers((UINT)m_Type, 1, &pBuffer);
	CONTEXT->DSSetConstantBuffers((UINT)m_Type, 1, &pBuffer);
	CONTEXT->HSSetConstantBuffers((UINT)m_Type, 1, &pBuffer);
	CONTEXT->GSSetConstantBuffers((UINT)m_Type, 1, &pBuffer);
	CONTEXT->PSSetConstantBuffers((UINT)m_Type, 1, &pBuffer);
}