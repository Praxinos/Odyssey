// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "Import/ImportImageSequenceStruct.h"

//---

class ISequencer;
class UBoardSequence;
class UMovieSceneSubSection;
class UShotSequence;

class FImportImageSequenceConverter
{
public:
    FImportImageSequenceConverter( const FImportImageSequenceStruct* iImageSequenceStruct, TWeakPtr<ISequencer> iSequencer, UBoardSequence* ioBoardSequence );

private:
    void Convert();

    void CreateBoardsRecursive( const TArray<FImportImageSequenceBoard>& iBoards, UBoardSequence* ioParentBoardSequence );
    void CreateBoard( const FImportImageSequenceBoard& iBoard, UBoardSequence* ioParentBoardSequence );
    void CreateShotsRecursive( const TArray<FImportImageSequenceShot>& iShots, UBoardSequence* ioParentBoardSequence );
    void CreateShot( const FImportImageSequenceShot& iShot, UBoardSequence* ioParentBoardSequence );
    void CreateAnimation( const TArray<FImportImageSequencePanel>& iPanels, UMovieSceneSubSection* iSubSection );

    int32 GetDuration( const FImportImageSequenceBoard& iBoard ) const;
    int32 GetDuration( const FImportImageSequenceShot& iShot ) const;

    int32 ConvertFromDisplayRateToTickResolution( int32 iDuration ) const;

private:
    TWeakPtr<ISequencer>                mSequencer;

    const FImportImageSequenceStruct*   mImageSequenceStruct;

    UBoardSequence*                     mBoardSequence;
};
