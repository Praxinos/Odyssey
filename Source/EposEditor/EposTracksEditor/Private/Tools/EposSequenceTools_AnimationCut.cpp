// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/EposSequenceTools.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "EditorAssetLibrary.h"
#include "Engine/Texture2D.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "ISequencer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "Subsystems/AssetEditorSubsystem.h"

#include "Board/BoardSequence.h"
#include "EposSequenceHelpers.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyLayerCell.h"
#include "ScalingComponent.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_AnimationCut"

//static
bool
BoardSequenceTools::CanCreateAnimationCut( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    //FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    //bool can_create = false;
    //for( auto animation_binding : iAnimationBindings )
    //{
    //    FDrawing drawing = ShotSequenceHelpers::GetDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame(), animation_binding );

    //    can_create |= ( !drawing.Exists() );
    //}

    //return can_create;
    return true;
}

//static
bool
BoardSequenceTools::CanCreateAnimationCut( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iAnimationBinding )
{
    return BoardSequenceTools::CanCreateAnimationCut( iSequencer, iSubSection, iFrameNumber, TArray<FGuid>( { iAnimationBinding } ) );
}

//static
bool
BoardSequenceTools::CanCreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    //FDrawing drawing = ShotSequenceHelpers::GetDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iAnimationBinding );

    //return !drawing.Exists();
    return true;
}

//static
bool
ShotSequenceTools::CanCreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding )
{
    //FDrawing drawing = ShotSequenceHelpers::GetDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iAnimationBinding );

    //return !drawing.Exists();
    return true;
}

//---

//static
void
BoardSequenceTools::CreateAnimationCut( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings, const FAnimationCutArgs& iAnimationCutArgs )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    ShotSequenceTools::CreateAnimationCut( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame(), iAnimationBindings, iAnimationCutArgs );
}

//static
void
BoardSequenceTools::CreateAnimationCut( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iAnimationBinding, const FAnimationCutArgs& iAnimationCutArgs )
{
    BoardSequenceTools::CreateAnimationCut( iSequencer, iSubSection, iFrameNumber, TArray<FGuid>( { iAnimationBinding } ), iAnimationCutArgs );
}

//static
void
BoardSequenceTools::CreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings, const FAnimationCutArgs& iAnimationCutArgs )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::CreateAnimationCut( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iAnimationBindings, iAnimationCutArgs );
}

//static
void
BoardSequenceTools::CreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding, const FAnimationCutArgs& iAnimationCutArgs )
{
    BoardSequenceTools::CreateAnimationCut( iSequencer, iFrameNumber, TArray<FGuid>( { iAnimationBinding } ), iAnimationCutArgs );
}

//static
void
ShotSequenceTools::CreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings, const FAnimationCutArgs& iAnimationCutArgs )
{
    CreateAnimationCut( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iAnimationBindings, iAnimationCutArgs );
}

//static
void
ShotSequenceTools::CreateAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding, const FAnimationCutArgs& iAnimationCutArgs )
{
    ShotSequenceTools::CreateAnimationCut( iSequencer, iFrameNumber, TArray<FGuid>( { iAnimationBinding } ), iAnimationCutArgs );
}

//static
void
ShotSequenceTools::CreateAnimationCut( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings, const FAnimationCutArgs& iAnimationCutArgs )
{
    const FScopedTransaction transaction( LOCTEXT( "transaction.create-animationcut", "Create a new animationcut" ) );

    //bool is_creation = false;
    //ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID );

    for( auto animation_binding : iAnimationBindings )
    {
        ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( iSequencer, iSequence, iSequenceID, animation_binding, iFrameNumber );

        for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> section : result.mSections )
        {
            UOdysseyAnimation* animation = section->GetAnimation();
            if( !animation )
                continue;

            FFrameNumber frame_in_timeline = section->ConvertFrameFromSequenceToTimeline( iFrameNumber );

            TArray<UOdysseyLayer*> layers = animation->GetLayerStack()->GetLayers();
            TArray<UOdysseyAnimationLayer*> animation_layers;
            for( UOdysseyLayer* layer : layers )
            {
                UOdysseyAnimationLayer* animation_layer = Cast<UOdysseyAnimationLayer>( layer );
                if( animation_layer )
                    animation_layers.Add( animation_layer );
            }

            for( UOdysseyAnimationLayer* animation_layer : animation_layers )
            {
                UOdysseyLayerCell* cell = animation_layer->GetCellAtFrame( frame_in_timeline.Value );
                if( cell )
                {
                    int32 frame_in_cell = frame_in_timeline.Value - cell->GetFrameRange().GetLowerBoundValue();
                    cell->Break( frame_in_cell, true );
                }
                else
                {
                    // Create a cell before all other cells
                    if( frame_in_timeline < animation_layer->GetFrameRange().GetLowerBoundValue() )
                    {
                        UOdysseyLayerCell* previous_first_cell = animation_layer->GetCells()[0];
                        int32 new_offset = previous_first_cell->GetFrameRange().GetLowerBoundValue() - frame_in_timeline.Value;
                        animation_layer->SetCellsOffset( animation_layer->GetCellsOffset() - new_offset );
                        UOdysseyLayerCell* new_cell = animation_layer->AddCell( animation_layer->GetDefaultCellClass(), 0 );
                        new_cell->SetExposure( new_offset );
                    }
                    // Create a cell after all other cells
                    else
                    {
                        UOdysseyLayerCell* previous_last_cell = animation_layer->GetCells().Last();
                        int32 new_offset = frame_in_timeline.Value - previous_last_cell->GetFrameRange().GetUpperBoundValue();
                        previous_last_cell->SetExposure( previous_last_cell->GetExposure() + new_offset - 1 ); // The last exposure will be the new cell (that's why -1)
                        UOdysseyLayerCell* new_cell = animation_layer->AddCell( animation_layer->GetDefaultCellClass() );
                    }
                }
            }

            section->Modify();
        }

        //FDrawing drawing = ShotSequenceHelpers::GetDrawing( iSequencer, iSequence, iSequenceID, iFrameNumber, plane_binding );
        //if( drawing.Exists() )
        //    continue;

        //ShotSequenceHelpers::FFindOrCreateMaterialDrawingResult result = ShotSequenceHelpers::FindOrCreateMaterialDrawingTrackAndSections( iSequencer, iSequence, iSequenceID, plane_binding, iFrameNumber );
        //if( !result.mSections.Num() )
        //    continue;

        //is_creation |= result.mTrackCreated;
        //is_creation |= result.mSectionsCreated;

        //UMovieScenePrimitiveMaterialSection* section = result.mSections[0].Get();
        //FMovieSceneObjectPathChannel* channel = &section->MaterialChannel;

        ////---

        //UMaterialInstanceConstant* new_material = nullptr;

        //if( iDrawingArgs.mTexture.IsValid() )
        //{
        //    new_material = ProjectAssetTools::CreateMaterialAndTexture( iSequencer, iSequence, iSequenceID, iDrawingArgs.mTexture.Get() );
        //}
        //else
        //{
        //    TArrayView<TWeakObjectPtr<>> objects = iSequencer.FindBoundObjects( plane_binding, iSequenceID );
        //    if( objects.Num() != 1 )
        //        continue;
        //    APlaneActor* plane = Cast<APlaneActor>( objects[0] );
        //    if( !plane )
        //        continue;

        //    UScalingComponent* scaling_component = plane->FindComponentByClass<UScalingComponent>();
        //    if( !scaling_component )
        //        continue;

        //    FVector camera_view_size = scaling_component->ComputeSizeOfCameraView( camera, 200 /* arbitrary as we only need its ratio */ );
        //    FVector camera_view_size_with_scaling = scaling_component->ComputeScaleWithScaleAndMargin( camera_view_size );
        //    const UEposTracksEditorSettings* settings = GetDefault<UEposTracksEditorSettings>();
        //    FIntPoint texture_size = scaling_component->ComputeTextureSize( camera_view_size_with_scaling, settings->TextureSettings.Height );

        //    if( channel->GetNumKeys() )
        //    {
        //        FDrawing first_drawing;
        //        first_drawing.mSection = result.mSections[0];
        //        first_drawing.mChannel = channel;
        //        first_drawing.mKeyHandle = channel->GetHandle( 0 );

        //        UMaterialInstance* material = first_drawing.GetMaterial();
        //        UTexture2D* first_texture = ProjectAssetTools::GetTexture2D( iSequence, material );
        //        if( first_texture )
        //        {
        //            texture_size = scaling_component->ComputeTextureSize( camera_view_size_with_scaling, first_texture->GetSurfaceHeight() );
        //        }
        //    }

        //    new_material = ProjectAssetTools::CreateMaterialAndTexture( iSequencer, iSequence, iSequenceID, texture_size );
        //}

        //if( !new_material )
        //    continue;

        //---

        //section->Modify();

        //FMovieSceneObjectPathChannelKeyValue material_objectpath( new_material );

        //UE::MovieScene::AddKeyToChannel( channel, iFrameNumber, material_objectpath, iSequencer.GetKeyInterpolation() );
    }

    //---

    //if( is_creation )
    //    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    //else
    //    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//---
//---
//---

//static
void
BoardSequenceTools::GotoPreviousAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::GotoPreviousAnimationCut( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceTools::HasPreviousAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    // Check on all animations
    // As the changement of auto-selection animation will always select one of them each time the current frame change
    //TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllAnimationCutTimes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetAnimation::kSelectedOrAll );
    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllAnimationCutTimes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
    int32 index = times.FindLastByPredicate( [result]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < result.mInnerTime.GetFrame(); } );

    return index != INDEX_NONE;
}

//static
void
ShotSequenceTools::GotoPreviousAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    GotoPreviousAnimationCut( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceTools::HasPreviousAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllAnimationCutTimes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID() ); // See comment above #315
    int32 index = times.FindLastByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < iFrameNumber; } );

    return index != INDEX_NONE;
}

//static
void
ShotSequenceTools::GotoPreviousAnimationCut( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllAnimationCutTimes( iSequencer, iSequence, iSequenceID ); // See comment above #315

    int32 index = times.FindLastByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < iFrameNumber; } );
    if( index == INDEX_NONE )
        return;

    FFrameNumber previous_time = times[index];

    const FMovieSceneSequenceHierarchy* hierarchy = iSequencer.GetSharedPlaybackState()->GetHierarchy();
    const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( iSequenceID );

    FMovieSceneInverseSequenceTransform localToRootTransform = subdata->RootToSequenceTransform.Inverse();
    TOptional<FFrameTime> previous_time_in_root = localToRootTransform.TryTransformTime( previous_time );
    if( !previous_time_in_root )
        return;

    iSequencer.SetGlobalTime( *previous_time_in_root );
}

//---

//static
void
BoardSequenceTools::GotoNextAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::GotoNextAnimationCut( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceTools::HasNextAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllAnimationCutTimes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId ); // See comment above #315
    FFrameNumber* next_time = times.FindByPredicate( [result]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > result.mInnerTime.GetFrame(); } );

    return !!next_time;
}

//static
void
ShotSequenceTools::GotoNextAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    GotoNextAnimationCut( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceTools::HasNextAnimationCut( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllAnimationCutTimes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID() ); // See comment above #315
    FFrameNumber* next_time = times.FindByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > iFrameNumber; } );

    return !!next_time;
}

//static
void
ShotSequenceTools::GotoNextAnimationCut( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllAnimationCutTimes( iSequencer, iSequence, iSequenceID ); // See comment above #315

    FFrameNumber* next_time = times.FindByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > iFrameNumber; } );
    if( !next_time )
        return;

    const FMovieSceneSequenceHierarchy* hierarchy = iSequencer.GetSharedPlaybackState()->GetHierarchy();
    const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( iSequenceID );

    FMovieSceneInverseSequenceTransform localToRootTransform = subdata->RootToSequenceTransform.Inverse();
    TOptional<FFrameTime> next_time_in_root = localToRootTransform.TryTransformTime( *next_time );
    if( !next_time_in_root )
        return;

    iSequencer.SetGlobalTime( *next_time_in_root );
}

#undef LOCTEXT_NAMESPACE
