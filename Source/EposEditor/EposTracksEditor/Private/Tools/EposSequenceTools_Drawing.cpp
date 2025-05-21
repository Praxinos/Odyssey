// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
#include "ScalingComponent.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Drawing"

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

    //TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOrAll );
    //int32 index = times.FindLastByPredicate( [result]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < result.mInnerTime.GetFrame(); } );

    //return index != INDEX_NONE;
    return false;
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
    //TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetPlane::kSelectedOrAll );
    //int32 index = times.FindLastByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < iFrameNumber; } );

    //return index != INDEX_NONE;
    return false;
}

//static
void
ShotSequenceTools::GotoPreviousDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    //TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( iSequencer, iSequence, iSequenceID, EGetPlane::kSelectedOrAll );

    //int32 index = times.FindLastByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame < iFrameNumber; } );
    //if( index == INDEX_NONE )
    //    return;

    //FFrameNumber previous_time = times[index];

    //const FMovieSceneSequenceHierarchy* hierarchy = iSequencer.GetSharedPlaybackState()->GetHierarchy();
    //const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( iSequenceID );

    //FMovieSceneInverseSequenceTransform localToRootTransform = subdata->RootToSequenceTransform.Inverse();
    //TOptional<FFrameTime> previous_time_in_root = localToRootTransform.TryTransformTime( previous_time );
    //if( !previous_time_in_root )
    //    return;

    //iSequencer.SetGlobalTime( *previous_time_in_root );
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

    //TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOrAll );
    //FFrameNumber* next_time = times.FindByPredicate( [result]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > result.mInnerTime.GetFrame(); } );

    //return !!next_time;
    return false;
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
    //TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetPlane::kSelectedOrAll );
    //FFrameNumber* next_time = times.FindByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > iFrameNumber; } );

    //return !!next_time;
    return false;
}

//static
void
ShotSequenceTools::GotoNextDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    //TArray<FFrameNumber> times = ShotSequenceHelpers::GetAllDrawingTimes( iSequencer, iSequence, iSequenceID, EGetPlane::kSelectedOrAll );

    //FFrameNumber* next_time = times.FindByPredicate( [iFrameNumber]( FFrameNumber iCurrentFrame ) { return iCurrentFrame > iFrameNumber; } );
    //if( !next_time )
    //    return;

    //const FMovieSceneSequenceHierarchy* hierarchy = iSequencer.GetSharedPlaybackState()->GetHierarchy();
    //const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( iSequenceID );

    //FMovieSceneInverseSequenceTransform localToRootTransform = subdata->RootToSequenceTransform.Inverse();
    //TOptional<FFrameTime> next_time_in_root = localToRootTransform.TryTransformTime( *next_time );
    //if( !next_time_in_root )
    //    return;

    //iSequencer.SetGlobalTime( *next_time_in_root );
}

#undef LOCTEXT_NAMESPACE
