#include "pch.h"
#include "TileView.h"

#include "AtlasView.h"

TileView::TileView()
	: TMESub("TileView", "##TileView")
	, m_DefaultSize(500.f)
	, m_CaptureScale(1.f)
	, m_SelectedCol(0)
	, m_SelectedRow(0)
{
}

TileView::~TileView()
{
}

void TileView::render_tick()
{
	CTileMap* pTileMap = GetTileMap();
	if (nullptr == pTileMap)
		return;

	Ptr<CTexture> pCaptureTex = pTileMap->GetCaptureTexture();
	
	// Image ���
	ImTextureID TexID = (nullptr == pCaptureTex) ? nullptr : pCaptureTex->GetSRV().Get();
	ImVec4 BorderColor = (nullptr == pCaptureTex) ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(1.f, 1.f, 1.f, 0.f);

	float AspectRatio = pTileMap->GetColumn() / pTileMap->GetRow();

	ImGui::Image(TexID
		, ImVec2(m_DefaultSize * m_CaptureScale, (m_DefaultSize / AspectRatio) * m_CaptureScale)
		, ImVec2(0.0f, 0.0f)
		, ImVec2(1.0f, 1.0f)
		, ImGui::GetStyleColorVec4(ImGuiCol_Text)
		, BorderColor);

	// �ֱ� ������ ȭ�� ��ġ
	m_ImageRectMin = Vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);	

	// ���콺 �� �Է¿� ���� ���� ����
	if (0.f < ImGui::GetIO().MouseWheel)
	{
		m_CaptureScale += 0.1f;

		if (3.f < m_CaptureScale)
		{
			m_CaptureScale = 3.f;
		}
	}
	else if (ImGui::GetIO().MouseWheel < 0.f)
	{
		m_CaptureScale -= 0.1f;
		if (m_CaptureScale < 0.2f)
		{
			m_CaptureScale = 0.2f;
		}
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		MouseCheck();
	}

	int RowCol[2] = { m_SelectedRow, m_SelectedCol };
	ImGui::InputInt2("Selected Row Col", RowCol, 0);
}

void TileView::Deactivate()
{
	CTileMap* pTileMap = GetTileMap();
	if(nullptr != pTileMap)
		pTileMap->SetCapture(false);
}


void TileView::MouseCheck()
{
	CTileMap* pTileMap = GetTileMap();
	if (nullptr == pTileMap->GetCaptureTexture())
		return;

	// ���� ���콺 ��ǥ��, �̹����� �»���� ���̰��� ���Ѵ�.
	Vec2 vMousePos = Vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	Vec2 OffsetMousePos = vMousePos - m_ImageRectMin;
		
	// Ÿ�ϸ��� ����ũ��� ĸó �ؽ��İ��� ���� ���̸� ���Ѵ�.
	Ptr<CTexture> pCaptureTex = pTileMap->GetCaptureTexture();

	Vec2 TileSize = pTileMap->GetTileSize();
	TileSize *= Vec2(pTileMap->GetColumn(), pTileMap->GetRow());
	Vec2 CaptureTexResolution = Vec2(pCaptureTex->GetWidth(), pCaptureTex->GetHeight());

	// ���� Ÿ���� ĸó �ؽ��ķ� ������ �ɶ� ����� ����
	Vec2 Ratio = CaptureTexResolution / TileSize;

	// ĸó �ؽ��ĸ� ImGui::Imge �� ����Ҷ� ����� ����
	float AspectRatio = pTileMap->GetColumn() / pTileMap->GetRow();

	Ratio *= Vec2( (m_DefaultSize * m_CaptureScale) / CaptureTexResolution.x
		          ,((m_DefaultSize * m_CaptureScale) / AspectRatio) / CaptureTexResolution.y);
	
	Vec2 TileEachSize = pTileMap->GetTileSize() * Ratio;

	m_SelectedCol = (int)(OffsetMousePos.x / TileEachSize.x);
	m_SelectedRow = (int)(OffsetMousePos.y / TileEachSize.y);

	if (OffsetMousePos.x < 0.f || pTileMap->GetColumn() <= m_SelectedCol
		|| OffsetMousePos.y < 0.f || pTileMap->GetRow() <= m_SelectedRow)
	{
		return;
	}


	int imgidx = GetAtlasView()->GetImgIdx();
	if (-1 == imgidx)
	{
		return;
	}

	pTileMap->SetImgIndex(m_SelectedRow, m_SelectedCol, imgidx);
}
