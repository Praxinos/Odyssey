// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "ObjectEditorUtils.h"

namespace FOdysseyObjectEditorUtils
{
    void ODYSSEYCOREEDITOR_API PreChangePropertyValue(UObject* Object, FName PropertyName);
    void ODYSSEYCOREEDITOR_API PostChangePropertyValue(UObject* Object, FName PropertyName, EPropertyChangeType::Type iChangeType = EPropertyChangeType::Unspecified);

    /**
     * Set the value on an UObject using reflection.
     * @param    Object            The object to copy the value into.
     * @param    PropertyName    The name of the property to set.
     * @param    Value            The value to assign to the property.
     *
     * @return true if the value was set correctly
     */
    template <typename ValueType>
    bool SetPropertyValue(UObject* Object, FName PropertyName, ValueType Value, EPropertyChangeType::Type iChangeType = EPropertyChangeType::Unspecified)
    {
#if WITH_EDITOR
        // Get the property addresses for the source and destination objects.
        FProperty* Property = FindFieldChecked<FProperty>(Object->GetClass(), PropertyName);

        // Get the property addresses for the object
        ValueType* SourceAddr = Property->ContainerPtrToValuePtr<ValueType>(Object);

        if ( SourceAddr == NULL )
        {
            return false;
        }

        if ( !Object->HasAnyFlags(RF_ClassDefaultObject) )
        {
            FEditPropertyChain PropertyChain;
            PropertyChain.AddHead(Property);

            PropertyChain.SetActivePropertyNode(Property);
            PropertyChain.SetActiveMemberPropertyNode(Property);

            Object->Modify();
            Object->PreEditChange(PropertyChain);
        }

        // Set the value on the destination object.
        *SourceAddr = Value;

        if ( !Object->HasAnyFlags(RF_ClassDefaultObject) )
        {
            FPropertyChangedEvent PropertyEvent(Property, iChangeType);
            Object->PostEditChangeProperty(PropertyEvent);
        }

        return true;
#else
        return false;
#endif
    }

    /**
     * Get the value on an UObject using reflection.
     * @param    Object            The object to copy the value into.
     * @param    PropertyName    The name of the property to set.
     * @param    Value            The value to assign to the property.
     *
     * @return true if the value was set correctly
     */
    template <typename ValueType>
    bool GetPropertyValue(UObject* Object, FName PropertyName, ValueType& oValue)
    {
        // Get the property addresses for the source and destination objects.
        FProperty* Property = FindFieldChecked<FProperty>(Object->GetClass(), PropertyName);

        // Get the property addresses for the object
        ValueType* SourceAddr = Property->ContainerPtrToValuePtr<ValueType>(Object);

        if ( SourceAddr == nullptr )
            return false;

        // Set the value on the destination object.
        oValue = *SourceAddr;

        return true;
    }

    /**
     * Returns wether the given object contains the given property
     *
     * @return true if the poroperty exists within the given object
     */
    bool ODYSSEYCOREEDITOR_API HasProperty(UObject* Object, FName PropertyName);
};
