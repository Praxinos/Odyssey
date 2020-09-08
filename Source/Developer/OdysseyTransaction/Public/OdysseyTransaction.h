// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "OdysseyModification.h"

/**
 * Describes a transaction by stocking a serie of states of items
 */
class ODYSSEYTRANSACTION_API FOdysseyTransaction
{
public:
    // Construction / Destruction
    FOdysseyTransaction(FName iName, int iQueueIndex);
    ~FOdysseyTransaction();

public:
    void AppendState( IOdysseyModification* iState );

    FName GetName();
    int GetQueueIndex();

    /* Get the last bits of information at the end of a record to be able to undo/redo this transaction */
    void SealModificationsAfterRecord();

    void Undo();
    void Redo();

private:
    TArray<IOdysseyModification*> mModifications;
    FName mName;
    int   mQueueIndex;
};
