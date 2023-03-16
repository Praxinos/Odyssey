// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>

#include "OdysseyAnimationConfiguration.generated.h"

UENUM()
enum EBackgroundColor
{
	kTransparent UMETA(DisplayName = "Transparent"),
	kWhite UMETA(DisplayName = "White"),
	kNormal UMETA(DisplayName = "Purple (127, 127, 255)")
};

UENUM()
enum EFormats
{
	kBGRA8 UMETA(DisplayName = "BGRA 8"),
	kRGBAF UMETA(DisplayName = "RGBA F")
};

USTRUCT()
struct FOdysseyAnimationConfiguration
{
	GENERATED_BODY()

public:
    ::ULIS::eFormat ULISFormat() const
    {
        switch(Format.GetValue())
        {
            case kBGRA8:
            {
                return ::ULIS::Format_BGRA8;
            }
            break;

            case kRGBAF:
            {
                return ::ULIS::Format_RGBAF;
            }
            break;
        }

        check(false); //should not be called
        return ::ULIS::Format_BGRA8;
    }

public:
    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration")
    FName                   Name = "Animation";

    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration", meta = (ClampMin = 1, ClampMax = 8192, UIMin = 1, UIMax = 8192))
    uint32                  Width = 1920;

    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration", meta=(ClampMin=1, ClampMax=8192, UIMin=1, UIMax=8192) )
    uint32                  Height = 1080;

    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration")
    TEnumAsByte<EFormats>   Format = kBGRA8;

    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration", meta=(ClampMin=1, UIMin=1, LinearDeltaSensitivity=1) )
    float                   FramesPerSecond = 24.f;

    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration")
	TEnumAsByte<EBackgroundColor> BackgroundColor = kTransparent;
};