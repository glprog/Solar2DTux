#include "Rtt_AndroidBuildDialog.h"
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
#include "Rtt_LinuxBuildDialog.h"
#include "Rtt_SimulatorRecents.h"
#include "Rtt_AndroidAppPackager.h"
#include "Rtt_TargetAndroidAppStore.h"
#include "ListKeyStore.h"
#include "Core/Rtt_FileSystem.h"
#include <string.h>

#define GOOGLE_PLAY_STORE_TARGET "Google Play"
#define AMAZON_STORE_TARGET "Amazon"
#define NO_STORE_TARGET "None"

namespace Rtt
{
	AndroidBuildDialog::AndroidBuildDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style):
		wxDialog(parent, id, title, pos, size, wxCAPTION)
	{
		appNameTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
		appVersionTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
		appVersionCodeTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
		appPackageNameTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
		appPathTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
		appBuildPathTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
		appBuildPathButton = new wxButton(this, wxID_OPEN, wxT("..."));
		const wxString targetAppStoreComboBoxChoices[] =
		{
			wxT(GOOGLE_PLAY_STORE_TARGET),
			wxT(AMAZON_STORE_TARGET),
			wxT(NO_STORE_TARGET),
		};
		targetAppStoreComboBox = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 3, targetAppStoreComboBoxChoices, wxCB_DROPDOWN | wxCB_READONLY);
		keystorePathTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
		keystorePathButton = new wxButton(this, wxID_FILE1, wxT("..."));
		const wxString keyAliasComboBoxChoices[] =
		{
			wxT("androiddebugkey"),
		};
		keyAliasComboBox = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, keyAliasComboBoxChoices, wxCB_DROPDOWN | wxCB_READONLY);
		installToDeviceCheckbox = new wxCheckBox(this, wxID_ANY, wxEmptyString);
		buildButton = new wxButton(this, wxID_OK, wxT("Build"));
		cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"));

		SetProperties();
		DoLayout();
	}

	void AndroidBuildDialog::SetProperties()
	{
		SetTitle(wxT("Build For Android"));
		appNameTextCtrl->SetToolTip(wxT("The name to give your projects output binary."));
		appVersionTextCtrl->SetToolTip(wxT("Your projects version number/string."));
		appVersionCodeTextCtrl->SetToolTip(wxT("Your projects version code (integer)."));
		appPackageNameTextCtrl->SetToolTip(wxT("A unique Java style package identifier for your proejct (e.g. com.mycompany.myapp)."));
		appPathTextCtrl->SetToolTip(wxT("The path to your projects main folder."));
		appBuildPathTextCtrl->SetToolTip(wxT("The path to your projects generated binary."));
		appBuildPathButton->SetMinSize(wxSize(40, 30));
		appBuildPathButton->SetToolTip(wxT("Click here to set a new path for your projects ouput binary."));
		targetAppStoreComboBox->SetToolTip(wxT("The app store you wish to target."));
		targetAppStoreComboBox->SetSelection(0);
		keystorePathTextCtrl->SetToolTip(wxT("The path to your projects android keystore."));
		keystorePathButton->SetMinSize(wxSize(40, 30));
		keystorePathButton->SetToolTip(wxT("Click here to set a new path for your projects ouput binary."));
		keyAliasComboBox->SetToolTip(wxT("Your projects android keystore alias."));
		keyAliasComboBox->SetSelection(0);
		installToDeviceCheckbox->SetToolTip(wxT("Installs this app to your connected android device via adb after a build."));
		keystorePassword = "android";
		keystorePasswordValid = true;
	}

	void AndroidBuildDialog::DoLayout()
	{
		wxBoxSizer *parentGridSizer = new wxBoxSizer(wxVERTICAL);
		wxFlexGridSizer *buttonGridSizer = new wxFlexGridSizer(1, 2, 0, 0);
		wxFlexGridSizer *optionsGridSizer = new wxFlexGridSizer(10, 3, 4, 10);
		wxStaticText *appNameLabel = new wxStaticText(this, wxID_ANY, wxT("Application Name:"));
		optionsGridSizer->Add(appNameLabel, 0, wxLEFT, 10);
		optionsGridSizer->Add(appNameTextCtrl, 0, wxEXPAND | wxRIGHT, 10);
		optionsGridSizer->Add(0, 0, 0, 0, 0);
		wxStaticText *appVersionLabel = new wxStaticText(this, wxID_ANY, wxT("Application Version:"));
		optionsGridSizer->Add(appVersionLabel, 0, wxLEFT, 10);
		optionsGridSizer->Add(appVersionTextCtrl, 0, wxEXPAND | wxRIGHT, 10);
		optionsGridSizer->Add(0, 0, 0, 0, 0);
		wxStaticText *appVersionCodeLabel = new wxStaticText(this, wxID_ANY, wxT("Application Version Code:"));
		optionsGridSizer->Add(appVersionCodeLabel, 0, wxLEFT, 10);
		optionsGridSizer->Add(appVersionCodeTextCtrl, 0, wxEXPAND | wxRIGHT, 10);
		optionsGridSizer->Add(0, 0, 0, 0, 0);
		wxStaticText *appPackageNameLabel = new wxStaticText(this, wxID_ANY, wxT("Package Name:"));
		optionsGridSizer->Add(appPackageNameLabel, 0, wxLEFT, 10);
		optionsGridSizer->Add(appPackageNameTextCtrl, 0, wxEXPAND | wxRIGHT, 10);
		optionsGridSizer->Add(0, 0, 0, 0, 0);
		wxStaticText *appPathLabel = new wxStaticText(this, wxID_ANY, wxT("Application Path:"));
		optionsGridSizer->Add(appPathLabel, 0, wxLEFT, 10);
		optionsGridSizer->Add(appPathTextCtrl, 0, wxEXPAND | wxRIGHT, 10);
		optionsGridSizer->Add(0, 0, 0, 0, 0);
		wxStaticText *appBuildPathLabel = new wxStaticText(this, wxID_ANY, wxT("Build Ouput Path:"));
		optionsGridSizer->Add(appBuildPathLabel, 0, wxLEFT, 10);
		optionsGridSizer->Add(appBuildPathTextCtrl, 0, wxEXPAND | wxRIGHT, 10);
		optionsGridSizer->Add(appBuildPathButton, 0, 0, 0);
		wxStaticText *targetAppStoreLabel = new wxStaticText(this, wxID_ANY, wxT("Target App Store:"));
		optionsGridSizer->Add(targetAppStoreLabel, 0, wxLEFT, 10);
		optionsGridSizer->Add(targetAppStoreComboBox, 0, wxEXPAND | wxRIGHT, 10);
		optionsGridSizer->Add(0, 0, 0, 0, 0);
		wxStaticText *keyStoreLabel = new wxStaticText(this, wxID_ANY, wxT("Keystore:"));
		optionsGridSizer->Add(keyStoreLabel, 0, wxLEFT, 10);
		optionsGridSizer->Add(keystorePathTextCtrl, 0, wxEXPAND | wxRIGHT, 10);
		optionsGridSizer->Add(keystorePathButton, 0, 0, 0);
		wxStaticText *keyAliasLabel = new wxStaticText(this, wxID_ANY, wxT("Key Alias:"));
		optionsGridSizer->Add(keyAliasLabel, 0, wxLEFT, 10);
		optionsGridSizer->Add(keyAliasComboBox, 0, wxEXPAND | wxRIGHT, 10);
		optionsGridSizer->Add(0, 0, 0, 0, 0);
		wxStaticText *installToDeviceLabel = new wxStaticText(this, wxID_ANY, wxT("Install After Build?"));
		optionsGridSizer->Add(installToDeviceLabel, 0, wxLEFT, 10);
		optionsGridSizer->Add(installToDeviceCheckbox, 0, 0, 0);
		optionsGridSizer->Add(0, 0, 0, 0, 0);
		optionsGridSizer->AddGrowableCol(1);
		parentGridSizer->Add(optionsGridSizer, 0, wxALL | wxEXPAND, 10);
		wxStaticLine *staticLineSeparator = new wxStaticLine(this, wxID_ANY);
		parentGridSizer->Add(staticLineSeparator, 0, wxBOTTOM | wxEXPAND | wxTOP, 5);
		buttonGridSizer->Add(buildButton, 1, wxRIGHT | wxTOP, 10);
		buttonGridSizer->Add(cancelButton, 1, wxRIGHT | wxTOP, 10);
		buttonGridSizer->AddGrowableCol(0);
		buttonGridSizer->AddGrowableCol(1);
		parentGridSizer->Add(buttonGridSizer, 1, wxALIGN_CENTER_HORIZONTAL, 0);
		SetSizer(parentGridSizer);
		Layout();
		Centre();
	}

	void AndroidBuildDialog::SetAppContext(SolarAppContext *appContext)
	{
		fCoronaAppContext = appContext;
		appNameTextCtrl->SetValue(fCoronaAppContext->GetAppName());
		appPathTextCtrl->SetValue(fCoronaAppContext->GetAppPath());
		appBuildPathTextCtrl->SetValue(fCoronaAppContext->GetSaveFolder());
		appVersionTextCtrl->SetValue("1.0");
		appVersionCodeTextCtrl->SetValue("1");

		char uname[256] = {0};
		int rc = getlogin_r(uname, sizeof(uname));
		std::string package("com.solar2d.");
		package.append(uname).append(".");
		package.append(fCoronaAppContext->GetAppName());
		appPackageNameTextCtrl->SetValue(package);

		std::string keystorePath(LinuxFileUtils::GetStartupPath(NULL));
		keystorePath.append("/Resources/debug.keystore");
		keystorePathTextCtrl->SetValue(keystorePath);

		ReadKeystore(keystorePath, keystorePassword.ToStdString().c_str(), true);
	}

	bool AndroidBuildDialog::ReadKeystore(std::string keystorePath, std::string password, bool showErrors)
	{
		keystorePasswordValid = false;
		ListKeyStore listKeyStore;  // uses Java to read keystore

		if (listKeyStore.GetAliasList(keystorePath.c_str(), password.c_str()))
		{
			keystorePasswordValid = true;

			if (listKeyStore.GetSize() < 1)
			{
				if (showErrors)
				{
					wxMessageDialog *errorDialog = new wxMessageDialog(NULL, wxT("The selected keystore doesn't have any aliases."), wxT("Solar2D Simulator"), wxOK | wxICON_ERROR);
				}
			}
			else
			{
				// Succeeded and there is at least one alias - add aliases to alias dropdown
				keyAliasComboBox->Clear();

				for (int i = 0; i < listKeyStore.GetSize(); i++)
				{
					keyAliasComboBox->Append(listKeyStore.GetAlias(i));
				}

				keyAliasComboBox->SetSelection(0);
			}
		}
		else // didn't get valid password, or keystore bad format
		{
			if (showErrors)
			{
				wxMessageDialog *errorDialog = new wxMessageDialog(NULL, wxT("The selected keystore is either invalid or an incorrect password was entered."), wxT("Solar2D Simulator"), wxOK | wxICON_ERROR);
			}
		}

		return keystorePasswordValid;
	}

	BEGIN_EVENT_TABLE(AndroidBuildDialog, wxDialog)
		EVT_BUTTON(wxID_OPEN, AndroidBuildDialog::OnSelectOutputPathClicked)
		EVT_BUTTON(wxID_FILE1, AndroidBuildDialog::OnSelectKeyStorePathClicked)
		EVT_BUTTON(wxID_OK, AndroidBuildDialog::OnBuildClicked)
		EVT_BUTTON(wxID_CANCEL, AndroidBuildDialog::OnCancelClicked)
	END_EVENT_TABLE();

	void AndroidBuildDialog::OnSelectOutputPathClicked(wxCommandEvent &event)
	{
		wxDirDialog openDirDialog(this, _("Choose Output Directory"), LinuxFileUtils::GetHomePath(), 0, wxDefaultPosition);

		if (openDirDialog.ShowModal() == wxID_OK)
		{
			appBuildPathTextCtrl->SetValue(openDirDialog.GetPath());
		}
	}

	void AndroidBuildDialog::OnSelectKeyStorePathClicked(wxCommandEvent &event)
	{
		wxFileDialog openFileDialog(this, _("Choose Keystore Path"), LinuxFileUtils::GetHomePath(), wxEmptyString, "KeyStore files |*.keystore", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

		if (openFileDialog.ShowModal() == wxID_OK)
		{
			keystorePathTextCtrl->SetValue(openFileDialog.GetPath());
			keystorePassword = "android";

			// this isn't the default "debug.keystore". request the password from the user
			if (keystorePathTextCtrl->GetValue().EndsWith("debug.keystore"))
			{
				wxPasswordEntryDialog *passEntryDialog = new wxPasswordEntryDialog(this, "Please enter the password for this keystore", wxGetPasswordFromUserPromptStr);

				if (passEntryDialog->ShowModal() == wxID_OK)
				{
					keystorePassword = passEntryDialog->GetValue();
				}

				passEntryDialog->Destroy();
			}

			ReadKeystore(keystorePathTextCtrl->GetValue().ToStdString(), keystorePassword.ToStdString().c_str(), true);
		}
	}

	void AndroidBuildDialog::OnBuildClicked(wxCommandEvent &event)
	{
		LinuxPlatform *platform = wxGetApp().GetPlatform();
		MPlatformServices *service = new LinuxPlatformServices(platform);
		Rtt::Runtime *runtimePointer = fCoronaAppContext->GetRuntime();
		wxString appName(appNameTextCtrl->GetValue());
		wxString sourceDir(appPathTextCtrl->GetValue());
		wxString outputDir(appBuildPathTextCtrl->GetValue());
		wxString appVersion(appVersionTextCtrl->GetValue());
		wxString packageName(appPackageNameTextCtrl->GetValue());
		wxString appVersionCode(appVersionCodeTextCtrl->GetValue());
		wxString keystore(keystorePathTextCtrl->GetValue());
		wxString keystoreAlias(keyAliasComboBox->GetValue());
		std::string androidTemplate(platform->getInstallDir());
		std::string tmp = Rtt_GetSystemTempDirectory();
		std::string targetAppStoreName(TargetAndroidAppStore::kGoogle.GetStringId());
		const char *identity = "no-identity";
		const char *bundleId = "bundleId";
		const char *provisionFile = "";
		int versionCode = wxAtoi(appVersionCode);
		bool installAfterBuild = installToDeviceCheckbox->GetValue() == true;
		const TargetDevice::Platform targetPlatform(TargetDevice::Platform::kAndroidPlatform);
		bool isDistribution = true;
		const char kBuildSettings[] = "build.settings";
		Rtt::String buildSettingsPath;
		wxMessageDialog *resultDialog = new wxMessageDialog(wxGetApp().GetFrame(), wxEmptyString, wxT("Build Error"), wxOK | wxICON_WARNING);

		// setup paths
		androidTemplate.append("/Resources");
		tmp.append("/CoronaLabs");

		AndroidAppPackager packager(*service, androidTemplate.c_str());
		bool foundBuildSettings = packager.ReadBuildSettings(sourceDir.c_str());
		const char *customBuildId = packager.GetCustomBuildId();
		bool checksPassed = foundBuildSettings && !appVersion.IsEmpty() && !appName.IsEmpty() && !appVersionCode.IsEmpty() && !packageName.IsEmpty() && versionCode != 0 && keystorePasswordValid;

		// pre-build validation
		if (!foundBuildSettings)
		{
			resultDialog->SetMessage(wxT("build.settings file not found in project path."));
		}

		if (appName.IsEmpty())
		{
			resultDialog->SetMessage(wxT("App name cannot be empty."));
		}

		if (appVersion.IsEmpty())
		{
			resultDialog->SetMessage(wxT("App version cannot be empty."));
		}

		if (appVersionCode.IsEmpty())
		{
			resultDialog->SetMessage(wxT("App version code cannot be empty."));
		}

		if (versionCode <= 0)
		{
			resultDialog->SetMessage(wxT("App version code must be an integer."));
		}

		if (packageName.IsEmpty())
		{
			resultDialog->SetMessage(wxT("App package name cannot be empty."));
		}

		if (!keystorePasswordValid)
		{
			resultDialog->SetMessage(wxT("The keystore password is invalid."));
		}

		// ensure unzip is installed on the users system
		if (!wxFileName::Exists("/usr/bin/unzip"))
		{
			checksPassed = false;
			resultDialog->SetMessage(wxT("unzip tool not found at /usr/bin - please install it via your package manager."));
		}

		// ensure Solar2DBuilder exists at the correct location
		wxString solar2DBuilderPath(LinuxFileUtils::GetHomePath());
		solar2DBuilderPath.append("/.local/share/Corona/Native/Corona/lin/bin/Solar2DBuilder");

		if (!wxFileName::Exists(solar2DBuilderPath))
		{
			checksPassed = false;
			resultDialog->SetMessage(wxT("Solar2DBuilder not found at ~/.local/share/Corona/Native/Corona/lin/bin/Solar2DBuilder - please re-install Solar2DTux."));
		}

		// ensure we have write access to the target output directory
		if (!wxFileName::IsDirWritable(outputDir))
		{
			resultDialog->SetMessage(wxT("I don't have write access to the selected output directory."));
			checksPassed = false;
		}

		// checks failed, show failure popup
		if (!checksPassed)
		{
			resultDialog->SetTitle("Invalid Settings");
			resultDialog->ShowModal();
			return;
		}

		Rtt_Log("Starting Android build...\n");

		// check if a custom build ID has been assigned
		if (!Rtt_StringIsEmpty(customBuildId))
		{
			Rtt_Log("Using custom Build Id %s\n", customBuildId);
		}

		// set the target app store
		if (targetAppStoreComboBox->GetValue().IsSameAs(GOOGLE_PLAY_STORE_TARGET))
		{
			targetAppStoreName = TargetAndroidAppStore::kGoogle.GetStringId();
		}
		else if (targetAppStoreComboBox->GetValue().IsSameAs(AMAZON_STORE_TARGET))
		{
			targetAppStoreName = TargetAndroidAppStore::kAmazon.GetStringId();
		}
		else if (targetAppStoreComboBox->GetValue().IsSameAs(NO_STORE_TARGET))
		{
			targetAppStoreName = TargetAndroidAppStore::kNone.GetStringId();
		}

		AndroidAppPackagerParams androidBuilderParams(
		    appName.ToStdString().c_str(), appVersion.ToStdString().c_str(), identity, provisionFile,
		    sourceDir.ToStdString().c_str(), outputDir.ToStdString().c_str(), androidTemplate.c_str(),
		    targetPlatform, targetAppStoreName.c_str(),
		    (S32)Rtt::TargetDevice::VersionForPlatform(Rtt::TargetDevice::kAndroidPlatform),
		    customBuildId, NULL,
		    packageName.ToStdString().c_str(), isDistribution, keystore.ToStdString().c_str(), keystorePassword.ToStdString().c_str(), keystoreAlias.ToStdString().c_str(), keystorePassword.ToStdString().c_str()/*alias_pwd.c_str()*/, versionCode);

		// select build template
		fCoronaAppContext->GetPlatform()->PathForFile(kBuildSettings, Rtt::MPlatform::kResourceDir, Rtt::MPlatform::kTestFileExists, buildSettingsPath);
		androidBuilderParams.SetBuildSettingsPath(buildSettingsPath.GetString());

		// build the app (warning! this is blocking call)
		platform->SetActivityIndicator(true);
		int buildResult = packager.Build(&androidBuilderParams, tmp.c_str());
		platform->SetActivityIndicator(false);
		EndModal(wxID_OK);
		wxGetApp().GetFrame()->RemoveSuspendedPanel();

		if (buildResult == 0)
		{
			Rtt_Log("Android build succeeded.\n");
		}
		else
		{
			Rtt_Log("Android build failed. Check the log for more details.\n");
		}

		int dialogResultFlags = buildResult == 0 ? wxOK | wxICON_INFORMATION : wxOK | wxICON_ERROR;
		resultDialog->SetTitle("Build Result");
		resultDialog->SetMessage(buildResult == 0 ? "Your application was built successfully." : "Failed to build the application.\nSee the console for more info.");
		resultDialog->SetMessageDialogStyle(dialogResultFlags);
		resultDialog->ShowModal();
		wxYield();

		// install after build
		if (buildResult == 0 && installAfterBuild)
		{
			std::string cmd("adb install -r \"");
			cmd.append(outputDir.ToStdString().c_str());
			cmd.append("/").append(appName.ToStdString().c_str());
			cmd.append(".apk").append("\"");
			wxExecute(cmd);
		}
	}

	void AndroidBuildDialog::OnCancelClicked(wxCommandEvent &event)
	{
		wxGetApp().GetFrame()->RemoveSuspendedPanel();
		EndModal(wxID_CLOSE);
	}
};
