// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FOdysseyModification.h"

/**
 * Describes a transaction by stocking a serie of states of items
 */
class ODYSSEYTRANSACTION_API FOdysseyTransaction
{
public:
    // Construction / Destruction
    FOdysseyTransaction(FName InName, int InQueueIndex);
    ~FOdysseyTransaction();

public:
    void AppendState( IOdysseyModification* InState );
    FName GetName();
    int GetQueueIndex();

    /* Get the last bits of information at the end of a record to be able to undo/redo this transaction */
    void SealModificationsAfterRecord();

    void Undo();
    void Redo();

private:
    TArray<IOdysseyModification*> Modifications;
    FName Name;
    int QueueIndex;
};
