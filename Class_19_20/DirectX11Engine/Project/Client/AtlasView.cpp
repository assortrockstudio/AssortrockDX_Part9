#include "pch.h"
#include "AtlasView.h"

#include <Engine/CTexture.h>

AtlasView::AtlasView()
	: TMESub("AtlasView", "##AtlasView")
	, m_DefaultSize(200.f)
	, m_AtlasScale(1.f)
	, m_ImgIdx(-1)
{
}

AtlasView::~AtlasView()
{
}

void AtlasView::render_tick()
{
	CTileMap* pTileMap = GetTileMap();
	if (nullptr == pTileMap)
		return;

	Ptr<CTexture> pAtlas = pTileMap->GetAtlas();
	if (nullptr == pAtlas)
		return;

	// Image ���
	ImTextureID TexID = (nullptr == pAtlas) ? nullptr : pAtlas->GetSRV().Get();
	ImVec4 BorderColor = (nullptr == pAtlas) ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(1.f, 1.f, 1.f, 0.f);

	float AspectRatio = pAtlas->GetWidth() / pAtlas->GetHeight();

	ImGui::Image(TexID
		, ImVec2(m_DefaultSize * m_AtlasScale, (m_DefaultSize / AspectRatio) * m_AtlasScale)
		, ImVec2(0.0f, 0.0f)
		, ImVec2(1.0f, 1.0f)
		, ImGui::GetStyleColorVec4(ImGuiCol_Text)
		, BorderColor);

	// �ֱ� ������ ȭ�� ��ġ
	m_ImageRectMin = Vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
	ImVec2 vMousePos = ImGui::GetMousePos();

	float rectmin[2] = { m_ImageRectMin.x, m_ImageRectMin.y };
	ImGui::InputFloat2("RectMin", rectmin, 0);

	float mousepos[2] = { vMousePos.x, vMousePos.y };	
	ImGui::InputFloat2("MousePos", mousepos, 0);

	ImGui::InputInt("Image Index", &m_ImgIdx, 0, ImGuiInputTextFlags_ReadOnly);


	if (0.f < ImGui::GetIO().MouseWheel)
	{
		m_AtlasScale += 0.1f;

		if (3.f < m_AtlasScale)
		{
			m_AtlasScale = 3.f;
		}
	}	
	else if(ImGui::GetIO().MouseWheel < 0.f)
	{
		m_AtlasScale -= 0.1f;
		if (m_AtlasScale < 0.2f)
		{
			m_AtlasScale = 0.2f;
		}
	}

	if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		MouseCheck();
	}
	
	if (m_ImgIdx != -1)
	{
		DrawSelectedTile();
	}	
}

void AtlasView::MouseCheck()
{
	// ���� ���콺 ��ǥ��, �̹����� �»���� ���̰��� ���Ѵ�.
	Vec2 vMousePos = Vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);	
	Vec2 OffsetMousePos = vMousePos - m_ImageRectMin;

	// Ÿ�ϸ��� �����ϴ� ��Ʋ�� �ؽ����� ���� �ػ󵵿�, AtlasView ���� ����ϴ� Image ���� ���� ���̰��� ���Ѵ�.
	CTileMap* pTileMap = GetTileMap();
	Ptr<CTexture> pAtlas = pTileMap->GetAtlas();
	float Ratio = m_DefaultSize * m_AtlasScale / pAtlas->GetWidth();

	// Ÿ�ϸ��� ���� �ػ󵵻󿡼��� Ÿ��ũ�⸦ ������ ���� ������ ������Ѽ� ���콺 ��ġ�� üũ�Ѵ�.
	Vec2 AtlasTileSize = pTileMap->GetAtlasTileSize();
	AtlasTileSize *= Ratio;

	int Col = (int)(OffsetMousePos.x / AtlasTileSize.x);
	int Row = (int)(OffsetMousePos.y / AtlasTileSize.y);
		
	if (OffsetMousePos.x < 0.f || pTileMap->GetAtlasMaxColumn() <= Col
	|| OffsetMousePos.y < 0.f || pTileMap->GetAtlasMaxRow() <= Row)
	{		
		return;
	}

	m_ImgIdx = pTileMap->GetAtlasMaxColumn() * Row + Col;
}

void AtlasView::DrawSelectedTile()
{
	// Ÿ�ϸ��� �����ϴ� ��Ʋ�� �ؽ����� ���� �ػ󵵿�, AtlasView ���� ����ϴ� Image ���� ���� ���̰��� ���Ѵ�.
	CTileMap* pTileMap = GetTileMap();
	Ptr<CTexture> pAtlas = pTileMap->GetAtlas();
	float Ratio = m_DefaultSize * m_AtlasScale / pAtlas->GetWidth();

	// Ÿ�ϸ��� ���� �ػ󵵻󿡼��� Ÿ��ũ�⸦ ������ ���� ������ ������Ѽ� ���콺 ��ġ�� üũ�Ѵ�.
	Vec2 AtlasTileSize = pTileMap->GetAtlasTileSize();
	AtlasTileSize *= Ratio;

	int Row = m_ImgIdx / (int)pTileMap->GetAtlasMaxColumn();
	int Col = m_ImgIdx % (int)pTileMap->GetAtlasMaxColumn();

	ImVec2 LeftTop = ImVec2(Col * AtlasTileSize.x, Row * AtlasTileSize.y);
	LeftTop.x += m_ImageRectMin.x;
	LeftTop.y += m_ImageRectMin.y;

	ImVec2 RightBot = ImVec2(LeftTop.x + AtlasTileSize.x, LeftTop.y + AtlasTileSize.y);

	ImGui::GetWindowDrawList()->AddRect(LeftTop, RightBot
									  , ImGui::GetColorU32(ImVec4(1.f, 1.f, 0.f, 1.f))
								      , 1.f, 0, 2);
}

//ImVec2 vScreenPos = ImGui::GetCursorScreenPos();
//ImVec2 vRectMin = ImGui::GetItemRectMin();
//ImVec2 vRectMax = ImGui::GetItemRectMax();
//ImVec2 vMousePos = ImGui::GetMousePos();