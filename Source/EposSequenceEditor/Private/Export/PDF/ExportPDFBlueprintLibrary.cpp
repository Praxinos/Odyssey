// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/PDF/ExportPDFBlueprintLibrary.h"

#include "MovieSceneSequence.h"

#define LOCTEXT_NAMESPACE "ExportPDFBlueprintLibrary"

//---

//static
int32
UExportPDFBlueprintLibrary::GetNumberOfPanels( const FExportStruct& iExportStruct )
{
    return iExportStruct.Panels.Num();
}

//static
FString
UExportPDFBlueprintLibrary::GetPanelFrameFormatted( const FExportStruct& iExportStruct, int32 iPanelIndex )
{
    return TEXT( "" );
}

//static
FFrameNumber
UExportPDFBlueprintLibrary::GetPanelFrame( const FExportStruct& iExportStruct, int32 iPanelIndex )
{
    if( !iExportStruct.Panels.IsValidIndex( iPanelIndex ) )
        return FFrameNumber();

    return iExportStruct.Panels[iPanelIndex].GlobalFrame;
}

//static
FText
UExportPDFBlueprintLibrary::GetPanelShotName( const FExportStruct& iExportStruct, int32 iPanelIndex )
{
    if( !iExportStruct.Panels.IsValidIndex( iPanelIndex ) )
        return FText::GetEmpty();

    return iExportStruct.Panels[iPanelIndex].mSequence->GetDisplayName();
}

//---

#undef LOCTEXT_NAMESPACE
