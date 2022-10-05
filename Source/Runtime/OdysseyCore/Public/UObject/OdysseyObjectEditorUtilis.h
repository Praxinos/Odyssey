// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
	
namespace FObjectEditorUtils
{
	/**
	 * Set the value on an UObject using reflection.
	 * @param	Object			The object to copy the value into.
	 * @param	PropertyName	The name of the property to set.
	 * @param	Value			The value to assign to the property.
	 *
	 * @return true if the value was set correctly
	 */
	template <typename ObjectType, typename ValueType>
	bool SetPropertyValue(ObjectType* Object, FName PropertyName, ValueType Value, EPropertyChangeType::Type iChangeType)
	{
		// Get the property addresses for the source and destination objects.
		FProperty* Property = FindFieldChecked<FProperty>(ObjectType::StaticClass(), PropertyName);

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
	}
};