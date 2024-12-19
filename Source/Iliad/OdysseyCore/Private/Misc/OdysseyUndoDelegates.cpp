// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Misc/OdysseyUndoDelegates.h"
#if WITH_EDITOR
#include "Editor/TransBuffer.h"
#endif

FOdysseyUndoDelegates&
FOdysseyUndoDelegates::Get()
{
    static FOdysseyUndoDelegates delegates;
    return delegates;
}

FOdysseyUndoDelegates::FOdysseyUndoDelegates()
{
#if WITH_EDITOR
    if (GEditor)
    {
        if (UTransBuffer* Transactor = Cast<UTransBuffer>(GEditor->Trans))
        {
            Transactor->OnTransactionStateChanged().AddRaw(this, &FOdysseyUndoDelegates::HandleTransactionStateChanged);
            Transactor->OnBeforeRedoUndo().AddRaw(this, &FOdysseyUndoDelegates::HandleBeforeUndoRedo);
            Transactor->OnRedo().AddRaw(this, &FOdysseyUndoDelegates::HandleAfterUndo);
            Transactor->OnUndo().AddRaw(this, &FOdysseyUndoDelegates::HandleAfterRedo);
        }
    }

    FCoreUObjectDelegates::OnObjectTransacted.AddRaw(this, &FOdysseyUndoDelegates::HandleObjectPostTransacted);
#endif
}

void
FOdysseyUndoDelegates::HandleTransactionStateChanged(const FTransactionContext& iTransactionContext, ETransactionStateEventType iTransactionState)
{
    if (!mOnTransactionStateChanged.Contains(iTransactionState))
        return;

    mOnTransactionStateChanged[iTransactionState].Broadcast();
    mOnTransactionStateChanged[iTransactionState].Clear();
}

void
FOdysseyUndoDelegates::HandleBeforeUndoRedo(const FTransactionContext& iTransactionContext)
{
    mOnBeforeUndoRedo.Broadcast();
    mOnBeforeUndoRedo.Clear();
}

void
FOdysseyUndoDelegates::HandleAfterUndo(const FTransactionContext& iTransactionContext, bool iSucceeded)
{
    mOnAfterUndoRedo.Broadcast(false);
    mOnAfterUndoRedo.Clear();
}

void
FOdysseyUndoDelegates::HandleAfterRedo(const FTransactionContext& iTransactionContext, bool iSucceeded)
{
    mOnAfterUndoRedo.Broadcast(true);
    mOnAfterUndoRedo.Clear();
}

void
FOdysseyUndoDelegates::HandleObjectPostTransacted(UObject* iObject, const FTransactionObjectEvent& iEvent)
{
    if (!mOnObjectPostTransacted.Contains(iObject))
        return;

    mOnObjectPostTransacted[iObject].Broadcast(iEvent);
    mOnObjectPostTransacted[iObject].Clear();
}

TMap<ETransactionStateEventType, FSimpleMulticastDelegate>&
FOdysseyUndoDelegates::OnTransactionStateChanged()
{
    return mOnTransactionStateChanged;
}

FSimpleMulticastDelegate&
FOdysseyUndoDelegates::OnBeforeUndoRedo()
{
    return mOnBeforeUndoRedo;
}

FOdysseyUndoDelegates::FOnAfterUndoRedo&
FOdysseyUndoDelegates::OnAfterUndoRedo()
{
    return mOnAfterUndoRedo;
}

TMap<UObject*, FOdysseyUndoDelegates::FOnObjectPostTransacted>&
FOdysseyUndoDelegates::OnObjectPostTransacted()
{
    return mOnObjectPostTransacted;
}
