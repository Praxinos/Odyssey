// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/EposSequenceTools.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "EditorAssetLibrary.h"
#include "ISequencer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"

#include "Board/BoardSequence.h"
#include "EposSequenceHelpers.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Drawing"

//static
bool
BoardSequenceTools::CanCreateDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    FDrawing drawing = ShotSequenceHelpers::GetDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame(), iPlaneBinding );

    return !drawing.Exists();
}

//static
bool
BoardSequenceTools::CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    FDrawing drawing = ShotSequenceHelpers::GetDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iPlaneBinding );

    return !drawing.Exists();
}

//static
bool
ShotSequenceTools::CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    FDrawing drawing = ShotSequenceHelpers::GetDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iPlaneBinding );

    return !drawing.Exists();
}

//---

//static
void
BoardSequenceTools::CreateDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    ShotSequenceTools::CreateDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame(), iPlaneBinding );
}

//static
void
BoardSequenceTools::CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::CreateDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iPlaneBinding );
}

//static
void
ShotSequenceTools::CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    CreateDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iPlaneBinding );
}

//static
void
ShotSequenceTools::CreateDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    const FScopedTransaction transaction( LOCTEXT( "transaction.create-plane-drawing", "Create a new drawing" ) );

    ShotSequenceHelpers::FFindOrCreateMaterialDrawingResult result = ShotSequenceHelpers::FindOrCreateMaterialDrawingTrackAndSections( iSequencer, iSequence, iSequenceID, iPlaneBinding, iFrameNumber );
    if( !result.mSections.Num() )
        return;

    UMovieScenePrimitiveMaterialSection* section = result.mSections[0].Get();
    FMovieSceneObjectPathChannel* channel = &section->MaterialChannel;

    TArrayView<TWeakObjectPtr<>> objects = iSequencer.FindBoundObjects( iPlaneBinding, iSequenceID );
    if( objects.Num() != 1 )
        return;
    APlaneActor* plane = Cast<APlaneActor>( objects[0] );
    if( !plane )
        return;

    //---

    section->Modify();

    UMaterialInstanceConstant* new_material = ProjectAssetTools::CreateMaterialAndTexture( iSequencer, iSequencer.GetRootMovieSceneSequence(), iSequence, ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID ), plane );
    if( !new_material )
        return;

    FMovieSceneObjectPathChannelKeyValue material_objectpath( new_material );

    UE::MovieScene::AddKeyToChannel( channel, iFrameNumber, material_objectpath, iSequencer.GetKeyInterpolation() );

    //---

    if( result.mTrackCreated || result.mSectionsCreated )
        iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    else
        iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//---

//static
bool
BoardSequenceTools::CanCloneDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    return CanCreateDrawing( iSequencer, iSubSection, iFrameNumber, iPlaneBinding );
}

//static
bool
BoardSequenceTools::CanCloneDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    return CanCreateDrawing( iSequencer, iFrameNumber, iPlaneBinding );
}

//static
bool
ShotSequenceTools::CanCloneDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    return CanCreateDrawing( iSequencer, iFrameNumber, iPlaneBinding );
}

//---

//static
void
BoardSequenceTools::CloneDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, UMovieSceneSection* iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    ShotSequenceTools::CloneDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iSection, iChannelHandle, iKeyHandle, result.mInnerTime.GetFrame() );
}

//static
void
ShotSequenceTools::CloneDrawing( ISequencer* iSequencer, UMovieSceneSection* iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle, FFrameNumber iFrameNumber )
{
    CloneDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iSection, iChannelHandle, iKeyHandle, iFrameNumber );
}

//static
void
ShotSequenceTools::CloneDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMovieSceneSection* iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle, FFrameNumber iFrameNumber )
{
    FDrawing drawing = ShotSequenceHelpers::ConvertToDrawing( iSection, iChannelHandle, iKeyHandle );
    if( !drawing.Exists() )
        return;

    UMaterialInstance* existing_material = drawing.GetMaterial();
    if( !existing_material )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "transaction.clone-plane-drawing", "Clone drawing" ) );

    iSection->Modify();

    UMaterialInstanceConstant* new_material = ProjectAssetTools::CloneMaterialAndTexture( iSequencer, iSequencer.GetRootMovieSceneSequence(), iSequence, existing_material );
    if( !new_material )
        return;

    FMovieSceneObjectPathChannelKeyValue material_objectpath( new_material );

    UE::MovieScene::AddKeyToChannel( drawing.mChannel, iFrameNumber, material_objectpath, iSequencer.GetKeyInterpolation() );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//---

//static
void
BoardSequenceTools::DeleteDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    ShotSequenceTools::DeleteDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iSections, iChannelHandles, iKeyHandles );
}

//static
void
ShotSequenceTools::DeleteDrawing( ISequencer* iSequencer, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles )
{
    DeleteDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iSections, iChannelHandles, iKeyHandles );
}

//static
void
ShotSequenceTools::DeleteDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArrayView<TWeakObjectPtr<UMovieSceneSection>> iSections, TArrayView<FMovieSceneChannelHandle> iChannelHandles, TArrayView<FKeyHandle> iKeyHandles )
{
    check( iSections.Num() == iChannelHandles.Num() && iChannelHandles.Num() == iKeyHandles.Num() );

    if( !iKeyHandles.Num() )
        return;

    const FScopedTransaction transaction( LOCTEXT( "transaction.delete-plane-drawing", "Delete drawing" ) );

    for( int i = 0; i < iKeyHandles.Num(); i++ )
    {
        FDrawing drawing = ShotSequenceHelpers::ConvertToDrawing( iSections[i], iChannelHandles[i], iKeyHandles[i] );
        if( !drawing.Exists() )
            return;

        drawing.mSection->Modify();

        drawing.mChannel->DeleteKeys( iKeyHandles.Slice( i, 1 ) );
    }

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
    // iChannelHandles are invalid at this point
}

//---
//---
//---

//static
void
BoardSequenceTools::GotoPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::GotoPreviousDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceTools::HasPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOrAll );
    int32 index = times.FindLastByPredicate( [result]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < result.mInnerTime.GetFrame(); } );

    return index != INDEX_NONE;
}

//static
void
ShotSequenceTools::GotoPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    GotoPreviousDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceTools::HasPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetPlane::kSelectedOrAll );
    int32 index = times.FindLastByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < iFrameNumber; } );

    return index != INDEX_NONE;
}

//static
void
ShotSequenceTools::GotoPreviousDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( iSequencer, iSequence, iSequenceID, EGetPlane::kSelectedOrAll );

    int32 index = times.FindLastByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < iFrameNumber; } );
    if( index == INDEX_NONE )
        return;

    FFrameNumber previous_time = times[index];

    const FMovieSceneSequenceHierarchy* hierarchy = iSequencer.GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( iSequencer.GetEvaluationTemplate().GetCompiledDataID() );
    const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( iSequenceID );

    iSequencer.SetGlobalTime( previous_time * subdata->RootToSequenceTransform.InverseLinearOnly() );
}

//---

//static
void
BoardSequenceTools::GotoNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::GotoNextDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceTools::HasNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOrAll );
    FFrameNumber* next_time = times.FindByPredicate( [result]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > result.mInnerTime.GetFrame(); } );

    return !!next_time;
}

//static
void
ShotSequenceTools::GotoNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    GotoNextDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceTools::HasNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetPlane::kSelectedOrAll );
    FFrameNumber* next_time = times.FindByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > iFrameNumber; } );

    return !!next_time;
}

//static
void
ShotSequenceTools::GotoNextDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( iSequencer, iSequence, iSequenceID, EGetPlane::kSelectedOrAll );

    FFrameNumber* next_time = times.FindByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > iFrameNumber; } );
    if( !next_time )
        return;

    const FMovieSceneSequenceHierarchy* hierarchy = iSequencer.GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( iSequencer.GetEvaluationTemplate().GetCompiledDataID() );
    const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( iSequenceID );

    iSequencer.SetGlobalTime( *next_time * subdata->RootToSequenceTransform.InverseLinearOnly() );
}

#undef LOCTEXT_NAMESPACE
