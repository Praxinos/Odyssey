// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "Engine/EngineBaseTypes.h"
#include "Export/ExportSettings.h"
#include "IImageWrapper.h"
#include "PatternKeywordList.h"

#include "ExportImageSequenceSettings.generated.h"

//---

enum class EExportImageSequencePatternKeyword : uint32
{
    ENUM_UNIQUE_ID( PanelIndex ),
    ENUM_UNIQUE_ID( StoryboardName ),
    ENUM_UNIQUE_ID( PanelFrame ),
};

const FPatternKeywordList& GetExportImageSequencePatternKeywordList();

UENUM( BlueprintType )
enum class EExportImageSequenceFileFormat : uint8
{
    PNG     = int8( EImageFormat::PNG ),
    JPEG    = int8( EImageFormat::JPEG ),
    BMP     = int8( EImageFormat::BMP ),
    EXR     = int8( EImageFormat::EXR ),
    //TGA     = int8( EImageFormat::TGA ), // Compress() is not implemented
    //TIFF    = int8( EImageFormat::TIFF ), // Compress() is not implemented
};

USTRUCT( BlueprintType )
struct FExportImageSequenceNumberFormat
{
    GENERATED_BODY()

public:
    /** The number of digits. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Number Format", meta=(UIMin = "1", UIMax = "10"))
    int32 NumDigits { 4 };
};

USTRUCT( BlueprintType )
struct FExportImageSequenceOptions
{
    GENERATED_BODY()

public:
    FExportImageSequenceOptions();

public:
    /** Storyboard path. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportImageSequence )
    FDirectoryPath ExportPath;

    /** The aspect ratio to respect (should come from the camera).
        This list is hidden in customization.
        If UPROPERTY is empty, there is no access through IPropertyHandle in customization
    */
    UPROPERTY( VisibleAnywhere, Category=ExportImageSequence, Transient )
    float AspectRatio { 1920.f / 1080.f };

    /** Image size. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportImageSequence )
    FIntPoint ImageSize { 1920, 1080 };

    /** Image viewmode. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportImageSequence, meta=(ValidEnumValues="VMI_Lit, VMI_Unlit, VMI_BrushWireframe, VMI_Wireframe, VMI_LightingOnly") )
    TEnumAsByte<EViewModeIndex> ViewMode = EViewModeIndex::VMI_Lit;

    /** File format. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportImageSequence )
    EExportImageSequenceFileFormat FileFormat { EExportImageSequenceFileFormat::PNG };

    /** Storyboard name. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportImageSequence )
    FString Pattern;

    FPatternKeywordLists mPatternKeywordLists;

    /** The panel index format. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportImageSequence, AdvancedDisplay, meta=(ShowOnlyInnerProperties) )
    FExportImageSequenceNumberFormat PanelIndexFormat { 4 };

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportImageSequence )
    FExportMarkSettings MarkSettings;
};

//---

// https://udn.unrealengine.com/s/question/0D54z00007eBzPfCAK/makeinstance-of-ipropertytypecustomization-of-my-structure-is-not-called
UCLASS( config=Odyssey )
class UExportImageSequenceSettings
    : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY( config, EditAnywhere, BlueprintReadWrite, Category=ExportImageSequence )
    FExportImageSequenceOptions Options;
};

//---

UCLASS( config=Odyssey )
class UExportImageSequenceUISettings
    : public UObject
{
    GENERATED_BODY()

public:
    float GetThumbnailScaleMultiplier() const;
    void SetThumbnailScaleMultiplier( float iMultiplier );

private:
    /** Storyboard path. */
    UPROPERTY( config, EditAnywhere, Category=ExportImageSequence )
    float mThumbnailScaleMultiplier { 100.f /* % */ };
};
