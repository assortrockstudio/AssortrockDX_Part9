#include "pch.h"
#include "Inspector.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CGameObject.h>
#include <Engine/components.h>

#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "CameraUI.h"
#include "Light2DUI.h"
#include "Light3DUI.h"
#include "Animator2DUI.h"
#include "Collider2DUI.h"
#include "TileMapUI.h"
#include "ParticleSystemUI.h"
#include "SkyBoxUI.h"
#include "ScriptUI.h"

#include "MeshUI.h"
#include "MeshDataUI.h"
#include "MaterialUI.h"
#include "TextureUI.h"
#include "PrefabUI.h"
#include "SoundUI.h"
#include "GraphicShaderUI.h"
#include "ComputeShaderUI.h"

#include "TileMapEditor.h"

Inspector::Inspector()
	: EditorUI("Inspector", "##Inspector")
	, m_TargetObject(nullptr)
	, m_arrComUI{}
{
	// ComponentUI ����
	CreateComponentUI();

	// AssetUI ����
	CreateAssetUI();
}

Inspector::~Inspector()
{

}

void Inspector::render_tick()
{

}

void Inspector::SetTargetObject(CGameObject* _Target)
{
	// ���� UI ��Ȱ��ȭ
	if (nullptr != m_TargetAsset)
	{
		m_arrAssetUI[(UINT)m_TargetAsset->GetAssetType()]->SetActive(false);
		m_TargetAsset = nullptr;
	}

	m_TargetObject = _Target;

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		// �ش� ������Ʈ�� ����� UI �� �����Ǿ����� �ʱ� ������ nullptr üũ ���־�� ��
		if (nullptr == m_arrComUI[i])
			continue;

		m_arrComUI[i]->SetTarget(m_TargetObject);
	}

	// Script ���� ó��
	if (nullptr == m_TargetObject)
	{
		for (size_t i = 0; i < m_vecScriptUI.size(); ++i)
		{
			m_vecScriptUI[i]->SetActive(false);
		}
	}
	else
	{
		// Object �� Script �� �ִ��� Ȯ��, ������ ScriptUI �� Ȱ��ȭ
		const vector<CScript*>& vecScripts = m_TargetObject->GetScripts();

		// ������Ʈ�� ��ũ��Ʈ �� ����, ��ũ��ƮUI �� �� ������ ������, ScriptUI �� �߰� �����Ѵ�.
		if (m_vecScriptUI.size() < vecScripts.size())
		{
			size_t iAddCount = vecScripts.size() - m_vecScriptUI.size();
			for (size_t i = 0; i < iAddCount; ++i)
			{
				ScriptUI* pScriptUI = nullptr;
				pScriptUI = new ScriptUI;
				pScriptUI->SetActive(false);
				pScriptUI->SetSeperate(true);
				m_vecScriptUI.push_back(pScriptUI);
				AddChildUI(pScriptUI);
			}
		}

		// �� ScriptUI �� ������ CScript �� �����Ѵ�.
		for (size_t i = 0; i < vecScripts.size(); ++i)
		{
			m_vecScriptUI[i]->SetScript(vecScripts[i]);
			m_vecScriptUI[i]->SetActive(true);
		}
	}
}

void Inspector::SetTargetAsset(Ptr<CAsset> _Asset)
{
	// ��� ������ƮUI ���� ��Ȱ��ȭ
	SetTargetObject(nullptr);

	// Ÿ�� ���� ����
	m_TargetAsset = _Asset;

	// ��� AssetUI ��Ȱ��ȭ
	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		if (nullptr == m_arrAssetUI[i])
			continue;
		m_arrAssetUI[i]->SetActive(false);
	}

	// Ÿ�� ������ nullptr �̸� �Լ� ����
	if (nullptr == m_TargetAsset)
		return;
	
	// Ÿ�� ���¿� �ش��ϴ� UI �� Ȱ��ȭ
	ASSET_TYPE AssetType = m_TargetAsset->GetAssetType();
	m_arrAssetUI[(UINT)AssetType]->SetActive(true);
	m_arrAssetUI[(UINT)AssetType]->SetTarget(m_TargetAsset);
}

void Inspector::CreateComponentUI()
{
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM] = new TransformUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER2D] = new Collider2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D] = new Animator2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA] = new CameraUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT2D] = new Light2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT3D] = new Light3DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER] = new MeshRenderUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::TILEMAP] = new TileMapUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::PARTICLESYSTEM] = new ParticleSystemUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::SKYBOX] = new SkyBoxUI;


	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr == m_arrComUI[i])
			continue;

		m_arrComUI[i]->SetActive(false);
		m_arrComUI[i]->SetSeperate(true);
		AddChildUI(m_arrComUI[i]);
	}

	// ScriptUI
	ScriptUI* pScriptUI = nullptr;
	pScriptUI = new ScriptUI;
	pScriptUI->SetActive(false);
	pScriptUI->SetSeperate(true);

	m_vecScriptUI.push_back(pScriptUI);

	AddChildUI(pScriptUI);
}



void Inspector::CreateAssetUI()
{
	m_arrAssetUI[(UINT)ASSET_TYPE::MESH] = new MeshUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::MESH_DATA] = new MeshDataUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::MATERIAL] = new MaterialUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::TEXTURE] = new TextureUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::PREFAB] = new PrefabUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::SOUND] = new SoundUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::GRAPHICS_SHADER] = new GraphicShaderUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::COMPUTE_SHADER] = new ComputeShaderUI;

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		if (nullptr == m_arrAssetUI[i])
			continue;

		m_arrAssetUI[i]->SetActive(false);
		m_arrAssetUI[i]->SetSeperate(true);
		AddChildUI(m_arrAssetUI[i]);
	}
}