#include "pch.h"
#include "ContentUI.h"

#include <Engine/CAssetMgr.h>
#include <Engine/CPathMgr.h>

#include "Inspector.h"
#include "TreeUI.h"


ContentUI::ContentUI()
	: EditorUI("Content", "##Content")
{
	// �ڽ� UI �� TreeUI ����
	m_Tree = new TreeUI("ContentTree");	
	AddChildUI(m_Tree);

	// TreeUI �� ��Ʈ��带 �������ʰ� ����
	m_Tree->ShowRoot(false);

	// TreeUI �� Drag ����� �Ҵ�.
	m_Tree->UseDrag(true);

	// ��� �׸��� ���� �̸��� ���̰� �Ѵ�.
	m_Tree->ShowFileNameOnly(true);

	// �׸� ���ý� ������ CallBack OR Delegate ���	
	m_Tree->RegisterSelectDelegate(this, (UI_DELEGATE1)&ContentUI::SelectAsset);

	// Content ������ �ִ� ��� ������ �޸𸮿� �ε�
	ReloadContent();

	// ���� Asset �� ���¸� Ʈ���� �ݿ�
	RenewContent();
}

ContentUI::~ContentUI()
{
}

void ContentUI::render_tick()
{
	if (CAssetMgr::GetInst()->IsAssetChanged())
	{
		RenewContent();
	}
}

void ContentUI::RenewContent()
{
	// Tree �� ������ ����
	m_Tree->Clear();

	// Dummy �� ��Ʈ��� �߰�	
	TreeNode* pRootNode = m_Tree->AddTreeNode(nullptr, "DummyRoot");	

	// AssetMgr �κ��� Asset ������ �޾ƿͼ� Ʈ���� �ݿ���Ų��.
	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		TreeNode* pCategoryNode = m_Tree->AddTreeNode(pRootNode, ASSET_TYPE_STRING[i]);
		pCategoryNode->SetFrame(true);
		
		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssets((ASSET_TYPE)i);

		for (const auto& pair : mapAsset)
		{
			m_Tree->AddTreeNode(pCategoryNode, ToString(pair.first).c_str(), (DWORD_PTR)pair.second.Get());
		}
	}
}

void ContentUI::ReloadContent()
{
	// Content ���� ������ �����ϴ� ��� ���ϸ��� �˾Ƴ���.
	FindFileName(CPathMgr::GetInst()->GetContentPath());

	// �˾Ƴ� ������ ����θ��� ���ؼ�, �ش� ������ �ε��Ѵ�.
	for (size_t i = 0; i < m_vecContentName.size(); ++i)
	{
		ASSET_TYPE type = GetAssetTypeFromExt(m_vecContentName[i]);

		switch (type)
		{
		case ASSET_TYPE::PREFAB:
			CAssetMgr::GetInst()->Load<CPrefab>(m_vecContentName[i], m_vecContentName[i]);
			break;
		case ASSET_TYPE::MESH:
			CAssetMgr::GetInst()->Load<CMesh>(m_vecContentName[i], m_vecContentName[i]);
			break;
		case ASSET_TYPE::MESH_DATA:
			//CAssetMgr::GetInst()->Load<CMeshData>(m_vecContentName[i], m_vecContentName[i]);
			break;
		case ASSET_TYPE::MATERIAL:
			CAssetMgr::GetInst()->Load<CMaterial>(m_vecContentName[i], m_vecContentName[i]);
			break;
		case ASSET_TYPE::TEXTURE:
			CAssetMgr::GetInst()->Load<CTexture>(m_vecContentName[i], m_vecContentName[i]);
			break;
		case ASSET_TYPE::SOUND:
			CAssetMgr::GetInst()->Load<CSound>(m_vecContentName[i], m_vecContentName[i]);
			break;		
		case ASSET_TYPE::END:
			continue;
			break;		
		}
	}

	// Content �������� �ִ� ������ ����������� �ٽ� �����Ѵ�.
	m_vecContentName.clear();

	// �ε��� ���°�, ���� ���� ���翩�θ� üũ�Ѵ�.
	wstring strContent = CPathMgr::GetInst()->GetContentPath();

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		if ((UINT)ASSET_TYPE::GRAPHICS_SHADER == i ||
			(UINT)ASSET_TYPE::COMPUTE_SHADER == i)
		{
			continue;
		}
		
		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssets((ASSET_TYPE)i);

		for (const auto& pair : mapAsset)
		{
			// Engine �� Asset �� ���������� �������� �ʴ´�.
			if (pair.second->IsEngineAsset())
				continue;

			if (!exists(strContent + pair.second->GetRelativePath()))
			{
				DeleteAsset(pair.second);
			}
		}
	}
}

void ContentUI::FindFileName(const wstring& _strFolderPath)
{
	wstring strFindPath = _strFolderPath + L"*.*";

	WIN32_FIND_DATA FindData = {};

	// ������ ���� �� ���͸��� �ش��ϴ� ������ Ž���ϴ� Ž����ü ����, �ڵ鰪 �ޱ�
	HANDLE hFind = FindFirstFile(strFindPath.c_str(), &FindData);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		MessageBox(nullptr, strFindPath.c_str(), L"���� Ž�� ����", MB_OK);
	}

	// �ش� ������ ���̻� ���� �� ������ �������� ������ ���� Ž�� ����
	while (FindNextFile(hFind, &FindData))
	{
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!wcscmp(FindData.cFileName, L".."))
				continue;

			FindFileName(_strFolderPath + FindData.cFileName + L"\\");
		}
		
		else
		{
			wstring RelativePath = CPathMgr::GetInst()->GetRelativePath(_strFolderPath + FindData.cFileName);
			m_vecContentName.push_back(RelativePath);
		}
	}

	FindClose(hFind);	
}

UINT ContentUI::SelectAsset(DWORD_PTR _Data)
{
	TreeNode* pSelectedNode = (TreeNode*)_Data;

	pSelectedNode->GetName();
	Ptr<CAsset> pAsset = (CAsset*)pSelectedNode->GetData();

	Inspector* pInspector = CImGuiMgr::GetInst()->FindEditorUI<Inspector>("Inspector");

	pInspector->SetTargetAsset(pAsset);

	return 0;
}

ASSET_TYPE ContentUI::GetAssetTypeFromExt(const path& _AssetName)
{
	path ext = _AssetName.extension();

	if ( ext == L".pref" || ext == L".PREF")
	{
		return ASSET_TYPE::PREFAB;
	}

	else if( ext == L".png" || ext == L".PNG" 
		|| ext == L".jpg" || ext == L".JPG"
		|| ext == L".bmp" || ext == L".BMP"
		|| ext == L".tga" || ext == L".TGA"
		|| ext == L".dds" || ext == L".DDS" )
	{
		return ASSET_TYPE::TEXTURE;
	}

	else if (ext == L".mesh" || ext == L".MESH")
	{
		return ASSET_TYPE::MESH;
	}

	else if (ext == L".mdat" || ext == L".MDAT")
	{
		return ASSET_TYPE::MESH_DATA;
	}

	else if (  ext == L".mp3" || ext == L".MP3"
			|| ext == L".wav" || ext == L".WAV"
			|| ext == L".ogg" || ext == L".OGG" )
	{
		return ASSET_TYPE::SOUND;
	}

	else if (ext == L".mtrl" || ext == L".MTRL")
	{
		return ASSET_TYPE::MATERIAL;
	}

	return ASSET_TYPE::END;
}
