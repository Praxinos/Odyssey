// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "UObject/OdysseyObjectPropertyTracker.h"
#include "UObject/OdysseyObjectEditorUtils.h"

class IOdysseySinglePropertyTracker
{
public:
    template <class T>
    using TOnChanged = TDelegate<void(const T&)>;

public:
    virtual ~IOdysseySinglePropertyTracker() {}

    virtual void OnChanged() = 0;
    virtual void UpdateValue(UObject* iObject, const FName& iPropertyName) = 0;
};

template <class T>
class TOdysseySinglePropertyTracker : public IOdysseySinglePropertyTracker
{
public:
    TOdysseySinglePropertyTracker(UObject* iObject, const FName& iPropertyName, const IOdysseySinglePropertyTracker::TOnChanged<T>& iOnChanged);

    virtual void OnChanged() override;
    virtual void UpdateValue(UObject* iObject, const FName& iPropertyName) override;

private:
    T mOldValue;
    IOdysseySinglePropertyTracker::TOnChanged<T> mOnChanged;
};

template <class T>
TOdysseySinglePropertyTracker<T>::TOdysseySinglePropertyTracker(UObject* iObject, const FName& iPropertyName, const IOdysseySinglePropertyTracker::TOnChanged<T>& iOnChanged)
    : mOnChanged(iOnChanged)
{
    FOdysseyObjectEditorUtils::GetPropertyValue(iObject, iPropertyName, mOldValue);
}

template <class T>
void
TOdysseySinglePropertyTracker<T>::OnChanged()
{
    mOnChanged.ExecuteIfBound(mOldValue);
}

template <class T>
void
TOdysseySinglePropertyTracker<T>::UpdateValue(UObject* iObject, const FName& iPropertyName)
{
    FOdysseyObjectEditorUtils::GetPropertyValue(iObject, iPropertyName, mOldValue);
}