// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWindow.h"
#include <ULIS>
#include "OdysseyAnimation.h" //For EOdysseyAnimationFormat

#include "SOdysseyAnimationConfigureWindow.generated.h"

class SEnumComboBox;

UENUM()
enum class EOdysseyAnimationBackgroundColor : uint8
{
    Transparent,
    White,
    Normal UMETA(DisplayName = "Purple (127, 127, 255)")
};

UENUM()
enum class EOdysseyAnimationDefaultLayerType : uint8
{
    Raster,
    Vector
};

USTRUCT()
struct ODYSSEYANIMATION_API FOdysseyAnimationConfiguration
{
    GENERATED_BODY()

public:
    ::ULIS::eFormat ULISFormat() const;
    FLinearColor GetBackgroundColor() const;

public:
    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration")
    FName                   Name = "Animation";

    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration", meta = (ClampMin = 1, ClampMax = 8192, UIMin = 1, UIMax = 8192))
    uint32                  Width = 1920;

    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration", meta=(ClampMin=1, ClampMax=8192, UIMin=1, UIMax=8192) )
    uint32                  Height = 1080;

    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration")
    EOdysseyAnimationFormat   Format = EOdysseyAnimationFormat::BGRA8;

    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration", meta=(ClampMin=1, UIMin=1, LinearDeltaSensitivity=1) )
    float                   FramesPerSecond = 24.f;

    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration")
    EOdysseyAnimationBackgroundColor BackgroundColor = EOdysseyAnimationBackgroundColor::Transparent;

    UPROPERTY(EditAnywhere, Category="OdysseyAnimationConfiguration")
    EOdysseyAnimationDefaultLayerType LayerType = EOdysseyAnimationDefaultLayerType::Raster;
};

class ODYSSEYANIMATION_API SOdysseyAnimationConfigureWindow
    : public SWindow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationConfigureWindow)
    {

    }
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs);
    void Construct( const FArguments& iArgs, const FOdysseyAnimationConfiguration& iProperties);

    bool GetWindowAnswer();

public:
    //Getters
    const FOdysseyAnimationConfiguration& GetConfiguration() const;

private:
    //Internal

    FReply OnAccept();
    FReply OnCancel();

private:
    FOdysseyAnimationConfiguration  mConfiguration;
    bool                            mWindowAnswer;
    TSharedPtr< SEnumComboBox >     mFormatComboBox;
};
