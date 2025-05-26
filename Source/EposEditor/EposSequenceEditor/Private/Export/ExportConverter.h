// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "Export/ExportSettings.h"

#include "Export/ExportStruct.h"

//---

class ISequencer;
class UBoardSequence;
class UEposMovieSceneSequence;
class UShotSequence;
struct FExportMarkSettings;
struct FMovieSceneSequenceTransform;

class FExportConverter
{
public:
    FExportConverter( TWeakPtr<ISequencer> iSequencer, FMovieSceneSequenceIDRef iEposSequenceId, const FExportMarkSettings* iMarkSettings, FExportStruct* oStruct );

private:
    void Convert();

    void ProcessSequencerMarks( UShotSequence& iShotSequence, FMovieSceneSequenceIDRef iSequenceId, const FMovieSceneSequenceTransform& iRootToSequenceTransform, TArray<FExportPanel>& ioPanels ) const;
    void ProcessAnimationCuts( UShotSequence& iShotSequence, FMovieSceneSequenceIDRef iSequenceId, const FMovieSceneSequenceTransform& iRootToSequenceTransform, TArray<FExportPanel>& ioPanels ) const;
    void ProcessFirstShotFrame( UShotSequence& iShotSequence, FMovieSceneSequenceIDRef iSequenceId, const FMovieSceneSequenceTransform& iRootToSequenceTransform, TArray<FExportPanel>& ioPanels ) const;

private:
    TWeakPtr<ISequencer>            mSequencer;
    FMovieSceneSequenceID           mSequenceId;
    const FExportMarkSettings*      mMarkSettings { nullptr };

    FExportStruct*                  mStruct { nullptr };
};
