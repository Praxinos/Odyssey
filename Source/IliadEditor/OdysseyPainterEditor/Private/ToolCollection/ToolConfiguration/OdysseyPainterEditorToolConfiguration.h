// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UObject/ObjectMacros.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "StructUtils/InstancedStruct.h"
#include "OdysseyPainterEditorToolConfiguration.generated.h"

USTRUCT()
struct FToolPropertySnapshot
{
    GENERATED_BODY()

    // One entry per property
    UPROPERTY()
    TMap<FName, FInstancedStruct> Values;

    bool operator==(const FToolPropertySnapshot& iOther) const;
    bool operator!=(const FToolPropertySnapshot& iOther) const
    {
        return !(*this == iOther);
    }
};

/**
 * Odyssey Tool Configuration
 * Stores a tool properties and such, so that they can be saved and loaded
 */
UCLASS(BlueprintType)
class UOdysseyPainterEditorToolConfiguration : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UObject> mToolClass;

    UPROPERTY()
    FToolPropertySnapshot mSnapshot;

    UPROPERTY(EditDefaultsOnly, Category = "Tool")
    FSlateBrush mIcon;
};
