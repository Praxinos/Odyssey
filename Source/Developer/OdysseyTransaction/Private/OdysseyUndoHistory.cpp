// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyUndoHistory.h"
#include "OdysseyTransaction.h"

#define LOCTEXT_NAMESPACE "OdysseyUndoHistory"

//---

FOdysseyUndoHistory::FOdysseyUndoHistory() :
    mUndoTransactionsList()
    , mIsRecording( false )
    , mCurrentTransactionIndex( -1 )
{
}

FOdysseyUndoHistory::~FOdysseyUndoHistory()
{
    for( int i = mUndoTransactionsList.Num() - 1; i >= 0; i-- )
    {
        if( mUndoTransactionsList[i].IsValid() )
            mUndoTransactionsList[i].Reset();
    }

    mUndoTransactionsList.Empty();
}

//---

void
FOdysseyUndoHistory::Record( FName iName )
{
    if( mIsRecording )
        return;

    mIsRecording = true;
    mCurrentTransactionIndex++;
    mUndoTransactionsList.Add( MakeShareable( new FOdysseyTransaction( iName, mCurrentTransactionIndex ) ) );
}

void
FOdysseyUndoHistory::EndRecord()
{
    if( !mIsRecording )
        return;

    mUndoTransactionsList.Last()->SealModificationsAfterRecord();
    mIsRecording = false;
}

bool
FOdysseyUndoHistory::IsRecording()
{
    return mIsRecording;
}

//---

FOdysseyTransaction*
FOdysseyUndoHistory::GetCurrentTransaction()
{
    checkf( ( mUndoTransactionsList.Num() > mCurrentTransactionIndex ) && ( mCurrentTransactionIndex >= 0 ), TEXT( "Out of bounds index of transaction in the Undo History" ) );

    return mUndoTransactionsList[mCurrentTransactionIndex].Get();
}

int
FOdysseyUndoHistory::GetCurrentTransactionIndex()
{
    return mCurrentTransactionIndex;
}

TArray<TSharedPtr<FOdysseyTransaction>>*
FOdysseyUndoHistory::GetTransactionsList()
{
    return &mUndoTransactionsList;
}

void
FOdysseyUndoHistory::SetCurrentTransactionIndex( int iIndex )
{
    if( mUndoTransactionsList.Num() > iIndex && iIndex >= 0 )
        mCurrentTransactionIndex = iIndex;
}

//---

void
FOdysseyUndoHistory::DeleteTransactionsAfterIndex( int iIndex )
{
    for( int i = mUndoTransactionsList.Num() - 1; i > iIndex; i-- )
    {
        mUndoTransactionsList[i].Reset();
        mUndoTransactionsList.Pop( /* bool AllowShrinking */ true );
    }
}

void
FOdysseyUndoHistory::DeleteAllTransactions()
{
    for( int i = mUndoTransactionsList.Num() - 1; i >= 0; i-- )
    {
        mUndoTransactionsList[i].Reset();
        mUndoTransactionsList.Pop( /* bool AllowShrinking */ true );
    }

    mCurrentTransactionIndex = -1;
}

void
FOdysseyUndoHistory::TransactionBetweenIndexes( int iStartIndex, int iEndIndex )
{
    //UE_LOG(LogTemp, Display, TEXT("InStartIndex %d, InEndIndex %d"), InStartIndex, InEndIndex );

    bool undo = iStartIndex > iEndIndex ? true : false;
    int step = undo ? -1 : 1;

    int currentIndex = iStartIndex;
    while( currentIndex != iEndIndex )
    {
        undo ? mUndoTransactionsList[currentIndex]->Undo() : mUndoTransactionsList[currentIndex + step]->Redo();
        currentIndex += step;
    }
}

#undef LOCTEXT_NAMESPACE
