// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Board/BoardSequenceCustomization.h"

#include "CineCameraActor.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "EditorModeManager.h"
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
#include "OdysseyAnimationActor.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposSequenceEditorStyle.h"
#include "Styles/EposTracksEditorStyle.h"
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

    mWeakSequencer = ioBuilder.GetSequencer().AsShared();
    // From 5.3, the registration is only done if the new focused sequence is NOT the same type than the previous one
    // It means that:
    // - going from board to inner board, NOT called
    // - going from board to inner shot, called
    // - going from shot to upper board, called
    mBoardSequence = Cast<UBoardSequence>( &ioBuilder.GetFocusedSequence() );

    //---

    // Listen for actor/component movement
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw( this, &FBoardSequenceCustomization::OnPrePropertyChanged );
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw( this, &FBoardSequenceCustomization::OnPostPropertyChanged );

    mBoardCommandList = MakeShared<FUICommandList>();

    BindCommands( mBoardCommandList );

    mWeakSequencer.Pin()->GetCommandBindings()->Append( mBoardCommandList.ToSharedRef() );

    TSharedPtr< ILevelEditor > levelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>( "LevelEditor" ).GetFirstLevelEditor();
    levelEditor->AppendCommands( mBoardCommandList.ToSharedRef() );

    //---

    FSequencerCustomizationInfo customization;

    TSharedRef<FExtender> ToolbarExtender = MakeShared<FExtender>();
    ToolbarExtender->AddToolBarExtension( "CurveEditor", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw( this, &FBoardSequenceCustomization::ExtendSequencerToolbar ) );
    customization.ToolbarExtender = ToolbarExtender;

    customization.OnBuildObjectBindingContextMenu = FOnGetSequencerMenuExtender::CreateRaw(this, &FBoardSequenceCustomization::CreateObjectBindingContextMenuExtender);
    //customization.OnBuildSidebarMenu = FOnGetSequencerMenuExtender::CreateRaw( this, &FBoardSequenceCustomization::CreateObjectBindingSidebarMenuExtender );

    //customization.OnReceivedDragOver.BindRaw( this, &FBoardSequenceCustomization::OnSequencerReceiveDragOver );
    //customization.OnReceivedDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerReceiveDrop );

    customization.OnAssetsDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerAssetsDrop );
    customization.OnClassesDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerClassesDrop );
    customization.OnActorsDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerActorsDrop );

    ioBuilder.AddCustomization( customization );

    UEposSequenceEditorBlueprintLibrary::SetSequencer( mWeakSequencer.Pin()->AsShared() );
    UEposNamingConventionBlueprintLibrary::SetSequencer( mWeakSequencer.Pin()->AsShared() );

    mSequencerActorAddedDelegates = mWeakSequencer.Pin()->OnActorAddedToSequencer().AddStatic( &ToolkitHelpers::HandleActorAddedToSequencer, mWeakSequencer.Pin().Get() );
    mSequencerActivatedDelegates = mWeakSequencer.Pin()->OnActivateSequence().AddStatic( &ToolkitHelpers::HandleOnActivateSequence, mWeakSequencer.Pin().Get() );
    mSequencerSelectionSectionChangedDelegates = mWeakSequencer.Pin()->GetSelectionChangedSections().AddStatic( &ToolkitHelpers::HandleOnSelectionChangedSections, mWeakSequencer.Pin().Get() );

    mSequencerBeginScrubbingDelegates = mWeakSequencer.Pin()->OnBeginScrubbingEvent().AddRaw( this, &FBoardSequenceCustomization::OnBeginScrubbing );
    mSequencerEndScrubbingDelegates = mWeakSequencer.Pin()->OnEndScrubbingEvent().AddRaw( this, &FBoardSequenceCustomization::OnEndScrubbing );
    mSequencerGlobalTimeChangedDelegates = mWeakSequencer.Pin()->OnGlobalTimeChanged().AddRaw( this, &FBoardSequenceCustomization::OnGlobalTimeChanged );

    FLevelEditorModule& levelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( "LevelEditor" );
    mSelectionChangedDelegates = levelEditorModule.OnActorSelectionChanged().AddRaw( this, &FBoardSequenceCustomization::OnObjectSelectedMulti );
    //ULevelEditorSubsystem& subsystem = GEditor->GetSubsystem<ULevelEditorSubsystem>();
}

void
FBoardSequenceCustomization::UnregisterSequencerCustomization()
{
    sgRegistrationCountDebug--;
    check( sgRegistrationCountDebug == 0 );

    FCoreUObjectDelegates::OnPreObjectPropertyChanged.RemoveAll( this );
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll( this );

    mBoardCommandList = nullptr;

    if( mWeakSequencer.IsValid() )
    {
        mWeakSequencer.Pin()->OnActorAddedToSequencer().Remove( mSequencerActorAddedDelegates );
        mWeakSequencer.Pin()->OnActivateSequence().Remove( mSequencerActivatedDelegates );
        mWeakSequencer.Pin()->GetSelectionChangedSections().Remove( mSequencerSelectionSectionChangedDelegates );

        mWeakSequencer.Pin()->OnBeginScrubbingEvent().Remove( mSequencerBeginScrubbingDelegates );
        mWeakSequencer.Pin()->OnBeginScrubbingEvent().Remove( mSequencerEndScrubbingDelegates );
        mWeakSequencer.Pin()->OnGlobalTimeChanged().Remove( mSequencerGlobalTimeChangedDelegates );
    }

    FLevelEditorModule& levelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( "LevelEditor" );
    levelEditorModule.OnActorSelectionChanged().Remove( mSelectionChangedDelegates );

    mWeakSequencer = nullptr;
    mBoardSequence = nullptr;
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
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          BoardSequenceTools::CreateCameraWithAnimation( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return BoardSequenceTools::CanCreateCamera( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().SnapCameraToViewportAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          BoardSequenceTools::SnapCameraToViewport( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return BoardSequenceTools::CanSnapCameraToViewport( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().PilotCameraAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          BoardSequenceTools::PilotCamera( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return BoardSequenceTools::CanPilotCamera( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().EjectCameraAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          BoardSequenceTools::EjectCamera( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return BoardSequenceTools::CanEjectCamera( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoPreviousCameraPosition,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          BoardSequenceTools::GotoPreviousCameraPosition( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return BoardSequenceTools::HasPreviousCameraPosition( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoNextCameraPosition,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          BoardSequenceTools::GotoNextCameraPosition( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return BoardSequenceTools::HasNextCameraPosition( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    //---

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().CreateAnimationAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          BoardSequenceTools::CreateAnimation( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return BoardSequenceTools::CanCreateAnimation( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().DetachAnimationAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          TArray<FGuid> animation_bindings;
                                          int32 animation_count = BoardSequenceTools::GetAttachedAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, nullptr, &animation_bindings );
                                          if( animation_count != 1 )
                                              return;
                                          BoardSequenceTools::DetachAnimation( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, animation_bindings[0] );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             int32 animation_count = BoardSequenceTools::GetAttachedAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                             if( animation_count > 1 )
                                             {
                                                 FNotificationInfo Info( LOCTEXT( "multiple-animations", "There are multiple animations. Select one of them." ) );
                                                 Info.ExpireDuration = 5.0f;
                                                 FSlateNotificationManager::Get().AddNotification( Info )->SetCompletionState( SNotificationItem::CS_Fail );
                                             }
                                             if( animation_count != 1 )
                                                 return false;
                                             return true;
                                         } ),
        FIsActionChecked(),
        FIsActionButtonVisible::CreateLambda( [this]()
                                              {
                                                  TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                  if( !sequencer )
                                                      return false;
                                                  return BoardSequenceTools::GetAttachedAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber ) <= 1;
                                              } )
    );

    //---

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().CreateAnimationCutAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          TArray<FGuid> animation_bindings;
                                          int32 animation_count = BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, nullptr, &animation_bindings );
                                          if( animation_count != 1 )
                                              return;
                                          BoardSequenceTools::CreateAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, animation_bindings[0] );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             TArray<FGuid> animation_bindings;
                                             int32 animation_count = BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, nullptr, &animation_bindings );
                                             if( animation_count > 1 )
                                             {
                                                 FNotificationInfo Info( LOCTEXT( "multiple-animations", "There are multiple animations. Select one of them." ) );
                                                 Info.ExpireDuration = 5.0f;
                                                 FSlateNotificationManager::Get().AddNotification( Info )->SetCompletionState( SNotificationItem::CS_Fail );
                                             }
                                             if( animation_count != 1 )
                                                 return false;
                                             return BoardSequenceTools::CanCreateAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, animation_bindings[0] );
                                         } ),
        FIsActionChecked(),
        FIsActionButtonVisible::CreateLambda( [this]()
                                              {
                                                  TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                  if( !sequencer )
                                                      return false;
                                                  return BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber ) <= 1;
                                              } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoPreviousAnimationCut,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          BoardSequenceTools::GotoPreviousAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return BoardSequenceTools::HasPreviousAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoNextAnimationCut,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          BoardSequenceTools::GotoNextAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return BoardSequenceTools::HasNextAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().DeactivateAllLighttables,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          LighttableTools::Deactivate( sequencer.Get() );
                                      } )
    );
}

//---

FText
FBoardSequenceCustomization::CreateInfoText() const
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
    if( !sequencer )
        return FText::GetEmpty();

    TSharedRef<INumericTypeInterface<double>> type_interface = sequencer->GetNumericTypeInterface().ToSharedRef();

    FMovieSceneSequenceID epos_root_sequence_id;
    const UMovieSceneSequence* root_board = EposSequenceHelpers::GetRootEposSequence( *sequencer, sequencer->GetFocusedTemplateID(), epos_root_sequence_id );
    // This should always be valid as we are inside the board customization
    check( root_board && root_board->IsA<UBoardSequence>() );
    const UMovieScene* root_moviescene = root_board ? root_board->GetMovieScene() : nullptr;

    const UBoardSequence* current_board = CastChecked<UBoardSequence>( sequencer->GetFocusedMovieSceneSequence() );
    const UMovieScene* current_moviescene = current_board ? current_board->GetMovieScene() : nullptr;

    UMovieSceneCinematicBoardTrack* board_track = current_moviescene ? current_moviescene->FindTrack<UMovieSceneCinematicBoardTrack>() : nullptr;
    UMovieSceneSection* board_section = board_track ? MovieSceneHelpers::FindSectionAtTime( board_track->GetAllSections(), sequencer->GetLocalTime().Time.FrameNumber ) : nullptr;
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
    FFrameRate sequence_tick_resolution = sequencer->GetFocusedTickResolution();
    FFrameRate sequence_display_rate = sequencer->GetFocusedDisplayRate();

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
        FFrameNumber current_frame_in_storyboard = EposSequenceHelpers::GetIntermediateTime( *sequencer, sequencer->GetGlobalTime(), epos_root_sequence_id ).Time.GetFrame();
        //FFrameNumber current_frame_in_storyboard = sequencer->GetGlobalTime().Time.GetFrame();
        FFrameNumber current_frame_in_sequence = sequencer->GetLocalTime().Time.GetFrame();
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

        FMovieSceneInverseSequenceTransform localToRootTransform = sequencer->GetFocusedMovieSceneSequenceTransform().Inverse();

        TOptional<FFrameTime> start_sequence_in_storyboard = localToRootTransform.TryTransformTime( start_sequence_in_sequence );
        if( start_sequence_in_storyboard )
            parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StartFrameOfSequence_InStoryboard, start_sequence_in_storyboard->GetFrame() );
        else
            parsed_string = ReplaceKeywordString( EInfoBarPatternKeyword::StartFrameOfSequence_InStoryboard, TEXT( "xxx" ) );

        TOptional<FFrameTime> stop_sequence_in_storyboard = localToRootTransform.TryTransformTime( stop_sequence_in_sequence );
        if( stop_sequence_in_storyboard )
            parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StopFrameOfSequence_InStoryboard, stop_sequence_in_storyboard->GetFrame() );
        else
            parsed_string = ReplaceKeywordString( EInfoBarPatternKeyword::StopFrameOfSequence_InStoryboard, TEXT( "xxx" ) );

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

        FMovieSceneInverseSequenceTransform localToRootTransform = sequencer->GetFocusedMovieSceneSequenceTransform().Inverse();

        TOptional<FFrameTime> start_subsequence_in_storyboard = localToRootTransform.TryTransformTime( start_subsequence_in_sequence );
        if( start_subsequence_in_storyboard )
            parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StartFrameOfSubsequence_InStoryboard, start_subsequence_in_storyboard->GetFrame() );
        else
            parsed_string = ReplaceKeywordString( EInfoBarPatternKeyword::StartFrameOfSubsequence_InStoryboard, TEXT( "xxx" ) );

        TOptional<FFrameTime> stop_subsequence_in_storyboard = localToRootTransform.TryTransformTime( stop_subsequence_in_sequence );
        if( stop_subsequence_in_storyboard )
            parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StopFrameOfSubsequence_InStoryboard, stop_subsequence_in_storyboard->GetFrame() );
        else
            parsed_string = ReplaceKeywordString( EInfoBarPatternKeyword::StopFrameOfSubsequence_InStoryboard, TEXT( "xxx" ) );

        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StartFrameOfSubsequence_InSequence, start_subsequence_in_sequence );
        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StopFrameOfSubsequence_InSequence, stop_subsequence_in_sequence );

        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StartFrameOfSubsequence_InSubSequence, start_subsequence_in_subsequence );
        parsed_string = ReplaceKeywordFrame( EInfoBarPatternKeyword::StopFrameOfSubsequence_InSubSequence, stop_subsequence_in_subsequence );
    }

    //--- Storyboard_Duration
    //--- Storyboard_TotalSequences
    {
        int32 storyboard_duration_in_tick = UE::MovieScene::DiscreteSize( root_playback_range );

        const FMovieSceneSequenceHierarchy* hierarchy = sequencer->GetSharedPlaybackState()->GetHierarchy();
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

static
const FText&
GetLighttableWarning()
{
    static FText warning;
    warning = FText::GetEmpty();

    if( !GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ) )
        warning = LOCTEXT( "warning-no-odyssey-edmode-tooltip", "\n\nWarning: Lighttable is only visible when Odyssey Mode is active" );

    return warning;
}

void
FBoardSequenceCustomization::ExtendSequencerToolbar( FToolBarBuilder& ToolbarBuilder )
{
    static const FName sSequencerToolbarStyleName = "SequencerToolbar";

    ToolbarBuilder.BeginStyleOverride( sSequencerToolbarStyleName );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().CreateCameraAtCurrentTime );
    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeCameraMenu ),
        LOCTEXT( "CameraOptions", "Options" ),
        LOCTEXT( "CameraOptionsToolTip", "Camera Options" ),
        TAttribute<FSlateIcon>(),
        true );
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().SnapCameraToViewportAtCurrentTime );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().CreateAnimationAtCurrentTime );
    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeAnimationSettingsMenu ),
        LOCTEXT( "AnimationOptions", "Options" ),
        LOCTEXT( "AnimationOptionsToolTip", "Animation Options" ),
        TAttribute<FSlateIcon>(),
        true );
    // The 2 following buttons should be exclusive visible:
    // - the first button is displayed when there is only 1 animation (or 0) available
    // - the second button is displayed when there are more than 2 animations available
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().DetachAnimationAtCurrentTime );
    ToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this]()
                                                  {
                                                      TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                      if( !sequencer )
                                                          return false;
                                                      return BoardSequenceTools::GetAttachedAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber ) > 1;
                                                  } )
        ),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeAnimationMenu ),
        FEposSequenceEditorCommands::Get().DetachAnimationAtCurrentTime->GetLabel(),
        FEposSequenceEditorCommands::Get().DetachAnimationAtCurrentTime->GetDescription(),
        FEposSequenceEditorCommands::Get().DetachAnimationAtCurrentTime->GetIcon() );

    auto GetLighttableTooltip = [this]() -> FText
        {
            TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
            if( !sequencer )
                return FText::Format( LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable{0}" ), GetLighttableWarning() );

            TArray<FGuid> animation_bindings;
            int32 animation_count = BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, nullptr, &animation_bindings );
            //check( animation_count == 1 );
            if( animation_count != 1 )
                return FText::Format( LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable{0}" ), GetLighttableWarning() );

            BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), sequencer->GetLocalTime().Time.FrameNumber );
            if( !result.mInnerSequence )
                return FText::Format( LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable{0}" ), GetLighttableWarning() );

            UMovieSceneSubSection* subsection = sequencer->FindSubSection( result.mInnerSequenceId );
            if( !subsection )
                return FText::Format( LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable{0}" ), GetLighttableWarning() );

            if( LighttableTools::GetState( sequencer.Get(), *subsection, animation_bindings[0] ) != 0 )
                return FText::Format( LOCTEXT( "disable-lighttable-tooltip", "Disable the lighttable{0}" ), GetLighttableWarning() );
            else
                return FText::Format( LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable{0}" ), GetLighttableWarning() );
        };

    auto GetLighttableIcon = [this]() -> FSlateIcon
        {
            TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
            if( !sequencer )
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );

            TArray<FGuid> animation_bindings;
            int32 animation_count = BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, nullptr, &animation_bindings );
            //check( animation_count == 1 );
            if( animation_count != 1 )
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );

            BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), sequencer->GetLocalTime().Time.FrameNumber );
            if( !result.mInnerSequence )
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );

            UMovieSceneSubSection* subsection = sequencer->FindSubSection( result.mInnerSequenceId );
            if( !subsection )
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );

            if( LighttableTools::GetState( sequencer.Get(), *subsection, animation_bindings[0] ) != 0 )
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOn" );
            else
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );
        };

    // The 2 following buttons should be exclusive visible:
    // - the first button is displayed when there is only 1 animation (or 0) available
    // - the second button is displayed when there are more than 2 animations available
    ToolbarBuilder.AddToolBarButton( FUIAction(
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;

                                          TArray<FGuid> animation_bindings;
                                          int32 animation_count = BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, nullptr, &animation_bindings );
                                          if( animation_count != 1 )
                                              return;

                                          BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), sequencer->GetLocalTime().Time.FrameNumber );
                                          if( !result.mInnerSequence )
                                              return;

                                          UMovieSceneSubSection* subsection = sequencer->FindSubSection( result.mInnerSequenceId );
                                          if( !subsection )
                                              return;

                                          if( LighttableTools::GetState( sequencer.Get(), *subsection, animation_bindings[0] ) != 0 )
                                              LighttableTools::Deactivate( sequencer.Get(), *subsection, animation_bindings[0] );
                                          else
                                              LighttableTools::Activate( sequencer.Get(), *subsection, animation_bindings[0] );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;

                                             if( !GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ) )
                                                 return false;

                                             return BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber ) == 1;
                                         } ),
        FIsActionChecked(),
        FIsActionButtonVisible::CreateLambda( [this]()
                                              {
                                                  TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                  if( !sequencer )
                                                      return false;

                                                  return BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber ) <= 1;
                                              } ) ),
        NAME_None,
        FText::GetEmpty(),
        MakeAttributeLambda( GetLighttableTooltip ),
        MakeAttributeLambda( GetLighttableIcon )
    );
    auto GetLighttableMultiTooltip = [this]() -> FText
        {
            return FText::Format( LOCTEXT( "LighttableOptionsTooltip", "Activate/Deactivate lighttable on animations{0}" ), GetLighttableWarning() );
        };

    ToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction::CreateLambda( [this]()
                                             {
                                                 if( !GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ) )
                                                     return false;

                                                 return true;
                                             } ),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this]()
                                                  {
                                                      TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                      if( !sequencer )
                                                          return false;

                                                      return BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber ) > 1;
                                                  } )
        ),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeLighttableMenu ),
        FText::GetEmpty(),
        MakeAttributeLambda( GetLighttableMultiTooltip ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" ) );

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoPreviousCameraPosition );
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoNextCameraPosition );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoPreviousAnimationCut );
    // The 2 following buttons should be exclusive visible:
    // - the first button is displayed when there is only 1 animation (or 0) available
    // - the second button is displayed when there are more than 2 animations available
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().CreateAnimationCutAtCurrentTime );
    ToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this]()
                                                  {
                                                      TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                      if( !sequencer )
                                                          return false;

                                                      return BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber ) > 1;
                                                  } )
        ),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeAnimationCutMenu ),
        FEposSequenceEditorCommands::Get().CreateAnimationCutAtCurrentTime->GetLabel(),
        FEposSequenceEditorCommands::Get().CreateAnimationCutAtCurrentTime->GetDescription(),
        FEposSequenceEditorCommands::Get().CreateAnimationCutAtCurrentTime->GetIcon() );
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoNextAnimationCut );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeSettingsMenu ),
        LOCTEXT( "Settings", "Settings" ),
        LOCTEXT( "SettingsToolTip", "Set settings" ),
        FSlateIcon( FEposSequenceEditorStyle::Get().GetStyleSetName(), "Settings" ) );

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
FBoardSequenceCustomization::MakeCameraMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    EposSequenceToolbarHelpers::MakeCameraSettingsEntries( MenuBuilder );

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeAnimationMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    TArray<AOdysseyAnimationActor*> animations;
    TArray<FGuid> animation_bindings;
    int32 animation_count = BoardSequenceTools::GetAttachedAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, &animations, &animation_bindings );
    if( !animation_count )
        return SNullWidget::NullWidget;

    for( int i = 0; i < animation_count; i++ )
    {
        AOdysseyAnimationActor* animation = animations[i];
        FGuid animation_binding = animation_bindings[i];

        MenuBuilder.AddMenuEntry(
            FText::FromString( animation->GetActorLabel() ),
            FText::GetEmpty(),
            FSlateIcon(),
            FUIAction(
                FExecuteAction::CreateLambda( [this, animation_binding]()
                                              {
                                                  TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                  if( !sequencer )
                                                      return;

                                                  BoardSequenceTools::DetachAnimation( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, animation_binding );
                                              } )
            )
        );
    }

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeLighttableMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    TArray<AOdysseyAnimationActor*> animations;
    TArray<FGuid> animation_bindings;
    int32 animation_count = BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, &animations, &animation_bindings );
    if( !animation_count )
        return SNullWidget::NullWidget;

    for( int i = 0; i < animation_count; i++ )
    {
        AOdysseyAnimationActor* animation = animations[i];
        FGuid animation_binding = animation_bindings[i];

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), sequencer->GetLocalTime().Time.FrameNumber );
        if( !result.mInnerSequence )
            continue;

        UMovieSceneSubSection* subsection = sequencer->FindSubSection( result.mInnerSequenceId );
        if( !subsection )
            continue;

        FText tooltip;
        if( LighttableTools::GetState( sequencer.Get(), *subsection, animation_binding ) != 0 )
            tooltip = FText::Format( LOCTEXT( "disable-lighttable-tooltip", "Disable the lighttable{0}" ), GetLighttableWarning() );
        else
            tooltip = FText::Format( LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable{0}" ), GetLighttableWarning() );

        FSlateIcon icon;
        if( LighttableTools::GetState( sequencer.Get(), *subsection, animation_binding ) != 0 )
            icon = FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOn" );
        else
            icon = FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );

        MenuBuilder.AddMenuEntry(
            FText::FromString( animation->GetActorLabel() ),
            tooltip,
            icon,
            FUIAction(
                FExecuteAction::CreateLambda( [this, animation_binding]()
                                              {
                                                  TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                  if( !sequencer )
                                                      return;

                                                  BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), sequencer->GetLocalTime().Time.FrameNumber );
                                                  if( !result.mInnerSequence )
                                                      return;

                                                  UMovieSceneSubSection* subsection = sequencer->FindSubSection( result.mInnerSequenceId );
                                                  if( !subsection )
                                                      return;

                                                  if( LighttableTools::GetState( sequencer.Get(), *subsection, animation_binding ) != 0 )
                                                      LighttableTools::Deactivate( sequencer.Get(), *subsection, animation_binding );
                                                  else
                                                      LighttableTools::Activate( sequencer.Get(), *subsection, animation_binding );
                                              } )
            )
        );
    }

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeAnimationCutMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    TArray<AOdysseyAnimationActor*> animations;
    TArray<FGuid> animation_bindings;
    int32 animation_count = BoardSequenceTools::GetAllAnimations( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, &animations, &animation_bindings );
    if( !animation_count )
        return SNullWidget::NullWidget;

    for( int i = 0; i < animation_count; i++ )
    {
        AOdysseyAnimationActor* animation = animations[i];
        FGuid animation_binding = animation_bindings[i];

        MenuBuilder.AddMenuEntry(
            FText::FromString( animation->GetActorLabel() ),
            FText::GetEmpty(),
            //LOCTEXT( "LockPlayback_Description", "When enabled, causes all runtime evaluation and the engine FPS to be locked to the current display frame rate" ),
            FSlateIcon(),
            FUIAction(
                FExecuteAction::CreateLambda( [this, animation_binding]()
                                              {
                                                  TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                  if( !sequencer )
                                                      return;

                                                  BoardSequenceTools::CreateAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, animation_binding );
                                              } ),
                FCanExecuteAction::CreateLambda( [this, animation_binding]()
                                                 {
                                                     TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                     if( !sequencer )
                                                         return false;

                                                     return BoardSequenceTools::CanCreateAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, animation_binding );
                                                 } )
            )/*,
            NAME_None,
            EUserInterfaceActionType::ToggleButton*/ //TODO: I don't know how, but there should be something to multi-select animations and create animation on them
        );
    }

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeSettingsMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    EposSequenceToolbarHelpers::MakeSettingsEntries( MenuBuilder, sequencer.Get() );

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeAnimationSettingsMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    EposSequenceToolbarHelpers::MakeAnimationSettingsEntries( MenuBuilder );

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
    TSharedPtr<ISequencer> Sequencer = mWeakSequencer.Pin();
    TSharedPtr<UE::Sequencer::FSequencerEditorViewModel> EditorViewModel = Sequencer->GetViewModel();

    FGuid ObjectBindingID = ObjectBindingModel->GetObjectGuid();
    UMovieSceneSequence* Sequence = Sequencer->GetFocusedMovieSceneSequence();
    UMovieScene* MovieScene = Sequence->GetMovieScene();

    if (!MovieScene || !ObjectBindingID.IsValid())
    {
        return;
    }

    //...

    MenuBuilder.BeginSection("Import/Export", LOCTEXT("ImportExportMenuSectionName", "Import/Export"));

    MenuBuilder.AddMenuEntry(
        LOCTEXT( "ImportFBX", "Import..." ),
        LOCTEXT( "ImportFBXTooltip", "Import FBX animation to this object" ),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateLambda( [this]
                                          {
                                              const TSharedPtr<ISequencer> Sequencer = mWeakSequencer.Pin();
                                              if( !Sequencer.IsValid() )
                                              {
                                                  return;
                                              }

                                              FEposSequenceFBXInterop Interop( Sequencer.ToSharedRef() );
                                              Interop.ImportFBXOntoSelectedNodes();
                                          } )
        ) );

    MenuBuilder.AddMenuEntry(
        LOCTEXT( "ExportFBX", "Export..." ),
        LOCTEXT( "ExportFBXTooltip", "Export FBX animation from this object" ),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateLambda( [this]
                                          {
                                              const TSharedPtr<ISequencer> Sequencer = mWeakSequencer.Pin();
                                              if( !Sequencer.IsValid() )
                                              {
                                                  return;
                                              }

                                              FEposSequenceFBXInterop Interop( Sequencer.ToSharedRef() );
                                              Interop.ExportFBX();
                                          } )
        ) );

    MenuBuilder.EndSection();
}

//TSharedPtr<FExtender> FBoardSequenceCustomization::CreateObjectBindingSidebarMenuExtender( FViewModelPtr InViewModel )
//{
//    TSharedRef<FExtender> Extender = MakeShared<FExtender>();
//
//    TSharedPtr<FObjectBindingModel> ObjectBindingModel = InViewModel->CastThisShared<FObjectBindingModel>();
//
//    Extender->AddMenuExtension( TEXT( "ObjectBindingActions" ), EExtensionHook::Before, nullptr,
//                                FMenuExtensionDelegate::CreateRaw( this, &FBoardSequenceCustomization::ExtendObjectBindingSidebarMenu, ObjectBindingModel ) );
//
//    return Extender.ToSharedPtr();
//}
//
//void FBoardSequenceCustomization::ExtendObjectBindingSidebarMenu( FMenuBuilder& MenuBuilder, TSharedPtr<FObjectBindingModel> ObjectBindingModel )
//{
//    ExtendObjectBindingContextMenu( MenuBuilder, ObjectBindingModel );
//}

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
FBoardSequenceCustomization::OnObjectSelectedMulti( const TArray<UObject*>& iObjects, bool bForceRefresh )
{
    // Store an history of selected actors to know later (when scrubbing) which actor best fit the auto-selection
    for( UObject* object : iObjects )
    {
        ACineCameraActor* camera = Cast<ACineCameraActor>( object );
        if( camera )
            BoardSequenceTools::AddSelectedActorToHistory( camera );

        AOdysseyAnimationActor* animation = Cast<AOdysseyAnimationActor>( object );
        if( animation )
            BoardSequenceTools::AddSelectedActorToHistory( animation );
    }
}

void
FBoardSequenceCustomization::OnGlobalTimeChanged()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    EMovieScenePlayerStatus::Type playback_state = sequencer->GetPlaybackStatus();

    //FString prefix = TEXT( "global time changed" );
    //FQualifiedFrameTime frame_time = sequencer->GetLocalTime();
    //UE_LOG( LogTemp, Warning, TEXT( "%s: frame=%d state=%d" ), *prefix, frame_time.Time.GetFrame().Value, playback_state );

    // Don't auto-select an actor if currently play or scrub because changing the actor in Odyssey mode is not instant
    // So just change the actor at the end of play/scrub
    if( playback_state != EMovieScenePlayerStatus::Playing && playback_state != EMovieScenePlayerStatus::Scrubbing )
    {
        UMovieScene* moviescene = sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();
        UMovieSceneCinematicBoardTrack* board_track = moviescene ? moviescene->FindTrack<UMovieSceneCinematicBoardTrack>() : nullptr;
        UMovieSceneSection* section = board_track ? MovieSceneHelpers::FindSectionAtTime( board_track->GetAllSections(), sequencer->GetLocalTime().Time.FloorToFrame() ) : nullptr;
        UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );

        //USelection* selection = GEditor->GetSelectedActors();
        //TArray<AActor*> actor_selected;
        //selection->GetSelectedObjects<AActor>( actor_selected );

        TArray<UMovieSceneTrack*> selected_tracks;
        sequencer->GetSelectedTracks( selected_tracks );
        TArray<TPair<UMovieSceneTrack*, int32>> selected_track_rows;
        sequencer->GetSelectedTrackRows( selected_track_rows );
        TArray<UMovieSceneFolder*> selected_folders;
        sequencer->GetSelectedFolders( selected_folders );
        TArray<UMovieSceneSection*> selected_sections;
        sequencer->GetSelectedSections( selected_sections );
        TArray<const IKeyArea*> selected_key_areas;
        sequencer->GetSelectedKeyAreas( selected_key_areas );
        TArray<FGuid> selected_bindings;
        sequencer->GetSelectedObjects( selected_bindings );

        // Check if something is already selected by the sequencer
        bool nothing_selected = selected_tracks.IsEmpty()
            && selected_track_rows.IsEmpty()
            && selected_folders.IsEmpty()
            && selected_sections.IsEmpty()
            && selected_key_areas.IsEmpty()
            && selected_bindings.IsEmpty();

        // if nothing is selected by the sequencer, always select an actor in the board track
        // if something is already selected in the board track (track or section), keep continuing auto-selecting in the board track
        // if something is already selected outside the board track, don't auto-select in the board track
        if( nothing_selected
            || selected_tracks.Contains( board_track )
            || selected_sections.ContainsByPredicate( [board_track]( const UMovieSceneSection* iSection )
                                                      {
                                                          return board_track->GetAllSections().Contains( iSection );
                                                      } ) )
        {
            AActor* actor_to_select = BoardSequenceTools::GuessActorToSelect( sequencer.Get(), sequencer->GetLocalTime().Time.FloorToFrame() );
            if( actor_to_select )
            {
                sequencer->EmptySelection();
                sequencer->SelectSection( subsection );

                GEditor->SelectNone( true /*bNoteSelectionChange*/, true /*bDeselectBSPSurfs*/ );
                // Do not notify, otherwise it lags
                // But as we filter on NOT scrubbing and NOT playing, we can enable notification
                GEditor->SelectActor( actor_to_select, true /*bInSelected*/, true /*bNotify*/, true /*bSelectEvenIfHidden*/ );
            }
            else
            {
                sequencer->EmptySelection();
                GEditor->SelectNone( true /*bNoteSelectionChange*/, true /*bDeselectBSPSurfs*/ );
            }
        }
    }

    //---

    if( GCurrentLevelEditingViewportClient && GCurrentLevelEditingViewportClient->IsAnyActorLocked() )
    {
        //FLevelViewportActorLock& lock = GCurrentLevelEditingViewportClient->GetCinematicActorLock();
        //AActor* current_piloted_actor = lock.GetLockedActor(); // return nullptr
        AActor* current_piloted_actor = GCurrentLevelEditingViewportClient->GetActiveActorLock().Get();
        if( current_piloted_actor )
        {
            FFrameNumber current_frame = sequencer->GetLocalTime().Time.GetFrame();

            ACineCameraActor* camera_at_current_frame = BoardSequenceTools::GetCamera( sequencer.Get(), current_frame );

            if( camera_at_current_frame )
            {
                if( current_piloted_actor != camera_at_current_frame )
                    BoardSequenceTools::PilotCamera( sequencer.Get(), current_frame );
            }
            else
            {
                //TODO: maybe eject the current piloted actor ?
                // (as at the current frame, now, there is no camera)
            }
        }
    }
}

void
FBoardSequenceCustomization::OnBeginScrubbing()
{
    //FString prefix = TEXT( "scrub begin" );
    //FQualifiedFrameTime frame_time = mWeakSequencer.Pin()->GetLocalTime();
    //EMovieScenePlayerStatus::Type state = mWeakSequencer.Pin()->GetPlaybackStatus();

    //UE_LOG( LogTemp, Warning, TEXT( "%s: frame=%d state=%d" ), *prefix, frame_time.Time.GetFrame().Value, state ); // 2=Scrubbing 0=Stopped
}

void
FBoardSequenceCustomization::OnEndScrubbing()
{
    //FString prefix = TEXT( "scrub end" );
    //FQualifiedFrameTime frame_time = mWeakSequencer.Pin()->GetLocalTime();
    //EMovieScenePlayerStatus::Type state = mWeakSequencer.Pin()->GetPlaybackStatus();

    //UE_LOG( LogTemp, Warning, TEXT( "%s: frame=%d state=%d" ), *prefix, frame_time.Time.GetFrame().Value, state );
}

//---

void
FBoardSequenceCustomization::OnPreTransformChanged( UObject& InObject )
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    if( !sequencer || !sequencer->IsAllowedToChange() )
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
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    if( !sequencer || !sequencer->IsAllowedToChange() )
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

    BoardSequenceTools::StopPilotingCamera( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, Actor, ExistingTransform, NewTransformData );
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
