// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/OdysseySinglePropertyTracker.h"

/**
 * @brief A helper class that tracks any changes to selected properties of a given object.
 *
 * Tracking a property means that you can be informed that the property has changed,
 * either by a SetPropertyValue call, or via undo/redo.
 *
 * The advantage of this tracker, is that it allows you to know what was the previous value
 * of the tracked property before it changed.
 * When multiple properties are changed at once (on undo for example), you can ask for oldvalues of other properties aswell.
 *
 * Tracking a property will store a copy of this property, so use it only when you need it
 */
class ODYSSEYCORE_API FOdysseyObjectPropertyTracker
{
public:
    ~FOdysseyObjectPropertyTracker();
    FOdysseyObjectPropertyTracker();

    /**
     * @brief Construct a property tracker to track properties changes of the given object
     *
     * @param iObject
     */
    FOdysseyObjectPropertyTracker(UObject* iObject);

public:
    /**
     * @brief Tracks the given property
     *
     * Please ensure the type of the template corresonds to the type of the property
     *
     * @tparam T
     * @param iPropertyName
     */
    template<class T> void Track(const FName& iPropertyName, const IOdysseySinglePropertyTracker::TOnChanged<T>& iOnChanged);

    /**
     * @brief Untracks the given property
     *
     * @param iPropertyName
     */
    void Untrack(const FName& iPropertyName);

private:
    void OnObjectPropertyChanged(UObject* iObject, FPropertyChangedEvent& iEvent);
    void OnObjectTransacted(UObject* iObject, const FTransactionObjectEvent& iEvent);

private:
    UObject* mObject;
    TMap<FName, class IOdysseySinglePropertyTracker*> mTrackers;
};

template<class T>
void
FOdysseyObjectPropertyTracker::Track(const FName& iPropertyName, const IOdysseySinglePropertyTracker::TOnChanged<T>& iOnChanged)
{
    if (!mTrackers.Contains(iPropertyName))
        mTrackers.Add(iPropertyName, new TOdysseySinglePropertyTracker(mObject, iPropertyName, iOnChanged));
}
