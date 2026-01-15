// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UObject/ObjectMacros.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyPainterEditorToolConfiguration.generated.h"

/**
 * Odyssey Tool Configuration
 * Stores a tool properties and such, so that they can be saved and loaded
 */
UCLASS(BlueprintType)
class UOdysseyPainterEditorToolConfiguration : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UObject> mToolClass;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UOdysseyPainterEditorTool> mTool;

    UPROPERTY(EditDefaultsOnly, Category = "Tool")
    FSlateBrush mIcon;
};
