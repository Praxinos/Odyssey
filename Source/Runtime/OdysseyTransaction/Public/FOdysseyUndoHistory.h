// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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
    void Record(FName InName);
    void EndRecord();
    FOdysseyTransaction* GetCurrentTransaction();
    int GetCurrentTransactionIndex();
    bool IsRecording();
    TArray<TSharedPtr<FOdysseyTransaction>>* GetTransactionsList();
    void SetCurrentTransactionIndex( int InIndex );

    void DeleteTransactionsAfterIndex( int InIndex );
    void DeleteAllTransactions();

    void TransactionBetweenIndexes( int InStartIndex, int InEndIndex );

private:
    TArray<TSharedPtr<FOdysseyTransaction>> UndoTransactionsList;
    bool bIsRecording;
    int CurrentTransactionIndex;
};
