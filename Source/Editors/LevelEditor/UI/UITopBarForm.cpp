#include "stdafx.h"
#include "UITopBarForm.h"

UITopBarForm::UITopBarForm()
{
	m_tUndo = EDevice->Resources->_CreateTexture("ed\\bar\\Undo"); m_timeUndo = 0;
	m_tRedo = EDevice->Resources->_CreateTexture("ed\\bar\\Redo"); m_timeRedo = 0;
	m_tNew = EDevice->Resources->_CreateTexture("ed\\bar\\new");
	m_tOpen = EDevice->Resources->_CreateTexture("ed\\bar\\open");
	m_tSave= EDevice->Resources->_CreateTexture("ed\\bar\\save");
	m_tCForm = EDevice->Resources->_CreateTexture("ed\\bar\\CForm");
	m_tAIMap = EDevice->Resources->_CreateTexture("ed\\bar\\AIMap");
	m_tGGraph = EDevice->Resources->_CreateTexture("ed\\bar\\GGraph");
	m_tPlayInEditor = EDevice->Resources->_CreateTexture("ed\\bar\\play_in_editor");
	m_tPlayPC = EDevice->Resources->_CreateTexture("ed\\bar\\play_pc");
	m_VerifySpaceRestrictors = false;
	RefreshBar();
	m_GameProcess.hProcess = nullptr;
}

UITopBarForm::~UITopBarForm()
{
	
}

void UITopBarForm::Draw()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + UI->GetMenuBarHeight()));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, UIToolBarSize));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoSavedSettings
		;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2( 2,2));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(2, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));
	ImGui::Begin("TOOLBAR", NULL, window_flags);
	{


		m_tUndo->Load();
		if (ImGui::ImageButton(m_tUndo->surface_get(), ImVec2(20, 20), ImVec2(m_timeUndo > EDevice->TimerAsync() ? 0.5 : 0, 0), ImVec2(m_timeUndo > EDevice->TimerAsync() ? 1 : 0.5, 1), 0))
		{
			m_timeUndo = EDevice->TimerAsync() + 130;
			ClickUndo();
		}ImGui::SameLine();
		m_tRedo->Load();
		if (ImGui::ImageButton(m_tRedo->surface_get(), ImVec2(20, 20), ImVec2(m_timeRedo > EDevice->TimerAsync() ? 0.5 : 0, 0), ImVec2(m_timeRedo > EDevice->TimerAsync() ? 1 : 0.5, 1), 0))
		{
			m_timeRedo = EDevice->TimerAsync() + 130;
			ClickRedo();
		}ImGui::SameLine();

		m_tNew->Load();
		if (ImGui::ImageButton(m_tNew->surface_get(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			ClickNew();
		}ImGui::SameLine();
		m_tOpen->Load();
		if (ImGui::ImageButton(m_tOpen->surface_get(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			ClickOpen();
		}ImGui::SameLine();
		m_tSave->Load();
		if (ImGui::ImageButton(m_tSave->surface_get(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			ClickSave();
		}ImGui::SameLine();
		m_tCForm->Load();
		if (ImGui::ImageButton(m_tCForm->surface_get(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			ClickCForm();
		}ImGui::SameLine();
		m_tAIMap->Load();
		if (ImGui::ImageButton(m_tAIMap->surface_get(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			ClickAIMap();
		}ImGui::SameLine();
		m_tGGraph->Load();
		if (ImGui::ImageButton(m_tGGraph->surface_get(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			ClickGGraph();
		}ImGui::SameLine();

		m_tPlayInEditor->Load();
		if (ImGui::ImageButton(m_tPlayInEditor->surface_get(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			ClickPlayInEditor();
		}
		{
			ImGui::SameLine(0,0);
			if (ImGui::ArrowButton("##PlaySettings", ImGuiDir_Down, ImVec2(ImGui::GetFrameHeight(), 20),0))
				ImGui::OpenPopup("test");

			ImGui::SameLine();
			if (ImGui::BeginPopup("test"))
			{
				ImGui::Checkbox("Verify space restrictors",&m_VerifySpaceRestrictors);
				ImGui::EndPopup();
			}
		}
		ImGui::SameLine();
		m_tPlayPC->Load();
		if (ImGui::ImageButton(m_tPlayPC->surface_get(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			ClickPlayPC();
		}
	}
	ImGui::SameLine(0,1);
	ImGui::End();
	ImGui::PopStyleVar(5);
	if (m_GameProcess.hProcess)
	{ 

		DWORD ExitCode = 0;
		if (GetExitCodeProcess(m_GameProcess.hProcess, &ExitCode) == 0)
		{
			Msg("! Cannot return exit code in game process (%d).\n", GetLastError());
			m_GameProcess.hProcess = 0;
			
		}
		else
		{
			if (ExitCode != STILL_ACTIVE)
			{
				CloseHandle(m_GameProcess.hProcess);
				CloseHandle(m_GameProcess.hThread);
				m_GameProcess.hProcess = 0;
			}
		}
	}
}
void UITopBarForm::RefreshBar()
{/*
	{
		m_bSelect = false;
		m_bAdd = false;

		switch (Tools->GetAction()) {
		case etaSelect: 	m_bSelect = true; 	break;
		case etaAdd:    m_bAdd = true; 		break;
		default: THROW;
		}
	}
	// settings
	m_bCsLocal = Tools->GetSettings(etfCSParent);
	m_bNuScale = Tools->GetSettings(etfNUScale);
	m_bNSnap = Tools->GetSettings(etfNormalAlign);
	m_bGSnap = Tools->GetSettings(etfGSnap);
	m_bOSnap = Tools->GetSettings(etfOSnap);
	m_bMoveToSnap = Tools->GetSettings(etfMTSnap);
	m_bVSnap = Tools->GetSettings(etfVSnap);
	m_bASnap = Tools->GetSettings(etfASnap);
	m_bMSnap = Tools->GetSettings(etfMSnap);*/
}

void UITopBarForm::ClickUndo()
{
	ExecCommand(COMMAND_UNDO);
}

void UITopBarForm::ClickRedo()
{
	ExecCommand(COMMAND_REDO);
}

void UITopBarForm::ClickNew()
{
	ExecCommand(COMMAND_CLEAR);
}
void UITopBarForm::ClickOpen()
{
	ExecCommand(COMMAND_LOAD);
}
void UITopBarForm::ClickSave()
{
	ExecCommand(COMMAND_SAVE, xr_string(LTools->m_LastFileName.c_str()));
}
void UITopBarForm::ClickCForm()
{
	Scene->BuildCForm();

}
void UITopBarForm::ClickAIMap()
{
	Scene->BuildAIMap();

}
void UITopBarForm::ClickGGraph()
{
	Scene->BuildGameGraph();

}
void UITopBarForm::ClickPlayInEditor()
{
	Scene->Play();
}
void UITopBarForm::ClickPlayPC()
{
	if (m_GameProcess.hProcess)
		return;

	if (!Scene->BuildForPCPlay())
		return;

	m_GameProcess = {};
	STARTUPINFO si = {};

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&m_GameProcess, sizeof(m_GameProcess));


	string_path CommandLine;
	xr_sprintf(CommandLine, "Xr3DA.exe -editor_scene -start server(editor/single/alife/new) client(localhost) -nointro -noprefetch");
	Msg("~ Run Game %s.\n", CommandLine);
	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		CommandLine,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&m_GameProcess)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		Msg("! PlayPC:CreateProcess failed (%d).\n", GetLastError());
		return;
	}
}
/*
void UITopBarForm::ClickZoom()
{
	ExecCommand(COMMAND_ZOOM_EXTENTS, FALSE);
}

void UITopBarForm::ClickZoomSel()
{
	ExecCommand(COMMAND_ZOOM_EXTENTS, TRUE);
}
void  UITopBarForm::ClickSelect()
{
	ExecCommand(COMMAND_CHANGE_ACTION, etaSelect);
	m_bSelect = true;
	m_bAdd = false;
}
void  UITopBarForm::ClickAdd()
{
	ExecCommand(COMMAND_CHANGE_ACTION, etaAdd);
	m_bSelect = false;
	m_bAdd = true;
}

void  UITopBarForm::ClickCsLocal(){ ExecCommand(COMMAND_SET_SETTINGS, etfCSParent, m_bCsLocal); }
void  UITopBarForm::ClickNuScale() { ExecCommand(COMMAND_SET_SETTINGS, etfNUScale, m_bNuScale); }
void  UITopBarForm::ClickGSnap() { ExecCommand(COMMAND_SET_SETTINGS, etfGSnap, m_bGSnap); }
void  UITopBarForm::ClickOSnap() { ExecCommand(COMMAND_SET_SETTINGS, etfOSnap, m_bOSnap); }
void  UITopBarForm::ClickMoveToSnap() { ExecCommand(COMMAND_SET_SETTINGS, etfMTSnap, m_bMoveToSnap); }
void  UITopBarForm::ClickNSnap() { ExecCommand(COMMAND_SET_SETTINGS, etfNormalAlign, m_bNSnap); }
void  UITopBarForm::ClickVSnap() { ExecCommand(COMMAND_SET_SETTINGS, etfVSnap, m_bVSnap); }
void  UITopBarForm::ClickASnap() { ExecCommand(COMMAND_SET_SETTINGS, etfASnap, m_bASnap); }
void  UITopBarForm::ClickMSnap() { ExecCommand(COMMAND_SET_SETTINGS, etfMSnap, m_bMSnap); }

void  UITopBarForm::ClickCameraP(){ EDevice->m_Camera.SetStyle(csPlaneMove); UI->RedrawScene();}
void  UITopBarForm::ClickCameraA(){ EDevice->m_Camera.SetStyle(cs3DArcBall); UI->RedrawScene();}
void  UITopBarForm::ClickCameraF(){ EDevice->m_Camera.SetStyle(csFreeFly); UI->RedrawScene();}

void  UITopBarForm::ClickViewB1() { EDevice->m_Camera.ViewBack(); UI->RedrawScene(); }
void  UITopBarForm::ClickViewB2() { EDevice->m_Camera.ViewBottom();UI->RedrawScene(); }
void  UITopBarForm::ClickViewF() { EDevice->m_Camera.ViewFront(); UI->RedrawScene();}
void  UITopBarForm::ClickViewL() { EDevice->m_Camera.ViewLeft();UI->RedrawScene(); }
void  UITopBarForm::ClickViewR() { EDevice->m_Camera.ViewRight(); UI->RedrawScene();}
void  UITopBarForm::ClickViewT() { EDevice->m_Camera.ViewTop();UI->RedrawScene(); }
void  UITopBarForm::ClickViewX() { EDevice->m_Camera.ViewReset(); UI->RedrawScene();}*/