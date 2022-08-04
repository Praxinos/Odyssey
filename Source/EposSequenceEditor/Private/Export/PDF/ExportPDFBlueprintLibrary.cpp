// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/PDF/ExportPDFBlueprintLibrary.h"

#include "ImageUtils.h"
#include "ISequencer.h"
#include "MovieSceneSequence.h"

#include "Board/BoardSequence.h"
#include "Export/PDF/ExportPDFSettings.h"
#include "Export/SceneRenderer.h"
#include "Shot/ShotSequence.h"

#define LOCTEXT_NAMESPACE "ExportPDFBlueprintLibrary"

//---

//static
FText
UExportPDFBlueprintLibrary::GetRootBoardName( const FExportStruct& iExportStruct )
{
    if( !iExportStruct.mSequencer.IsValid() )
        return FText::GetEmpty();

    if( !iExportStruct.mSequencer.Pin()->GetRootMovieSceneSequence() )
        return FText::GetEmpty();

    return iExportStruct.mSequencer.Pin()->GetRootMovieSceneSequence()->GetDisplayName();
}

//static
TArray<FSequenceNameElements>
UExportPDFBlueprintLibrary::GetNameElementsOfPanels( const FExportStruct& iExportStruct )
{
    TArray<FSequenceNameElements> name_elements_list;

    TArray<UShotSequence*> already_used_shot_sequence;

    for( auto& panel : iExportStruct.Panels )
    {
        UShotSequence* shot_sequence = Cast<UShotSequence>( panel.mSequence );
        if( !shot_sequence )
            continue;

        if( already_used_shot_sequence.Contains( shot_sequence ) )
            continue;

        already_used_shot_sequence.Add( shot_sequence );

        name_elements_list.Add( shot_sequence->NameElements );
    }

    return name_elements_list;
}

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
    if( !iExportStruct.Panels.IsValidIndex( iPanelIndex ) )
        return TEXT( "" );

    if( !iExportStruct.mSequencer.IsValid() )
        return TEXT( "" );

    return iExportStruct.mSequencer.Pin()->GetNumericTypeInterface()->ToString( iExportStruct.Panels[iPanelIndex].GlobalFrame.Value );
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

////static
//const UMovieSceneSequence*
//UExportPDFBlueprintLibrary::GetPanelSequence( const FExportStruct& iExportStruct, int32 iPanelIndex )
//{
//    if( !iExportStruct.Panels.IsValidIndex( iPanelIndex ) )
//        return nullptr;
//
//    return iExportStruct.Panels[iPanelIndex].mSequence;
//}

//---

//static
const UTexture2D*
UExportPDFBlueprintLibrary::GetPanelTexture2D( const FExportStruct& iExportStruct, int32 iPanelIndex, int32 iHeight )
{
    if( !iExportStruct.Panels.IsValidIndex( iPanelIndex ) )
        return nullptr;

    if( !iExportStruct.mSequencer.IsValid() )
        return nullptr;

    float aspect_ratio = GetMostRelevantCameraAspectRatio( iExportStruct.mSequencer.Pin().Get(), iExportStruct.Panels[iPanelIndex].mSequence );

    iHeight = ( iHeight <= 0 ) ? 512 : iHeight;
    FIntPoint image_size( iHeight * aspect_ratio, iHeight );

    FSceneRenderer thumbnail_renderer( iExportStruct.mSequencer, &iExportStruct.Panels[iPanelIndex], image_size );
    TArray<FColor> samples;
    thumbnail_renderer.RenderPlane( samples );

    //PATCH: sometimes, the pixels have their alpha to 0 ... (see with Elodie)
    for( int i = 0; i < samples.Num(); i++ )
        samples[i].A = 255;

    //---

    TArray64<uint8> samples8;
    FImageUtils::PNGCompressImageArray( image_size.X, image_size.Y, samples, samples8 );

    UTexture2D* texture = FImageUtils::ImportBufferAsTexture2D( samples8 );
    texture->SRGB = false;
    texture->UpdateResource();

    return texture;

    //--- or

    //FString texture_name = TEXT( "panel-thumbnail-" ) + FString::FormatAsNumber( iPanelIndex );

    //FCreateTexture2DParameters params;
    //UTexture2D* texture = FImageUtils::CreateTexture2D( image_size.X, image_size.Y, samples, GetTransientPackage(), texture_name, RF_Transient, params );
    //texture->UpdateResource();
    ////texture->AddToRoot();

    //return texture;
}

//---

//static
TArray<UStoryNote*>
UExportPDFBlueprintLibrary::GetPanelNotes( const FExportStruct& iExportStruct, int32 iPanelIndex )
{
    TArray<UStoryNote*> notes;

    if( !iExportStruct.Panels.IsValidIndex( iPanelIndex ) )
        return notes;

    if( !iExportStruct.mSequencer.IsValid() )
        return notes;

    return EposSequenceHelpers::GetNotesRecursive( iExportStruct.mSequencer.Pin()->GetRootMovieSceneSequence(), iExportStruct.Panels[iPanelIndex].GlobalFrame );
}

//---

#undef LOCTEXT_NAMESPACE
