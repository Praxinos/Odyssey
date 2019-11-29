// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyBlockUndoable.h"

UOdysseyBlockUndoable::~UOdysseyBlockUndoable()
{
    mArray.Empty();
}

UOdysseyBlockUndoable::UOdysseyBlockUndoable()
    : mArray()
{
    this->SetFlags( RF_Transactional );
    /*testSave = 8;
    testLoad = 2;
    
    
    FString SavePath = "/Users/praxinos/Documents/TestSave/MySave.save";
    mToBinary << testSave;

    FFileHelper::SaveArrayToFile( mToBinary, *SavePath );
    
	TArray<uint8> TheBinaryArray;
    FFileHelper::LoadFileToArray(TheBinaryArray, *SavePath);
    
    FMemoryReader FromBinary = FMemoryReader(TheBinaryArray, true); //true, free data after done
    
    FromBinary << testLoad;
    
    UE_LOG(LogTemp, Display, TEXT("%d"), testLoad);*/
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
TArray< uint8 >&
UOdysseyBlockUndoable::GetArray()
{
    return mArray;
}

void
UOdysseyBlockUndoable::SetArray( uint8* iStart, int iSize )
{
    mArray.Empty();
    for( int i = 0; i < iSize; i++ )
        mArray.Add( *(iStart + ( sizeof(uint8) * i )) );
}

//static
UOdysseyBlockUndoable*
UOdysseyBlockUndoable::CreateTransient()
{
	UOdysseyBlockUndoable* newBlock = NULL;

    newBlock = NewObject<UOdysseyBlockUndoable>(
        GetTransientPackage(),
        NAME_None,
        RF_Transient | RF_Transactional
        );

    newBlock->mArray = TArray<uint8>();

	return newBlock;
}
