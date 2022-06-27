// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ExportImageSequenceNamingFormatter.h"

#include "Internationalization/Regex.h"
#include "ISequencer.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSequence.h"
#include "MovieSceneSequenceVisitor.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Export/ExportImageSequenceSettings.h"
#include "Export/ExportImageSequenceStruct.h"
#include "Shot/ShotSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#define LOCTEXT_NAMESPACE "ExportImageSequenceNamingFormatter"

//---

FExportImageSequenceNamingFormatter::FExportImageSequenceNamingFormatter( TWeakPtr<ISequencer> iSequencer, const FExportImageSequencePanel* iImageSequencePanel, int32 iIndex, const FExportImageSequenceOptions* iOptions )
    : mSequencer( iSequencer )
    , mCurrentPanel( iImageSequencePanel )
    , mIndex( iIndex )
    , mImageSequenceOptions( iOptions )
{
}

//---

bool
FExportImageSequenceNamingFormatter::FormatName( FString& oName )
{
    if( !mCurrentPanel )
        return false;

    TSharedRef<INumericTypeInterface<double>> type_interface = mSequencer.Pin()->GetNumericTypeInterface();

    UMovieSceneSequence* root_sequence = mSequencer.Pin()->GetRootMovieSceneSequence();

    //---

    FString parsed_string = mImageSequenceOptions->Pattern;

    auto ReplaceKeywordInt        = [&]( EExportImageSequencePatternKeyword iKeywordId, int iValue, int32 iNumDigits )  -> FString  { return parsed_string.Replace( *mImageSequenceOptions->mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *FString::Printf( TEXT( "%0*d" ), iNumDigits, iValue ) ); };
    //auto ReplaceKeywordIntAsFrame = [&]( EExportImageSequencePatternKeyword iKeywordId, int iValue )                    -> FString  { return parsed_string.Replace( *mImageSequenceOptions->mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *type_interface->ToString( iValue ) ); };
    auto ReplaceKeywordFrame      = [&]( EExportImageSequencePatternKeyword iKeywordId, FFrameNumber iValue )           -> FString  { return parsed_string.Replace( *mImageSequenceOptions->mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *type_interface->ToString( iValue.Value ) ); };
    auto ReplaceKeywordString     = [&]( EExportImageSequencePatternKeyword iKeywordId, FString iValue )                -> FString  { return parsed_string.Replace( *mImageSequenceOptions->mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *iValue ); };

    //---

    parsed_string = ReplaceKeywordInt( EExportImageSequencePatternKeyword::PanelIndex, mIndex + 1, mImageSequenceOptions->PanelIndexFormat.NumDigits ); // To start at 1 and not 0, snif...

    parsed_string = ReplaceKeywordFrame( EExportImageSequencePatternKeyword::PanelFrame, mCurrentPanel->GlobalFrame );

    FString storyboard_name = root_sequence->GetName();
    parsed_string = ReplaceKeywordString( EExportImageSequencePatternKeyword::StoryboardName, storyboard_name );

    //---

    TTuple<UShotSequence*, FMovieSceneSequenceID> shot_info = GetShot();

    if( shot_info.Get<0>() )
    {
        int32 shot_index = shot_info.Get<0>()->NameElements.Index;
        parsed_string = ReplaceKeywordInt( EExportImageSequencePatternKeyword::ShotIndex, shot_index, mImageSequenceOptions->ShotIndexFormat.NumDigits );
    }

    //---

    TTuple<UMovieSceneSequence*, FMovieSceneSequenceID> sequence_info = GetSequence();

    if( sequence_info.Get<0>() )
    {
        int32 board_index = INDEX_NONE;

        UBoardSequence* board_sequence = Cast<UBoardSequence>( sequence_info.Get<0>() );
        if( board_sequence )
        {
            board_index = board_sequence->NameElements.Index;

            //FEposSequenceModule& module = FModuleManager::LoadModuleChecked<FEposSequenceModule>( "EposSequence" );
            //UNamingFormatterBoard* namingFormatter = module.GetNamingFormatter<UNamingFormatterBoard>();

            //parsed_string = namingFormatter->FormatName( board_sequence, parsed_string );

        }

        UShotSequence* shot_sequence = Cast<UShotSequence>( sequence_info.Get<0>() );
        if( shot_sequence )
            board_index = shot_sequence->NameElements.Index;

        parsed_string = ReplaceKeywordInt( EExportImageSequencePatternKeyword::BoardIndex, board_index, mImageSequenceOptions->BoardIndexFormat.NumDigits );
    }

    //---

    oName = parsed_string;

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

        FFrameNumber mGlobalFrame; // In tick resolution

        TTuple<UShotSequence*, FMovieSceneSequenceID> mInfo;
    };


    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitSections = true;
    params.bVisitMasterTracks = true;
    params.bVisitSubSequences = true;

    FSequenceShotVisitor shot_visitor;
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

    UMovieSceneSequence* sequence = mSequencer.Pin()->GetRootMovieSceneSequence();
    if( !sequence )
        return info;

    UMovieScene* focusedMovieScene = sequence->GetMovieScene();
    if( !focusedMovieScene )
        return info;

    UMovieSceneCinematicBoardTrack* boardTrack = focusedMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( !boardTrack )
        return info;

    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( MovieSceneHelpers::FindSectionAtTime( boardTrack->GetAllSections(), mCurrentPanel->GlobalFrame ) );
    if( !board_section )
        return info;

    if( !board_section->GetSequence() )
        return info;

    return TTuple<UMovieSceneSequence*, FMovieSceneSequenceID>( board_section->GetSequence(), board_section->GetSequenceID() );
}

//---

#undef LOCTEXT_NAMESPACE
