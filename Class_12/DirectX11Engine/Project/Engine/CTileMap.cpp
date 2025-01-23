#include "pch.h"
#include "CTileMap.h"

#include "CAssetMgr.h"
#include "components.h"

#include "CStructuredBuffer.h"
#include "CTexture.h"

#include "CDevice.h"


CTileMap::CTileMap()
	: CRenderComponent(COMPONENT_TYPE::TILEMAP)
	, m_Row(1)
	, m_Col(1)
	, m_AtlasMaxRow(0)
	, m_AtlasMaxCol(0)
	, m_TileEachSize(Vec2(32.f, 32.f))
	, m_CaptureTexMaxResolution(Vec2(2048, 2048))
{
	SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"TileMapMtrl"));
	
	m_TileBuffer = new CStructuredBuffer;
}

CTileMap::CTileMap(const CTileMap& _Other)
	: CRenderComponent(_Other)
	, m_Row(_Other.m_Row)
	, m_Col(_Other.m_Col)
	, m_TileEachSize(_Other.m_TileEachSize)
	, m_Atlas(_Other.m_Atlas)
	, m_AtlasMaxRow(_Other.m_AtlasMaxRow)
	, m_AtlasMaxCol(_Other.m_AtlasMaxCol)
	, m_AtlasTileEachSize(_Other.m_AtlasTileEachSize)
	, m_AtlasResolution(_Other.m_AtlasResolution)
	, m_vecTileInfo(_Other.m_vecTileInfo)
	, m_TileBuffer(nullptr)
{
	//m_TileBuffer = new CStructuredBuffer(*_Other.m_TileBuffer);

	m_TileBuffer = new CStructuredBuffer;
	m_TileBuffer->Create(sizeof(tTileInfo), m_vecTileInfo.size(), SB_TYPE::SRV_ONLY, true, m_vecTileInfo.data());	
}

CTileMap::~CTileMap()
{
	delete m_TileBuffer;
}

void CTileMap::begin()
{
	SetRowCol(m_Row, m_Col);
}

void CTileMap::finaltick()
{

}

void CTileMap::render()
{
	// ��ġ���� 
	Transform()->Binding();

	// ��������
	GetMaterial()->SetTexParam(TEX_0, m_Atlas);
	GetMaterial()->SetScalarParam(VEC2_0, m_AtlasResolution);
	GetMaterial()->SetScalarParam(VEC2_1, m_AtlasTileEachSize);
	GetMaterial()->SetScalarParam(VEC2_2, Vec2(m_Col, m_Row));
	GetMaterial()->SetScalarParam(INT_0, m_AtlasMaxRow);
	GetMaterial()->SetScalarParam(INT_1, m_AtlasMaxCol);
	GetMaterial()->SetScalarParam(INT_2, 0);
	GetMaterial()->SetScalarParam(INT_3, 0); // CaptureMode �� �ƴϴ�.
	GetMaterial()->Binding();

	// Ÿ������ ���ε�
	if (m_TileBuffer->GetElementCount() < m_vecTileInfo.size())
		m_TileBuffer->Create(sizeof(tTileInfo), m_vecTileInfo.size(), SB_TYPE::SRV_ONLY, true, m_vecTileInfo.data());
	else
		m_TileBuffer->SetData(m_vecTileInfo.data(), m_vecTileInfo.size());

	m_TileBuffer->Binding(20);

	// �޽� ���ε� �� ������
	GetMesh()->render();

	if (m_EditorCapture)
	{
		CaptureRender();
	}	
}

void CTileMap::CaptureRender()
{
	// 1. ���� Ÿ���� ��Ȳ�� ĸ���� �ؽ���(�׸��� �׸� ������, ����Ÿ��) �� üũ
	CheckCaptureTexture();

	// 2. ����Ÿ���� �Ͻ������� ��ü�Ǿ�� ��
	// ������ ���õǾ��ִ� Ÿ���� ������ �˾Ƴ�
	ComPtr<ID3D11RenderTargetView> pRTV = nullptr;
	ComPtr<ID3D11DepthStencilView> pDSV = nullptr;
	CONTEXT->OMGetRenderTargets(1, pRTV.GetAddressOf(), pDSV.GetAddressOf());

	// CaptureTexture �� ����Ÿ���� ����ó����
	CONTEXT->OMSetRenderTargets(1, m_CaptureTex->GetRTV().GetAddressOf(), nullptr);

	// ViewPort ����
	// �ؽ��� ũ�� ������ ����
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = m_CaptureTex->GetWidth();
	viewport.Height = m_CaptureTex->GetHeight();
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	CONTEXT->RSSetViewports(1, &viewport);

	// 3. Ÿ���� ��ü�� ����Ÿ������ ������ �Ѵ�.	
	GetMaterial()->SetScalarParam(INT_3, 1); // CaptureMode �̴�
	GetMaterial()->Binding();
	GetMesh()->render();

	// 4. ĸó �������� ���� �Ŀ��� �ٽ� ���� ����Ÿ�ٰ� �����ؽ��ķ� �ǵ�����
	CONTEXT->OMSetRenderTargets(1, pRTV.GetAddressOf(), pDSV.Get());

	// ViewPort ����
	// ������ ȭ�鿡 ������ ������ ����	
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = CDevice::GetInst()->GetRenderResolution().x;
	viewport.Height = CDevice::GetInst()->GetRenderResolution().y;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	CONTEXT->RSSetViewports(1, &viewport);
}

void CTileMap::CheckCaptureTexture()
{
	// 1. �ؽ��İ� �����ϴ���
	if (nullptr == m_CaptureTex)
	{
		Vec2 vResolution = Vec2((float)m_Col * m_TileEachSize.x, (float)m_Row * m_TileEachSize.y);

		if (m_CaptureTexMaxResolution.x < vResolution.x)
			vResolution.x = m_CaptureTexMaxResolution.x;
		if (m_CaptureTexMaxResolution.y < vResolution.y)
			vResolution.y = m_CaptureTexMaxResolution.y;

		m_CaptureTex = new CTexture(true);
		m_CaptureTex->Create((UINT)vResolution.x, (UINT)vResolution.y
							, DXGI_FORMAT_R8G8B8A8_UNORM
							, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);
	}
	else
	{
		// Capture Texture �� �����Ǿ�������, ����Ǵ� �ػ� ���� �������, ���� �����.
		Vec2 vResolution = Vec2((float)m_Col * m_TileEachSize.x, (float)m_Row * m_TileEachSize.y);

		if (m_CaptureTexMaxResolution.x < vResolution.x)
			vResolution.x = m_CaptureTexMaxResolution.x;
		if (m_CaptureTexMaxResolution.y < vResolution.y)
			vResolution.y = m_CaptureTexMaxResolution.y;

		if (m_CaptureTex->GetWidth() < vResolution.x || m_CaptureTex->GetHeight() < vResolution.y)
		{			
			m_CaptureTex = new CTexture(true);
			m_CaptureTex->Create((UINT)vResolution.x, (UINT)vResolution.y
				, DXGI_FORMAT_R8G8B8A8_UNORM
				, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);			
		}
	}
}

void CTileMap::SetRowCol(UINT _Row, UINT _Col)
{
	m_Row = _Row;
	m_Col = _Col;

	Transform()->SetRelativeScale(Vec3(m_TileEachSize.x * m_Col, m_TileEachSize.y * m_Row, 1.f));

	m_vecTileInfo.clear();
	m_vecTileInfo.resize(m_Row * m_Col);

	for (int i = 0; i < m_vecTileInfo.size(); ++i)
	{
		m_vecTileInfo[i].ImgIdx = i % (m_AtlasMaxRow * m_AtlasMaxCol);
		//m_vecTileInfo[i].ImgIdx = 7;
	}
}

void CTileMap::SetTileEachSize(Vec2 _vSize)
{
	m_TileEachSize = _vSize;
	Transform()->SetRelativeScale(Vec3(m_TileEachSize.x * m_Col, m_TileEachSize.y * m_Row, 1.f));
}

void CTileMap::SetImgIndex(int _Row, int _Col, int _imgidx)
{	
	if(_Row < -1 || m_Row <= _Row || _Col < -1 || m_Col <= _Col)
	{
		return;
	}
	int idx = _Row * m_Col + _Col;
	m_vecTileInfo[idx].ImgIdx = _imgidx;
}

void CTileMap::SetAtlasTexture(Ptr<CTexture> _Tex)
{
	m_Atlas = _Tex;

	if (nullptr != m_Atlas)
	{
		m_AtlasResolution = Vec2(m_Atlas->GetWidth(), m_Atlas->GetHeight());
	}	
}

void CTileMap::SetAtlasTileSize(Vec2 _TileSize)
{
	assert(m_Atlas.Get());	

	// ��Ʋ�� �ؽ�ó �ȿ��� Ÿ�� 1���� �����ϴ� ũ��
	m_AtlasTileEachSize = _TileSize;

	// ��Ʋ�� �ؽ��� �ػ󵵸� Ÿ��1�� ������� ����� 
	// ��Ʋ��Ʈ �ؽ��Ŀ� Ÿ���� ���� � �����ϴ��� Ȯ��
	m_AtlasMaxCol = m_Atlas->GetWidth() / m_AtlasTileEachSize.x;
	m_AtlasMaxRow = m_Atlas->GetHeight() / m_AtlasTileEachSize.y;
}



void CTileMap::SaveToLevelFile(FILE* _File)
{
	CRenderComponent::SaveToLevelFile(_File);

	fwrite(&m_Row, sizeof(UINT), 1, _File);
	fwrite(&m_Col, sizeof(UINT), 1, _File);
	fwrite(&m_TileEachSize, sizeof(Vec2), 1, _File);
	
	SaveAssetRef(m_Atlas, _File);
	fwrite(&m_AtlasTileEachSize, sizeof(Vec2), 1, _File);


	size_t i = m_vecTileInfo.size();
	fwrite(&i, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < m_vecTileInfo.size(); ++i)
	{
		fwrite(&m_vecTileInfo[i], sizeof(tTileInfo), 1, _File);
	}
}

void CTileMap::LoadFromLevelFile(FILE* _File)
{
	CRenderComponent::LoadFromLevelFile(_File);

	fread(&m_Row, sizeof(UINT), 1, _File);
	fread(&m_Col, sizeof(UINT), 1, _File);
	fread(&m_TileEachSize, sizeof(Vec2), 1, _File);
	LoadAssetRef(m_Atlas, _File);
	fread(&m_AtlasTileEachSize, sizeof(Vec2), 1, _File);

	size_t size = 0;
	fread(&size, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < size; ++i)
	{
		tTileInfo info = {};
		fread(&info, sizeof(tTileInfo), 1, _File);
		m_vecTileInfo.push_back(info);
	}

	// ���Ͽ��� ���� ������ TileMap �� ���� ����
	SetAtlasTexture(m_Atlas);
	SetAtlasTileSize(m_AtlasTileEachSize);
	SetRowCol(m_Row, m_Col);
}