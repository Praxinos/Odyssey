// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyBrushBlueprint.h"
#include "OdysseyMouseCursor.h"
#include "UObject/TemplateString.h"

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

    /** Step to rotate the canvas in viewport. */
    UPROPERTY(config, EditAnywhere, Category=Viewport)
    float ViewportRotationStep = 15.f;
    //PATCH: (maybe)
    // Until SOdysseyViewport will be in SOdysseyWidgets module, all value stored here (settings in PainterEditor module) must be add/set as SLATE_ATTRIBUTE in SOdysseyWidgets
    // which can lead to maaaany attributes ...
    // A discussion must be done to know if:
    // - we want to keep both disconnected like now, but in this case, SOdysseyViewport MUST BE totally customizable in a GOOD way
    // - or move SOdysseyViewport inside PainterEditor module to have easily access to all those settings here (in this case, remove references in OdysseyVector module)

    /** Step to zoom the canvas in viewport. */
    UPROPERTY(config, EditAnywhere, Category=Viewport)
    float ViewportZoomStep = .1f;
    //PATCH: See comment above

    /** Pattern of the left status bar in viewport
      *
      * {odysseyAnimation:width}: width of the animation
      * {odysseyAnimation:height}: height of the animation
      * {odysseyAnimation:name}: name of the animation
      * {odysseyAnimation:currentLayerName}: current layer name of the animation
      * {odysseyAnimation:framerate}: framerate of the animation
      *
      * {odysseyViewport2d:x}: X coordinate of the cursor in the viewport 2D
      * {odysseyViewport2d:y}: Y coordinate of the cursor in the viewport 2D
      * {odysseyViewport2d:r}: RED component of the pixel under the cursor in the viewport 2D
      * {odysseyViewport2d:g}: GREEN component of the pixel under the cursor in the viewport 2D
      * {odysseyViewport2d:b}: BLUE component of the pixel under the cursor in the viewport 2D
      * {odysseyViewport2d:a}: ALPHA component of the pixel under the cursor in the viewport 2D
      */
    UPROPERTY(config, EditAnywhere, Category=Viewport )
    FTemplateString StatusBarTemplateString = { .Template = TEXT( "{odysseyAnimation:width} x {odysseyAnimation:height} px" ) };
    //PATCH: See comment above
    // Moreover, every tokens are defined in SOdysseyWidgets module -_-
    //
    //TODO: use a customization to replace the FTemplateString default widget generated by this one SNamingTokensEditableTextBox to be able to have auto-complete

    /** Color to use for the texture border, if enabled. */
    UPROPERTY(config, EditAnywhere, Category=TextureBorder)
    FColor TextureBorderColor;

    /** If true, displays a border around the texture. */
    UPROPERTY(config)
    bool TextureBorderEnabled;

private:
     UFUNCTION()
     static TArray<FString> GetCursorOptions();

public:
    /** Defines the defaults values of the brush being used when opening the editor */
    UPROPERTY(config, EditAnywhere, Category=Defaults, meta=(ShowOnlyInnerProperties) )
    FBrushDefaults BrushDefaults;

    /** Defines the default cursor of the tool being used when opening the editor
      *
      * (The painter editor needs to be re-opened)
      */
    UPROPERTY(config, EditAnywhere, Category=Defaults, meta=(GetOptions="GetCursorOptions") )
    FString ToolMouseCursor = TEXT( "Cross" );

public:
     FMouseCursor GetToolMouseCursor() const;
};
