// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "UObject/OdysseyObjectEditorUtils.h"

void
FOdysseyObjectEditorUtils::PreChangePropertyValue(UObject* Object, FName PropertyName)
{
    // Get the property addresses for the source and destination objects.
    FProperty* Property = FindFieldChecked<FProperty>(Object->GetClass(), PropertyName);

    if ( !Object->HasAnyFlags(RF_ClassDefaultObject) )
    {
        FEditPropertyChain PropertyChain;
        PropertyChain.AddHead(Property);

        Object->Modify();
        Object->PreEditChange(PropertyChain);
    }
}

void
FOdysseyObjectEditorUtils::PostChangePropertyValue(UObject* Object, FName PropertyName, EPropertyChangeType::Type iChangeType)
{
    // Get the property addresses for the source and destination objects.
    FProperty* Property = FindFieldChecked<FProperty>(Object->GetClass(), PropertyName);

    if ( !Object->HasAnyFlags(RF_ClassDefaultObject) )
    {
        FPropertyChangedEvent PropertyEvent(Property, iChangeType);
        Object->PostEditChangeProperty(PropertyEvent);
    }
}

bool
FOdysseyObjectEditorUtils::HasProperty(UObject* Object, FName PropertyName)
{
    // Get the property addresses for the source and destination objects.
    FProperty* Property = FindFieldChecked<FProperty>(Object->GetClass(), PropertyName);

    if ( !Property )
        return false;

    return true;
}
