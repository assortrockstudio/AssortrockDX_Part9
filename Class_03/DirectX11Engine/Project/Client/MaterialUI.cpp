#include "pch.h"
#include "MaterialUI.h"

#include <Engine/CMaterial.h>
#include <Engine/CGraphicShader.h>

#include "ParamUI.h"

MaterialUI::MaterialUI()
	: AssetUI("MaterialUI", "##MaterialUI", ASSET_TYPE::MATERIAL)
	, m_ParamDescMaxWidth(0)
{
}

MaterialUI::~MaterialUI()
{
}

void MaterialUI::render_tick()
{
	render_title();

	string strName = ToString(GetTarget()->GetKey());

	Ptr<CMaterial> pMtrl = dynamic_cast<CMaterial*>(GetTarget().Get());
	assert(pMtrl.Get());

	// �޽� �̸�
	ImGui::Text("Material Name");
	ImGui::SameLine(110);
	ImGui::InputText("##MaterialNameMaterialUI", (char*)strName.c_str(), strName.capacity(), ImGuiInputTextFlags_ReadOnly);

	// ����� ���̴� ���� ���
	string strShaderName = nullptr == pMtrl->GetShader() ? "" : ToString(pMtrl->GetShader()->GetKey());

	ImGui::Text("Shader Name");
	ImGui::SameLine(110);
	ImGui::InputText("##ShaderNameShaderUI", (char*)strShaderName.c_str(), strShaderName.capacity(), ImGuiInputTextFlags_ReadOnly);

	// DragDrop
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content");

		if (nullptr != payload)
		{
			DWORD_PTR dwData = 0;
			memcpy(&dwData, payload->Data, payload->DataSize);

			Ptr<CAsset> pAsset = (CAsset*)dwData;
			if (pAsset->GetAssetType() == ASSET_TYPE::GRAPHICS_SHADER)
			{
				Ptr<CGraphicShader> pShader = (CGraphicShader*)pAsset.Get();
				pMtrl->SetShader(pShader);
			}
		}

		ImGui::EndDragDropTarget();
	}








	ImGui::Separator();
	ImGui::Text("Shader Parameter");
	ImGui::Text("");

	// ������ �����ϴ� ���̴��� �ִٸ�
	if (nullptr == pMtrl->GetShader())
		return;

	// �Ķ���� UI �� Desc ������ ����Ҷ� �ִ� ���̰� ����
	ParamUI::SetDescWidth(m_ParamDescMaxWidth);
	
	// ���̴��� ��û�� �Ķ���� ��� Ȯ��
	Ptr<CGraphicShader> pShader = pMtrl->GetShader();	
	vector<tShaderScalarParam> ScalarParam = pShader->GetScalarParam();
	
	// ���̴��� ��û�� �� �Ķ���Ϳ� �����ϴ� ������ MaterialUI �� ����
	for (size_t i = 0; i < ScalarParam.size(); ++i)
	{
		// �Ķ���� ������� �ִ� ���α��̸� ����Ѵ�.
		ImVec2 vDescWidth = ImGui::CalcTextSize(ScalarParam[i].strDesc.c_str());
		if (m_ParamDescMaxWidth < vDescWidth.x)
			m_ParamDescMaxWidth = vDescWidth.x;
		

		// �Ķ���� Ÿ�Կ� ���� ������ �����Ų��.
		switch (ScalarParam[i].Param)
		{
		case INT_0:
		case INT_1:
		case INT_2:
		case INT_3:
			ParamUI::Param_DragInt(ScalarParam[i].strDesc, (int*)pMtrl->GetScalarParam(ScalarParam[i].Param));			
			break;
		case FLOAT_0:
		case FLOAT_1:
		case FLOAT_2:
		case FLOAT_3:
			ParamUI::Param_DragFloat(ScalarParam[i].strDesc, (float*)pMtrl->GetScalarParam(ScalarParam[i].Param), 0.01f);
			break;
		case VEC2_0:
		case VEC2_1:
		case VEC2_2:
		case VEC2_3:
			ParamUI::Param_DragVec2(ScalarParam[i].strDesc, (Vec2*)pMtrl->GetScalarParam(ScalarParam[i].Param));
			break;
		case VEC4_0:
		case VEC4_1:
		case VEC4_2:
		case VEC4_3:
			ParamUI::Param_DragVec4(ScalarParam[i].strDesc, (Vec4*)pMtrl->GetScalarParam(ScalarParam[i].Param));
			break;
		case MAT_0:
		case MAT_1:
		case MAT_2:
		case MAT_3:
			ParamUI::Param_DragMat(ScalarParam[i].strDesc, (Matrix*)pMtrl->GetScalarParam(ScalarParam[i].Param));
			break;		
		}		
	}


	// ���̴��� ��û�� �ؽ��� �Ķ���� ��� Ȯ��	
	vector<tShaderTexParam> TexParam = pShader->GetTexParam();
	for (size_t i = 0; i < TexParam.size(); ++i)
	{
		// �Ķ���� ������� �ִ� ���α��̸� ����Ѵ�.
		ImVec2 vDescWidth = ImGui::CalcTextSize(TexParam[i].strDesc.c_str());
		if (m_ParamDescMaxWidth < vDescWidth.x)
			m_ParamDescMaxWidth = vDescWidth.x;
				
		// ListUI �� ����Ŭ�������� ȣ�� �� Delegate ���
		ParamUI::RegisterTexDelegate(this, (UI_DELEGATE1)&MaterialUI::SelectTexture);

		// �ؽ��� �Ķ���� ���� ���
		if (ParamUI::Param_Tex(TexParam[i].strDesc, pMtrl->GetTexParam(TexParam[i].Param)))
		{
			m_TargetParam = TexParam[i].Param;
		}

		ImGui::Text("");
	}
}

void MaterialUI::TargetChanged()
{
	m_ParamDescMaxWidth = 0;
}

UINT MaterialUI::SelectTexture(DWORD_PTR _Selected)
{
	string* pStr = (string*)_Selected;

	wstring strTexKey = ToWString(*pStr);

	Ptr<CTexture> pTexture = CAssetMgr::GetInst()->FindAsset<CTexture>(strTexKey);
		
	Ptr<CMaterial> pMtrl = dynamic_cast<CMaterial*>(GetTarget().Get());

	pMtrl->SetTexParam(m_TargetParam, pTexture);

	SetFocus();

	return 0;
}
