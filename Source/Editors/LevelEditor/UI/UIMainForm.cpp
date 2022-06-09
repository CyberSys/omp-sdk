#include "stdafx.h"
#include "..\XrECore\Editor\EditorChooseEvents.h"

UIMainForm* MainForm = nullptr;
UIMainForm::UIMainForm()
{
    EnableReceiveCommands();
    if (!ExecCommand(COMMAND_INITIALIZE, (u32)0, (u32)0))
    {
        FlushLog();
        exit(-1);
    }
    ExecCommand(COMMAND_UPDATE_GRID);
    ExecCommand(COMMAND_RENDER_FOCUS);
    FillChooseEvents();
    m_TopBar = xr_new<UITopBarForm>();
    m_Render = xr_new<UIRenderForm>();
    m_MainMenu = xr_new<UIMainMenuForm>();
    m_LeftBar = xr_new<UILeftBarForm>();
    m_Properties = xr_new<UILPropertiesFrom>();
    m_Render->SetContextMenuEvent(TOnRenderContextMenu(this, &UIMainForm::DrawContextMenu));
    m_Render->SetToolBarEvent(TOnRenderToolBar(this, &UIMainForm::DrawRenderToolBar));
    if (dynamic_cast<CLevelPreferences*>(EPrefs)->OpenObjectList)
    {
        UIObjectList::Show();
    }
    if (!dynamic_cast<CLevelPreferences*>(EPrefs)->OpenProperties)
    {
        m_Properties->Close();
    }

    m_tMenu = EDevice->Resources->_CreateTexture("ed\\bar\\menu");
    m_tSelect = EDevice->Resources->_CreateTexture("ed\\bar\\select");
    m_tAdd = EDevice->Resources->_CreateTexture("ed\\bar\\add");
    m_tMove = EDevice->Resources->_CreateTexture("ed\\bar\\move");
    m_tScale = EDevice->Resources->_CreateTexture("ed\\bar\\scale");
    m_tRotate = EDevice->Resources->_CreateTexture("ed\\bar\\rotate");
    m_tNSnap = EDevice->Resources->_CreateTexture("ed\\bar\\nsnap");
    m_tZoomSel = EDevice->Resources->_CreateTexture("ed\\bar\\zoomsel");

    m_tGrid = EDevice->Resources->_CreateTexture("ed\\bar\\grid");
    m_tScaleGrid = EDevice->Resources->_CreateTexture("ed\\bar\\scale_grid");
    m_tAngle = EDevice->Resources->_CreateTexture("ed\\bar\\angle");

    LTools->GetGimzo()->SetStep(Gizmo::EType::Move, 0.1f);
    LTools->GetGimzo()->SetStep(Gizmo::EType::Scale, 0.1f);
    LTools->GetGimzo()->SetStep(Gizmo::EType::Rotate, 5.625f);

    LTools->GetGimzo()->SwitchStep(Gizmo::EType::Move, true);
    LTools->GetGimzo()->SwitchStep(Gizmo::EType::Scale, true);
    LTools->GetGimzo()->SwitchStep(Gizmo::EType::Rotate, true);

}

UIMainForm::~UIMainForm()
{
    dynamic_cast<CLevelPreferences*>(EPrefs)->OpenProperties = !m_Properties->IsClosed();
    dynamic_cast<CLevelPreferences*>(EPrefs)->OpenObjectList = UIObjectList::IsOpen();
    ClearChooseEvents();
    xr_delete(m_Properties);
    xr_delete(m_LeftBar);
    xr_delete(m_MainMenu);
    xr_delete(m_Render);
    xr_delete(m_TopBar);
    
    m_tMenu.destroy();
    m_tSelect.destroy();
    m_tAdd.destroy();
    m_tMove.destroy();
    m_tScale.destroy();
    m_tRotate.destroy();
    m_tNSnap.destroy();
    m_tZoomSel.destroy();
    m_tGrid.destroy();
    m_tScaleGrid.destroy();
    m_tAngle.destroy();

    ExecCommand(COMMAND_DESTROY, (u32)0, (u32)0);
}

void UIMainForm::Draw()
{
    m_MainMenu->Draw();
    m_TopBar->Draw();
    m_LeftBar->Draw();
    m_Properties->Draw();
    ImGui::ShowDemoWindow(&bOpen);
    m_Render->Draw();
}

bool UIMainForm::Frame()
{
    if(UI)  return UI->Idle();
    return false;
}

void UIMainForm::DrawContextMenu()
{
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Copy"))
        {
            ExecCommand(COMMAND_COPY);
        }
        if (ImGui::MenuItem("Paste"))
        {
            ExecCommand(COMMAND_PASTE);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Cut"))
        {
            ExecCommand(COMMAND_CUT);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
        {
            ExecCommand(COMMAND_DELETE_SELECTION);
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Visiblity"))
    {
        if (ImGui::MenuItem("Hide Selected"))
        {
            ExecCommand(COMMAND_HIDE_SEL, FALSE);
        }
        if (ImGui::MenuItem("Hide Unselected"))
        {
            ExecCommand(COMMAND_HIDE_UNSEL);
        }
        if (ImGui::MenuItem("Hide All"))
        {
            ExecCommand(COMMAND_HIDE_ALL, FALSE);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Unhide All"))
        {
            ExecCommand(COMMAND_HIDE_ALL, TRUE);
        }
        ImGui::EndMenu();
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Properties"))
    {
        ExecCommand(COMMAND_SHOW_PROPERTIES);
    }
   
   
}

void UIMainForm::DrawRenderToolBar(ImVec2 Size)
{
    {
        ImGui::BeginGroup();
        m_tMenu->Load();
        {
            if (ImGui::ImageButton(m_tMenu->surface_get(), ImVec2(16, ImGui::GetFontSize())))
            {
               
            }
        }
        ImGui::EndGroup();
    }

	bool bIsSupportMove = false;
	bool bIsSupportRotate = false;
	bool bIsSupportScale = false;
    {
        ETAction Action = LTools->GetAction();

    
        if (Scene->GetTool(LTools->CurrentClassID())&& Scene->GetTool(LTools->CurrentClassID())->FindControl(0,etaSelect))
        {
          auto Control=   Scene->GetTool(LTools->CurrentClassID())->FindControl(0, etaSelect);
		  bIsSupportMove = Control->IsSupportMove();
          bIsSupportRotate = Control->IsSupportRotate();
          bIsSupportScale = Control->IsSupportScale();
        }
		if (!bIsSupportMove&&Action == etaSelect && LTools->GetGimzo()->GetType() == Gizmo::EType::Move)
		{
            LTools->GetGimzo()->SetType(Gizmo::EType::None);
        }
		if (!bIsSupportRotate && Action == etaSelect && LTools->GetGimzo()->GetType() == Gizmo::EType::Rotate)
		{
			LTools->GetGimzo()->SetType(Gizmo::EType::None);
		}
		if (!bIsSupportScale && Action == etaSelect && LTools->GetGimzo()->GetType() == Gizmo::EType::Scale)
		{
			LTools->GetGimzo()->SetType(Gizmo::EType::None);
		}

        ImGui::SameLine(0, ImGui::GetFontSize() * 5);
        ImGui::BeginGroup();


        m_tSelect->Load();
        {
            bool bPushColor = false;
            if (Action == etaSelect&&LTools->GetGimzo()->GetType() == Gizmo::EType::None)
            {
                bPushColor = true;
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Border));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            }
            if (ImGui::ImageButton(m_tSelect->surface_get(), ImVec2(16, ImGui::GetFontSize())))
            {
                LTools->SetAction(etaSelect);
                LTools->GetGimzo()->SetType(Gizmo::EType::None);
            }
            if (bPushColor)
            {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
        }

        ImGui::SameLine();

        {
            bool bPushColor = false;
            if (Action==etaAdd)
            {
                bPushColor = true;
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Border));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            }
            m_tAdd->Load();
            if (ImGui::ImageButton(m_tAdd->surface_get(), ImVec2(16, ImGui::GetFontSize())))
            {
                LTools->SetAction(etaAdd);
                LTools->GetGimzo()->SetType(Gizmo::EType::None);
            }
            if (bPushColor)
            {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
        }
     

        ImGui::SameLine();
        {

            bool bPushColor = false;

			ImGui::BeginDisabled(!bIsSupportMove);
            if (Action == etaSelect && LTools->GetGimzo()->GetType() == Gizmo::EType::Move)
            {
                bPushColor = true;
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Border));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            }
            m_tMove->Load();
            if (ImGui::ImageButton(m_tMove->surface_get(), ImVec2(16, ImGui::GetFontSize())))
            {
                LTools->SetAction(etaSelect);
                LTools->GetGimzo()->SetType(Gizmo::EType::Move);
            }

            if (bPushColor)
            {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
			}
			ImGui::EndDisabled();
        }
        

        ImGui::SameLine();
        {
            bool bPushColor = false;
            ImGui::BeginDisabled(!bIsSupportScale);
            if (Action == etaSelect && LTools->GetGimzo()->GetType() == Gizmo::EType::Scale)
            {
                bPushColor = true;
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Border));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            }
            m_tScale->Load();
            if (ImGui::ImageButton(m_tScale->surface_get(), ImVec2(16, ImGui::GetFontSize())))
            {
                LTools->SetAction(etaSelect);
                LTools->GetGimzo()->SetType(Gizmo::EType::Scale);
            }
            if (bPushColor)
            {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
            ImGui::EndDisabled();
        }

        ImGui::SameLine();
        { 
            bool bPushColor = false;

			ImGui::BeginDisabled(!bIsSupportRotate);
            if (Action == etaSelect && LTools->GetGimzo()->GetType() == Gizmo::EType::Rotate)
            {
                bPushColor = true;
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Border));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            }
            m_tRotate->Load();
            if (ImGui::ImageButton(m_tRotate->surface_get(), ImVec2(16, ImGui::GetFontSize())))
            {
                LTools->SetAction(etaSelect);
                LTools->GetGimzo()->SetType(Gizmo::EType::Rotate);
            }
            if (bPushColor)
            {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }

			ImGui::EndDisabled();
        }

        ImGui::EndGroup();
    }
    {
        ImGui::SameLine(0, ImGui::GetFontSize() * 5);
        ImGui::BeginGroup();

        {
            bool bPushColor = false;
            if (Tools->GetSettings(etfNormalAlign))
            {
                bPushColor = true;
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Border));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            }
            m_tNSnap->Load();
            if (ImGui::ImageButton(m_tNSnap->surface_get(), ImVec2(16, ImGui::GetFontSize()), ImVec2(0, 0), ImVec2(0.5f, 1.f)))
            {
                ExecCommand(COMMAND_SET_SETTINGS, etfNormalAlign, !Tools->GetSettings(etfNormalAlign));
            }
			if (bPushColor)
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}
        }

        ImGui::SameLine();

        m_tZoomSel->Load();
        if (ImGui::ImageButton(m_tZoomSel->surface_get(), ImVec2(16, ImGui::GetFontSize()),ImVec2(0,0), ImVec2(0.5f, 1.f)))
		{
			ExecCommand(COMMAND_ZOOM_EXTENTS, TRUE);
        }
        ImGui::EndGroup();
    }
    {
        string_path Temp;
        ImGui::SameLine(0, ImGui::GetFontSize() * 5);
        ImGui::BeginGroup();

        {
            bool bPushColor = false;

            if (LTools->GetGimzo()->IsStepEnable(Gizmo::EType::Move))
            {
                bPushColor = true;
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Border));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            }
            m_tGrid->Load();
            if (ImGui::ImageButton(m_tGrid->surface_get(), ImVec2(16, ImGui::GetFontSize())))
            {
                LTools->GetGimzo()->SwitchStep(Gizmo::EType::Move, !LTools->GetGimzo()->IsStepEnable(Gizmo::EType::Move));
            }
            if (bPushColor)
            {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }

        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
        xr_sprintf(Temp, "%.2f", LTools->GetGimzo()->GetStep(Gizmo::EType::Move));
        if (ImGui::BeginCombo("##move", Temp, ImGuiComboFlags_None))
        {
            if (ImGui::Selectable("0.01", false)) 
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Move, 0.01f);
            }
            if (ImGui::Selectable("0.05", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Move, 0.05f);
            }
            if (ImGui::Selectable("0.1", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Move, 0.1f);
            }
            if (ImGui::Selectable("1", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Move, 1.f);
            }
            if (ImGui::Selectable("2", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Move, 2.f);
            }
            if (ImGui::Selectable("5", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Move, 5.f);
            }
            if (ImGui::Selectable("50", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Move, 50.f);
            }
            if (ImGui::Selectable("200", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Move, 200.f);
            }
            if (ImGui::Selectable("1000", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Move, 1000.f);
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine(0, ImGui::GetFontSize());

        {
            bool bPushColor = false;
            if (LTools->GetGimzo()->IsStepEnable(Gizmo::EType::Scale))
            {
                bPushColor = true;
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Border));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            }
            m_tScaleGrid->Load();
            if (ImGui::ImageButton(m_tScaleGrid->surface_get(), ImVec2(16, ImGui::GetFontSize())))
            {
                LTools->GetGimzo()->SwitchStep(Gizmo::EType::Scale, !LTools->GetGimzo()->IsStepEnable(Gizmo::EType::Scale));
            }
            if (bPushColor)
            {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
        xr_sprintf(Temp, "%.2f", LTools->GetGimzo()->GetStep(Gizmo::EType::Scale));
        if (ImGui::BeginCombo("##scale", Temp, ImGuiComboFlags_None))
        {
            if (ImGui::Selectable("0.01", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Scale, 0.01f);
            }
            if (ImGui::Selectable("0.05", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Scale, 0.05f);
            }
            if (ImGui::Selectable("0.1", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Scale, 0.1f);
            }
            if (ImGui::Selectable("1", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Scale, 1.f);
            }
            if (ImGui::Selectable("2", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Scale, 2.f);
            }
            if (ImGui::Selectable("5", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Scale, 5.f);
            }
            if (ImGui::Selectable("50", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Scale, 50.f);
            }
            if (ImGui::Selectable("200", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Scale, 200.f);
            }
            if (ImGui::Selectable("1000", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Scale, 1000.f);
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine(0, ImGui::GetFontSize());

        {
            bool bPushColor = false;
            if (LTools->GetGimzo()->IsStepEnable(Gizmo::EType::Rotate))
            {
                bPushColor = true;
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Border));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            }
            m_tAngle->Load();
            if (ImGui::ImageButton(m_tAngle->surface_get(), ImVec2(16, ImGui::GetFontSize())))
            {
                LTools->GetGimzo()->SwitchStep(Gizmo::EType::Rotate, !LTools->GetGimzo()->IsStepEnable(Gizmo::EType::Rotate));
            }
            if (bPushColor)
            {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 5);
        xr_sprintf(Temp, "%.5f", LTools->GetGimzo()->GetStep(Gizmo::EType::Rotate));
        if (ImGui::BeginCombo("##rotate", Temp, ImGuiComboFlags_None))
        {
            if (ImGui::Selectable("1.40625", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Rotate, 1.40625f);
            }
            if (ImGui::Selectable("2.8125", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Rotate, 2.8125f);
            }
            if (ImGui::Selectable("5.625", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Rotate, 5.625f);
            }
            if (ImGui::Selectable("11.25", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Rotate, 11.25f);
            }
            if (ImGui::Selectable("22.5", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Rotate, 22.5f);
            }
            if (ImGui::Selectable("45", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Rotate, 45.f);
            }
            if (ImGui::Selectable("90", false))
            {
                LTools->GetGimzo()->SetStep(Gizmo::EType::Rotate, 90.f);
            }
            ImGui::EndCombo();
        }
        ImGui::EndGroup();
    }


}
