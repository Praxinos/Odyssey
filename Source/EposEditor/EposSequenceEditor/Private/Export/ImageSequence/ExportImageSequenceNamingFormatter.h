// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "MovieSceneSequenceID.h"

//---

struct FExportPanel;
struct FExportImageSequenceOptions;
struct FExportStruct;
class ISequencer;
class UEposMovieSceneSequence;
class UMovieSceneSequence;
class UShotSequence;

class FExportImageSequenceNamingFormatter
{
public:
    FExportImageSequenceNamingFormatter( TWeakPtr<ISequencer> iSequencer, const FExportPanel* iPanel, int32 iIndex, const FExportImageSequenceOptions* iOptions );

    bool FormatName( const FString& iPatternToFormat, FString& oPatternFormatted );

private:
    TTuple<UShotSequence*, FMovieSceneSequenceID>       GetShot() const;
    TTuple<UMovieSceneSequence*, FMovieSceneSequenceID> GetSequence() const;

private:
    TWeakPtr<ISequencer>                mSequencer;
    UEposMovieSceneSequence*            mRootEposSequence { nullptr };
    FMovieSceneSequenceID               mRootEposSequenceId;
    FFrameNumber                        mFrameInRootEposSequence;
    const FExportPanel*                 mCurrentPanel { nullptr };
    int32                               mIndex;
    const FExportImageSequenceOptions*  mImageSequenceOptions { nullptr };
};
