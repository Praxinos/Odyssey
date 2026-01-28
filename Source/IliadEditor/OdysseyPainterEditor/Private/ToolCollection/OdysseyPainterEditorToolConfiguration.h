// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyPainterEditorToolConfiguration.generated.h"

UENUM()
enum class EToolIconSource : uint8
{
    None,
    Style,
    Texture
};

USTRUCT()
struct FIconToolConfiguration
{
    GENERATED_BODY()

public:
    const FSlateBrush* MakeIconBrush() const;

public:
    UPROPERTY(EditAnywhere)
    EToolIconSource mIconSource = EToolIconSource::None;

    // Style icon
    UPROPERTY(EditAnywhere)
    FName mIconStyleSet = FName();

    // Texture icon
    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> mIconTexture = nullptr;

    // Tint
    UPROPERTY(EditAnywhere)
    FLinearColor mIconTint = FLinearColor::White;
};


/**
 * Odyssey Tool Configuration
 * Stores a tool properties and such, so that they can be saved and loaded
 */
UCLASS(BlueprintType)
class UOdysseyPainterEditorToolConfiguration : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Tool")
    TSubclassOf<UObject> mToolClass;

    UPROPERTY(EditAnywhere, Category = "Tool")
    TObjectPtr<UOdysseyPainterEditorTool> mTool;

public:
    UPROPERTY(EditAnywhere, Category = "Tool")
    FIconToolConfiguration mIconToolConfiguration;

public:
    UPROPERTY(EditAnywhere)
    FSlateBrush mBrush;
};
