// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

//---

struct FExportImageSequencePanel;
struct FExportImageSequenceOptions;
struct FExportImageSequenceStruct;
class ISequencer;
class UMovieSceneSequence;
class UShotSequence;

class FExportImageSequenceNamingFormatter
{
public:
    FExportImageSequenceNamingFormatter( TWeakPtr<ISequencer> iSequencer, const FExportImageSequencePanel* iImageSequencePanel, int32 iIndex, const FExportImageSequenceOptions* iOptions );

    bool FormatName( const FString& iPatternToFormat, FString& oPatternFormatted );

private:
    TTuple<UShotSequence*, FMovieSceneSequenceID>       GetShot() const;
    TTuple<UMovieSceneSequence*, FMovieSceneSequenceID> GetSequence() const;

private:
    TWeakPtr<ISequencer>                mSequencer;
    const FExportImageSequencePanel*    mCurrentPanel { nullptr };
    int32                               mIndex;
    const FExportImageSequenceOptions*  mImageSequenceOptions { nullptr };
};
