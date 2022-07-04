// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

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

UENUM()
enum class EExportImageSequenceFileFormat : int8
{
    PNG     = int8( EImageFormat::PNG ),
    JPEG    = int8( EImageFormat::JPEG ),
    BMP     = int8( EImageFormat::BMP ),
    EXR     = int8( EImageFormat::EXR ),
    //TGA     = int8( EImageFormat::TGA ), // Compress() is not implemented
    //TIFF    = int8( EImageFormat::TIFF ), // Compress() is not implemented
};

USTRUCT()
struct FExportImageSequenceNumberFormat
{
    GENERATED_BODY()

public:
    /** The number of digits. */
    UPROPERTY(config, EditAnywhere, Category="Number Format", meta=(UIMin = "1", UIMax = "10"))
    uint32 NumDigits { 4 };
};

USTRUCT()
struct FExportImageSequenceMarkSettings
{
    GENERATED_BODY()

public:
    /** Make a panel when a drawing exists. */
    UPROPERTY(config, EditAnywhere, Category="Mark")
    bool Drawings { true };

    /** Make a panel when a sequencer mark exists. */
    UPROPERTY(config, EditAnywhere, Category="Mark")
    bool Marks { false };

    /** Make a panel always on the first frame of a shot. */
    UPROPERTY(config, EditAnywhere, Category="Mark")
    bool FirstFrameOfShot { false };

    ///** Make a panel every N frames (restart at each shot). */
    //UPROPERTY(config, EditAnywhere, Category="Mark")
    //bool EveryNFrameForEachShot { false };

    ///** Interval (start of each shot). */
    //UPROPERTY(config, EditAnywhere, Category="Mark")
    //int32 IntervalForEachShot;

    ///** Make a panel every N frames (start of root board). */
    //UPROPERTY(config, EditAnywhere, Category="Mark")
    //bool EveryNFrameForRootBoard { false };

    ///** Interval (start of root board). */
    //UPROPERTY(config, EditAnywhere, Category="Mark")
    //int32 IntervalForRootBoard;
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

    FPatternKeywordLists mPatternKeywordLists;

    /** The panel index format. */
    UPROPERTY(EditAnywhere, Category=ExportImageSequence, AdvancedDisplay, meta=(ShowOnlyInnerProperties))
    FExportImageSequenceNumberFormat PanelIndexFormat { 4 };

    UPROPERTY( EditAnywhere, Category=ExportImageSequence )
    FExportImageSequenceMarkSettings MarkSettings;
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
