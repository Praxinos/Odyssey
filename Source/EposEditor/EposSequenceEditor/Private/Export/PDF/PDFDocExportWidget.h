// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "Export/ExportStruct.h"

#include "PDFDocExportWidget.generated.h"

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
 * Base class for pdf doc
 */
UCLASS(Blueprintable, Abstract)
class EPOSSEQUENCEEDITOR_API UPDFDocExportWidget
    : public UUserWidget
{
public:
    GENERATED_BODY()

    /**
    * Called to build the pdf layout from the panel struct.
    *
    * @param    PanelList       The structure which contains all the panels to construct the layout.
    * @param    Preview         To know if it's for the preview in the export panel or the final pdf file.
    */
    UFUNCTION( BlueprintImplementableEvent, Category="Odyssey|Storyboard Editor|PDF Doc" )
    void OnConstructPDFLayout( const FExportStruct& PanelList, bool Preview );

    /**
    * Get the number of page in the pdf file
    *
    * @return The list of page to export
    */
    UFUNCTION( BlueprintImplementableEvent, BlueprintCallable, Category="Odyssey|Storyboard Editor|PDF Doc" )
    int32 GetTotalPDFPageNumber();

    /**
    * Get the current pdf page number
    *
    * @return The current page number
    */
    UFUNCTION( BlueprintImplementableEvent, BlueprintCallable, Category="Odyssey|Storyboard Editor|PDF Doc" )
    int32 GetCurrentPDFPageNumber();

    /**
    * Handle the given page in the pdf file (always between 1 and GetTotalPDFPageNumber())
    *
    * @param The page in the pdf file
    */
    UFUNCTION( BlueprintImplementableEvent, BlueprintCallable, Category="Odyssey|Storyboard Editor|PDF Doc" )
    void HandlePDFPageNumber( int32 PDFPageNumber );

    /**
    * Get the format of the given page
    *
    * @param The page in the pdf file
    * @return The page format
    */
    UFUNCTION( BlueprintImplementableEvent, BlueprintCallable, Category="Odyssey|Storyboard Editor|PDF Doc" )
    EPDFPageFormat GetPDFPageFormat( int32 PDFPageNumber );

    /**
    * Get the orientation of the given page
    *
    * @param The page in the pdf file
    * @return The page orientation
    */
    UFUNCTION( BlueprintImplementableEvent, BlueprintCallable, Category="Odyssey|Storyboard Editor|PDF Doc" )
    EPDFPageOrientation GetPDFPageOrientation( int32 PDFPageNumber );
};
