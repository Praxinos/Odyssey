// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Helpers/EposSequenceToolHelpers.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "ISequencer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"

#include "Board/BoardSequence.h"
#include "Board/BoardSequenceHelpers.h"
#include "Shot/ShotSequenceHelpers.h"

#define LOCTEXT_NAMESPACE "EposSequenceToolHelpers_Drawing"

//static
bool
BoardSequenceToolHelpers::CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
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
ShotSequenceToolHelpers::CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    return ShotSequenceHelpers::GetDrawingIndex( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iPlaneBinding ) == INDEX_NONE;
}

//---

//static
void
BoardSequenceToolHelpers::CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceToolHelpers::CreateDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iPlaneBinding );
}

//static
void
ShotSequenceToolHelpers::CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    CreateDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iPlaneBinding );
}

//static
void
ShotSequenceToolHelpers::CreateDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
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

    FString package_name;
    FString asset_name;
    UMaterialInstanceConstant* new_material = CreateCurrentMaterialPlaneAsset( iSequence, iSequencer.GetRootMovieSceneSequence(), package_name, asset_name );
    if( !new_material )
        return;

    UTexture2D* new_texture = CreateCurrentTexture2DAsset( iSequence, new_material, package_name, asset_name );
    if( !new_texture )
        return;

    new_material->SetTextureParameterValueEditorOnly( TEXT( "DrawingTexture" ), new_texture );
//TODO: manage lighttable or in the delegate inside section row ?

    //---

    FMovieSceneObjectPathChannelKeyValue material_objectpath( new_material );

    UE::MovieScene::AddKeyToChannel( drawing_data.mChannel, iFrameNumber, material_objectpath, iSequencer.GetKeyInterpolation() );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//---
//---
//---

//static
void
BoardSequenceToolHelpers::GotoPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceToolHelpers::GotoPreviousDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceToolHelpers::HasPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
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
ShotSequenceToolHelpers::GotoPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    ShotSequenceToolHelpers::GotoPreviousDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceToolHelpers::HasPreviousDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetPlane::kSelectedOrAll );
    int32 index = times.FindLastByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < iFrameNumber; } );

    return index != INDEX_NONE;
}

//static
void
ShotSequenceToolHelpers::GotoPreviousDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
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
BoardSequenceToolHelpers::GotoNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceToolHelpers::GotoNextDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceToolHelpers::HasNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
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
ShotSequenceToolHelpers::GotoNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    ShotSequenceToolHelpers::GotoNextDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceToolHelpers::HasNextDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetPlane::kSelectedOrAll );
    FFrameNumber* next_time = times.FindByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > iFrameNumber; } );

    return !!next_time;
}

//static
void
ShotSequenceToolHelpers::GotoNextDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
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
