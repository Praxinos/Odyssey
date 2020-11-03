// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
