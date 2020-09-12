//////////////////////////////////////////////////////////////////////////////
//
// This file is part of the Corona game engine.
// For overview and more information on licensing please refer to README.md
// Home page: https://github.com/coronalabs/corona
// Contact: support@coronalabs.com
//
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <math.h>
#include <wx/string.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include "Rtt_LuaContext.h"
#include "Rtt_LuaFile.h"
#include "Rtt_MPlatform.h"
#include "Rtt_PlatformAppPackager.h"
#include "Rtt_PlatformPlayer.h"
#include "Rtt_PlatformSimulator.h"
#include "Rtt_RenderingStream.h"
#include "Rtt_Runtime.h"
#include "Rtt_SimulatorAnalytics.h"
#include "Rtt_LinuxPlatform.h"
#include "Rtt_LinuxSimulatorServices.h"
#include "Rtt_LinuxSimulatorView.h"
#include "Rtt_LinuxFileUtils.h"
#include "Rtt_SimulatorRecents.h"
#include "Rtt_WebAppPackager.h"
#include "Rtt_LinuxAppPackager.h"
#include "Rtt_AndroidAppPackager.h"
#include "Core/Rtt_FileSystem.h"

#define SIMULATOR_CONFIG_SHOW_RUNTIME_ERRORS "/showRuntimeErrors"
#define SIMULATOR_CONFIG_RELAUNCH_ON_FILE_CHANGE "/relaunchOnFileChange"
#define SIMULATOR_CONFIG_OPEN_LAST_PROJECT "/openLastProject"
#define SIMULATOR_CONFIG_LAST_PROJECT_DIRECTORY "/lastProjectDirectory"
#define SIMULATOR_CONFIG_WINDOW_X_POSITION "/xPos"
#define SIMULATOR_CONFIG_WINDOW_Y_POSITION "/yPos"
#define SIMULATOR_CONFIG_SKIN_ID "/skinID"
#define SIMULATOR_CONFIG_SKIN_WIDTH "/skinWidth"
#define SIMULATOR_CONFIG_SKIN_HEIGHT "/skinHeight"
#define SIMULATOR_CONFIG_SKIN_ZOOMED_WIDTH "/zoomedWidth"
#define SIMULATOR_CONFIG_SKIN_ZOOMED_HEIGHT "/zoomedHeight"

namespace Rtt
{
	LinuxPlatformServices::LinuxPlatformServices(MPlatform *platform)
		: fPlatform(platform)
	{
	}

	// initialise vars
	const float  LinuxSimulatorView::skinScaleFactor = 1.5;
	const int  LinuxSimulatorView::skinMinWidth = 320;
	wxString LinuxSimulatorView::Config::settingsFilePath = wxEmptyString;
	wxString LinuxSimulatorView::Config::lastProjectDirectory  = wxEmptyString;
	bool LinuxSimulatorView::Config::showRuntimeErrors = true;
	bool LinuxSimulatorView::Config::openLastProject = false;
	LinuxPreferencesDialog::RelaunchType LinuxSimulatorView::Config::relaunchOnFileChange = LinuxPreferencesDialog::RelaunchType::Always;
	int LinuxSimulatorView::Config::windowXPos = 10;
	int LinuxSimulatorView::Config::windowYPos = 10;
	int LinuxSimulatorView::Config::skinID = 6246;
	int LinuxSimulatorView::Config::skinWidth = 320;
	int LinuxSimulatorView::Config::skinHeight = 480;
	int LinuxSimulatorView::Config::zoomedWidth = LinuxSimulatorView::Config::skinWidth;
	int LinuxSimulatorView::Config::zoomedHeight = LinuxSimulatorView::Config::skinHeight;
	wxConfig *LinuxSimulatorView::Config::configFile;
	std::map<int, LinuxSimulatorView::SkinProperties> LinuxSimulatorView::fSkins;

	void LinuxSimulatorView::Config::Load()
	{
		if (LinuxSimulatorView::Config::settingsFilePath.IsEmpty())
		{
			LinuxSimulatorView::Config::settingsFilePath = LinuxFileUtils::GetHomePath();
			LinuxSimulatorView::Config::settingsFilePath.append("/.Solar2D/simulator.conf");
			LinuxSimulatorView::Config::configFile = new wxFileConfig(wxEmptyString, wxEmptyString, LinuxSimulatorView::Config::settingsFilePath);
		}

		// read from the simulator config file or create it, if it doesn't exist
		if (wxFileExists(LinuxSimulatorView::Config::settingsFilePath))
		{
			int relaunchOnFileChange = 0;
			LinuxSimulatorView::Config::configFile->Read(wxT(SIMULATOR_CONFIG_LAST_PROJECT_DIRECTORY), &LinuxSimulatorView::Config::lastProjectDirectory);
			LinuxSimulatorView::Config::configFile->Read(wxT(SIMULATOR_CONFIG_SHOW_RUNTIME_ERRORS), &LinuxSimulatorView::Config::showRuntimeErrors);
			LinuxSimulatorView::Config::configFile->Read(wxT(SIMULATOR_CONFIG_OPEN_LAST_PROJECT), &LinuxSimulatorView::Config::openLastProject);
			LinuxSimulatorView::Config::configFile->Read(wxT(SIMULATOR_CONFIG_RELAUNCH_ON_FILE_CHANGE), &relaunchOnFileChange);
			LinuxSimulatorView::Config::configFile->Read(wxT(SIMULATOR_CONFIG_WINDOW_X_POSITION), &LinuxSimulatorView::Config::windowXPos);
			LinuxSimulatorView::Config::configFile->Read(wxT(SIMULATOR_CONFIG_WINDOW_Y_POSITION), &LinuxSimulatorView::Config::windowYPos);
			LinuxSimulatorView::Config::configFile->Read(wxT(SIMULATOR_CONFIG_SKIN_ID), &LinuxSimulatorView::Config::skinID);
			LinuxSimulatorView::Config::configFile->Read(wxT(SIMULATOR_CONFIG_SKIN_WIDTH), &LinuxSimulatorView::Config::skinWidth);
			LinuxSimulatorView::Config::configFile->Read(wxT(SIMULATOR_CONFIG_SKIN_HEIGHT), &LinuxSimulatorView::Config::skinHeight);
			LinuxSimulatorView::Config::configFile->Read(wxT(SIMULATOR_CONFIG_SKIN_ZOOMED_WIDTH), &LinuxSimulatorView::Config::zoomedWidth);
			LinuxSimulatorView::Config::configFile->Read(wxT(SIMULATOR_CONFIG_SKIN_ZOOMED_HEIGHT), &LinuxSimulatorView::Config::zoomedHeight);
			LinuxSimulatorView::Config::relaunchOnFileChange = static_cast<LinuxPreferencesDialog::RelaunchType>(relaunchOnFileChange);
		}
		else
		{
			LinuxSimulatorView::Config::Save();
		}
	}

	void LinuxSimulatorView::Config::Save()
	{
		LinuxSimulatorView::Config::configFile->Write(wxT(SIMULATOR_CONFIG_LAST_PROJECT_DIRECTORY), LinuxSimulatorView::Config::lastProjectDirectory);
		LinuxSimulatorView::Config::configFile->Write(wxT(SIMULATOR_CONFIG_SHOW_RUNTIME_ERRORS), LinuxSimulatorView::Config::showRuntimeErrors);
		LinuxSimulatorView::Config::configFile->Write(wxT(SIMULATOR_CONFIG_OPEN_LAST_PROJECT), LinuxSimulatorView::Config::openLastProject);
		LinuxSimulatorView::Config::configFile->Write(wxT(SIMULATOR_CONFIG_RELAUNCH_ON_FILE_CHANGE), static_cast<int>(LinuxSimulatorView::Config::relaunchOnFileChange));
		LinuxSimulatorView::Config::configFile->Write(wxT(SIMULATOR_CONFIG_WINDOW_X_POSITION), LinuxSimulatorView::Config::windowXPos);
		LinuxSimulatorView::Config::configFile->Write(wxT(SIMULATOR_CONFIG_WINDOW_Y_POSITION), LinuxSimulatorView::Config::windowYPos);
		LinuxSimulatorView::Config::configFile->Write(wxT(SIMULATOR_CONFIG_SKIN_ID), LinuxSimulatorView::Config::skinID);
		LinuxSimulatorView::Config::configFile->Write(wxT(SIMULATOR_CONFIG_SKIN_WIDTH), LinuxSimulatorView::Config::skinWidth);
		LinuxSimulatorView::Config::configFile->Write(wxT(SIMULATOR_CONFIG_SKIN_HEIGHT), LinuxSimulatorView::Config::skinHeight);
		LinuxSimulatorView::Config::configFile->Write(wxT(SIMULATOR_CONFIG_SKIN_ZOOMED_WIDTH), LinuxSimulatorView::Config::zoomedWidth);
		LinuxSimulatorView::Config::configFile->Write(wxT(SIMULATOR_CONFIG_SKIN_ZOOMED_HEIGHT), LinuxSimulatorView::Config::zoomedHeight);
		LinuxSimulatorView::Config::configFile->Flush();
	}

	void LinuxSimulatorView::Config::Cleanup()
	{
		delete LinuxSimulatorView::Config::configFile;
	}

	bool LinuxSimulatorView::LoadSkin(lua_State *L, int skinID, std::string filePath)
	{
		int top = lua_gettop(L);
		int status = Lua::DoFile(L, filePath.c_str(), 0, true);
		lua_pop(L, 1); // remove DoFile result
		lua_getglobal(L, "simulator");

		SkinProperties skin;

		if (lua_type(L, -1) == LUA_TTABLE)
		{
			lua_getfield(L, -1, "device");
			if (lua_type(L, -1) == LUA_TSTRING)
			{
				skin.device = lua_tostring(L, -1);
			}
			lua_pop(L, 1);

			lua_getfield(L, -1, "screenOriginX");
			if (lua_type(L, -1) == LUA_TNUMBER)
			{
				skin.screenOriginX = lua_tointeger(L, -1);
			}
			lua_pop(L, 1);

			lua_getfield(L, -1, "screenOriginY");
			if (lua_type(L, -1) == LUA_TNUMBER)
			{
				skin.screenOriginY = lua_tointeger(L, -1);
			}
			lua_pop(L, 1);

			lua_getfield(L, -1, "screenWidth");
			if (lua_type(L, -1) == LUA_TNUMBER)
			{
				skin.screenWidth = lua_tointeger(L, -1);
			}
			lua_pop(L, 1);

			lua_getfield(L, -1, "screenHeight");
			if (lua_type(L, -1) == LUA_TNUMBER)
			{
				skin.screenHeight = lua_tointeger(L, -1);
			}
			lua_pop(L, 1);

			lua_getfield(L, -1, "androidDisplayApproximateDpi");
			if (lua_type(L, -1) == LUA_TNUMBER)
			{
				skin.androidDisplayApproximateDpi  = lua_tointeger(L, -1);
			}
			lua_pop(L, 1);

			lua_getfield(L, -1, "displayManufacturer");
			if (lua_type(L, -1) == LUA_TSTRING)
			{
				skin.displayManufacturer = lua_tostring(L, -1);
			}
			lua_pop(L, 1);

			lua_getfield(L, -1, "displayName");
			if (lua_type(L, -1) == LUA_TSTRING)
			{
				skin.displayName = lua_tostring(L, -1);
			}
			lua_pop(L, 1);

			lua_getfield(L, -1, "isUprightOrientationPortrait");
			if (lua_type(L, -1) == LUA_TBOOLEAN)
			{
				skin.isUprightOrientationPortrait = lua_toboolean(L, -1);
			}
			lua_pop(L, 1);

			lua_getfield(L, -1, "supportsScreenRotation");
			if (lua_type(L, -1) == LUA_TBOOLEAN)
			{
				skin.supportsScreenRotation = lua_toboolean(L, -1);
			}
			lua_pop(L, 1);

			lua_getfield(L, -1, "windowTitleBarName");
			if (lua_type(L, -1) == LUA_TSTRING)
			{
				skin.windowTitleBarName = lua_tostring(L, -1);
			}
			lua_pop(L, 1);

			// pop simulator
			lua_pop(L, 1);
			lua_settop(L, top);

			wxString skinTitle(skin.windowTitleBarName);
			skinTitle.append(wxString::Format(wxT(" (%ix%i)"), skin.screenWidth, skin.screenHeight));
			skin.skinTitle = skinTitle;
			skin.id = skinID;
			skin.selected = false;

			fSkins[skinID] = skin;
		}
		else
		{
			printf("Couldn't find 'simulator' table\n");
		}

		return (status == 0);
	}

	LinuxSimulatorView::SkinProperties LinuxSimulatorView::GetSkinProperties(int skinID)
	{
		if (fSkins.count(skinID) > 0)
		{
			return fSkins[skinID];
		}

		return fSkins.begin()->second;
	}

	LinuxSimulatorView::SkinProperties LinuxSimulatorView::GetSkinProperties(wxString skinTitle)
	{
		SkinProperties foundSkin;

		for (int i = 0; i < fSkins.size(); i++)
		{
			if (fSkins[i].skinTitle.IsSameAs(skinTitle))
			{
				foundSkin = fSkins[i];
				break;
			}
		}

		return foundSkin;
	}

	void LinuxSimulatorView::DeselectSkins()
	{
		for (int i = 0; i < fSkins.size(); i++)
		{
			fSkins[i].selected = false;
		}
	}

	void LinuxSimulatorView::SelectSkin(int skinID)
	{
		if (fSkins.count(skinID) > 0)
		{
			DeselectSkins();
			fSkins[skinID].selected = true;
		}
	}

	bool LinuxSimulatorView::IsRunningOnSimulator()
	{
#ifdef Rtt_SIMULATOR
		return true;
#endif

		return false;
	}

	void LinuxSimulatorView::OnLinuxPluginGet(const char *appPath, const char *appName, LinuxPlatform *platform)
	{
		const char *identity = "no-identity";

		// Create the app packager.
		MPlatformServices *service = new LinuxPlatformServices(platform);
		LinuxAppPackager packager(*service);

		// Read the application's "build.settings" file.
		bool wasSuccessful = packager.ReadBuildSettings(appPath);

		if (!wasSuccessful)
		{
			return;
		}

		int targetVersion = Rtt::TargetDevice::kLinux;
		const TargetDevice::Platform targetPlatform(TargetDevice::Platform::kLinuxPlatform);

		std::string linuxtemplate(platform->getInstallDir());
		linuxtemplate.append("/Resources/template_x64.tgz");

		// Package build settings parameters.
		LinuxAppPackagerParams linuxBuilderParams(
		    appName, NULL, identity, NULL,
		    appPath, NULL, NULL,
		    targetPlatform, targetVersion,
		    Rtt::TargetDevice::kLinux, NULL,
		    NULL, NULL, false, NULL, false, false, true);

		const char kBuildSettings[] = "build.settings";
		Rtt::String buildSettingsPath;
		platform->PathForFile(kBuildSettings, Rtt::MPlatform::kResourceDir, Rtt::MPlatform::kTestFileExists, buildSettingsPath);
		linuxBuilderParams.SetBuildSettingsPath(buildSettingsPath.GetString());
		int rc = packager.Build(&linuxBuilderParams, NULL);
	}

	void LinuxSimulatorView::OnWebBuild(wxCommandEvent &e)
	{
#ifndef CORONABUILDER_LINUX
		webBuildParams *params = (webBuildParams *)e.GetEventUserData();
		SolarAppContext *ctx = params->fCtx;
		bool useStandardResources = params->fUseStandardResources->GetValue();
		bool runAfterBuild = params->fRunAfterBuild->GetValue();
		bool createFBInstantArchive = params->fCreateFBInstance->GetValue();
		const char *srcDir = ctx->GetAppPath();
		const char *dstDir = ctx->GetSaveFolder().c_str();
		const char *identity = "no-identity";
		const char *applicationName = ctx->GetAppName().c_str();

		// Create the app packager.
		LinuxPlatform *platform = wxGetApp().GetPlatform();
		MPlatformServices *service = new LinuxPlatformServices(platform);
		WebAppPackager packager(*service);

		// Read the application's "build.settings" file.
		bool wasSuccessful = packager.ReadBuildSettings(srcDir);

		if (!wasSuccessful)
		{
			return;
		}

		// Check if a custom build ID has been assigned.
		// This is typically assigned to daily build versions of Corona.
		const char *customBuildId = packager.GetCustomBuildId();

		if (!Rtt_StringIsEmpty(customBuildId))
		{
			Rtt_Log("\nUsing custom Build Id %s\n", customBuildId);
		}

		// these are currently unused
		const char *bundleId = "bundleId"; //TODO
		const char *sdkRoot = "";
		int targetVersion = Rtt::TargetDevice::kWeb1_0;
		const TargetDevice::Platform targetPlatform(TargetDevice::Platform::kWebPlatform);
		const char *versionName = "1.0.0";
		bool isDistribution = true;

		std::string webtemplate = platform->getInstallDir();
		webtemplate += LUA_DIRSEP;
		webtemplate += "Resources";
		webtemplate += LUA_DIRSEP;
		webtemplate += "webtemplate.zip";

		// Package build settings parameters.
		WebAppPackagerParams webBuilderParams(
		    applicationName, versionName, identity, NULL,
		    srcDir, dstDir, NULL,
		    targetPlatform, targetVersion,
		    Rtt::TargetDevice::kWebGenericBrowser, customBuildId,
		    NULL, bundleId, isDistribution, useStandardResources, runAfterBuild, webtemplate.c_str(), createFBInstantArchive);

		// Select build template
		Rtt::Runtime *runtimePointer = ctx->GetRuntime();
		//	U32 luaModules = runtimePointer->VMContext().GetModules();
		//	webBuilderParams.InitializeProductId(luaModules);

		const char kBuildSettings[] = "build.settings";
		Rtt::String buildSettingsPath;
		ctx->GetPlatform()->PathForFile(kBuildSettings, Rtt::MPlatform::kResourceDir, Rtt::MPlatform::kTestFileExists, buildSettingsPath);
		webBuilderParams.SetBuildSettingsPath(buildSettingsPath.GetString());

		std::string tmp = Rtt_GetSystemTempDirectory();
		tmp += LUA_DIRSEP;
		tmp += "CoronaLabs";

		// Have the server build the app. (Warning! This is a long blocking call.)
		platform->SetActivityIndicator(true);
		int rc = packager.Build(&webBuilderParams, tmp.c_str());
		platform->SetActivityIndicator(false);

		params->fDlg->Close();

		wxMessageDialog *dial;

		if (rc == 0)
		{
			dial = new wxMessageDialog(NULL, wxT("Your application was built successfully."), wxT("Solar2D Simulator"), wxOK | wxICON_INFORMATION);
		}
		else
		{
			dial = new wxMessageDialog(NULL, wxT("Failed to build application."), wxT("Solar2D Simulator"), wxOK | wxICON_ERROR);
		}

		dial->ShowModal();
#endif
	}

	void LinuxSimulatorView::OnCancel(wxCommandEvent &e)
	{
		cancelBuild *eb = (cancelBuild *)e.GetEventUserData();
		eb->fDlg->Close();
	}

	/// <summary>Opens a dialog to build the currently selected project as an app.</summary>
	void LinuxSimulatorView::OnBuildForWeb(SolarAppContext *ctx)
	{
		wxDialog *OpenDialog = new wxDialog(NULL, -1, "HTML5 Build Setup (beta)", wxDefaultPosition, wxSize(550, 280));
		wxPanel *panel = new wxPanel(OpenDialog, -1);
		wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

		int y = 15;
		int h = 30;
		int x2 = 170;
		wxSize sz = wxSize(350, 30);
		new wxStaticText(panel, -1, wxT("Application Name:"), wxPoint(20, y));
		new wxTextCtrl(panel, -1, ctx->GetAppName(), wxPoint(x2, y - 2), sz);
		y += h;
		new wxStaticText(panel, -1, wxT("Version Code:"), wxPoint(20, y));
		new wxTextCtrl(panel, -1, "1", wxPoint(x2, y - 2), sz);
		y += h;
		new wxStaticText(panel, -1, wxT("Project Path:"), wxPoint(20, y));
		new wxTextCtrl(panel, -1, ctx->GetAppPath(), wxPoint(x2, y - 2), sz);
		y += h;
		new wxStaticText(panel, -1, wxT("Save to Folder:"), wxPoint(20, y));
		new wxTextCtrl(panel, -1, ctx->GetSaveFolder(), wxPoint(x2, y - 2), sz);

		y += h + 5;
		wxCheckBox *useStandardResources = new wxCheckBox(panel, -1, "Include Widget Resources", wxPoint(x2, y));
		y += h;
		wxCheckBox *runAfterBuild = new wxCheckBox(panel, -1, "Run after build?", wxPoint(x2, y));
		y += h;
		wxCheckBox *createFBInstance = new wxCheckBox(panel, -1, "Create FB Instant archive", wxPoint(x2, y));

		wxButton *okButton = new wxButton(OpenDialog, -1, wxT("Build"), wxDefaultPosition, wxSize(70, 30));
		okButton->Bind(wxEVT_BUTTON, &LinuxSimulatorView::OnWebBuild, wxID_ANY, wxID_ANY, new webBuildParams(OpenDialog, ctx, useStandardResources, runAfterBuild, createFBInstance));
		okButton->SetDefault();

		wxButton *closeButton = new wxButton(OpenDialog, -1, wxT("Cancel"), wxDefaultPosition, wxSize(70, 30));
		closeButton->Bind(wxEVT_BUTTON, &LinuxSimulatorView::OnCancel, wxID_ANY, wxID_ANY, new cancelBuild(OpenDialog)); // cancel

		hbox->Add(okButton, 1);
		hbox->Add(closeButton, 1, wxLEFT, 5);

		vbox->Add(panel, 1);
		vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

		OpenDialog->SetSizer(vbox);
		OpenDialog->Centre();
		int rc = OpenDialog->ShowModal();
		OpenDialog->Destroy();
	}
} // namespace Rtt
