// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "IOdysseyStylusInputModule.h"

#include "CoreMinimal.h"
#include "Framework/Docking/TabManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "ISettingsModule.h"
#include "Logging/LogMacros.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SWindow.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#include "SStylusInputDebugWidget.h"
#include "OdysseyStylusInputSettings.h"
#if PLATFORM_WINDOWS
	#include "Windows/WintabStylusInputInterface.h"
	#include "Windows/InkStylusInputInterface.h"
#elif PLATFORM_MAC
	#include "Mac/NSEventStylusInputInterface.h"
#endif

#define LOCTEXT_NAMESPACE "FOdysseyStylusInputModule"

//---

static const FName StylusInputDebugTabName = FName("StylusInputDebug");

class FOdysseyStylusInputModule
	: public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		// register settings
		ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (settingsModule)
		{
			settingsModule->RegisterSettings("Editor", "Plugins", "ILIADStylusInput"
				, LOCTEXT("OdysseyPainterEditorSettingsName", "ILIAD Stylus Input")
				, LOCTEXT("OdysseyPainterEditorSettingsDescription", "Configure the behaviour of stylus inputs in ILIAD.")
				, GetMutableDefault<UOdysseyStylusInputSettings>());
		}
	}

	virtual void ShutdownModule() override
	{
		// unregister settings
		ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (settingsModule)
		{
			settingsModule->UnregisterSettings("Editor", "Plugins", "ILIADStylusInput");
		}
	}
};

IMPLEMENT_MODULE(FOdysseyStylusInputModule, OdysseyStylusInput)

//---
//---
//---

void UOdysseyStylusInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogStylusInput, Log, TEXT("Initializing StylusInput subsystem."));

	const UOdysseyStylusInputSettings& settings = *GetDefault<UOdysseyStylusInputSettings>();
	// Create the StylusInputInterface corresponding to the wanted StylusInputDriver
	SetStylusInputDriver(settings.GetStylusDriver());

	if (!InputInterface.IsValid())
	{
		UE_LOG(LogStylusInput, Log, TEXT("StylusInput not supported on this platform."));
		return;
	}

	// Manage registration of the debug tab
	const TSharedRef<FGlobalTabmanager>& TabManager = FGlobalTabmanager::Get();
	const IWorkspaceMenuStructure& MenuStructure = WorkspaceMenu::GetMenuStructure();

	TabManager->RegisterNomadTabSpawner(StylusInputDebugTabName,
		FOnSpawnTab::CreateUObject(this, &UOdysseyStylusInputSubsystem::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("DebugTabTitle", "Stylus Input Debug"))
		.SetTooltipText(LOCTEXT("DebugTabTooltip", "Debug panel to display current values of stylus inputs."))
		.SetGroup(MenuStructure.GetDeveloperToolsMiscCategory());
}

void UOdysseyStylusInputSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FGlobalTabmanager::Get()->UnregisterTabSpawner( StylusInputDebugTabName );

	InputInterface.Reset();

	UE_LOG( LogStylusInput, Log, TEXT( "Shutting down StylusInput subsystem." ) );
}

//---

void UOdysseyStylusInputSubsystem::SetStylusInputDriver(EOdysseyStylusInputDriver iDriver)
{
	InputInterface.Reset();

	switch (iDriver)
	{
		case OdysseyStylusInputDriver_None:
			InputInterface = nullptr;
			break;
#if PLATFORM_WINDOWS
		case OdysseyStylusInputDriver_Ink:
			InputInterface = CreateStylusInputInterfaceInk();
			break;
		case OdysseyStylusInputDriver_Wintab:
			InputInterface = CreateStylusInputInterfaceWintab();
			break;
#elif PLATFORM_MAC
		case OdysseyStylusInputDriver_NSEvent:
			InputInterface = CreateStylusInputInterfaceNSEvent();
			break;
#endif
		default:
			InputInterface = nullptr;
	}

	OnStylusInputChangedCB.ExecuteIfBound(InputInterface);
}

//---

void UOdysseyStylusInputSubsystem::AddMessageHandler(IStylusMessageHandler& InHandler)
{
	MessageHandlers.AddUnique(&InHandler);
}

void UOdysseyStylusInputSubsystem::RemoveMessageHandler(IStylusMessageHandler& InHandler)
{
	MessageHandlers.Remove(&InHandler);
}

//---

int32 UOdysseyStylusInputSubsystem::NumInputDevices() const
{
	if (InputInterface.IsValid())
	{
		return InputInterface->NumInputDevices();
	}
	return 0;
}

const IStylusInputDevice* UOdysseyStylusInputSubsystem::GetInputDevice(int32 Index) const
{
	if (InputInterface.IsValid())
	{
		return InputInterface->GetInputDevice(Index);
	}
	return nullptr;
}

void UOdysseyStylusInputSubsystem::Tick(float DeltaTime)
{
	if (InputInterface.IsValid())
	{
		InputInterface->Tick();

		for (int32 DeviceIdx = 0; DeviceIdx < NumInputDevices(); ++DeviceIdx)
		{
			IStylusInputDevice* InputDevice = InputInterface->GetInputDevice(DeviceIdx);
			if (InputDevice->IsDirty())
			{
				InputDevice->Tick();
				TArray<FStylusState> tmp( InputDevice->GetCurrentState() );

				for (IStylusMessageHandler* Handler : MessageHandlers)
				{
					for( const FStylusState& stylus_state : tmp )
					{
						Handler->OnStylusStateChanged( InputInterface->Widget(), stylus_state, DeviceIdx );
					}
				}
			}
		}
	}
}

//---

TSharedRef<SDockTab> UOdysseyStylusInputSubsystem::OnSpawnPluginTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SStylusInputDebugWidget, *this)
		];
}

#undef LOCTEXT_NAMESPACE
