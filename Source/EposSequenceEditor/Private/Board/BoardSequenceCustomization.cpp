// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Board/BoardSequenceCustomization.h"

#include "CineCameraActor.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Engine/Selection.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "ILevelEditor.h"
#include "LevelEditor.h"
#include "LevelEditorViewport.h"
#include "MovieSceneTimeHelpers.h"
#include "Sections/MovieSceneSubSection.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "MVVM/ViewModels/ObjectBindingModel.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposNamingConventionBlueprintLibrary.h"
#include "EposSequenceEditorBlueprintLibrary.h"
#include "EposSequenceEditorCommands.h"
#include "EposSequenceHelpers.h"
#include "EposSequenceToolbarHelpers.h"
#include "EposTracksModule.h"
#include "Misc/EposSequenceFBXInterop.h"
#include "PlaneActor.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposSequenceEditorStyle.h"
#include "ToolkitHelpers.h"
#include "Tools/EposSequenceTools.h"
#include "Tools/LighttableTools.h"
#include "Widgets/SInfoBar.h"

#define LOCTEXT_NAMESPACE "BoardSequenceCustomization"

//---

static int32 sgRegistrationCountDebug = 0; // Just to be sure to always register (and unregister) customization synchronously

void
FBoardSequenceCustomization::RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder ) // This is called each time the focused sequence changed (ie. when double-clicking on a section to go inside its subsequence)
{
    sgRegistrationCountDebug++;

    mSequencer = &ioBuilder.GetSequencer();
    // From 5.3, the registration is only done if the new focused sequence is NOT the same type than the previous one
    // It means that:
    // - going from board to inner board, NOT called
    // - going from board to inner shot, called
    // - going from shot to upper board, called
    mBoardSequence = Cast<UBoardSequence>( &ioBuilder.GetFocusedSequence() );

    //---

    mSequencer->OnCloseEvent().AddRaw( this, &FBoardSequenceCustomization::OnSequencerClosed );

    // Listen for actor/component movement
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw( this, &FBoardSequenceCustomization::OnPrePropertyChanged );
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw( this, &FBoardSequenceCustomization::OnPostPropertyChanged );

    mBoardCommandList = MakeShared<FUICommandList>();

    BindCommands( mBoardCommandList );

    mSequencer->GetCommandBindings()->Append( mBoardCommandList.ToSharedRef() );

    TSharedPtr< ILevelEditor > levelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>( "LevelEditor" ).GetFirstLevelEditor();
    levelEditor->AppendCommands( mBoardCommandList.ToSharedRef() );

    //---

    FSequencerCustomizationInfo customization;

    TSharedRef<FExtender> ToolbarExtender = MakeShared<FExtender>();
    ToolbarExtender->AddToolBarExtension( "CurveEditor", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw( this, &FBoardSequenceCustomization::ExtendSequencerToolbar ) );
    customization.ToolbarExtender = ToolbarExtender;

    customization.OnBuildObjectBindingContextMenu = FOnGetSequencerMenuExtender::CreateRaw(this, &FBoardSequenceCustomization::CreateObjectBindingContextMenuExtender);

    //customization.OnReceivedDragOver.BindRaw( this, &FBoardSequenceCustomization::OnSequencerReceiveDragOver );
    //customization.OnReceivedDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerReceiveDrop );

    customization.OnAssetsDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerAssetsDrop );
    customization.OnClassesDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerClassesDrop );
    customization.OnActorsDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerActorsDrop );

    ioBuilder.AddCustomization( customization );

    UEposSequenceEditorBlueprintLibrary::SetSequencer( mSequencer->AsShared() );
    UEposNamingConventionBlueprintLibrary::SetSequencer( mSequencer->AsShared() );

    mSequencerActorAddedDelegates = mSequencer->OnActorAddedToSequencer().AddStatic( &ToolkitHelpers::HandleActorAddedToSequencer, mSequencer );
    mSequencerActivatedDelegates = mSequencer->OnActivateSequence().AddStatic( &ToolkitHelpers::HandleOnActivateSequence, mSequencer );
    mSequencerSelectionSectionChangedDelegates = mSequencer->GetSelectionChangedSections().AddStatic( &ToolkitHelpers::HandleOnSelectionChangedSections, mSequencer );

    mSequencerBeginScrubbingDelegates = mSequencer->OnBeginScrubbingEvent().AddRaw( this, &FBoardSequenceCustomization::OnBeginScrubbing );
    mSequencerEndScrubbingDelegates = mSequencer->OnEndScrubbingEvent().AddRaw( this, &FBoardSequenceCustomization::OnEndScrubbing );
    mSequencerGlobalTimeChangedDelegates = mSequencer->OnGlobalTimeChanged().AddRaw( this, &FBoardSequenceCustomization::OnGlobalTimeChanged );
}

void
FBoardSequenceCustomization::UnregisterSequencerCustomization()
{
    sgRegistrationCountDebug--;
    check( sgRegistrationCountDebug == 0 );

    FCoreUObjectDelegates::OnPreObjectPropertyChanged.RemoveAll( this );
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll( this );

    mBoardCommandList = nullptr;

    if( mSequencer )
    {
        mSequencer->OnActorAddedToSequencer().Remove( mSequencerActorAddedDelegates );
        mSequencer->OnActivateSequence().Remove( mSequencerActivatedDelegates );
        mSequencer->GetSelectionChangedSections().Remove( mSequencerSelectionSectionChangedDelegates );

        mSequencer->OnBeginScrubbingEvent().Remove( mSequencerBeginScrubbingDelegates );
        mSequencer->OnBeginScrubbingEvent().Remove( mSequencerEndScrubbingDelegates );
        mSequencer->OnGlobalTimeChanged().Remove( mSequencerGlobalTimeChangedDelegates );
    }

    mSequencer = nullptr;
    mBoardSequence = nullptr;
}

void
FBoardSequenceCustomization::OnSequencerClosed( TSharedRef<ISequencer> iSequencer )
{
    if( &iSequencer.Get() == mSequencer )
    {
        mSequencer->OnCloseEvent().RemoveAll( this );

        mSequencer = nullptr;
    }
}

//---

void
FBoardSequenceCustomization::BindCommands( TSharedPtr<FUICommandList> ioCommandList )
{
    // To map actions on a newly command list and append it to the parent CommandList, it MUST BE reference as a class member
    // because the Append() stores the new command list as a weak ptr
    //
    // To map actions when a sequencer is opened, try to catch when a sequencer is opened (maybe inside the EposSequenceEditor module ?)
    //
    // To map actions on a more global context, the command list may be created in the EposSequenceEditor module and registered to the level editor like:
    //      TSharedPtr< ILevelEditor > levelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>( "LevelEditor" ).GetFirstLevelEditor();
    //      levelEditor->AppendCommands( CommandList.ToSharedRef() );
    FEposSequenceEditorActionCallbacks::MapActions( ioCommandList );

    //---

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().CreateCameraAtCurrentTime,
        FExecuteAction::CreateLambda( [this](){ BoardSequenceTools::CreateCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
        FCanExecuteAction::CreateLambda( [this](){ return BoardSequenceTools::CanCreateCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().SnapCameraToViewportAtCurrentTime,
        FExecuteAction::CreateLambda( [this](){ BoardSequenceTools::SnapCameraToViewport( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
        FCanExecuteAction::CreateLambda( [this](){ return BoardSequenceTools::CanSnapCameraToViewport( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().PilotCameraAtCurrentTime,
        FExecuteAction::CreateLambda( [this](){ BoardSequenceTools::PilotCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
        FCanExecuteAction::CreateLambda( [this](){ return BoardSequenceTools::CanPilotCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().EjectCameraAtCurrentTime,
        FExecuteAction::CreateLambda( [this](){ BoardSequenceTools::EjectCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
        FCanExecuteAction::CreateLambda( [this](){ return BoardSequenceTools::CanEjectCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoPreviousCameraPosition,
        FExecuteAction::CreateLambda( [this](){ BoardSequenceTools::GotoPreviousCameraPosition( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
        FCanExecuteAction::CreateLambda( [this](){ return BoardSequenceTools::HasPreviousCameraPosition( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoNextCameraPosition,
        FExecuteAction::CreateLambda( [this](){ BoardSequenceTools::GotoNextCameraPosition( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
        FCanExecuteAction::CreateLambda( [this](){ return BoardSequenceTools::HasNextCameraPosition( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
    );

    //---

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().CreatePlaneAtCurrentTime,
        FExecuteAction::CreateLambda( [this](){ BoardSequenceTools::CreatePlane( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
        FCanExecuteAction::CreateLambda( [this](){ return BoardSequenceTools::CanCreatePlane( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().DetachPlaneAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                        {
                                            TArray<FGuid> plane_bindings;
                                            int32 plane_count = BoardSequenceTools::GetAttachedPlanes( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, nullptr, &plane_bindings );
                                            if( plane_count != 1 )
                                                return;
                                            BoardSequenceTools::DetachPlane( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, plane_bindings[0] );
                                        } ),
        FCanExecuteAction::CreateLambda( [this]()
                                            {
                                                int32 plane_count = BoardSequenceTools::GetAttachedPlanes( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber );
                                                if( plane_count > 1 )
                                                {
                                                    FNotificationInfo Info( LOCTEXT( "multiple-planes", "There are multiple planes. Select one of them." ) );
                                                    Info.ExpireDuration = 5.0f;
                                                    FSlateNotificationManager::Get().AddNotification( Info )->SetCompletionState( SNotificationItem::CS_Fail );
                                                }
                                                return plane_count == 1;
                                            } )
    );

    //---

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().CreateDrawingAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                        {
                                            TArray<FGuid> plane_bindings;
                                            int32 plane_count = BoardSequenceTools::GetAllPlanes( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, nullptr, &plane_bindings );
                                            if( plane_count != 1 )
                                                return;
                                            BoardSequenceTools::CreateDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, plane_bindings[0] );
                                        } ),
        FCanExecuteAction::CreateLambda( [this]()
                                            {
                                                TArray<FGuid> plane_bindings;
                                                int32 plane_count = BoardSequenceTools::GetAllPlanes( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, nullptr, &plane_bindings );
                                                if( plane_count > 1 )
                                                {
                                                    FNotificationInfo Info( LOCTEXT( "multiple-planes", "There are multiple planes. Select one of them." ) );
                                                    Info.ExpireDuration = 5.0f;
                                                    FSlateNotificationManager::Get().AddNotification( Info )->SetCompletionState( SNotificationItem::CS_Fail );
                                                }
                                                return plane_count == 1 && BoardSequenceTools::CanCreateDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, plane_bindings[0] );
                                            } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoPreviousDrawing,
        FExecuteAction::CreateLambda( [this](){ BoardSequenceTools::GotoPreviousDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
        FCanExecuteAction::CreateLambda( [this](){ return BoardSequenceTools::HasPreviousDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoNextDrawing,
        FExecuteAction::CreateLambda( [this](){ BoardSequenceTools::GotoNextDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
        FCanExecuteAction::CreateLambda( [this](){ return BoardSequenceTools::HasNextDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().DeactivateAllLighttables,
        FExecuteAction::CreateLambda( [this](){ LighttableTools::Deactivate( mSequencer ); } )
    );
}

//---

FText
FBoardSequenceCustomization::CreateInfoText() const
{
    TSharedRef<INumericTypeInterface<double>> type_interface = mSequencer->GetNumericTypeInterface();

    FMovieSceneSequenceID epos_root_sequence_id;
    const UMovieSceneSequence* root_board = EposSequenceHelpers::GetRootEposSequence( *mSequencer, mSequencer->GetFocusedTemplateID(), epos_root_sequence_id );
    // This should always be valid as we are inside the board customization
    check( root_board && root_board->IsA<UBoardSequence>() );
    const UMovieScene* root_moviescene = root_board ? root_board->GetMovieScene() : nullptr;

    const UBoardSequence* current_board = CastChecked<UBoardSequence>( mSequencer->GetFocusedMovieSceneSequence() );
    const UMovieScene* current_moviescene = current_board ? current_board->GetMovieScene() : nullptr;

    UMovieSceneCinematicBoardTrack* board_track = current_moviescene ? current_moviescene->FindTrack<UMovieSceneCinematicBoardTrack>() : nullptr;
    UMovieSceneSection* board_section = board_track ? MovieSceneHelpers::FindSectionAtTime( board_track->GetAllSections(), mSequencer->GetLocalTime().Time.FrameNumber ) : nullptr;
    UMovieSceneSubSection* board_subsection = Cast<UMovieSceneSubSection>( board_section );

    const UMovieSceneSequence* current_subsequence = board_subsection ? board_subsection->GetSequence() : nullptr;
    const UBoardSequence* current_subboard = Cast<UBoardSequence>( current_subsequence );
    const UShotSequence* current_subshot = Cast<UShotSequence>( current_subsequence );
    const UMovieScene* current_submoviescene = current_subsequence ? current_subsequence->GetMovieScene() : nullptr;

    TRange<FFrameNumber> root_playback_range = root_moviescene ? root_moviescene->GetPlaybackRange() : TRange<FFrameNumber>::Empty();
    TRange<FFrameNumber> sequence_playback_range = current_moviescene ? current_moviescene->GetPlaybackRange() : TRange<FFrameNumber>::Empty();
    TRange<FFrameNumber> subsequence_playback_range = current_submoviescene ? current_submoviescene->GetPlaybackRange() : TRange<FFrameNumber>::Empty();

    FFrameRate root_tick_resolution = root_moviescene->GetTickResolution();
    FFrameRate root_display_rate = root_moviescene->GetDisplayRate();
    FFrameRate sequence_tick_resolution = mSequencer->GetFocusedTickResolution();
    FFrameRate sequence_display_rate = mSequencer->GetFocusedDisplayRate();

    //-

    const UEposSequenceEditorSettings* settings = GetDefault<UEposSequenceEditorSettings>();
    const FInfoBarSettings& infobar_settings = settings->InfoBarSettings;

    FString parsed_string = infobar_settings.Pattern;

    auto ReplaceKeywordInt        = [&]( EInfoBarPatternKeyword iKeywordId, int iValue )          -> FString  { return parsed_string.Replace( *infobar_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *FString::FromInt( iValue ) ); };
    auto ReplaceKeywordIntAsFrame = [&]( EInfoBarPatternKeyword iKeywordId, int iValue )          -> FString  { return parsed_string.Replace( *infobar_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *type_interface->ToString( iValue ) ); };
    auto ReplaceKeywordFrame      = [&]( EInfoBarPatternKeyword iKeywordId, FFrameNumber iValue ) -> FString  { return parsed_string.Replace( *infobar_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *type_interface->ToString( iValue.Value ) ); };
    auto ReplaceKeywordString     = [&]( EInfoBarPatternKeyword iKeywordId, FString iValue )      -> FString  { return parsed_string.Replace( *infobar_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *iValue ); };

    //--- CurrentFrame_InStoryboard
    //--- CurrentFrame_InSequence
    //--- CurrentFrame_InSubsequence
    {
        FFrameNumber current_frame_in_storyboard = EposSequenceHelpers::GetIntermediateTime( *mSequencer, mSequencer->GetGlobalTime(), epos_root_sequence_id ).Time.GetFrame();
        //FFrameNumber current_frame_in_storyboard = mSequencer->GetGlobalTime().Time.GetFrame();
        FFrameNumber current_frame_in_sequence = mSequencer->GetLocalTime().Time.GetFrame();
        FFrameNumber current_frame_in_subsequence = board_subsection ? current_frame_in_sequence - board_subsection->GetInclusiveStartFrame() : FFrameNumber();

        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::CurrentFrame_InStoryboard, current_frame_in_storyboard );
        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::CurrentFrame_InSequence, current_frame_in_sequence );
        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::CurrentFrame_InSubsequence, current_frame_in_subsequence );
    }

    //--- StartFrameOfStoryboard_InStoryboard
    //--- StopFrameOfStoryboard_InStoryboard
    {
        FFrameNumber start_storyboard_in_storyboard = UE::MovieScene::DiscreteInclusiveLower( root_playback_range );
        FFrameNumber stop_storyboard_in_storyboard = UE::MovieScene::DiscreteExclusiveUpper( root_playback_range );
        // convert to display rate -> substract 1 to get the (inclusive) last frame -> convert back to tick resolution
        stop_storyboard_in_storyboard = FFrameRate::TransformTime( FFrameRate::TransformTime( stop_storyboard_in_storyboard, root_tick_resolution, root_display_rate ).FloorToFrame() - 1, root_display_rate, root_tick_resolution ).FloorToFrame();

        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StartFrameOfStoryboard_InStoryboard, start_storyboard_in_storyboard );
        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StopFrameOfStoryboard_InStoryboard, stop_storyboard_in_storyboard );
    }

    //--- StartFrameOfSequence_InStoryboard
    //--- StartFrameOfSequence_InSequence
    //--- StopFrameOfSequence_InStoryboard
    //--- StopFrameOfSequence_InSequence
    {
        FFrameNumber start_sequence_in_sequence = UE::MovieScene::DiscreteInclusiveLower( sequence_playback_range );
        FFrameNumber stop_sequence_in_sequence = UE::MovieScene::DiscreteExclusiveUpper( sequence_playback_range );
        // convert to display rate -> substract 1 to get the (inclusive) last frame -> convert back to tick resolution
        stop_sequence_in_sequence = FFrameRate::TransformTime( FFrameRate::TransformTime( stop_sequence_in_sequence, sequence_tick_resolution, sequence_display_rate ).FloorToFrame() - 1, sequence_display_rate, sequence_tick_resolution ).FloorToFrame();

        FFrameNumber start_sequence_in_storyboard = ( start_sequence_in_sequence * mSequencer->GetFocusedMovieSceneSequenceTransform().InverseNoLooping() ).GetFrame();
        FFrameNumber stop_sequence_in_storyboard = ( stop_sequence_in_sequence * mSequencer->GetFocusedMovieSceneSequenceTransform().InverseNoLooping() ).GetFrame();

        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StartFrameOfSequence_InStoryboard, start_sequence_in_storyboard );
        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StopFrameOfSequence_InStoryboard, stop_sequence_in_storyboard );

        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StartFrameOfSequence_InSequence, start_sequence_in_sequence );
        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StopFrameOfSequence_InSequence, stop_sequence_in_sequence );
    }

    //--- StartFrameOfSubsequence_InStoryboard
    //--- StartFrameOfSubsequence_InSequence
    //--- StartFrameOfSubsequence_InSubSequence
    //--- StopFrameOfSubsequence_InStoryboard
    //--- StopFrameOfSubsequence_InSequence
    //--- StopFrameOfSubsequence_InSubSequence
    {
        FFrameNumber start_subsequence_in_sequence = board_subsection ? board_subsection->GetInclusiveStartFrame() : FFrameNumber();
        FFrameNumber stop_subsequence_in_sequence = board_subsection ? board_subsection->GetExclusiveEndFrame() : FFrameNumber();
        stop_subsequence_in_sequence = FFrameRate::TransformTime( FFrameRate::TransformTime( stop_subsequence_in_sequence, sequence_tick_resolution, sequence_display_rate ).FloorToFrame() - 1, sequence_display_rate, sequence_tick_resolution ).FloorToFrame();

        FFrameNumber start_subsequence_in_subsequence = board_subsection ? start_subsequence_in_sequence - board_subsection->GetInclusiveStartFrame() : FFrameNumber();
        FFrameNumber stop_subsequence_in_subsequence = board_subsection ? stop_subsequence_in_sequence - board_subsection->GetInclusiveStartFrame() : FFrameNumber();

        FFrameNumber start_subsequence_in_storyboard = ( start_subsequence_in_sequence * mSequencer->GetFocusedMovieSceneSequenceTransform().InverseNoLooping() ).GetFrame();
        FFrameNumber stop_subsequence_in_storyboard = ( stop_subsequence_in_sequence * mSequencer->GetFocusedMovieSceneSequenceTransform().InverseNoLooping() ).GetFrame();

        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StartFrameOfSubsequence_InStoryboard, start_subsequence_in_storyboard );
        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StopFrameOfSubsequence_InStoryboard, stop_subsequence_in_storyboard );

        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StartFrameOfSubsequence_InSequence, start_subsequence_in_sequence );
        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StopFrameOfSubsequence_InSequence, stop_subsequence_in_sequence );

        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StartFrameOfSubsequence_InSubSequence, start_subsequence_in_subsequence );
        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StopFrameOfSubsequence_InSubSequence, stop_subsequence_in_subsequence );
    }

    //--- Storyboard_Duration
    //--- Storyboard_TotalSequences
    {
        int32 storyboard_duration_in_tick = UE::MovieScene::DiscreteSize( root_playback_range );

        const FMovieSceneSequenceHierarchy* hierarchy = mSequencer->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( mSequencer->GetEvaluationTemplate().GetCompiledDataID() );
        int32 storyboard_total_sequences = hierarchy ? hierarchy->AllSubSequenceData().Num() : 0;

        parsed_string = ReplaceKeywordIntAsFrame( EInfoBarPatternKeyword::Storyboard_Duration, storyboard_duration_in_tick );
        parsed_string = ReplaceKeywordInt( EInfoBarPatternKeyword::Storyboard_TotalSequences, storyboard_total_sequences );
    }

    //--- Sequence_Duration
    //--- Sequence_NumberOfSubsequences
    //--- Sequence_Index
    //--- Sequence_Name
    {
        int32 sequence_duration_in_tick = UE::MovieScene::DiscreteSize( sequence_playback_range );
        int32 sequence_number_of_sections = board_track ? board_track->GetAllSections().Num() : 0;
        int32 sequence_index = current_board->NameElements.Index;
        FText sequence_name = current_board->GetDisplayName();

        parsed_string = ReplaceKeywordIntAsFrame( EInfoBarPatternKeyword::Sequence_Duration, sequence_duration_in_tick ); //TOCHECK: convert duration like time, correct ?
        parsed_string = ReplaceKeywordInt( EInfoBarPatternKeyword::Sequence_NumberOfSubsequences, sequence_number_of_sections );
        parsed_string = ReplaceKeywordInt( EInfoBarPatternKeyword::Sequence_Index, sequence_index );
        parsed_string = ReplaceKeywordString( EInfoBarPatternKeyword::Sequence_Name, sequence_name.ToString() );
    }

    //--- Subsequence_Duration
    //--- Subsequence_Index
    //--- Subsequence_Name
    {
        int32 subsequence_duration_in_tick = UE::MovieScene::DiscreteSize( subsequence_playback_range );

        int32 subsequence_index = current_subboard ? current_subboard->NameElements.Index : INDEX_NONE;
        if( subsequence_index == INDEX_NONE )
            subsequence_index = current_subshot ? current_subshot->NameElements.Index : INDEX_NONE;

        FText subsequence_name = current_subboard ? current_subboard->GetDisplayName() : FText::GetEmpty();
        if( subsequence_name.IsEmpty() )
            subsequence_name = current_subshot ? current_subshot->GetDisplayName() : FText::GetEmpty();

        parsed_string = ReplaceKeywordIntAsFrame( EInfoBarPatternKeyword::Subsequence_Duration, subsequence_duration_in_tick );
        parsed_string = ReplaceKeywordInt( EInfoBarPatternKeyword::Subsequence_Index, subsequence_index );
        parsed_string = ReplaceKeywordString( EInfoBarPatternKeyword::Subsequence_Name, subsequence_name.ToString() );
    }

    //---

    //USelection* SelectedActors = GEditor->GetSelectedSet( APlaneActor::StaticClass() );
    //TArray<APlaneActor*> selected_planes;
    //SelectedActors->GetSelectedObjects( selected_planes );

    //TArray<FString> planes;
    //for( auto selected_plane : selected_planes )
    //    planes.Add( selected_plane->GetName() );
    //FString planes_list = FString::Join( planes, TEXT( ", " ) );

    //---

    TArray<FString> lines;
    parsed_string.ParseIntoArrayLines( lines );

    FString parsed_string_with_sep = FString::Join( lines, *infobar_settings.Separator );

    return FText::FromString( parsed_string_with_sep );

    //FFormatOrderedArguments args;
    //args.Add( FText::Format( LOCTEXT( "info-bar.duration", "Duration: {0}" ), FText::FromString( duration ) ) );
    //args.Add( FText::Format( LOCTEXT( "info-bar.board-section-count", "Board Sections: {0}" ), number_of_sections ) );
    //if( planes.Num() )
    //    args.Add( FText::Format( LOCTEXT( "info-bar.selected-planes", "Selected Planes: {0}" ), FText::FromString( planes_list ) ) );

    //return FText::Join( FText::FromString( TEXT( " - " ) ), args );
    ////return FText::Format( LOCTEXT( "info-bar", "Duration: {0} - Board Sections: {1} - Selected Planes: [{2}]" ), FText::FromString( duration ), number_of_sections, FText::FromString( planes_list ) );
}

void
FBoardSequenceCustomization::ExtendSequencerToolbar( FToolBarBuilder& ToolbarBuilder )
{
    static const FName sSequencerToolbarStyleName = "SequencerToolbar";

    ToolbarBuilder.BeginStyleOverride( sSequencerToolbarStyleName );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoPreviousCameraPosition );
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoNextCameraPosition );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoPreviousDrawing );
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoNextDrawing );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeSettingsMenu ),
        LOCTEXT( "Settings", "Settings" ),
        LOCTEXT( "SettingsToolTip", "Set settings" ),
        FSlateIcon( FEposSequenceEditorStyle::Get().GetStyleSetName(), "Settings" ) );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeHelpMenu ),
        LOCTEXT( "Help", "Help" ),
        LOCTEXT( "HelpToolTip", "Help" ),
        FSlateIcon( FEposSequenceEditorStyle::Get().GetStyleSetName(), "Help" )  );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddWidget( SNew( SInfoBar )
                              .Padding( FMargin( 10.f, 3.f ) ) // To simulate a space with the previous separator
                              .Text_Raw( this, &FBoardSequenceCustomization::CreateInfoText )
                              .ToolTipText( LOCTEXT( "infobar-tooltip", "Double-click on the infobar to open its pattern settings" ) )
                              .OnDoubleClicked( FPointerEventHandler::CreateLambda( []( const FGeometry&, const FPointerEvent& ) { FEposSequenceEditorActionCallbacks::OpenSequenceEditorSettings(); return FReply::Handled(); } ) )
                              );

    ToolbarBuilder.EndStyleOverride();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeSettingsMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    EposSequenceToolbarHelpers::MakeSettingsEntries( MenuBuilder, mSequencer );

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeHelpMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    EposSequenceToolbarHelpers::MakeHelpEntries( MenuBuilder );

    return MenuBuilder.MakeWidget();
}

//---

TSharedPtr<FExtender>
FBoardSequenceCustomization::CreateObjectBindingContextMenuExtender(UE::Sequencer::FViewModelPtr InViewModel)
{
    TSharedRef<FExtender> Extender = MakeShared<FExtender>();
    TSharedPtr<UE::Sequencer::FObjectBindingModel> ObjectBindingModel = InViewModel->CastThisShared<UE::Sequencer::FObjectBindingModel>();
    Extender->AddMenuExtension(
        "ObjectBindingActions", EExtensionHook::Before, nullptr,
        FMenuExtensionDelegate::CreateRaw(this, &FBoardSequenceCustomization::ExtendObjectBindingContextMenu, ObjectBindingModel));
    return Extender.ToSharedPtr();
}

void
FBoardSequenceCustomization::ExtendObjectBindingContextMenu(FMenuBuilder& MenuBuilder, TSharedPtr<UE::Sequencer::FObjectBindingModel> ObjectBindingModel)
{
    ISequencer* Sequencer = mSequencer;
    //TSharedPtr<ISequencer> Sequencer = WeakSequencer.Pin();
    TSharedPtr<UE::Sequencer::FSequencerEditorViewModel> EditorViewModel = Sequencer->GetViewModel();

    FGuid ObjectBindingID = ObjectBindingModel->GetObjectGuid();
    UMovieScene* MovieScene = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();

    if (!MovieScene || !ObjectBindingID.IsValid())
    {
        return;
    }

    FMovieSceneSpawnable* Spawnable = MovieScene->FindSpawnable(ObjectBindingID);

    if (Spawnable)
    {
        check(!"todo: from FLevelSequenceCustomization in Plugins")
    }
    else
    {
        //MenuBuilder.BeginSection("Possessable");

        //MenuBuilder.AddMenuEntry(FSequencerCommands::Get().ConvertToSpawnable);

        //MenuBuilder.AddSubMenu(
        //    LOCTEXT("DynamicPossession", "Dynamic Possession"),
        //    LOCTEXT("DynamicPossessionTooltip", "Specify a Blueprint method that will find a compatible actor for this binding"),
        //    FNewMenuDelegate::CreateRaw(this, &FLevelSequenceCustomization::AddDynamicPossessionMenu, ObjectBindingModel));

        //MenuBuilder.EndSection();
    }

    MenuBuilder.BeginSection("Import/Export", LOCTEXT("ImportExportMenuSectionName", "Import/Export"));

    MenuBuilder.AddMenuEntry(
        LOCTEXT("ImportFBX", "Import..."),
        LOCTEXT("ImportFBXTooltip", "Import FBX animation to this object"),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateLambda([=] {
                FEposSequenceFBXInterop Interop(EditorViewModel->GetSequencer());
                Interop.ImportFBXOntoSelectedNodes();
                })
        ));

    MenuBuilder.AddMenuEntry(
        LOCTEXT("ExportFBX", "Export..."),
        LOCTEXT("ExportFBXTooltip", "Export FBX animation from this object"),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateLambda([=] {
                FEposSequenceFBXInterop Interop(EditorViewModel->GetSequencer());
                Interop.ExportFBX();
                })
        ));

    MenuBuilder.EndSection();
}

//---

// click
//
// down on antoher frame
// - nothing
//
// up on anther frame
// (playback status stopped)
// - scrub end: old frame
// - time changed: new frame
//
//LogTemp: Warning: scrub end: frame=0 state=0
//LogTemp: Warning: global time changed : frame = 133000 state = 0
//

// drag
//
// down on another frame
// - nothing
//
// dragging
// (playback status scrubbing)
// - scrub begin: old frame
// - time changed: new frame
//
// up (end drag)
// (playback status stopped)
// - scrub end: new frame
//
//LogTemp: Warning: scrub begin: frame=133000 state=2
//LogTemp: Warning: global time changed: frame=162000 state=2
//LogTemp: Warning: global time changed: frame=163000 state=2
//LogTemp: Warning: global time changed: frame=164000 state=2
//LogTemp: Warning: global time changed: frame=165000 state=2
//LogTemp: Warning: global time changed: frame=166000 state=2
//LogTemp: Warning: global time changed: frame=167000 state=2
//LogTemp: Warning: global time changed: frame=168000 state=2
//LogTemp: Warning: global time changed: frame=169000 state=2
//LogTemp: Warning: scrub end: frame=169000 state=0
//

void
FBoardSequenceCustomization::OnGlobalTimeChanged()
{
    //FString prefix = TEXT( "global time changed" );
    //FQualifiedFrameTime frame_time = mSequencer->GetLocalTime();
    //EMovieScenePlayerStatus::Type state = mSequencer->GetPlaybackStatus();

    //UE_LOG( LogTemp, Warning, TEXT( "%s: frame=%d state=%d" ), *prefix, frame_time.Time.GetFrame().Value, state );

    //---

    if( !GCurrentLevelEditingViewportClient  )
        return;

    if( !GCurrentLevelEditingViewportClient->IsAnyActorLocked() )
        return;

    //---

    //FLevelViewportActorLock& lock = GCurrentLevelEditingViewportClient->GetCinematicActorLock();
    //AActor* current_piloted_actor = lock.GetLockedActor(); // return nullptr
    AActor* current_piloted_actor = GCurrentLevelEditingViewportClient->GetActiveActorLock().Get();
    if( !current_piloted_actor )
        return;

    //---

    FFrameNumber current_frame = mSequencer->GetLocalTime().Time.GetFrame();

    ACineCameraActor* camera_at_current_frame = BoardSequenceTools::GetCamera( mSequencer, current_frame );

    if( !camera_at_current_frame )
    {
        //TODO: maybe eject the current piloted actor ?
        // (as at the current frame, now, there is no camera)
        return;
    }

    //---

    if( current_piloted_actor == camera_at_current_frame )
        return;

    //---

    BoardSequenceTools::PilotCamera( mSequencer, current_frame );
}

void
FBoardSequenceCustomization::OnBeginScrubbing()
{
    //FString prefix = TEXT( "scrub begin" );
    //FQualifiedFrameTime frame_time = mSequencer->GetLocalTime();
    //EMovieScenePlayerStatus::Type state = mSequencer->GetPlaybackStatus();

    //UE_LOG( LogTemp, Warning, TEXT( "%s: frame=%d state=%d" ), *prefix, frame_time.Time.GetFrame().Value, state ); // 2=Scrubbing 0=Stopped
}

void
FBoardSequenceCustomization::OnEndScrubbing()
{
    //FString prefix = TEXT( "scrub end" );
    //FQualifiedFrameTime frame_time = mSequencer->GetLocalTime();
    //EMovieScenePlayerStatus::Type state = mSequencer->GetPlaybackStatus();

    //UE_LOG( LogTemp, Warning, TEXT( "%s: frame=%d state=%d" ), *prefix, frame_time.Time.GetFrame().Value, state );
}

//---

void
FBoardSequenceCustomization::OnPreTransformChanged( UObject& InObject )
{
    if( !mSequencer->IsAllowedToChange() )
        return;

    ACineCameraActor* Actor = Cast<ACineCameraActor>( &InObject );
    // If Sequencer is allowed to autokey and we are clicking on an Actor that can't be autokeyed
    if( !Actor || Actor->IsEditorOnly() )
        return;

    USceneComponent* SceneComponentThatChanged = Actor->GetRootComponent();
    check( SceneComponentThatChanged );

    // Cache off the existing transform so we can detect which components have changed
    // and keys only when something has changed
    FTransformData Transform( SceneComponentThatChanged );

    mObjectToExistingTransform.Add( &InObject, Transform );

    // Do not manage track creation as it should already exist, and otherwise, do nothing (for the moment)
}

void
FBoardSequenceCustomization::OnTransformChanged( UObject& InObject )
{
    if( !mSequencer->IsAllowedToChange() )
        return;

    ACineCameraActor* Actor = Cast<ACineCameraActor>( &InObject );
    // If the Actor that just finished transforming doesn't have autokey disabled
    if( !Actor || Actor->IsEditorOnly() )
        return;

    USceneComponent* SceneComponentThatChanged = Actor->GetRootComponent();
    check( SceneComponentThatChanged );

    // Find an existing transform if possible.  If one exists we will compare against the new one to decide what components of the transform need keys
    TOptional<FTransformData> ExistingTransform;
    if( const FTransformData* Found = mObjectToExistingTransform.Find( &InObject ) )
    {
        ExistingTransform = *Found;
    }

    // Remove it from the list of cached transforms.
    // @todo sequencer livecapture: This can be made much for efficient by not removing cached state during live capture situation
    mObjectToExistingTransform.Remove( &InObject );

    // Build new transform data
    FTransformData NewTransformData( SceneComponentThatChanged );

    //---

    BoardSequenceTools::StopPilotingCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, Actor, ExistingTransform, NewTransformData );
}

void
FBoardSequenceCustomization::OnPrePropertyChanged( UObject* InObject, const FEditPropertyChain& InPropertyChain )
{
    FProperty* PropertyAboutToChange = InPropertyChain.GetActiveMemberNode()->GetValue();
    const FName MemberPropertyName = PropertyAboutToChange != nullptr ? PropertyAboutToChange->GetFName() : NAME_None;
    const bool bTransformationToChange =
        ( MemberPropertyName == USceneComponent::GetRelativeLocationPropertyName() ||
          MemberPropertyName == USceneComponent::GetRelativeRotationPropertyName() ||
          MemberPropertyName == USceneComponent::GetRelativeScale3DPropertyName() );

    if( InObject && bTransformationToChange )
    {
        OnPreTransformChanged( *InObject );
    }
}

void
FBoardSequenceCustomization::OnPostPropertyChanged( UObject* InObject, FPropertyChangedEvent& InPropertyChangedEvent )
{
    const FName MemberPropertyName = InPropertyChangedEvent.MemberProperty != nullptr ? InPropertyChangedEvent.MemberProperty->GetFName() : NAME_None;
    const bool bTransformationChanged =
        ( MemberPropertyName == USceneComponent::GetRelativeLocationPropertyName() ||
          MemberPropertyName == USceneComponent::GetRelativeRotationPropertyName() ||
          MemberPropertyName == USceneComponent::GetRelativeScale3DPropertyName() );

    if( InObject && bTransformationChanged )
    {
        OnTransformChanged( *InObject );
    }
}

//---

//bool
//FBoardSequenceCustomization::OnSequencerReceiveDragOver( const FGeometry& iGeometry, const FDragDropEvent& iEvent, FReply& oReply )
//{
//    oReply = FReply::Unhandled();
//
//    return false;
//}
//
//bool
//FBoardSequenceCustomization::OnSequencerReceiveDrop( const FGeometry& iGeometry, const FDragDropEvent& iEvent, FReply& oReply )
//{
//    oReply = FReply::Unhandled();
//
//    return false;
//}

ESequencerDropResult
FBoardSequenceCustomization::OnSequencerAssetsDrop( const TArray<UObject*>& iAssets, const FAssetDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::Unhandled; // Process the default behavior for assets
}

ESequencerDropResult
FBoardSequenceCustomization::OnSequencerClassesDrop( const TArray<TWeakObjectPtr<UClass>>& iClasses, const FClassDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::DropDenied;    // Don't accept classes
}

ESequencerDropResult
FBoardSequenceCustomization::OnSequencerActorsDrop( const TArray<TWeakObjectPtr<AActor>>& iActors, const FActorDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::Unhandled; // Process the default behavior for actors
}

#undef LOCTEXT_NAMESPACE
