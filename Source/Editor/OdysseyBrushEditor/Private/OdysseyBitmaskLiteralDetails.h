// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Types/SlateEnums.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class UK2Node_BitmaskLiteral;

/** Details customization for the "Make Bitmask Literal" node */
class FBitmaskLiteralDetails : public IDetailCustomization
{
public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<class IDetailCustomization> MakeInstance()
    {
        return MakeShareable(new FBitmaskLiteralDetails);
    }

    FBitmaskLiteralDetails()
        : TargetNode(NULL)
    {
    }

    /** IDetailCustomization interface */
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

protected:
    TSharedPtr<FString> GetBitmaskEnumTypeName() const;
    void OnBitmaskEnumTypeChanged(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo);

private:
    /** The target node */
    UK2Node_BitmaskLiteral* TargetNode;

    /** Map of enum types used as bitmasks */
    TMap<FName, UEnum*> BitmaskEnumTypeMap;

    /** Array of enum type names used by the UI */
    TArray<TSharedPtr<FString>> BitmaskEnumTypeNames;
};
