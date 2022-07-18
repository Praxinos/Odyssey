// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

class FExportConverter
{
public:
    FExportConverter( TWeakPtr<ISequencer> iSequencer, const UMovieSceneSequence* iRootSequence, const FExportMarkSettings* iMarkSettings, FExportStruct* oStruct );

private:
    void Convert();

private:
    TWeakPtr<ISequencer>            mSequencer;
    const UMovieSceneSequence*      mRootSequence { nullptr };
    const FExportMarkSettings*      mMarkSettings { nullptr };

    FExportStruct*                  mStruct { nullptr };
};
