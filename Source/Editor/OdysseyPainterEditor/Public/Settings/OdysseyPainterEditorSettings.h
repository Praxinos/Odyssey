// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyStylusInputDriver.h"

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

/**
 * Implements the Editor's user settings.
 */
UCLASS(config=EditorPerProjectUserSettings)
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorSettings
    : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    virtual void PostEditChangeProperty( struct FPropertyChangedEvent& iPropertyChangedEvent ) override;
    
    void RefreshStylusInputDriver();

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
    
    /** Color to use for the texture border, if enabled. */
    UPROPERTY(config, EditAnywhere, Category=StylusDriver )
    TEnumAsByte<EOdysseyStylusInputDriver> StylusInputDriver;

    /** If true, displays a border around the texture. */
    UPROPERTY(config)
    bool TextureBorderEnabled;
};
