// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Misc/FrameRate.h"

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimation.h"

#include "OdysseyAnimationSettings.generated.h"

USTRUCT( BlueprintType )
struct FOdysseyAnimationSettings
{
    GENERATED_BODY()

public:
    /** Controls the width (a multiple of 4) of the animation. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Animation Asset", meta=(ClampMin="16", ClampMax="8192", UIMin="16", UIMax="8192", Multiple="4", DisplayName="Width (px)"))
    int32 Width = 1920;

    /** Controls the height (a multiple of 4) of the animation.
      * (Its width is computed from the camera ratio when it's available)
      */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Animation Asset", meta=(ClampMin="16", ClampMax="8192", UIMin="16", UIMax="8192", Multiple="4", DisplayName="Height (px)") )
    int32 Height = 1080;

    /** The pixel format of the animation. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Animation Asset")
    EOdysseyAnimationFormat Format = EOdysseyAnimationFormat::BGRA8;

    /** The framerate of the animation. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Animation Asset" )
    FFrameRate FrameRate = FFrameRate( 24.f, 1.f );

    /** The background color of the animation. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Animation Asset")
    bool AddLayerBackground = false;

    /** The background color of the animation. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Animation Asset", meta=(EditCondition="AddLayerBackground", EditConditionHides))
    FLinearColor LayerBackgroundColor = FLinearColor::White;

    /** The default layer type of the animation. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Animation Asset")
    TSubclassOf<class UOdysseyAnimationLayer> DefaultLayerClass = UOdysseyAnimationLayerImageRaster::StaticClass();
};
