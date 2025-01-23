#include "pch.h"
#include "CameraUI.h"

#include <Engine/CRenderMgr.h>
#include <Engine/CCamera.h>

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

CameraUI::CameraUI()
	: ComponentUI("Camera", "##CameraUI", COMPONENT_TYPE::CAMERA)
{
}

CameraUI::~CameraUI()
{
}


void CameraUI::render_tick()
{
	render_title();

    CCamera* pCam = GetTarget()->Camera();

    PROJ_TYPE ProjType = pCam->GetProjType();
    bool IsPerspective = ProjType == PROJ_TYPE::PERSPECTIVE;
    bool IsOrthographic = ProjType == PROJ_TYPE::ORTHOGRAPHIC;



	// PROJ_TYPE
    ImGui::Text("Projection Type");
    ImGui::SameLine(120);

    const char* items[2] = {};

    items[0] = "ORTHOGRAPHIC";
    items[1] = "PERSPECTIVE";

    if (ImGui::BeginCombo("##CameraProjType", items[(UINT)ProjType], 0))
    {
        for (int i = 0; i < 2; ++i)
        {
            const bool is_selected = ((UINT)ProjType == i);
            if (ImGui::Selectable(items[i], is_selected))
            {
                pCam->SetProjType((PROJ_TYPE)i);
            }
        }
        ImGui::EndCombo();
    }
    	
	// ī�޶� Priority
    // CRenderMgr �κ��� ī�޶� ��� �ִ� Capacity�� �˾Ƴ���.
    vector<CCamera*>& vecCam = CRenderMgr::GetInst()->GetRegisteredCamera();

    ImGui::Text("Camera Priority"); 
    ImGui::SameLine(120);
    int iPriority = pCam->GetCameraPriority();
    if (ImGui::InputInt("##CameraPriority", &iPriority))
    {
        // ī�޶� �켱�������� �����ߴٸ�
        int CurPriority = pCam->GetCameraPriority();
        
        // �����Ϸ��� ī�޶� �켱���� ���̶� ������ ī�޶� �����ȿ� �ִ��� Ȯ�� �� ����
        CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
        vector<CGameObject*> vecCamObj;
        pCurLevel->FindObjectByComponent(COMPONENT_TYPE::CAMERA, vecCamObj);

        // ������ �켱������ ������ �ִ� �ٸ� ī�޶� �ִٸ�
        // �����Ϸ��� ī�޶��� �켱�������� �ְ�
        for (size_t i = 0; i < vecCamObj.size(); ++i)
        {
            if (vecCamObj[i]->Camera()->GetCameraPriority() == iPriority)
            {
                vecCamObj[i]->Camera()->SetCameraPriority(CurPriority);
                break;
            }
        }

        // ���� ī�޶�� ���ο� �켱������ ����
        pCam->SetCameraPriority(iPriority);
    }

	// FOV
    float FOV = pCam->GetFOV();
    FOV = (FOV * 180.f) / XM_PI;
    ImGui::SetNextItemWidth(400.f);
    ImGui::Text("FOV"); 
    ImGui::SameLine(120);

    ImGui::BeginDisabled(IsOrthographic);

    if (ImGui::DragFloat("##CameraFOV", &FOV, 0.1f, 0.f, 0.f, "%.3f"))
    {
        FOV = (FOV * XM_PI) / 180.f;
        pCam->SetFOV(FOV);
    }

    ImGui::EndDisabled();


	// Far
    float Far = pCam->GetFar();
  
    ImGui::Text("Far");
    ImGui::SameLine(120);

    if (ImGui::DragFloat("##CameraFar", &Far, 1.f))
    {
        if (Far < 2.f)
            Far = 2.f;

        pCam->SetFar(Far);
    }


	// Width
	// AspectRatio
    ImGui::Text("Width");
    ImGui::SameLine(120);
    float Width = pCam->GetWidth();
    if (ImGui::DragFloat("##CameraWidth", &Width, 1.f))
    {
        if (Width < 1.f)
            Width = 1.f;
        pCam->SetWidth(Width);
    }

    ImGui::Text("AspectRatio\t");
    ImGui::SameLine(120);
    float AspectRatio = pCam->GetAspectRaito();
    if (ImGui::DragFloat("##CameraAspectRatio", &AspectRatio, 1.f))
    {
        if (AspectRatio < 0.f)
            AspectRatio = 0.f;
        pCam->SetAspectRaito(AspectRatio);
    }

	// Scale(Orthographic)
    ImGui::Text("Projection Scale"); 
    ImGui::SameLine(120);
    float Scale = pCam->GetScale();

    ImGui::BeginDisabled(IsPerspective);

    if (ImGui::DragFloat("##CameraScale", &Scale, 0.01f))
    {
        if (Scale < 0.1f)
            Scale = 0.1f;
        pCam->SetScale(Scale);
    }

    ImGui::EndDisabled();

	// Layer Check
    ImGui::Text("Layer Check");
    ImGui::SameLine(120);
}