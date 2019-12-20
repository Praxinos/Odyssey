// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "IOdysseyStylusInputModule.h"
#include "CoreMinimal.h"

#include "Framework/Docking/TabManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "Logging/LogMacros.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SWindow.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "SStylusInputDebugWidget.h"

#define LOCTEXT_NAMESPACE "FOdysseyStylusInputModule"


static const FName StylusInputDebugTabName = FName("StylusInputDebug");

class FOdysseyStylusInputModule : public IModuleInterface
{
};

IMPLEMENT_MODULE(FOdysseyStylusInputModule, OdysseyStylusInput)

// This is the function that all platform-specific implementations are required to implement.
TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterface();

#if PLATFORM_WINDOWS
#include "WindowsStylusInputInterface.h"
#else
TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterface() { return TSharedPtr<IStylusInputInterfaceInternal>(); }
#endif

// TODO: Other platforms

void UOdysseyStylusInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogStylusInput, Log, TEXT("Initializing StylusInput subsystem."));

	InputInterface = CreateStylusInputInterface();

	if (!InputInterface.IsValid())
	{
		UE_LOG(LogStylusInput, Log, TEXT("StylusInput not supported on this platform."));
		return;
	}

	const TSharedRef<FGlobalTabmanager>& TabManager = FGlobalTabmanager::Get();
	const IWorkspaceMenuStructure& MenuStructure = WorkspaceMenu::GetMenuStructure();

	TabManager->RegisterNomadTabSpawner(StylusInputDebugTabName,
		FOnSpawnTab::CreateUObject(this, &UOdysseyStylusInputSubsystem::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("DebugTabTitle", "Stylus Input Debug"))
		.SetTooltipText(LOCTEXT("DebugTabTooltip", "Debug panel to display current values of stylus inputs."))
		.SetGroup(MenuStructure.GetDeveloperToolsMiscCategory());
}

FOnStylusInputChanged& UOdysseyStylusInputSubsystem::OnStylusInputChanged()
{
    return OnStylusInputChangedCB;
}

void UOdysseyStylusInputSubsystem::SetStylusInputInterface( TSharedPtr<IStylusInputInterfaceInternal> iStylusInput )
{
    InputInterface.Reset();
    InputInterface = iStylusInput;

    OnStylusInputChangedCB.ExecuteIfBound( InputInterface );
}

void UOdysseyStylusInputSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FGlobalTabmanager::Get()->UnregisterTabSpawner(StylusInputDebugTabName);

	InputInterface.Reset();

	UE_LOG(LogStylusInput, Log, TEXT("Shutting down StylusInput subsystem."));
}

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

void UOdysseyStylusInputSubsystem::AddMessageHandler(IStylusMessageHandler& InHandler)
{
	MessageHandlers.AddUnique(&InHandler);
}

void UOdysseyStylusInputSubsystem::RemoveMessageHandler(IStylusMessageHandler& InHandler)
{
	MessageHandlers.Remove(&InHandler);
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

TSharedRef<SDockTab> UOdysseyStylusInputSubsystem::OnSpawnPluginTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SStylusInputDebugWidget, *this)
		];
}



#undef LOCTEXT_NAMESPACE
