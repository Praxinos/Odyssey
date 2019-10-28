// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "FOdysseyTransaction.h"

#define LOCTEXT_NAMESPACE "FOdysseyTransaction"

FOdysseyTransaction::FOdysseyTransaction(FName InName, int InQueueIndex ) :
    Modifications(),
    Name(InName),
    QueueIndex( InQueueIndex )
{
}


FOdysseyTransaction::~FOdysseyTransaction()
{
}

void FOdysseyTransaction::AppendState( IOdysseyModification* InState )
{
    Modifications.Add(InState);
}

FName FOdysseyTransaction::GetName()
{
    return Name;
}

int FOdysseyTransaction::GetQueueIndex()
{
    return QueueIndex;
}

void FOdysseyTransaction::SealModificationsAfterRecord()
{
    for ( int i = 0; i < Modifications.Num(); i++)
        Modifications[i]->SealModification();
}

void FOdysseyTransaction::Undo()
{
    for ( int i = 0; i < Modifications.Num(); i++)
        Modifications[i]->Undo();
}

void FOdysseyTransaction::Redo()
{
    for ( int i = 0; i < Modifications.Num(); i++)
        Modifications[i]->Redo();
}



#undef LOCTEXT_NAMESPACE
