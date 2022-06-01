// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "IImageWrapper.h"

#include "ExportImageSequenceSettings.generated.h"

//---

UENUM()
enum class EExportImageSequencePatternKeyword : uint8
{
    PanelIndex,
    StoryboardName,
    BoardIndex, // board only in the first level
    ShotIndex,
    PanelFrame,
};

USTRUCT()
struct FExportImageSequencePatternKeyword
{
    GENERATED_BODY()

public:
    EExportImageSequencePatternKeyword mKeywordId;
    FString mKeywordWithBraces;
    FText mHelp;
};

//---

USTRUCT()
struct FExportImageSequenceNumberFormat
{
    GENERATED_BODY()

public:
    /** The number of digits. */
    UPROPERTY(config, EditAnywhere, Category="Number Format", meta=(UIMin = "1", UIMax = "10"))
    uint32 NumDigits { 4 };
};

UENUM()
enum class EExportImageSequenceFileFormat
{
    PNG     = int32( EImageFormat::PNG ),
    JPEG    = int32( EImageFormat::JPEG ),
    BMP     = int32( EImageFormat::BMP ),
    EXR     = int32( EImageFormat::EXR ),
    //TGA     = int32( EImageFormat::TGA ), // Compress() is not implemented
    //TIFF    = int32( EImageFormat::TIFF ), // Compress() is not implemented
};

USTRUCT()
struct FExportImageSequenceOptions
{
    GENERATED_BODY()

public:
    FExportImageSequenceOptions();

public:
    /** Storyboard path. */
    UPROPERTY( EditAnywhere, Category=ExportImageSequence )
    FDirectoryPath ExportPath;

    /** The aspect ratio to respect (should come from the camera).
        This list is hidden in customization.
        If UPROPERTY is empty, there is no access through IPropertyHandle in customization
    */
    UPROPERTY( VisibleAnywhere, Category=ExportImageSequence, Transient )
    float AspectRatio { 1920.f / 1080.f };

    /** Storyboard path. */
    UPROPERTY( EditAnywhere, Category=ExportImageSequence )
    FIntPoint ImageSize { 1920, 1080 };

    /** File format. */
    UPROPERTY( EditAnywhere, Category=ExportImageSequence )
    EExportImageSequenceFileFormat FileFormat { EExportImageSequenceFileFormat::PNG };

    /** Storyboard name. */
    UPROPERTY( EditAnywhere, Category=ExportImageSequence )
    FString Pattern;

    /** List of all keywords.
        This list is hidden in customization.
        If UPROPERTY is empty, there is no access through IPropertyHandle in customization
    */
    UPROPERTY( VisibleAnywhere, Category=ExportImageSequence, Transient )
    TMap<EExportImageSequencePatternKeyword, FExportImageSequencePatternKeyword> PatternKeywords;

    /** The sequence index format. */
    UPROPERTY(EditAnywhere, Category=ExportImageSequence, AdvancedDisplay, meta=(ShowOnlyInnerProperties))
    FExportImageSequenceNumberFormat BoardIndexFormat { 4 };

    /** The shot index format. */
    UPROPERTY(EditAnywhere, Category=ExportImageSequence, AdvancedDisplay, meta=(ShowOnlyInnerProperties))
    FExportImageSequenceNumberFormat ShotIndexFormat { 4 };

    /** The panel index format. */
    UPROPERTY(EditAnywhere, Category=ExportImageSequence, AdvancedDisplay, meta=(ShowOnlyInnerProperties))
    FExportImageSequenceNumberFormat PanelIndexFormat { 4 };
};

//---

// https://udn.unrealengine.com/s/question/0D54z00007eBzPfCAK/makeinstance-of-ipropertytypecustomization-of-my-structure-is-not-called
UCLASS( config=Epos )
class UExportImageSequenceSettings
    : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY( config, EditAnywhere, Category=ExportImageSequence )
    FExportImageSequenceOptions Options;
};

//---

UCLASS( config=Epos )
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
