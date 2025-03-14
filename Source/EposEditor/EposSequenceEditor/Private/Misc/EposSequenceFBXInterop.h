// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2024

#pragma once

#include "ISequencer.h"

// In 5.4, the level sequence class has been moved to Editor/MovieSceneTools/LevelSequenceFBXInterop
// Now, it should be possible to use directly it
// Because it is now public
// The only modification is the title of the export panel which says "Level Sequence"
// Otherwise, the mecanic should be ok for all MovieSceneSequence subtype
//
// The USequencerExportTask should be useable here but it doesn't
// The class is not flagged as dll exportable...
//
// So, the 2 classes (FLevelSequenceFBXInterop and USequencerExportTask) must be rewrite
// Or directly use the FLevelSequenceFBXInterop inside the board/shot customization

class FEposSequenceFBXInterop
{
public:

    FEposSequenceFBXInterop(TSharedRef<ISequencer> InSequencer);

    /** Imports the animation from an fbx file. */
    void ImportFBX();
    void ImportFBXOntoSelectedNodes();

    /** Exports the animation to an fbx file. */
    void ExportFBX();

private:

    /** Exports sequence to a FBX file */
    void ExportFBXInternal(const FString& ExportFilename, const TArray<FGuid>& Bindings, const TArray<UMovieSceneTrack*>& Tracks);

private:

    TWeakPtr<ISequencer> WeakSequencer;
};
