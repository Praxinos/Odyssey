// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "UObject/OdysseyObjectPropertyTracker.h"
#include "Misc/TransactionObjectEvent.h"

FOdysseyObjectPropertyTracker::~FOdysseyObjectPropertyTracker()
{
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
    FCoreUObjectDelegates::OnObjectTransacted.RemoveAll(this);
}

FOdysseyObjectPropertyTracker::FOdysseyObjectPropertyTracker()
{
}

FOdysseyObjectPropertyTracker::FOdysseyObjectPropertyTracker(UObject* iObject)
    : mObject(iObject)
{
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FOdysseyObjectPropertyTracker::OnObjectPropertyChanged);
    FCoreUObjectDelegates::OnObjectTransacted.AddRaw(this, &FOdysseyObjectPropertyTracker::OnObjectTransacted);
}

void
FOdysseyObjectPropertyTracker::OnObjectPropertyChanged(UObject* iObject, FPropertyChangedEvent& iEvent)
{
    if (iObject != mObject)
        return;

    FName propertyName = iEvent.GetPropertyName();
    IOdysseySinglePropertyTracker** trackerPtr = mTrackers.Find(propertyName);
    if (!trackerPtr)
        return;

    IOdysseySinglePropertyTracker* tracker = *trackerPtr;
    tracker->OnChanged();
    tracker->UpdateValue(mObject, propertyName);
}

void
FOdysseyObjectPropertyTracker::OnObjectTransacted(UObject* iObject, const FTransactionObjectEvent& iEvent)
{
    if (iObject != mObject)
        return;

    if (iEvent.GetEventType() != ETransactionObjectEventType::UndoRedo)
        return;

    const TArray<FName>& propertyNames = iEvent.GetChangedProperties();

    //Call OnChanged first
    for(const FName& propertyName : propertyNames)
    {
        IOdysseySinglePropertyTracker** trackerPtr = mTrackers.Find(propertyName);
        if (!trackerPtr)
            return;

        IOdysseySinglePropertyTracker* tracker = *trackerPtr;
        tracker->OnChanged();
    }

    //Call UpdateValue after all calls at OnChanged has been made
    for(const FName& propertyName : propertyNames)
    {
        IOdysseySinglePropertyTracker** trackerPtr = mTrackers.Find(propertyName);
        if (!trackerPtr)
            return;

        IOdysseySinglePropertyTracker* tracker = *trackerPtr;
        tracker->UpdateValue(mObject, propertyName);
    }
}

void
FOdysseyObjectPropertyTracker::Untrack(const FName& iPropertyName)
{
    mTrackers.Remove(iPropertyName);
}