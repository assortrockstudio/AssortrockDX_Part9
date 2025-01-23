#pragma once
#include "EditorUI.h"

class TreeUI;

class ContentUI :
    public EditorUI
{
private:
    TreeUI*             m_Tree;
    vector<wstring>     m_vecContentName;   // Content ������ �ִ� ���µ��� �����

public:
    // Renew : ����, Reset : �ʱ� ���·� �ǵ���
    void RenewContent();
    void ReloadContent();

private:
    void FindFileName(const wstring& _strFolderPath);
    UINT SelectAsset(DWORD_PTR _Data);
    ASSET_TYPE GetAssetTypeFromExt(const path& _AssetName);


public:    
    virtual void render_tick() override;

public:
    ContentUI();
    ~ContentUI();
};

