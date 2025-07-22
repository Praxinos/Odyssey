// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImageAnchor.generated.h"

UENUM(BlueprintType)
enum class EOdysseyImageAnchor: uint8
{
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
    Custom
};

USTRUCT(BlueprintType)
struct FOdysseyImageAnchor
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Odyssey")
    EOdysseyImageAnchor Position = EOdysseyImageAnchor::TopLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Odyssey", meta = (EditCondition = "Position==EOdysseyImageAnchor::Custom", EditConditionHides))
    FVector2D CustomPosition = FVector2D(0.f, 0.f);

    FVector2D GetPosition(FVector2D iDestination, FVector2D iSourceSize) const;
};
