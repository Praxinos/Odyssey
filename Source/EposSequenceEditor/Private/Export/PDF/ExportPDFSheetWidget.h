// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "Export/ExportStruct.h"

#include "ExportPDFSheetWidget.generated.h"

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
    * @param    PanelStruct     The structure which contains all the panels to construct the layout.
    */
    UFUNCTION( BlueprintImplementableEvent )
    void OnConstructPDFLayout( const FExportStruct& PanelStruct, bool Preview );
    
    /**
    * Called to get a list of page to export (first page is 1)
    * @return The list of page to export
    */
    UFUNCTION( BlueprintImplementableEvent )
    TArray<int32> GetPDFPageList();

    /**
    * Set the current page to export (first page is 1)
    * @param The current page to export
    */
    UFUNCTION( BlueprintImplementableEvent, BlueprintCallable )
    bool SetCurrentPDFPage( int32 CurrentPage );
};
