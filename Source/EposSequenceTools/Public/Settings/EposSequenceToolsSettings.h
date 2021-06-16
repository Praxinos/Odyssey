// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "CineCameraComponent.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/Object.h"

#include "EposSequenceToolsSettings.generated.h"

//---

USTRUCT()
struct FCameraSettings
{
    GENERATED_BODY()

public:
    /** Controls the filmback of the camera. */
    UPROPERTY( config, EditAnywhere, Category = Camera )
    FCameraFilmbackSettings Filmback;

    /** Controls the camera's lens. */
    UPROPERTY( config, EditAnywhere, Category = Camera )
    FCameraLensSettings LensSettings;

    /** Current focal length of the camera (i.e. controls FoV, zoom) */
    UPROPERTY( config, EditAnywhere, Category = Camera )
    float CurrentFocalLength;

    /** Current aperture, in terms of f-stop (e.g. 2.8 for f/2.8) */
    UPROPERTY( config, EditAnywhere, Category = Camera )
    float CurrentAperture;

#if WITH_EDITORONLY_DATA
    /** Read-only. Control this value with CurrentFocalLength (and filmback settings). */
    UPROPERTY( config, VisibleAnywhere, Category = Camera )
    float CurrentHorizontalFOV;
#endif
};

//---

USTRUCT()
struct FTextureSettings
{
    GENERATED_BODY()

    /** Controls the height (a multiple of 4) of the texture (drawing). (Its width is computed from the camera ratio) */
    UPROPERTY(config, EditAnywhere, Category="Texture", meta=(ClampMin="16", ClampMax="4096", Multiple="4"))
    int32 Height { 1080 };
};

//---

/**
 * Epos Tools settings.
 */
UCLASS(config=Epos, meta=(DisplayName="Epos Tools"))
class EPOSSEQUENCETOOLS_API UEposSequenceToolsSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

    //~ UDeveloperSettings Interface
    virtual FName GetCategoryName() const override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif
    //~

protected:
    //~ UObject Interface
    virtual void PostInitProperties() override;
    //~

private:
    void UpdateValues();

public:
    UPROPERTY(config, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FCameraSettings CameraSettings;

    UPROPERTY(config, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FTextureSettings TextureSettings;
};
