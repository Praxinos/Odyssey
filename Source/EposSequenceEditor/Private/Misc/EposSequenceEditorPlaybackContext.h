// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Templates/SharedPointer.h"
#include "UObject/WeakObjectPtr.h"

class UEposMovieSceneSequence;
class UWorld;

/**
 * Class that manages the current UWorld context that a level-sequence editor should use for playback
 */
class FEposSequenceEditorPlaybackContext
    : public TSharedFromThis<FEposSequenceEditorPlaybackContext>
{
public:

    FEposSequenceEditorPlaybackContext( UEposMovieSceneSequence* iEposSequence );
    ~FEposSequenceEditorPlaybackContext();

    /**
     * Gets the level sequence for which we are trying to find the context.
     */
    UEposMovieSceneSequence* GetEposSequence() const;

    /**
     * Resolve the current world context pointer. Can never be nullptr.
     */
    UWorld* GetPlaybackContext() const;

    /**
     * Returns GetPlaybackContext as a plain object.
     */
    UObject* GetPlaybackContextAsObject() const;

private:

    using FContextAndClient = TTuple<UWorld*, void*>;
    //using FContextAndClient = TTuple<UWorld*, ALevelSequenceActor*>;

    /**
     * Compute the new playback context based on the user's current auto-bind settings.
     * Will use the first encountered PIE or Simulate world if possible, else the Editor world as a fallback
     */
    static FContextAndClient ComputePlaybackContextAndClient( const UEposMovieSceneSequence* InLevelSequence );

    /**
     * Update the cached context and client pointers if needed.
     */
    void UpdateCachedContextAndClient() const;

private:

    /** Level sequence that we should find a context for */
    TWeakObjectPtr<UEposMovieSceneSequence> mEposSequence;

    /** Mutable cached context pointer */
    mutable TWeakObjectPtr<UWorld> mWeakCurrentContext;

    /** Mutable cached client pointer */
    //mutable TWeakObjectPtr<ALevelSequenceActor> mWeakCurrentClient;
};
