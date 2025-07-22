// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "UObject/OdysseyObjectEditorUtils.h"

void
FOdysseyObjectEditorUtils::PreChangePropertyValue(UObject* Object, FName PropertyName)
{
#if WITH_EDITOR
    // Get the property addresses for the source and destination objects.
    FProperty* Property = FindFieldChecked<FProperty>(Object->GetClass(), PropertyName);

    if ( !Object->HasAnyFlags(RF_ClassDefaultObject) )
    {
        FEditPropertyChain PropertyChain;
        PropertyChain.AddHead(Property);

        Object->Modify();
        Object->PreEditChange(PropertyChain);
    }
#endif
}

void
FOdysseyObjectEditorUtils::PostChangePropertyValue(UObject* Object, FName PropertyName, EPropertyChangeType::Type iChangeType)
{
#if WITH_EDITOR
    // Get the property addresses for the source and destination objects.
    FProperty* Property = FindFieldChecked<FProperty>(Object->GetClass(), PropertyName);

    if ( !Object->HasAnyFlags(RF_ClassDefaultObject) )
    {
        FPropertyChangedEvent PropertyEvent(Property, iChangeType);
        Object->PostEditChangeProperty(PropertyEvent);
    }
#endif
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
