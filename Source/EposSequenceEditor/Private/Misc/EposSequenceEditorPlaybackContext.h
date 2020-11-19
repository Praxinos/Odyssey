// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Templates/SharedPointer.h"
#include "UObject/WeakObjectPtr.h"

class UWorld;

/**
 * Class that manages the current UWorld context that a level-sequence editor should use for playback
 */
class FEposSequenceEditorPlaybackContext : public TSharedFromThis<FEposSequenceEditorPlaybackContext>
{
public:

    UObject* GetPlaybackContext() const;

private:

    static UWorld* ComputePlaybackContext();

    /** Mutable cached context pointer */
    mutable TWeakObjectPtr<UWorld> mWeakCurrentContext;
};
