// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class FOdysseyTransaction;

/**
 * Implements the undo history model
 */
class ODYSSEYTRANSACTION_API FOdysseyUndoHistory
{
public:
    // Construction / Destruction
    FOdysseyUndoHistory();
    ~FOdysseyUndoHistory();

public:
    void Record( FName InName );
    void EndRecord();
    bool IsRecording();

    FOdysseyTransaction* GetCurrentTransaction();
    int GetCurrentTransactionIndex();
    TArray<TSharedPtr<FOdysseyTransaction>>* GetTransactionsList();
    void SetCurrentTransactionIndex( int iIndex );

    void DeleteTransactionsAfterIndex( int iIndex );
    void DeleteAllTransactions();

    void TransactionBetweenIndexes( int iStartIndex, int iEndIndex );

private:
    TArray<TSharedPtr<FOdysseyTransaction>> mUndoTransactionsList;
    bool mIsRecording;
    int  mCurrentTransactionIndex;
};
