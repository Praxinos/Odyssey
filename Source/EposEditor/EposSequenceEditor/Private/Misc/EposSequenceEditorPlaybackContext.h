// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Templates/SharedPointer.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakInterfacePtr.h"

class IMovieScenePlaybackClient;
class UEposMovieSceneSequence;

#define UE_API EPOSSEQUENCEEDITOR_API

/**
 * Class that manages the current UWorld context that an epos-sequence editor should use for playback
 */
class FEposSequenceEditorPlaybackContext
    : public TSharedFromThis<FEposSequenceEditorPlaybackContext>
{
public:

    UE_API FEposSequenceEditorPlaybackContext( UEposMovieSceneSequence* iEposSequence );
    UE_API ~FEposSequenceEditorPlaybackContext();

    /**
     * Gets the level sequence for which we are trying to find the context.
     */
    UE_API UEposMovieSceneSequence* GetEposSequence() const;

    /**
     * Build a world picker widget that allows the user to choose a world, and exit the auto-bind settings
     */
    //UE_API TSharedRef<SWidget> BuildWorldPickerCombo();

    /**
     * Resolve the current world context pointer. Can never be nullptr.
     */
    UE_API UObject* GetPlaybackContext() const;

    /**
     * Returns GetPlaybackContext as a plain object.
     */
    UE_API UObject* GetPlaybackContextAsObject() const;

    /**
     * Resolve the current playback client. May be nullptr.
     */
    UE_API UObject* GetPlaybackClientAsUObject() const;

    /**
     * Returns GetPlaybackClient as an interface pointer.
     */
    UE_API IMovieScenePlaybackClient* GetPlaybackClientAsInterface() const;

    /**
     * Retrieve all the event contexts for the current world
     */
    //UE_API TArray<UObject*> GetEventContexts() const;

    /**
     * Specify a new world to use as the context. Persists until the next PIE or map change event.
     * May be null, in which case the context will be recomputed automatically
     */
    //UE_API void OverrideWith( UWorld* InNewContext, IMovieScenePlaybackClient* InNewClient );

private:

    using FContextAndClient = TTuple<UWorld*, IMovieScenePlaybackClient*>;

    /**
     * Compute the new playback context based on the user's current auto-bind settings.
     * Will use the first encountered PIE or Simulate world if possible, else the Editor world as a fallback
     */
    static UE_API FContextAndClient ComputePlaybackContextAndClient( const UEposMovieSceneSequence* iEposSequence );

    /**
     * Update the cached context and client pointers if needed.
     */
    UE_API void UpdateCachedContextAndClient() const;

    /**
     * Gets both the context and client.
     */
     //TTuple<UWorld*, IMovieScenePlaybackClient*>
    UE_API FContextAndClient GetPlaybackContextAndClient() const;

    UE_API void OnPieEvent( bool );
    UE_API void OnMapChange( uint32 );
    UE_API void OnWorldListChanged( UWorld* );

private:

    /** Level sequence that we should find a context for */
    TWeakObjectPtr<UEposMovieSceneSequence> EposSequence;

    /** Mutable cached context pointer */
    mutable TWeakObjectPtr<UWorld> WeakCurrentContext;

    /** Mutable cached client pointer */
    mutable TWeakInterfacePtr<IMovieScenePlaybackClient> WeakCurrentClient;
};

#undef UE_API
