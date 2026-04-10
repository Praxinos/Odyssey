// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Brush, meta = (AllowPrivateAccess = "true", DisplayThumbnail = "true", DisplayName = "Default Brush", AllowedClasses="/Script/OdysseyBrush.OdysseyBrush"))
    TSoftObjectPtr<UOdysseyBrush> DefaultBrush;
};

USTRUCT( BlueprintType )
struct ODYSSEYPAINTEREDITOR_API FCheckboardPreset
{
    GENERATED_USTRUCT_BODY()

    /** The name of the preset */
    UPROPERTY( config, EditAnywhere, Category = CheckboardPreset )
    FString Name = TEXT( "Preset" );

    /** The color one of the checkboard */
    UPROPERTY( config, EditAnywhere, Category = CheckboardPreset )
    FColor ColorOne = FColor( 255, 255, 255 );

    /** The color two of the checkboard */
    UPROPERTY( config, EditAnywhere, Category = CheckboardPreset )
    FColor ColorTwo = FColor( 247, 247, 247 );

    /** The size of the checkboard */
    UPROPERTY( config, EditAnywhere, Category = CheckboardPreset )
    int32 Size = 16;
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
    static UOdysseyPainterEditorSettings* Get();

public:
#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif

public:
    void SetInteractiveMode();
    void RemoveInteractiveMode();
private:
    bool bIsInteractiveMode = false;

public:
    DECLARE_MULTICAST_DELEGATE( FOnBackgroundColorChanged );
    DECLARE_MULTICAST_DELEGATE( FOnCheckerColorChanged );
    DECLARE_MULTICAST_DELEGATE( FOnCheckerSizeChanged );

    /** Gets the current viewport background color. */
    FColor GetBackgroundColor() const;
    /** Sets the current viewport background color. */
    void SetBackgroundColor( FColor BackgroundColor );
    /** Gets the multicast delegate which is run whenever the viewport background color is changed. */
    FOnBackgroundColorChanged& GetOnBackgroundColorChanged();

    /** Gets the current (canvas) checker background color one. */
    FColor GetCheckerColorOne() const;
    /** Gets the current (canvas) checker background color two. */
    FColor GetCheckerColorTwo() const;
    /** Sets the current (canvas) checker background color one. */
    void SetCheckerColorOne( FColor CheckerColorOne );
    /** Sets the current (canvas) checker background color two. */
    void SetCheckerColorTwo( FColor CheckerColorTwo );
    /** Sets the current (canvas) checker background color one & two. */
    void SetCheckerColor( FColor CheckerColorOne, FColor CheckerColorTwo );
    /** Gets the multicast delegate which is run whenever the (canvas) checker background color is changed. */
    FOnBackgroundColorChanged& GetOnCheckerColorChanged();

    /** Gets the current (canvas) checker size. */
    int32 GetCheckerSize() const;
    /** Sets the current (canvas) checker size. */
    void SetCheckerSize( int32 CheckerSize );
    /** Gets the multicast delegate which is run whenever the (canvas) checker size is changed. */
    FOnCheckerSizeChanged& GetOnCheckerSizeChanged();

    /** Gets the current (canvas) checker presets. */
    TArray<FCheckboardPreset> GetCheckerPresets() const;

public:
    /** The type of background to draw in the texture editor view port. */
    UPROPERTY(config)
    TEnumAsByte<EOdysseyPainterEditorBackgrounds> Background;

    /** The type of display when viewing volume textures. */
    UPROPERTY(config)
    TEnumAsByte<EOdysseyPainterEditorVolumeViewMode> VolumeViewMode;

private:
    /** Background and foreground color used by Texture preview view ports. */
    UPROPERTY(config, EditAnywhere, Category=Background)
    FColor BackgroundColor;

    /** The first color of the checkered background. */
    UPROPERTY(config, EditAnywhere, Category=Background)
    FColor CheckerColorOne;

    /** The second color of the checkered background. */
    UPROPERTY(config, EditAnywhere, Category=Background)
    FColor CheckerColorTwo;

    /** The size of the checkered background tiles.
    *   It is a power of 2 (like 2, 4, 8, 16, 32, 64, ...)
    */
    UPROPERTY(config, EditAnywhere, Category=Background, meta=(ClampMin="2", ClampMax="512"))
    int32 CheckerSize;

    /** A list of checkered background presets. */
    UPROPERTY(config, EditAnywhere, Category=Background)
    TArray<FCheckboardPreset> CheckerPresets;

private:
    FOnBackgroundColorChanged OnBackgroundColorChangedEvent;
    FOnCheckerColorChanged OnCheckerColorChangedEvent;
    FOnCheckerSizeChanged OnCheckerSizeChangedEvent;

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
