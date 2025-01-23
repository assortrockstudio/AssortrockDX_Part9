#include "pch.h"
#include "CLevelSaveLoad.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/components.h>
#include <Engine/CCollisionMgr.h>
#include <Engine/CScript.h>
#include <Scripts/CScriptMgr.h>

#include <Engine/CCollisionMgr.h>

void CLevelSaveLoad::SaveLevel(CLevel* _Level, const wstring& _FilePath)
{
	FILE* pFile = nullptr;

	_wfopen_s(&pFile, _FilePath.c_str(), L"wb");

	if (nullptr == pFile)
	{
		MessageBox(nullptr, L"���� ���� ����", L"���� ���� ����", MB_OK);
		return;
	}

	// ������ �̸��� ����
	SaveWString(_Level->GetName(), pFile);

	// ������ ������ ��� ���̾� ������ ����
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// ���̾��� �̸��� ����
		CLayer* pLayer = _Level->GetLayer(i);
		SaveWString(pLayer->GetName(), pFile);

		// ���̾ ������ ������Ʈ���� ����(�θ� ������Ʈ�� ����)
		const vector<CGameObject*>& vecParents = pLayer->GetParentObjects();

		// ParentObject ���� ����
		size_t ObjCount = vecParents.size();
		fwrite(&ObjCount, sizeof(size_t), 1, pFile);

		// Layer �� �Ҽӵ� ������ GameObject ������ ����
		for (size_t i = 0; i < ObjCount; ++i)
		{
			SaveGameObject(vecParents[i], pFile);
		}
	}

	// �浹ü ����
	CCollisionMgr::GetInst()->SaveToLevelFile(pFile);


	fclose(pFile);
}

void CLevelSaveLoad::SaveGameObject(CGameObject* _Object, FILE* _File)
{
	// �̸�
	SaveWString(_Object->GetName(), _File);
		
	// ������Ʈ
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		CComponent* pComopnent = _Object->GetComponent((COMPONENT_TYPE)i);
		if (nullptr == pComopnent)
			continue;

		// ������ ������Ʈ�� Ÿ���� ����
		COMPONENT_TYPE type = pComopnent->GetComponentType();
		fwrite(&type, sizeof(UINT), 1, _File);
		
		// ������Ʈ�� ������ ����
		SaveWString(pComopnent->GetName(), _File);
		pComopnent->SaveToLevelFile(_File);
	}
	COMPONENT_TYPE end = COMPONENT_TYPE::END;
	fwrite(&end, sizeof(UINT), 1, _File);
	
	// ��ũ��Ʈ
	const vector<CScript*>& vecScripts = _Object->GetScripts();
	size_t ScriptCount = vecScripts.size();

	// ��ũ��Ʈ �������� ����
	fwrite(&ScriptCount, sizeof(size_t), 1, _File);

	// ��� ��ũ��Ʈ�� ������ ����
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		wstring scriptName = CScriptMgr::GetScriptName(vecScripts[i]);
		SaveWString(scriptName, _File);
		vecScripts[i]->SaveToLevelFile(_File);
	}

	// �ڽ� ������Ʈ
	const vector<CGameObject*>& vecChild = _Object->GetChildren();

	size_t ChildCount = vecChild.size();
	fwrite(&ChildCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		SaveGameObject(vecChild[i], _File);
	}
}

CLevel* CLevelSaveLoad::LoadLevel(const wstring& _FilePath)
{
	FILE* pFile = nullptr;

	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");

	if (nullptr == pFile)
	{
		MessageBox(nullptr, L"���� �ҷ����� ����", L"���� �ε� ����", MB_OK);
		return nullptr;
	}

	CLevel* pLevel = new CLevel;

	// ������ �̸��� �ε�
	wstring strLevelName;
	LoadWString(strLevelName, pFile);
	pLevel->SetName(strLevelName);

	// ������ ������ ��� ���̾� ������ �ε�
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// ���̾��� �̸��� �ε�
		CLayer* pLayer = pLevel->GetLayer(i);
		wstring strLayerName;
		LoadWString(strLayerName, pFile);		
		pLayer->SetName(strLayerName);

		// ���̾ ������ ������Ʈ���� �ε�(�θ� ������Ʈ�� ����)
		// ParentObject ���� �ε�
		size_t ObjCount = 0;
		fread(&ObjCount, sizeof(size_t), 1, pFile);

		// ����� ������Ʈ ������ŭ �ݺ��� ���鼭, ������Ʈ�� �ε��ؼ� ���̾ �ٽ� �־��ش�.
		for (size_t i = 0; i < ObjCount; ++i)
		{		
			CGameObject* pParentObject = LoadGameObject(pFile);
			pLayer->AddObject(pParentObject);
		}
	}

	CCollisionMgr::GetInst()->LoadFromLevelFile(pFile);



	fclose(pFile);

	return pLevel;
}

CGameObject* CLevelSaveLoad::LoadGameObject(FILE* _File)
{
	CGameObject* pObject = new CGameObject;

	// �̸�
	wstring Name;
	LoadWString(Name, _File);
	pObject->SetName(Name);
		
	// ������Ʈ
	COMPONENT_TYPE type = COMPONENT_TYPE::END;
	CComponent* pComponent = nullptr;
	while (true)
	{
		fread(&type, sizeof(UINT), 1, _File);

		if (COMPONENT_TYPE::END == type)
			break;

		switch (type)
		{
		case COMPONENT_TYPE::TRANSFORM:
			pComponent = new CTransform;
			break;
		case COMPONENT_TYPE::CAMERA:
			pComponent = new CCamera;
			break;
		case COMPONENT_TYPE::COLLIDER2D:
			pComponent = new CCollider2D;
			break;
		case COMPONENT_TYPE::COLLIDER3D:
			//pComponent = new CCollider2D;
			break;
		case COMPONENT_TYPE::ANIMATOR2D:
			pComponent = new CAnimator2D;
			break;
		case COMPONENT_TYPE::ANIMATOR3D:

			break;
		case COMPONENT_TYPE::LIGHT2D:
			pComponent = new CLight2D;
			break;
		case COMPONENT_TYPE::LIGHT3D:

			break;
		case COMPONENT_TYPE::MESHRENDER:
			pComponent = new CMeshRender;
			break;
		case COMPONENT_TYPE::DECAL:

			break;
		case COMPONENT_TYPE::PARTICLESYSTEM:
			pComponent = new CParticleSystem;
			break;
		case COMPONENT_TYPE::TILEMAP:
			pComponent = new CTileMap;
			break;
		case COMPONENT_TYPE::LANDSCAPE:
		
			break;
		}

		LoadWString(Name, _File);
		pComponent->SetName(Name);

		pObject->AddComponent(pComponent);
		pComponent->LoadFromLevelFile(_File);
	}

	// ��ũ��Ʈ	
	size_t ScriptCount = 0;
	fread(&ScriptCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ScriptCount; ++i)
	{
		wstring ScriptName;
		LoadWString(ScriptName, _File);
		CScript* pScript = CScriptMgr::GetScript(ScriptName);
		pObject->AddComponent(pScript);
		pScript->LoadFromLevelFile(_File);
	}		

	// �ڽ� ������Ʈ	
	size_t ChildCount = 0;
	fread(&ChildCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ChildCount; ++i)
	{
		CGameObject* pChild = LoadGameObject(_File);
		pObject->AddChild(pChild);
	}

	return pObject;
}
