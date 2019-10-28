// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "FOdysseyUndoHistory.h"
#include "FOdysseyTransaction.h"

#define LOCTEXT_NAMESPACE "FOdysseyUndoHistory"


// Construction / Destruction------------------------------------------

FOdysseyUndoHistory::FOdysseyUndoHistory() :
    UndoTransactionsList(),
    bIsRecording(false),
    CurrentTransactionIndex(-1)
{
}

FOdysseyUndoHistory::~FOdysseyUndoHistory()
{
    for( int i = UndoTransactionsList.Num() - 1; i >= 0; i--)
    {
        if( UndoTransactionsList[i].IsValid())
             UndoTransactionsList[i].Reset();
    }
    UndoTransactionsList.Empty();
}

void
FOdysseyUndoHistory::Record(FName InName)
{
    if(bIsRecording)
        return;
    else
    {
        bIsRecording = true;
        CurrentTransactionIndex++;
        UndoTransactionsList.Add( MakeShareable( new FOdysseyTransaction( InName, CurrentTransactionIndex ) ) );
    }
}

void
FOdysseyUndoHistory::EndRecord()
{
    if( !bIsRecording )
        return;
    else
    {
        UndoTransactionsList.Last()->SealModificationsAfterRecord();
        bIsRecording = false;
    }
}

bool FOdysseyUndoHistory::IsRecording()
{
    return bIsRecording;
}


FOdysseyTransaction* FOdysseyUndoHistory::GetCurrentTransaction()
{
    checkf((UndoTransactionsList.Num() > CurrentTransactionIndex) && (CurrentTransactionIndex >= 0), TEXT("Out of bounds index of transaction in the Undo History"));
    return UndoTransactionsList[CurrentTransactionIndex].Get();
}

int FOdysseyUndoHistory::GetCurrentTransactionIndex()
{
    return CurrentTransactionIndex;
}


TArray<TSharedPtr<FOdysseyTransaction>>* FOdysseyUndoHistory::GetTransactionsList()
{
    return &UndoTransactionsList;
}

void FOdysseyUndoHistory::SetCurrentTransactionIndex( int InIndex )
{
    if( UndoTransactionsList.Num() > InIndex && InIndex >= 0 )
        CurrentTransactionIndex = InIndex;
}


void FOdysseyUndoHistory::DeleteTransactionsAfterIndex( int InIndex )
{
    for (int i = UndoTransactionsList.Num() - 1; i > InIndex; i-- )
    {
        UndoTransactionsList[i].Reset();
        UndoTransactionsList.Pop( /* bool AllowShrinking */ true );
    }
}

void FOdysseyUndoHistory::DeleteAllTransactions()
{
    for (int i = UndoTransactionsList.Num() - 1; i >= 0; i-- )
    {
        UndoTransactionsList[i].Reset();
        UndoTransactionsList.Pop( /* bool AllowShrinking */ true );
    }

    CurrentTransactionIndex = -1;
}

void FOdysseyUndoHistory::TransactionBetweenIndexes( int InStartIndex, int InEndIndex )
{

    //UE_LOG(LogTemp, Display, TEXT("InStartIndex %d, InEndIndex %d"), InStartIndex, InEndIndex );

    bool undo = InStartIndex > InEndIndex ? true : false;
    int step = undo ? -1 : 1;

    int currentIndex = InStartIndex;
    while (currentIndex != InEndIndex )
    {
        undo ? UndoTransactionsList[currentIndex]->Undo() : UndoTransactionsList[currentIndex + step]->Redo();
        currentIndex += step;
    }
}




#undef LOCTEXT_NAMESPACE
