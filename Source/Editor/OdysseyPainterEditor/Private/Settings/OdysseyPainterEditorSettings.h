// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyBrushBlueprint.h"

#include "OdysseyPainterEditorSettings.generated.h"

/**
 * Enumerates background for the texture editor view port.
 */
UENUM()
enum EOdysseyPainterEditorBackgrounds
{
    kOdysseyPainterEditorBackground_SolidColor    UMETA(DisplayName="Solid Color"),
    kOdysseyPainterEditorBackground_Checkered     UMETA(DisplayName="Checkered"),
    kOdysseyPainterEditorBackground_CheckeredFill UMETA(DisplayName="Checkered (Fill)")
};

UENUM()
enum EOdysseyPainterEditorVolumeViewMode
{
    kOdysseyPainterEditorVolumeViewMode_DepthSlices UMETA(DisplayName="Depth Slices"),
    kOdysseyPainterEditorVolumeViewMode_VolumeTrace UMETA(DisplayName="Trace Into Volume"),
};

USTRUCT(BlueprintType)
struct ODYSSEYPAINTEREDITOR_API FBrushDefaults
{
    GENERATED_USTRUCT_BODY()

public:
    FBrushDefaults();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Brush, meta = (AllowPrivateAccess = "true", DisplayThumbnail = "true", DisplayName = "Default Brush", AllowedClasses="OdysseyBrush"))
    UOdysseyBrush* DefaultBrush;
};

/**
 * Implements the Editor's user settings.
 */
UCLASS(config=EditorPerProjectUserSettings)
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorSettings
    : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    /** The type of background to draw in the texture editor view port. */
    UPROPERTY(config)
    TEnumAsByte<EOdysseyPainterEditorBackgrounds> Background;

    /** The type of display when viewing volume textures. */
    UPROPERTY(config)
    TEnumAsByte<EOdysseyPainterEditorVolumeViewMode> VolumeViewMode;

    /** Background and foreground color used by Texture preview view ports. */
    UPROPERTY(config, EditAnywhere, Category=Background)
    FColor BackgroundColor;

    /** The first color of the checkered background. */
    UPROPERTY(config, EditAnywhere, Category=Background)
    FColor CheckerColorOne;

    /** The second color of the checkered background. */
    UPROPERTY(config, EditAnywhere, Category=Background)
    FColor CheckerColorTwo;

    /** The size of the checkered background tiles. */
    UPROPERTY(config, EditAnywhere, Category=Background, meta=(ClampMin="2", ClampMax="4096"))
    int32 CheckerSize;
    
    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FKey Rotation;
    
    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FKey Pan;
    
    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FKey PickColor;

public:
    /** Whether the texture should scale to fit the view port. */
    UPROPERTY(config)
    bool FitToViewport;

    /** Color to use for the texture border, if enabled. */
    UPROPERTY(config, EditAnywhere, Category=TextureBorder)
    FColor TextureBorderColor;

    /** If true, displays a border around the texture. */
    UPROPERTY(config)
    bool TextureBorderEnabled;

    /** Defines the defaults values of the brush being used when opening the editor */
    UPROPERTY(config, EditAnywhere, Category=Defaults )
    FBrushDefaults BrushDefaults;
};
