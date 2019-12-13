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
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
TArray64< uint8 >&
UOdysseyBlockUndoable::GetArray()
{
    return mArray;
}

void
UOdysseyBlockUndoable::SetArray( TArray64< uint8 >& iNewArray )
{
    mArray = iNewArray;
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

    newBlock->mArray = TArray64<uint8>();

	return newBlock;
}
