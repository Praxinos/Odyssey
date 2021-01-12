// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "IStylusState.h"

#include "EditorSubsystem.h"
#include "TickableEditorObject.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"

#include "OdysseyStylusInputDriver.h"

#include "IOdysseyStylusInputModule.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogStylusInput, Log, All);

/**
 * Module to handle Wacom-style tablet input using styluses.
 */
class ODYSSEYSTYLUSINPUT_API IOdysseyStylusInputModule : public IModuleInterface
{
public:

	/**
	 * Retrieve the module instance.
	 */
	static inline IOdysseyStylusInputModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IOdysseyStylusInputModule>("OdysseyStylusInput");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("OdysseyStylusInput");
	}
};

// This is the interface that all platform-specific implementations must implement.
class IStylusInputInterfaceInternal
{
public:
	virtual void Tick() = 0;

	virtual IStylusInputDevice* GetInputDevice(int32 Index) const = 0;
	virtual int32 NumInputDevices() const = 0;

    virtual TWeakPtr<SWindow> Window() const = 0;
    virtual TWeakPtr<SWidget> Widget() const = 0;
};


ODYSSEYSTYLUSINPUT_API TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterface();
ODYSSEYSTYLUSINPUT_API TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterfaceWintab();
ODYSSEYSTYLUSINPUT_API TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterfaceNSEvent();


DECLARE_DELEGATE_OneParam( FOnStylusInputChanged, TSharedPtr<IStylusInputInterfaceInternal> );

UCLASS()
class ODYSSEYSTYLUSINPUT_API UOdysseyStylusInputSubsystem :
	public UEditorSubsystem, 
	public FTickableEditorObject
{
	GENERATED_BODY()
public:
	// UEditorSubsystem implementation
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void SetStylusInputInterface( TSharedPtr<IStylusInputInterfaceInternal> iStylusInput );
	void SetStylusInputDriver(EOdysseyStylusInputDriver iDriver);
	FOnStylusInputChanged& OnStylusInputChanged();

	/** Retrieve the input device that is at the given index, or nullptr if not found. Corresponds to the StylusIndex in IStylusMessageHandler. */
	const IStylusInputDevice* GetInputDevice(int32 Index) const;

	/** Return the number of active input devices. */
	int32 NumInputDevices() const; 

	/** Add a message handler to receive messages from the stylus. */
	void AddMessageHandler(IStylusMessageHandler& MessageHandler);

	/** Remove a previously registered message handler. */
	void RemoveMessageHandler(IStylusMessageHandler& MessageHandler);

	// FTickableEditorObject implementation
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT( UOdysseyStylusInputSubsystem, STATGROUP_Tickables); }

private:
	TSharedPtr<IStylusInputInterfaceInternal> InputInterface;
	FOnStylusInputChanged OnStylusInputChangedCB;
	TArray<IStylusMessageHandler*> MessageHandlers;

	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& Args);

	/** FTickableEditorObject must be destroyed on the main thread. */
	virtual bool IsDestructionThreadSafe() const override { return false; }
};
