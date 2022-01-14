// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "NamingConventionSettings.generated.h"

//---

USTRUCT()
struct FNamingConventionNumberFormat
{
    GENERATED_BODY()

public:
    /** The first number. */
    UPROPERTY(config, EditAnywhere, meta=(UIMin = "1", UIMax = "100"))
    uint32 StartNumber { 10 };

    /** The default increment. */
    UPROPERTY(config, EditAnywhere, meta=(UIMin = "1", UIMax = "100"))
    uint32 Increment { 10 };

    /** The number of digits. */
    UPROPERTY(config, EditAnywhere, meta=(UIMin = "1", UIMax = "10"))
    uint32 NumDigits { 4 };
};

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

    /** The plane number format. */
    UPROPERTY(config, EditAnywhere, Category=Plane, meta=(ShowOnlyInnerProperties))
    FNamingConventionNumberFormat IndexFormat { 10, 10, 4 };
};

USTRUCT()
struct FNamingConventionCamera
{
    GENERATED_BODY()

public:
    /** The camera pattern. */
    UPROPERTY( config, EditAnywhere, Category=Camera )
    FString Pattern { TEXT( "camera_{camera-index}" ) };

    /** The camera number format. */
    UPROPERTY(config, EditAnywhere, Category=Camera, meta=(ShowOnlyInnerProperties))
    FNamingConventionNumberFormat IndexFormat { 10, 10, 4 };
};

USTRUCT()
struct FNamingConventionShot
{
    GENERATED_BODY()

public:
    /** The shot pattern. */
    UPROPERTY( config, EditAnywhere, Category=Shot )
    FString Pattern { TEXT( "shot_{shot-index}" ) };

    /** The shot number format. */
    UPROPERTY(config, EditAnywhere, Category=Shot, meta=(ShowOnlyInnerProperties))
    FNamingConventionNumberFormat IndexFormat { 10, 10, 4 };

    /** The take number format. */
    UPROPERTY(config, EditAnywhere, Category=Shot, meta=(ShowOnlyInnerProperties))
    FNamingConventionNumberFormat TakeFormat { 1, 1, 2 };
};

USTRUCT()
struct FNamingConventionBoard
{
    GENERATED_BODY()

public:
    /** The camera pattern. */
    UPROPERTY( config, EditAnywhere, Category=Board )
    FString Pattern { TEXT( "board_{board-index}" ) };

    /** The shot number format. */
    UPROPERTY(config, EditAnywhere, Category=Board, meta=(ShowOnlyInnerProperties))
    FNamingConventionNumberFormat IndexFormat { 10, 10, 4 };
};

USTRUCT()
struct FNamingConventionGlobal
{
    GENERATED_BODY()

public:
    /** The studio name. */
    UPROPERTY( config, EditAnywhere, Category=Global )
    FString StudioName { TEXT( "MyStudio" ) };

    /** The studio accronym. */
    UPROPERTY( config, EditAnywhere, Category=Global )
    FString StudioAccronym { TEXT( "MS" ) };

    /** The title of the production. */
    UPROPERTY( config, EditAnywhere, Category=Global )
    FString ProductionName { TEXT( "MyProductionTitle" ) };

    /** The accronym of the production. */
    UPROPERTY( config, EditAnywhere, Category=Global )
    FString ProductionAccronym { TEXT( "MPT" ) };

    /** The initials of the user. */
    UPROPERTY( config, EditAnywhere, Category=Global )
    FString Initials { TEXT( "MI" ) };
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
    /** The naming convention for boards. */
    UPROPERTY(config, EditAnywhere, Category=GlobalNamingConvention, meta=(ShowOnlyInnerProperties) )
    FNamingConventionGlobal GlobalNaming;

    /** The naming convention for boards. */
    UPROPERTY(config, EditAnywhere, Category=BoardNamingConvention, meta=(ShowOnlyInnerProperties) )
    FNamingConventionBoard BoardNaming;

    /** The naming convention for boards. */
    UPROPERTY(config, EditAnywhere, Category=ShotNamingConvention, meta=(ShowOnlyInnerProperties) )
    FNamingConventionShot ShotNaming;

    /** The naming convention for planes. */
    UPROPERTY(config, EditAnywhere, Category=PlaneNamingConvention, meta=(ShowOnlyInnerProperties) )
    FNamingConventionPlane PlaneNaming;

    /** The naming convention for cameras. */
    UPROPERTY(config, EditAnywhere, Category=CameraNamingConvention, meta=(ShowOnlyInnerProperties) )
    FNamingConventionCamera CameraNaming;
};
