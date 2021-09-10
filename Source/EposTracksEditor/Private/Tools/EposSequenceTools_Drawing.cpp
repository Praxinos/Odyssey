// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/EposSequenceTools.h"

#include "AssetRegistryModule.h"
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
    return ShotSequenceHelpers::GetDrawingIndex( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame(), iPlaneBinding ) == INDEX_NONE;
}

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

//---

//static
bool
BoardSequenceTools::CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    return ShotSequenceHelpers::GetDrawingIndex( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iPlaneBinding ) == INDEX_NONE;
}

//static
bool
ShotSequenceTools::CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    return ShotSequenceHelpers::GetDrawingIndex( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iPlaneBinding ) == INDEX_NONE;
}

//---

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
    ShotSequenceHelpers::FDrawingData drawing_data;
    int32 key_index = ShotSequenceHelpers::GetDrawingIndex( iSequencer, iSequence, iSequenceID, iFrameNumber, iPlaneBinding, &drawing_data );
    if( key_index != INDEX_NONE )
        return;

    if( !drawing_data.mSection )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "CreateDrawing", "Create a new drawing" ) );

    drawing_data.mSection->Modify();

    //---

    UMaterialInstanceConstant* new_material = ProjectAssetTools::CreateMaterialAndTexture( iSequence, ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID ), iSequencer.GetRootMovieSceneSequence() );
    if( !new_material )
        return;

    //---

    FMovieSceneObjectPathChannelKeyValue material_objectpath( new_material );

    UE::MovieScene::AddKeyToChannel( drawing_data.mChannel, iFrameNumber, material_objectpath, iSequencer.GetKeyInterpolation() );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//static
void
BoardSequenceTools::CloneDrawing( ISequencer* iSequencer, UMaterialInstance* iMaterialToClone, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::CloneDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iMaterialToClone, result.mInnerTime.GetFrame(), iPlaneBinding );
}

//static
void
ShotSequenceTools::CloneDrawing( ISequencer* iSequencer, UMaterialInstance* iMaterialToClone, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    CloneDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iMaterialToClone, iFrameNumber, iPlaneBinding );
}

//static
void
ShotSequenceTools::CloneDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMaterialInstance* iMaterialToClone, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    ShotSequenceHelpers::FDrawingData drawing_data;
    int32 key_index = ShotSequenceHelpers::GetDrawingIndex( iSequencer, iSequence, iSequenceID, iFrameNumber, iPlaneBinding, &drawing_data );
    if( key_index != INDEX_NONE )
        return;

    if( !drawing_data.mSection )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "CloneDrawing", "Clone drawing" ) );

    drawing_data.mSection->Modify();

    //---

    UMaterialInstanceConstant* new_material = ProjectAssetTools::CloneMaterialAndTexture( iSequence, iMaterialToClone, iSequencer.GetRootMovieSceneSequence() );
    if( !new_material )
        return;

    //---

    FMovieSceneObjectPathChannelKeyValue material_objectpath( new_material );

    UE::MovieScene::AddKeyToChannel( drawing_data.mChannel, iFrameNumber, material_objectpath, iSequencer.GetKeyInterpolation() );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//static
void
BoardSequenceTools::DeleteDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::DeleteDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iPlaneBinding );
}

//static
void
ShotSequenceTools::DeleteDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    DeleteDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iPlaneBinding );
}

//static
void
ShotSequenceTools::DeleteDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    ShotSequenceHelpers::FDrawingData drawing_data;
    int32 key_index = ShotSequenceHelpers::GetDrawingIndex( iSequencer, iSequence, iSequenceID, iFrameNumber, iPlaneBinding, &drawing_data );
    if( key_index == INDEX_NONE )
        return;

    if( !drawing_data.mSection )
        return;

    FKeyHandle key_handle = drawing_data.mChannel->GetData().GetHandle( key_index );

    IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( "AssetRegistry" ).Get();

    //---

    const FScopedTransaction transaction( LOCTEXT( "DeleteDrawing", "Delete drawing" ) );

    drawing_data.mSection->Modify();

    //---

    // Get the current key and its material
    FMovieSceneObjectPathChannelKeyValue key_value;
    UE::MovieScene::GetKeyValue( drawing_data.mChannel, key_handle, key_value );
    UMaterialInstance* material_to_delete = Cast<UMaterialInstance>( key_value.Get() );

    // Reset the current key to break the link between key and material asset
    UE::MovieScene::AssignValue( drawing_data.mChannel, key_handle, nullptr );

    //-

    UEditorAssetLibrary::SaveLoadedAsset( iSequence );

    TArray<FString> paths;
    paths.Add( FPaths::GetPath( iSequence->GetPathName() ) );
    AssetRegistry.ScanPathsSynchronous( paths, true );

    //-

    // Delete the material asset
    int32 count_deleted = 1; // Arbitrary set to 1 to delete the key if no material
    if( material_to_delete )
        count_deleted = ProjectAssetTools::DeleteMaterialAndTexture( iSequence, material_to_delete, iSequencer.GetRootMovieSceneSequence() );

    drawing_data.mSection->Modify();

    // Delete the key if material asset was really deleted or set again the key value to its original value
    if( count_deleted )
        drawing_data.mChannel->DeleteKeys( MakeArrayView( &key_handle, 1 ) );
    else
        UE::MovieScene::AssignValue( drawing_data.mChannel, key_handle, material_to_delete );

    //-

    UEditorAssetLibrary::SaveLoadedAsset( iSequence );

    AssetRegistry.ScanPathsSynchronous( paths, true );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
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
