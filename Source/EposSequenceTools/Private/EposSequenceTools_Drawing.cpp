// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposSequenceTools.h"

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
#include "ResourceAssetTools.h"
#include "Shot/ShotSequenceHelpers.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Drawing"

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
