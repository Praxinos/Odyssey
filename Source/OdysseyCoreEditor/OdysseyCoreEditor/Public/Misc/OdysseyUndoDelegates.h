// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include "Misc/ITransaction.h"

class FTransactionObjectEvent;

class ODYSSEYCOREEDITOR_API FOdysseyUndoDelegates
{
public:
    static FOdysseyUndoDelegates& Get();

private:
    FOdysseyUndoDelegates();

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnObjectPostTransacted, const FTransactionObjectEvent&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnAfterUndoRedo, bool /* iIsRedo */);

    TMap<ETransactionStateEventType, FSimpleMulticastDelegate>& OnTransactionStateChanged();
    FSimpleMulticastDelegate& OnBeforeUndoRedo();
    FOnAfterUndoRedo& OnAfterUndoRedo();
    TMap<UObject*, FOnObjectPostTransacted>& OnObjectPostTransacted();

private:
    void HandleTransactionStateChanged(const FTransactionContext& iTransactionContext, ETransactionStateEventType iTransactionState);
    void HandleBeforeUndoRedo(const FTransactionContext& iTransactionContext);
    void HandleAfterUndo(const FTransactionContext& iTransactionContext, bool iSucceeded);
    void HandleAfterRedo(const FTransactionContext& iTransactionContext, bool iSucceeded);
    void HandleObjectPostTransacted(UObject* iObject, const FTransactionObjectEvent& iEvent);

private:
    TMap<ETransactionStateEventType, FSimpleMulticastDelegate> mOnTransactionStateChanged;
    FSimpleMulticastDelegate mOnBeforeUndoRedo;
    FOnAfterUndoRedo mOnAfterUndoRedo;
    TMap<UObject*, FOnObjectPostTransacted> mOnObjectPostTransacted;
};
