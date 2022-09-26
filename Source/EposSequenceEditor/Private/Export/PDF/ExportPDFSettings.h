// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "IImageWrapper.h"

#include "Export/ExportSettings.h"

#include "ExportPDFSettings.generated.h"

//---

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

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportPDF, meta=(MetaClass="PDFDocExportWidget") )
    FSoftClassPath PDFDocWidgetClassPath { TEXT( "/Epos/PDF/Thumbnails/WBP_PDFDoc_Thumbnails_Landscape.WBP_PDFDoc_Thumbnails_Landscape_C" ) };

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ExportPDF )
    FExportMarkSettings MarkSettings;
};

//---

// https://udn.unrealengine.com/s/question/0D54z00007eBzPfCAK/makeinstance-of-ipropertytypecustomization-of-my-structure-is-not-called
UCLASS( config=Epos )
class UExportPDFSettings
    : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY( config, EditAnywhere, Category=ExportPDF, meta=(ShowOnlyInnerProperties) )
    FExportPDFOptions Options;
};
