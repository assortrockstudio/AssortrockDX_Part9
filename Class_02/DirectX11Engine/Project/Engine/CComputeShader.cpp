#include "pch.h"
#include "CComputeShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"

CComputeShader::CComputeShader(UINT _ThreadPerGroupX, UINT _ThreadPerGroupY, UINT _ThreadPerGroupZ)
	:CShader(ASSET_TYPE::COMPUTE_SHADER)
	, m_ThreadPerGroupX(_ThreadPerGroupX)
	, m_ThreadPerGroupY(_ThreadPerGroupY)
	, m_ThreadPerGroupZ(_ThreadPerGroupZ)
	, m_GroupX(1)
	, m_GroupY(1)
	, m_GroupZ(1)
{
}

CComputeShader::~CComputeShader()
{
}

int CComputeShader::CreateComputeShader(const wstring& _strFilePath, const string& _CSFuncName)
{
	if (FAILED(D3DCompileFromFile(_strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _CSFuncName.c_str(), "cs_5_0", D3DCOMPILE_DEBUG, 0
		, m_CSBlob.GetAddressOf()
		, m_ErrBlob.GetAddressOf())))
	{
		if (nullptr != m_ErrBlob)
		{
			MessageBoxA(nullptr
				, (char*)m_ErrBlob->GetBufferPointer()
				, "��ǻƮ ���̴� ������ ����", MB_OK);
		}
		else
		{
			MessageBox(nullptr
				, L"������ ã�� �� �����ϴ�."
				, L"��ǻƮ ���̴� ������ ����", MB_OK);
		}

		return E_FAIL;
	}
		

	// ComputeShader ��ü ����ä�
	if (FAILED(DEVICE->CreateComputeShader(m_CSBlob->GetBufferPointer()
		, m_CSBlob->GetBufferSize(), nullptr, m_CS.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}


int CComputeShader::Execute()
{
	// �ʿ��� �����͸� ���ε��Ѵ�.
	if (FAILED(Binding()))
	{
		return E_FAIL;
	}

	// ����� �׷� ������ ����Ѵ�.
	CalculateGroupNum();

	// ��������͸� ComputeShader ����
	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Const);
	pCB->Binding_CS();


	// ComputeShader ����
	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);
	CONTEXT->Dispatch(m_GroupX, m_GroupY, m_GroupZ); // Group Count

	Clear();

	return S_OK;
}

