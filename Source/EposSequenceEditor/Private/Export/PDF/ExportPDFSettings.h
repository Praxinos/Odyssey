// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "IImageWrapper.h"

#include "Export/ExportSettings.h"

#include "ExportPDFSettings.generated.h"

//---

USTRUCT()
struct FExportPDFOptions
{
    GENERATED_BODY()

public:
    /** Storyboard path. */
    UPROPERTY( EditAnywhere, Category=ExportPDF )
    FDirectoryPath ExportPath;

    /** Storyboard name. */
    UPROPERTY( EditAnywhere, Category=ExportPDF )
    FString ExportFile;

    UPROPERTY( EditAnywhere, Category=ExportPDF, meta=(MetaClass="ExportPDFSheetWidget") )
    FSoftClassPath SheetClassPath { TEXT( "/Epos/WBP_PDFSheet_Landscape_3x2.WBP_PDFSheet_Landscape_3x2_C" ) };

    UPROPERTY( EditAnywhere, Category=ExportPDF )
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
