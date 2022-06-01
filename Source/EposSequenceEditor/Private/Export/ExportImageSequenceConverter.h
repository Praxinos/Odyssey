// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Export/ExportImageSequenceStruct.h"

//---

class ISequencer;
class UBoardSequence;
class UEposMovieSceneSequence;
class UShotSequence;

class FExportImageSequenceConverter
{
public:
    FExportImageSequenceConverter( TWeakPtr<ISequencer> iSequencer, const UMovieSceneSequence* iRootSequence, FExportImageSequenceStruct* oImageSequenceStruct );

private:
    void Convert();

private:
    TWeakPtr<ISequencer>            mSequencer;

    FExportImageSequenceStruct*     mImageSequenceStruct;

    const UMovieSceneSequence*      mRootSequence;
};
