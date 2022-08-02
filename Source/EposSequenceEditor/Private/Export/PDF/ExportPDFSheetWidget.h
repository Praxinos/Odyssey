// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "Export/ExportStruct.h"

#include "ExportPDFSheetWidget.generated.h"

UENUM()
enum class EPDFPageFormat : uint8
{
    A4,
};

UENUM()
enum class EPDFPageOrientation : uint8
{
    Landscape,
    Portrait,
};


/**
 * Base class for pdf sheets
 */
UCLASS(Blueprintable, Abstract)
class EPOSSEQUENCEEDITOR_API UExportPDFSheetWidget
    : public UUserWidget
{
public:
    GENERATED_BODY()

    /**
    * Called to build the pdf layout from the panel struct.
    * @param    PanelList       The structure which contains all the panels to construct the layout.
    * @param    Preview         To know if it's for the preview in the export panel or the final pdf file.
    */
    UFUNCTION( BlueprintImplementableEvent )
    void OnConstructPDFLayout( const FExportStruct& PanelList, bool Preview );

    /**
    * Get the number of page in the pdf file
    * @return The list of page to export
    */
    UFUNCTION( BlueprintImplementableEvent, BlueprintCallable )
    int32 GetTotalPDFPageNumber();

    /**
    * Handle the given page in the pdf file (always between 1 and GetTotalPDFPageNumber())
    * @param The page in the pdf file
    */
    UFUNCTION( BlueprintImplementableEvent, BlueprintCallable )
    void HandlePDFPageNumber( int32 PDFPageNumber );
    
    /**
    * Get the format of the given page
    * @param The page in the pdf file
    * @return The page format
    */
    UFUNCTION( BlueprintImplementableEvent, BlueprintCallable )
    EPDFPageFormat GetPDFPageFormat( int32 PDFPageNumber );
    
    /**
    * Get the orientation of the given page
    * @param The page in the pdf file
    * @return The page orientation
    */
    UFUNCTION( BlueprintImplementableEvent, BlueprintCallable )
    EPDFPageOrientation GetPDFPageOrientation( int32 PDFPageNumber );
};
