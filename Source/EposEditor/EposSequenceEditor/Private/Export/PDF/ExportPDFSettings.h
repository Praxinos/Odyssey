// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "IImageWrapper.h"

#include "Export/ExportSettings.h"

#include "ExportPDFSettings.generated.h"

//---

class UPDFDocExportWidget;

USTRUCT( BlueprintType )
struct FExportPDFOptions
{
    GENERATED_BODY()

public:
    /** Storyboard path. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportPDF )
    FDirectoryPath ExportPath;

    /** Storyboard name. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportPDF )
    FString ExportFile;

    /**
     * Widget Blueprint used as a layout to export the PDF.
     *
     * For example portrait, landscape or thumbnails layouts.
     */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportPDF )
    TSoftClassPtr<UPDFDocExportWidget> PDFDocWidgetSoftClass { FSoftObjectPath( TEXT( "/Odyssey/PDF/Thumbnails/WBP_PDFDoc_Thumbnails_Landscape.WBP_PDFDoc_Thumbnails_Landscape_C" ) ) };

    /**
     * Marks to use as panels in the PDF.
     *
     * Choose what should be considered as panels to export:
     * - animation cuts
     * - Sequencer marks
     * - the first frame of each shot
     */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportPDF )
    FExportMarkSettings MarkSettings;
};

//---

// https://udn.unrealengine.com/s/question/0D54z00007eBzPfCAK/makeinstance-of-ipropertytypecustomization-of-my-structure-is-not-called
UCLASS( config=Odyssey )
class UExportPDFSettings
    : public UObject
{
    GENERATED_BODY()

public:
    /** Options used to export the storyboard as a PDF file. */
    UPROPERTY( config, EditAnywhere, BlueprintReadWrite, Category=ExportPDF, meta=(ShowOnlyInnerProperties) )
    FExportPDFOptions Options;
};
