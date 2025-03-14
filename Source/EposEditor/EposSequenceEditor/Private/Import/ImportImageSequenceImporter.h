// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Import/ImportImageSequenceSettings.h"
#include "Import/ImportImageSequenceStruct.h"

//---

class UBoardSequence;

class FImportImageSequenceImporter
{
public:
    FImportImageSequenceImporter( const FImportImageSequenceOptions& iOptions, FString& oErrorMessage );

    const FImportImageSequenceStruct& GetImageSequenceStruct() const;

private:
    void Build( const FPatternKeywordLists& iPatternKeywordLists, FString& oErrorMessage );

private:
    FString mImageSequencePath;
    FString mImageSequenceFilePattern;

    FImportImageSequenceStruct    mImageSequenceStruct;
};
