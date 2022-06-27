// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "PatternKeywordList.h"

#include "NamingConventionSettings.generated.h"

//---

USTRUCT()
struct FNamingConventionNumberFormat
{
    GENERATED_BODY()

public:
    /** The first number. */
    UPROPERTY(config, EditAnywhere, Category="Number Format", meta=(UIMin = "1", UIMax = "100"))
    uint32 StartNumber { 10 };

    /** The default increment. */
    UPROPERTY(config, EditAnywhere, Category="Number Format", meta=(UIMin = "1", UIMax = "100"))
    uint32 Increment { 10 };

    /** The number of digits. */
    UPROPERTY(config, EditAnywhere, Category="Number Format", meta=(UIMin = "1", UIMax = "10"))
    uint32 NumDigits { 4 };
};

//---

enum class ENamingConventionPlanePatternKeyword : uint32
{
    ENUM_UNIQUE_ID( PlaneIndex ),
};

EPOSNAMINGCONVENTION_API const FPatternKeywordList& GetNamingConventionPlanePatternKeywordList();

USTRUCT()
struct FNamingConventionPlane
{
    GENERATED_BODY()

public:
    FNamingConventionPlane();

public:
    /** The plane pattern. */
    UPROPERTY(config, EditAnywhere, Category="Plane")
    FString Pattern;

    FPatternKeywordLists mPatternKeywordLists;

    /** The plane number format. */
    UPROPERTY(config, EditAnywhere, Category="Plane", meta=(ShowOnlyInnerProperties))
    FNamingConventionNumberFormat IndexFormat { 10, 10, 4 };
};

enum class ENamingConventionCameraPatternKeyword : uint32
{
    ENUM_UNIQUE_ID( CameraIndex ),
};

EPOSNAMINGCONVENTION_API const FPatternKeywordList& GetNamingConventionCameraPatternKeywordList();

USTRUCT()
struct FNamingConventionCamera
{
    GENERATED_BODY()

public:
    FNamingConventionCamera();

public:
    /** The camera pattern. */
    UPROPERTY( config, EditAnywhere, Category="Camera" )
    FString Pattern;

    FPatternKeywordLists mPatternKeywordLists;

    /** The camera number format. */
    UPROPERTY(config, EditAnywhere, Category="Camera", meta=(ShowOnlyInnerProperties))
    FNamingConventionNumberFormat IndexFormat { 10, 10, 4 };
};

enum class ENamingConventionCommonPatternKeyword : uint32
{
    ENUM_UNIQUE_ID( StudioName ),
    ENUM_UNIQUE_ID( StudioAcronym ),
    ENUM_UNIQUE_ID( LicenseName ),
    ENUM_UNIQUE_ID( LicenseAcronym ),
    ENUM_UNIQUE_ID( ProductionName ),
    ENUM_UNIQUE_ID( ProductionAcronym ),
    ENUM_UNIQUE_ID( Season ),
    ENUM_UNIQUE_ID( Episode ),
    ENUM_UNIQUE_ID( Part ),
    //ENUM_UNIQUE_ID( DepartmentName ),
    //ENUM_UNIQUE_ID( DepartmentAcronym ),
    ENUM_UNIQUE_ID( Initials ),
};

EPOSNAMINGCONVENTION_API const FPatternKeywordList& GetNamingConventionCommonPatternKeywordList();

enum class ENamingConventionShotPatternKeyword : uint32
{
    ENUM_UNIQUE_ID( ShotIndex ),
    ENUM_UNIQUE_ID( TakeIndex ),
};

EPOSNAMINGCONVENTION_API const FPatternKeywordList& GetNamingConventionShotPatternKeywordList();

USTRUCT()
struct FNamingConventionShot
{
    GENERATED_BODY()

public:
    FNamingConventionShot();

public:
    /** The shot pattern. */
    UPROPERTY( config, EditAnywhere, Category="Shot" )
    FString Pattern;

    FPatternKeywordLists mPatternKeywordLists;

    /** The shot number format. */
    UPROPERTY(config, EditAnywhere, Category="Shot", meta=(ShowOnlyInnerProperties))
    FNamingConventionNumberFormat IndexFormat { 10, 10, 4 };

    /** The take number format. */
    UPROPERTY(config, EditAnywhere, Category="Shot", meta=(ShowOnlyInnerProperties))
    FNamingConventionNumberFormat TakeFormat { 1, 1, 2 };
};

enum class ENamingConventionBoardPatternKeyword : uint32
{
    ENUM_UNIQUE_ID( BoardIndex ),
};

EPOSNAMINGCONVENTION_API const FPatternKeywordList& GetNamingConventionBoardPatternKeywordList();

USTRUCT()
struct FNamingConventionBoard
{
    GENERATED_BODY()

public:
    FNamingConventionBoard();

public:
    /** The camera pattern. */
    UPROPERTY( config, EditAnywhere, Category="Board" )
    FString Pattern;

    FPatternKeywordLists mPatternKeywordLists;

    /** The shot number format. */
    UPROPERTY(config, EditAnywhere, Category="Board", meta=(ShowOnlyInnerProperties))
    FNamingConventionNumberFormat IndexFormat { 10, 10, 4 };
};

USTRUCT()
struct FNamingConventionGlobal
{
    GENERATED_BODY()

public:
    /** The studio name. */
    UPROPERTY( config, EditAnywhere, Category="Global" )
    FString StudioName { TEXT( "MyStudio" ) };
    /** The studio acronym. */
    UPROPERTY( config, EditAnywhere, Category="Global" )
    FString StudioAcronym { TEXT( "MS" ) };

    /** The license name. */
    UPROPERTY( config, EditAnywhere, Category="Global" )
    FString LicenseName;
    /** The license acronym. */
    UPROPERTY( config, EditAnywhere, Category="Global" )
    FString LicenseAcronym;

    /** The production title. */
    UPROPERTY( config, EditAnywhere, Category="Global" )
    FString ProductionName { TEXT( "MyProductionTitle" ) };
    /** The production acronym. */
    UPROPERTY( config, EditAnywhere, Category="Global" )
    FString ProductionAcronym { TEXT( "MPT" ) };

    /** Is it a serie?. */
    UPROPERTY( config, EditAnywhere, Category="Global", meta=(InlineEditConditionToggle) )
    bool IsSerie { false };

    /** The season number. */
    UPROPERTY( config, EditAnywhere, Category="Global", meta=(EditCondition="IsSerie") )
    int32 Season { INDEX_NONE };
    /** The number of digits of season. */
    UPROPERTY( config, EditAnywhere, Category="Global", meta=(EditCondition="IsSerie", UIMin = "1", UIMax = "10") )
    uint32 SeasonNumDigits { 2 };

    /** The episode number. */
    UPROPERTY( config, EditAnywhere, Category="Global", meta=(EditCondition="IsSerie") )
    int32 Episode { INDEX_NONE };
    /** The number of digits of episode. */
    UPROPERTY( config, EditAnywhere, Category="Global", meta=(EditCondition="IsSerie", UIMin = "1", UIMax = "10") )
    uint32 EpisodeNumDigits { 2 };

    /** The part of the production. */
    UPROPERTY( config, EditAnywhere, Category="Global" )
    FString Part;

    ///** The department name. */
    //UPROPERTY( config, EditAnywhere, Category="Global" )
    //FString DepartmentName;
    ///** The department acronym. */
    //UPROPERTY( config, EditAnywhere, Category="Global" )
    //FString DepartmentAcronym;
};

USTRUCT()
struct FNamingConventionUser
{
    GENERATED_BODY()

public:
    ///** The department name. */
    //UPROPERTY( config, EditAnywhere, Category="User" )
    //FString DepartmentName;
    ///** The department acronym. */
    //UPROPERTY( config, EditAnywhere, Category="User" )
    //FString DepartmentAcronym;

    /** The initials of the user. */
    UPROPERTY( config, EditAnywhere, Category="User" )
    FString Initials;
};

//---

// Settings for the naming convention
UCLASS(config=Epos, meta=(DisplayName="Epos Naming Convention"))
class EPOSNAMINGCONVENTION_API UNamingConventionSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    //~ UDeveloperSettings Interface
    virtual FName GetContainerName() const override;
    virtual FName GetCategoryName() const override;

public:
    /** The naming convention for boards. */
    UPROPERTY(config, EditAnywhere, Category="GlobalNamingConvention", meta=(ShowOnlyInnerProperties) )
    FNamingConventionGlobal GlobalNaming;

    /** The naming convention for boards. */
    UPROPERTY(config, EditAnywhere, Category="UserNamingConvention", meta=(ShowOnlyInnerProperties) )
    FNamingConventionUser UserNaming;

    /** The naming convention for boards. */
    UPROPERTY(config, EditAnywhere, Category="BoardNamingConvention", meta=(ShowOnlyInnerProperties) )
    FNamingConventionBoard BoardNaming;

    /** The naming convention for boards. */
    UPROPERTY(config, EditAnywhere, Category="ShotNamingConvention", meta=(ShowOnlyInnerProperties) )
    FNamingConventionShot ShotNaming;

    /** The naming convention for planes. */
    UPROPERTY(config, EditAnywhere, Category="PlaneNamingConvention", meta=(ShowOnlyInnerProperties) )
    FNamingConventionPlane PlaneNaming;

    /** The naming convention for cameras. */
    UPROPERTY(config, EditAnywhere, Category="CameraNamingConvention", meta=(ShowOnlyInnerProperties) )
    FNamingConventionCamera CameraNaming;
};
