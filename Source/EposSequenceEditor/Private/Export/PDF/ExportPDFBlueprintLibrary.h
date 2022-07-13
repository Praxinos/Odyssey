// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Export/ExportStruct.h"

#include "ExportPDFBlueprintLibrary.generated.h"

//---

UCLASS()
class /*EPOSSEQUENCEEDITOR_API*/ UExportPDFBlueprintLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /*
     * Get the number of panels
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static int32 GetNumberOfPanels( const FExportStruct& ExportStruct );

    /*
     * Get the frame number of a panel as formatted string
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static FString GetPanelFrameFormatted( const FExportStruct& ExportStruct, int32 PanelIndex );

    /*
     * Get the frame number of a panel
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static FFrameNumber GetPanelFrame( const FExportStruct& ExportStruct, int32 PanelIndex );

    /*
     * Get the shot name of a panel
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static FText GetPanelShotName( const FExportStruct& ExportStruct, int32 PanelIndex );
};
