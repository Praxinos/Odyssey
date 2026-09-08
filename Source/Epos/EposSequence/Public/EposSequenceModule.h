// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleManager.h"

#include "INamingFormatter.h" // Because if template

class ISequencer;

class FEposSequenceModule
    : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

public:
    DECLARE_MULTICAST_DELEGATE_ThreeParams( FOnNewActorTrackAdded, const AActor& /*SourceActor*/, const FGuid& /*Binding*/, TSharedPtr<ISequencer> /*Sequencer*/ );
    /** Callback to set up defaults for new actor tracks */
    virtual FOnNewActorTrackAdded& OnNewActorTrackAdded();

public:
    EPOSSEQUENCE_API void RegisterNamingFormatter( UNamingFormatter* iFormatter );
    EPOSSEQUENCE_API void UnregisterNamingFormatter( UNamingFormatter* iFormatter );

    void RegisterTelemetry();
    void UnregisterTelemetry();

public:
    template<typename Formatter>
    UNamingFormatter* GetNamingFormatter()
    {
        for( auto formatter : mNamingFormatters )
        {
            if( formatter->IsA<Formatter>() )
            {
                return formatter;
            }
        }

        return nullptr;
    }

private:
    FOnNewActorTrackAdded mNewActorTrackAdded;

    TArray<UNamingFormatter*> mNamingFormatters;
};
