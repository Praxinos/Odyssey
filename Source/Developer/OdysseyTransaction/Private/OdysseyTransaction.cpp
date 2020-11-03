// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTransaction.h"

#define LOCTEXT_NAMESPACE "OdysseyTransaction"

//---

FOdysseyTransaction::FOdysseyTransaction( FName iName, int iQueueIndex ) :
    mModifications()
    , mName( iName )
    , mQueueIndex( iQueueIndex )
{
}

FOdysseyTransaction::~FOdysseyTransaction()
{
}

//---

void
FOdysseyTransaction::AppendState( IOdysseyModification* iState )
{
    mModifications.Add( iState );
}

FName
FOdysseyTransaction::GetName()
{
    return mName;
}

int
FOdysseyTransaction::GetQueueIndex()
{
    return mQueueIndex;
}

void
FOdysseyTransaction::SealModificationsAfterRecord()
{
    for( int i = 0; i < mModifications.Num(); i++ )
        mModifications[i]->SealModification();
}

void
FOdysseyTransaction::Undo()
{
    for( int i = 0; i < mModifications.Num(); i++ )
        mModifications[i]->Undo();
}

void
FOdysseyTransaction::Redo()
{
    for( int i = 0; i < mModifications.Num(); i++ )
        mModifications[i]->Redo();
}

#undef LOCTEXT_NAMESPACE
