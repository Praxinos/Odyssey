// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
#include "PlaneActor.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Drawing"

//static
bool
BoardSequenceTools::CanCreateDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    bool can_create = false;
    for( auto plane_binding : iPlaneBindings )
    {
        FDrawing drawing = ShotSequenceHelpers::GetDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame(), plane_binding );

        can_create |= ( !drawing.Exists() );
    }

    return can_create;
}

//static
bool
BoardSequenceTools::CanCreateDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    return BoardSequenceTools::CanCreateDrawing( iSequencer, iSubSection, iFrameNumber, TArray<FGuid>( { iPlaneBinding } ) );
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
BoardSequenceTools::CreateDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    ShotSequenceTools::CreateDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame(), iPlaneBindings );
}

//static
void
BoardSequenceTools::CreateDrawing( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceTools::CreateDrawing( iSequencer, iSubSection, iFrameNumber, TArray<FGuid>( { iPlaneBinding } ) );
}

//static
void
BoardSequenceTools::CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::CreateDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iPlaneBindings );
}

//static
void
BoardSequenceTools::CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceTools::CreateDrawing( iSequencer, iFrameNumber, TArray<FGuid>( { iPlaneBinding } ) );
}

//static
void
ShotSequenceTools::CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings )
{
    CreateDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iPlaneBindings );
}

//static
void
ShotSequenceTools::CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    ShotSequenceTools::CreateDrawing( iSequencer, iFrameNumber, TArray<FGuid>( { iPlaneBinding } ) );
}

//static
void
ShotSequenceTools::CreateDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings )
{
    const FScopedTransaction transaction( LOCTEXT( "transaction.create-plane-drawing", "Create a new drawing" ) );

    bool is_creation = false;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID );

    for( auto plane_binding : iPlaneBindings )
    {
        FDrawing drawing = ShotSequenceHelpers::GetDrawing( iSequencer, iSequence, iSequenceID, iFrameNumber, plane_binding );
        if( drawing.Exists() )
            continue;

        ShotSequenceHelpers::FFindOrCreateMaterialDrawingResult result = ShotSequenceHelpers::FindOrCreateMaterialDrawingTrackAndSections( iSequencer, iSequence, iSequenceID, plane_binding, iFrameNumber );
        if( !result.mSections.Num() )
            continue;

        is_creation |= result.mTrackCreated;
        is_creation |= result.mSectionsCreated;

        UMovieScenePrimitiveMaterialSection* section = result.mSections[0].Get();
        FMovieSceneObjectPathChannel* channel = &section->MaterialChannel;

        TArrayView<TWeakObjectPtr<>> objects = iSequencer.FindBoundObjects( plane_binding, iSequenceID );
        if( objects.Num() != 1 )
            continue;
        APlaneActor* plane = Cast<APlaneActor>( objects[0] );
        if( !plane )
            continue;

        //---

        const UEposTracksEditorSettings* settings = GetDefault<UEposTracksEditorSettings>();
        FIntPoint texture_size = plane->ComputeTextureSize( camera, settings->TextureSettings.Height );

        if( channel->GetNumKeys() )
        {
            FDrawing first_drawing;
            first_drawing.mSection = result.mSections[0];
            first_drawing.mChannel = channel;
            first_drawing.mKeyHandle = channel->GetData().GetHandle( 0 );

            UMaterialInstance* material = first_drawing.GetMaterial();
            UTexture2D* first_texture = ProjectAssetTools::GetTexture2D( iSequence, material );
            if( first_texture )
                texture_size = plane->ComputeTextureSize( camera, first_texture->GetSurfaceHeight() );
        }

        //---

        section->Modify();

        UMaterialInstanceConstant* new_material = ProjectAssetTools::CreateMaterialAndTexture( iSequencer, iSequencer.GetRootMovieSceneSequence(), iSequence, texture_size );
        if( !new_material )
            continue;

        FMovieSceneObjectPathChannelKeyValue material_objectpath( new_material );

        UE::MovieScene::AddKeyToChannel( channel, iFrameNumber, material_objectpath, iSequencer.GetKeyInterpolation() );
    }

    //---

    if( is_creation )
        iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    else
        iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//---

//static
bool
BoardSequenceTools::IsDrawingInEditionMode( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    //---

    bool is_edited = false;

    TArray<FGuid> plane_bindings;
    ShotSequenceHelpers::GetAllPlanes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kAll, nullptr, &plane_bindings );

    for( auto plane_binding : plane_bindings )
    {
        TArray<FDrawing> drawings = ShotSequenceHelpers::GetAllDrawings( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, plane_binding );
        for( auto drawing : drawings )
        {
            is_edited |= ShotSequenceTools::IsDrawingInEditionMode( iSequencer, result.mInnerSequence, result.mInnerSequenceId, drawing );
        }
    }

    return is_edited;
}

//static
bool
ShotSequenceTools::IsDrawingInEditionMode( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FDrawing& iDrawing )
{
    UMaterialInstance* material = iDrawing.GetMaterial();
    UTexture2D* texture = ProjectAssetTools::GetTexture2D( iSequence, material );
    if( !texture )
        return false;

    //---

    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

    TArray<IAssetEditorInstance*> opened_editors = AssetEditorSubsystem->FindEditorsForAsset( texture );
    //FName name = opened_editors.Num() ? opened_editors[0]->GetEditorName() : NAME_None;

    return !!opened_editors.Num();
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
