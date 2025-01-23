#include "pch.h"
#include "ScriptUI.h"

#include <Scripts/CScriptMgr.h>
#include <Engine/CScript.h>
#include "ParamUI.h"

ScriptUI::ScriptUI()
	: ComponentUI("ScriptUI", "##ScriptUI", COMPONENT_TYPE::SCRIPT)
	, m_TargetScript(nullptr)
{
}

ScriptUI::~ScriptUI()
{
}

void ScriptUI::SetScript(CScript* _Script)
{
	m_TargetScript = _Script;

	string strScriptName = ToString(CScriptMgr::GetScriptName(m_TargetScript));

	SetName(strScriptName.c_str());
}

void ScriptUI::render_tick()
{
	render_scriptname();

	// ����� Script �� ������Ƽ ������ Ȯ��
	const vector<tScriptProperty>& vecPropoerty = m_TargetScript->GetScriptProperty();

	ParamUI::SetDescWidth(120);

	for (size_t i = 0; i < vecPropoerty.size(); ++i)
	{
		switch (vecPropoerty[i].Type)
		{
		case PROPERTY_TYPE::INT:
			ParamUI::Param_DragInt(vecPropoerty[i].Desc, (int*)vecPropoerty[i].pData, 1);
			break;
		case PROPERTY_TYPE::FLOAT:
			ParamUI::Param_DragFloat(vecPropoerty[i].Desc, (float*)vecPropoerty[i].pData, 1);
			break;
		case PROPERTY_TYPE::VEC2:
			break;
		case PROPERTY_TYPE::VEC3:
			break;
		case PROPERTY_TYPE::VEC4:
			break;
		case PROPERTY_TYPE::TEXTURE:
			break;		
		}
	}
}


void ScriptUI::render_scriptname()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(6.f / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(6.f / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(6.f / 7.0f, 0.7f, 0.7f));

	wstring strScriptName = CScriptMgr::GetScriptName(m_TargetScript);
	ImGui::Button(ToString(strScriptName).c_str());

	ImGui::PopStyleColor(3);
	ImGui::PopID();
}



// 1. Script �����͸� �˾Ƶ�, Ŭ�������� �˾Ƴ� �� �־���Ѵ�.

// 2. ������ ��� ��ũ��Ʈ ����Ʈ�� �˾Ƴ��� �־�� �Ѵ�.

// 3. Script �� Ŭ���� ���ָ̹�����, Script ��ü�� ������ų �� �־�� �Ѵ�.