// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Import/ImageSequenceStruct.h"

//---

class ISequencer;
class UBoardSequence;
class UShotSequence;

class FImageSequenceConverter
{
public:
    FImageSequenceConverter( const FImageSequenceStruct* iImageSequenceStruct, TWeakPtr<ISequencer> iSequencer, UBoardSequence* ioBoardSequence );

private:
    void Convert();

    void CreateBoardsRecursive( const TArray<FImageSequenceBoard>& iBoards, UBoardSequence* ioParentBoardSequence );
    void CreateBoard( const FImageSequenceBoard& iBoard, UBoardSequence* ioParentBoardSequence );
    void CreateShotsRecursive( const TArray<FImageSequenceShot>& iShots, UBoardSequence* ioParentBoardSequence );
    void CreateShot( const FImageSequenceShot& iShot, UBoardSequence* ioParentBoardSequence );
    void CreateDrawings( const TArray<FImageSequenceFrame>& iFrames, UMovieSceneSubSection* iSubSection );

    int32 GetDuration( const FImageSequenceBoard& iBoard ) const;
    int32 GetDuration( const FImageSequenceShot& iShot ) const;

    int32 ConvertFromDisplayRateToTickResolution( int32 iDuration ) const;

private:
    TWeakPtr<ISequencer>        mSequencer;

    const FImageSequenceStruct* mImageSequenceStruct;

    UBoardSequence*             mBoardSequence;
};
