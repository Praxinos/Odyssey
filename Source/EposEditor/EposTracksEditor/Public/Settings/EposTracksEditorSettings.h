// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "CineCameraComponent.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/Object.h"

#include "EposTracksEditorSettings.generated.h"

//---

USTRUCT( BlueprintType )
struct FCameraSettings
{
    GENERATED_BODY()

public:
    /** Controls the filmback of the camera. */
    UPROPERTY( config, EditAnywhere, BlueprintReadWrite, Category = Camera )
    FCameraFilmbackSettings Filmback;

    /** Controls the camera's lens. */
    UPROPERTY( config, EditAnywhere, BlueprintReadWrite, Category = Camera )
    FCameraLensSettings LensSettings;

    /** Controls the camera's focus. */
    UPROPERTY( config, EditAnywhere, BlueprintReadWrite, Category = Camera )
    FCameraFocusSettings FocusSettings;

    /** Current focal length of the camera (i.e. controls FoV, zoom) */
    UPROPERTY( config, EditAnywhere, BlueprintReadWrite, Category = Camera )
    float CurrentFocalLength = -1; // Compute and set in UpdateValues()

    /** Current aperture, in terms of f-stop (e.g. 2.8 for f/2.8) */
    UPROPERTY( config, EditAnywhere, BlueprintReadWrite, Category = Camera )
    float CurrentAperture = -1; // Compute and set in UpdateValues()

#if WITH_EDITORONLY_DATA
    /** Read-only. Control this value with CurrentFocalLength (and filmback settings). */
    UPROPERTY( config, VisibleAnywhere, Category = Camera )
    float CurrentHorizontalFOV = -1; // Compute and set in UpdateValues()
#endif
};

//---

USTRUCT( BlueprintType )
struct FPlaneSettings
{
    GENERATED_BODY()

    /** This will make the plane bigger than the original size viewed by the camera by adding a margin to the plane */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Plane", meta=(UIMin = "0", ClampMin = "0", UIMax = "200", ClampMax = "200", Units=Percent))
    float SafeMargin { 0.f };

    /** This will rescale the original size of the plane
      * The original size is the one viewed by the camera
      * The scale is applied before safe margin
      * The default value (which means no rescale) is (100%, 100%)
      */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Plane", meta=(AllowPreserveRatio, DisplayName="Relative Scaling (%)"))
    FVector2D RelativeScaling { 100.f, 100.f };
};

//---

USTRUCT( BlueprintType )
struct FTextureSettings
{
    GENERATED_BODY()

    /** Controls the height (a multiple of 4) of the texture (drawing). (Its width is computed from the plane ratio) */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Texture", meta=(ClampMin="16", ClampMax="4096", Multiple="4", DisplayName="Height (px)"))
    int32 Height { 1080 };
};

//---

USTRUCT( BlueprintType )
struct FBoardSectionSettings
{
    GENERATED_BODY()

public:
    /** Select the background color of board sections. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=BoardTrack)
    FLinearColor BoardSectionColor { .94f, .39f, .6f, .25f };

    /** Select the background color of shot sections. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=BoardTrack)
    FLinearColor ShotSectionColor { .6f, .39f, .94f, .25f };
};

//---

/**
 * Epos Tracks Editor settings.
 */
UCLASS(BlueprintType, config=Odyssey, meta=(DisplayName="Epos Tracks Editor"))
class EPOSTRACKSEDITOR_API UEposTracksEditorSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

    //~ UDeveloperSettings Interface
    virtual FName GetContainerName() const override;
    virtual FName GetCategoryName() const override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif
    //~

public:
    void SetDefaultSectionDuration( float iDuration );

protected:
    //~ UObject Interface
    virtual void PostInitProperties() override;
    //~

private:
    UFUNCTION( BlueprintCallable, Category=Settings )
    void UpdateValues();

public:
    /** Specifies Camera stuff. (UpdateValues() must be called after camera modifications) */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ShowOnlyInnerProperties))
    FCameraSettings CameraSettings;

    /** Specifies Plane stuff. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ShowOnlyInnerProperties))
    FPlaneSettings PlaneSettings;

    /** Specifies Texture stuff. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ShowOnlyInnerProperties))
    FTextureSettings TextureSettings;

    /** The default duration for new section in seconds. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=BoardTrack, meta=(ClampMin=0.1f, Units=s))
    float DefaultSectionDuration { 3 };

    /** Specifies Board track stuff. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ShowOnlyInnerProperties))
    FBoardSectionSettings BoardTrackSettings;
};
