// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "NamingConventionSettings.generated.h"

//---

USTRUCT()
struct FNamingConventionPlane
{
    GENERATED_BODY()

public:
    bool CheckPattern();

public:
    /** The plane pattern. */
    UPROPERTY(config, EditAnywhere, Category=Plane)
    FString Pattern { TEXT("plane_{plane-index}") };

    /** The first plane number. */
    UPROPERTY(config, EditAnywhere, Category=Plane, meta=(UIMin = "1", UIMax = "100"))
    uint32 StartNumber { 10 };

    /** The default plane increment. */
    UPROPERTY(config, EditAnywhere, Category=Plane, meta=(UIMin = "1", UIMax = "100"))
    uint32 Increment { 10 };

    /** The number of digits for the plane number. */
    UPROPERTY(config, EditAnywhere, Category=Plane, meta=(UIMin = "1", UIMax = "10"))
    uint32 NumDigits { 4 };
};

USTRUCT()
struct FNamingConventionCamera
{
    GENERATED_BODY()

public:
    /** The camera pattern. */
    UPROPERTY( config, EditAnywhere, Category=Camera )
    FString Pattern { TEXT( "camera_{camera-index}" ) };

    /** The first plane number. */
    UPROPERTY(config, EditAnywhere, Category=Camera, meta=(UIMin = "1", UIMax = "100"))
    uint32 StartNumber { 10 };

    /** The default plane increment. */
    UPROPERTY(config, EditAnywhere, Category=Camera, meta=(UIMin = "1", UIMax = "100"))
    uint32 Increment { 10 };

    /** The number of digits for the plane number. */
    UPROPERTY(config, EditAnywhere, Category=Camera, meta=(UIMin = "1", UIMax = "10"))
    uint32 NumDigits { 4 };
};

//---

// Settings for the naming convention
UCLASS(config=EditorPerProjectUserSettings, meta=(DisplayName="Epos Naming Convention"))
class /*EPOSNAMINGCONVENTION_API*/ UNamingConventionSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    //~ UDeveloperSettings Interface
    virtual FName GetContainerName() const override;
    virtual FName GetCategoryName() const override;

public:
    /** The naming convention for planes. */
    UPROPERTY(config, EditAnywhere, Category=PlaneNamingConvention, meta=(ShowOnlyInnerProperties) )
    FNamingConventionPlane PlaneNaming;

    /** The naming convention for cameras. */
    UPROPERTY(config, EditAnywhere, Category=CameraNamingConvention, meta=(ShowOnlyInnerProperties) )
    FNamingConventionCamera CameraNaming;
};
