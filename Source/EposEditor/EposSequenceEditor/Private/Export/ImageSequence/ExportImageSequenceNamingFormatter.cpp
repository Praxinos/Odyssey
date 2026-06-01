// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Export/ImageSequence/ExportImageSequenceNamingFormatter.h"

#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneRootOverridePath.h"
#include "Internationalization/Regex.h"
#include "ISequencer.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSequence.h"
#include "MovieSceneSequenceVisitor.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceModule.h"
#include "Export/ExportStruct.h"
#include "Export/ImageSequence/ExportImageSequenceSettings.h"
#include "INamingFormatter.h"
#include "Settings/NamingConventionSettings.h"
#include "Shot/ShotSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#define LOCTEXT_NAMESPACE "ExportImageSequenceNamingFormatter"

//---

FExportImageSequenceNamingFormatter::FExportImageSequenceNamingFormatter( TWeakPtr<ISequencer> iSequencer, const FExportPanel* iPanel, int32 iIndex, const FExportImageSequenceOptions* iOptions )
    : mSequencer( iSequencer )
    , mCurrentPanel( iPanel )
    , mIndex( iIndex )
    , mImageSequenceOptions( iOptions )
{
    check( mCurrentPanel );

    mRootEposSequence = EposSequenceHelpers::GetRootEposSequence( *mSequencer.Pin().Get(), mCurrentPanel->mSequenceId, mRootEposSequenceId );
    if( !mRootEposSequence )
        return;

    FMovieSceneSequenceTransform transform_to_sequence;
    if( mRootEposSequenceId != MovieSceneSequenceID::Root )
    {
        const FMovieSceneSequenceHierarchy* hierarchy = mSequencer.Pin()->GetEvaluationTemplate().GetHierarchy();
        const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( mRootEposSequenceId );
        if( !subdata )
            return;

        transform_to_sequence = subdata->RootToSequenceTransform;
    }

    mFrameInRootEposSequence = ( mCurrentPanel->GlobalFrame * transform_to_sequence ).GetFrame();
}

//---

bool
FExportImageSequenceNamingFormatter::FormatName( const FString& iPatternToFormat, FString& oPatternFormatted )
{
    if( !mCurrentPanel )
        return false;
    if( !mRootEposSequence )
        return false;
    if( mRootEposSequenceId == MovieSceneSequenceID::Invalid )
        return false;

    TSharedRef<INumericTypeInterface<double>> type_interface = mSequencer.Pin()->GetNumericTypeInterface().ToSharedRef();

    //---

    FString parsed_string = iPatternToFormat;

    auto ReplaceKeywordInt        = [&]( auto iKeywordId, int iValue, int32 iNumDigits )  -> FString  { return parsed_string.Replace( *mImageSequenceOptions->mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *FString::Printf( TEXT( "%0*d" ), iNumDigits, iValue ) ); };
    //auto ReplaceKeywordIntAsFrame = [&]( auto iKeywordId, int iValue )                    -> FString  { return parsed_string.Replace( *mImageSequenceOptions->mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *type_interface->ToString( iValue ) ); };
    auto ReplaceKeywordFrame      = [&]( auto iKeywordId, FFrameNumber iValue )           -> FString  { return parsed_string.Replace( *mImageSequenceOptions->mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *type_interface->ToString( iValue.Value ) ); };
    auto ReplaceKeywordString     = [&]( auto iKeywordId, FString iValue )                -> FString  { return parsed_string.Replace( *mImageSequenceOptions->mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *iValue ); };

    //---

    parsed_string = ReplaceKeywordInt( EExportImageSequencePatternKeyword::PanelIndex, mIndex + 1, mImageSequenceOptions->PanelIndexFormat.NumDigits ); // To start at 1 and not 0, snif...

    parsed_string = ReplaceKeywordFrame( EExportImageSequencePatternKeyword::PanelFrame, mFrameInRootEposSequence );

    FString storyboard_name = mRootEposSequence->GetName();
    parsed_string = ReplaceKeywordString( EExportImageSequencePatternKeyword::StoryboardName, storyboard_name );

    //---

    FEposSequenceModule& module_epos_sequence = FModuleManager::LoadModuleChecked<FEposSequenceModule>( "EposSequence" );

    TTuple<UShotSequence*, FMovieSceneSequenceID> shot_info = GetShot();

    if( shot_info.Get<0>() )
    {
        UNamingFormatterShot* namingFormatter = Cast<UNamingFormatterShot>( module_epos_sequence.GetNamingFormatter<UNamingFormatterShot>() );

        /*bool is_formatted =*/ namingFormatter->FormatName( shot_info.Get<0>(), parsed_string, parsed_string );
    }

    //---

    TTuple<UMovieSceneSequence*, FMovieSceneSequenceID> sequence_info = GetSequence();

    if( sequence_info.Get<0>() )
    {
        UNamingFormatterBoard* namingFormatter = Cast<UNamingFormatterBoard>( module_epos_sequence.GetNamingFormatter<UNamingFormatterBoard>() );

        /*bool is_formatted =*/ namingFormatter->FormatName( sequence_info.Get<0>(), parsed_string, parsed_string );
    }

    //---

    // oPatternFormatted MUST only be set before a return
    // It's to manage the case when iPatternToFormat == oPatternFormatted
    oPatternFormatted = parsed_string;

    return true;
}

TTuple<UShotSequence*, FMovieSceneSequenceID>
FExportImageSequenceNamingFormatter::GetShot() const
{
    struct FSequenceShotVisitor
        : UE::MovieScene::ISequenceVisitor
    {
        virtual void VisitSection( UMovieSceneTrack* iTrack, UMovieSceneSection* iSection, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace )
        {
            UE::MovieScene::FSubSequencePath subsequencepath( iLocalSpace.SequenceID, mSequencer->GetSharedPlaybackState() );

            if( !subsequencepath.Contains( mRootEposSequenceId ) )
                return;

            UMovieSceneSingleCameraCutSection* cameracut_section = Cast<UMovieSceneSingleCameraCutSection>( iSection );
            if( !cameracut_section )
                return;

            FFrameTime local_reference_time = mGlobalFrame * iLocalSpace.RootToSequenceTransform;
            if( !iSection->IsTimeWithinSection( local_reference_time.GetFrame() ) )
                return;

            check( mInfo.Get<1>() == MovieSceneSequenceID::Invalid );

            UShotSequence* sequence = iSection->GetTypedOuter<UShotSequence>();
            check( sequence );

            mInfo = TTuple<UShotSequence*, FMovieSceneSequenceID>( sequence, iLocalSpace.SequenceID );
        }

        ISequencer* mSequencer;
        FMovieSceneSequenceID mRootEposSequenceId;

        FFrameNumber mGlobalFrame; // In tick resolution

        TTuple<UShotSequence*, FMovieSceneSequenceID> mInfo;
    };


    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitSections = true;
    params.bVisitRootTracks = true;
    params.bVisitSubSequences = true;

    FSequenceShotVisitor shot_visitor;
    shot_visitor.mSequencer = mSequencer.Pin().Get();
    shot_visitor.mRootEposSequenceId = mRootEposSequenceId;
    shot_visitor.mGlobalFrame = mCurrentPanel->GlobalFrame;

    // Visit all shots
    VisitSequence( mSequencer.Pin()->GetRootMovieSceneSequence(), params, shot_visitor );

    // if the found shot is the same as the root sequence, return like not found
    // because the root sequence has empty NameElements values
    if( shot_visitor.mInfo.Get<0>() == mSequencer.Pin()->GetRootMovieSceneSequence() )
        return TTuple<UShotSequence*, FMovieSceneSequenceID>();

    return shot_visitor.mInfo;
}

TTuple<UMovieSceneSequence*, FMovieSceneSequenceID>
FExportImageSequenceNamingFormatter::GetSequence() const
{
    TTuple<UMovieSceneSequence*, FMovieSceneSequenceID> info;

    UMovieSceneSequence* sequence = mRootEposSequence;
    if( !sequence )
        return info;

    UMovieScene* focusedMovieScene = sequence->GetMovieScene();
    if( !focusedMovieScene )
        return info;

    UMovieSceneCinematicBoardTrack* boardTrack = focusedMovieScene->FindTrack<UMovieSceneCinematicBoardTrack>();
    if( !boardTrack )
        return info;

    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( EposSequenceHelpers::FindSectionAtTime( boardTrack->GetAllSections(), mFrameInRootEposSequence ) );
    if( !board_section )
        return info;

    if( !board_section->GetSequence() )
        return info;

    return TTuple<UMovieSceneSequence*, FMovieSceneSequenceID>( board_section->GetSequence(), board_section->GetSequenceID() );
}

//---

#undef LOCTEXT_NAMESPACE
