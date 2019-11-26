// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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
