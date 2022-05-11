// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Import/ImageSequenceStruct.h"
#include "StoryboardCreationDialog/StoryboardImportImageSequenceSettings.h"

//---

class UBoardSequence;

class FImageSequenceImporter
{
public:
    FImageSequenceImporter( const FStoryboardImportImageSequenceSettings& iSettings, FString& oErrorMessage );

    const FImageSequenceStruct& GetImageSequenceStruct() const;

private:
    void Build( const TMap<EImageSequencePatternKeyword, FImageSequencePatternKeyword>& iKeywords, FString& oErrorMessage );

private:
    FString                 mImageSequencePath;
    FString                 mImageSequenceFilePattern;

    FImageSequenceStruct    mImageSequenceStruct;
};
