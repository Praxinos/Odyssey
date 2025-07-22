// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Export/PDF/ExportPDFBlueprintLibrary.h"

#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneSequenceHierarchy.h"
#include "ImageUtils.h"
//#include "ImageWriteTask.h"
#include "ISequencer.h"
#include "MovieSceneSequence.h"
#include "MovieSceneTimeHelpers.h"

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

    if( !iExportStruct.Panels.Num() )
        return FText::GetEmpty();

    UEposMovieSceneSequence* root_epos_sequence = EposSequenceHelpers::GetRootEposSequence( *iExportStruct.mSequencer.Pin().Get(), iExportStruct.Panels[0].mSequenceId );

    if( !root_epos_sequence )
        return FText::GetEmpty();

    return root_epos_sequence->GetDisplayName();
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
int32
UExportPDFBlueprintLibrary::GetPanelDuration( const FExportStruct& iExportStruct, int32 iPanelIndex )
{
    if( !iExportStruct.Panels.IsValidIndex( iPanelIndex ) )
        return 0;

    TRange<FFrameNumber> range;

    if( iPanelIndex == iExportStruct.Panels.Num() - 1 )
    {
        UMovieSceneSequence* sequence = iExportStruct.mSequencer.Pin()->GetRootMovieSceneSequence();
        UMovieScene* moviescene = sequence->GetMovieScene();
        TRange<FFrameNumber> total_range = moviescene->GetPlaybackRange();

        if( !total_range.Contains( iExportStruct.Panels[iPanelIndex].GlobalFrame ) )
            return 0;

        range = UE::MovieScene::MakeDiscreteRange( iExportStruct.Panels[iPanelIndex].GlobalFrame, UE::MovieScene::DiscreteExclusiveUpper( total_range ) );
    }
    else
    {
        range = UE::MovieScene::MakeDiscreteRange( iExportStruct.Panels[iPanelIndex].GlobalFrame, iExportStruct.Panels[iPanelIndex + 1].GlobalFrame );
    }

    return UE::MovieScene::DiscreteSize( range );
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
UExportPDFBlueprintLibrary::GetPanelTexture2D( const FExportStruct& iExportStruct, int32 iPanelIndex, int32 iHeight, EViewModeIndex iViewMode )
{
    if( !iExportStruct.Panels.IsValidIndex( iPanelIndex ) )
        return nullptr;

    if( !iExportStruct.mSequencer.IsValid() )
        return nullptr;

    float aspect_ratio = GetMostRelevantCameraAspectRatio( iExportStruct.mSequencer.Pin().Get(), iExportStruct.Panels[iPanelIndex].mSequenceId );

    iHeight = ( iHeight <= 0 ) ? 512 : iHeight;
    FIntPoint image_size( iHeight * aspect_ratio, iHeight );

    FSceneRenderer thumbnail_renderer( iExportStruct.mSequencer, &iExportStruct.Panels[iPanelIndex], image_size, iViewMode );
    TArray<FColor> samples;
    thumbnail_renderer.RenderPlane( samples );

    //PATCH: sometimes, the pixels have their alpha to 0 ... (see with Elodie)
    for( int i = 0; i < samples.Num(); i++ )
        samples[i].A = 255;

    //---

    // For testing real png image
    //FString name = FString::FormatAsNumber( iPanelIndex );

    //TUniquePtr<FImageWriteTask> ImageTask = MakeUnique<FImageWriteTask>();
    //ImageTask->Format = EImageFormat::PNG;
    //ImageTask->CompressionQuality = 100;
    //ImageTask->Filename = TEXT( "C:/Users/Mike/Documents/Unreal Projects/dev_50_epos/Plugins/Odyssey/samples/export-" ) + name;
    //ImageTask->PixelData = MakeUnique<TImagePixelData<FColor>>( image_size, TArray64<FColor>( samples ) );
    //ImageTask->PixelPreProcessors.Add( TAsyncAlphaWrite<FColor>( 255 ) );

    //ImageTask->RunTask();

    //---

    TArray64<uint8> samples8;
    FImageUtils::PNGCompressImageArray( image_size.X, image_size.Y, samples, samples8 );

    UTexture2D* texture = FImageUtils::ImportBufferAsTexture2D( samples8 );
    //texture->SRGB = false;
    //texture->UpdateResource();

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

    ISequencer* sequencer = iExportStruct.mSequencer.Pin().Get();

    FMovieSceneSequenceID root_epos_sequence_id;
    UEposMovieSceneSequence* root_epos_sequence = EposSequenceHelpers::GetRootEposSequence( *sequencer, iExportStruct.Panels[iPanelIndex].mSequenceId, root_epos_sequence_id );
    if( !root_epos_sequence )
        return notes;

    FMovieSceneSequenceTransform transform_to_sequence;
    if( root_epos_sequence_id != MovieSceneSequenceID::Root )
    {
        const FMovieSceneSequenceHierarchy* hierarchy = sequencer->GetEvaluationTemplate().GetHierarchy();
        const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( root_epos_sequence_id );
        if( !subdata )
            return notes;

        transform_to_sequence = subdata->RootToSequenceTransform;
    }

    FFrameTime local_time = iExportStruct.Panels[iPanelIndex].GlobalFrame * transform_to_sequence;

    return EposSequenceHelpers::GetNotesRecursive( root_epos_sequence, local_time.GetFrame() );
}

//---

#undef LOCTEXT_NAMESPACE
