// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2024

#pragma once

#include "ISequencer.h"

class FEposSequenceFBXInterop
{
public:

    FEposSequenceFBXInterop(TSharedPtr<ISequencer> InSequencer);

    /** Imports the animation from an fbx file. */
    void ImportFBX();
    void ImportFBXOntoSelectedNodes();

    /** Exports the animation to an fbx file. */
    void ExportFBX();

private:

    /** Exports sequence to a FBX file */
    void ExportFBXInternal(const FString& ExportFilename, const TArray<FGuid>& Bindings, const TArray<UMovieSceneTrack*>& Tracks);

private:

    TSharedPtr<ISequencer> Sequencer;
};
