// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shot/ShotSequenceCustomization.h"

#include "CineCameraActor.h"
#include "ILevelEditor.h"
#include "LevelEditor.h"
#include "MVVM/ViewModels/ObjectBindingModel.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"
#include "SequencerUtilities.h"

#include "EposNamingConventionBlueprintLibrary.h"
#include "EposSequenceEditorBlueprintLibrary.h"
#include "EposSequenceEditorCommands.h"
#include "EposSequenceHelpers.h"
#include "EposSequenceToolbarHelpers.h"
#include "Misc/EposSequenceFBXInterop.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposSequenceEditorStyle.h"
#include "Styles/EposTracksEditorStyle.h"
#include "ToolkitHelpers.h"
#include "Tools/EposSequenceTools.h"
#include "Tools/LighttableTools.h"

#define LOCTEXT_NAMESPACE "ShotSequenceCustomization"

//---

static int32 sgRegistrationCountDebug = 0; // Just to be sure to always register (and unregister) customization synchronously

void
FShotSequenceCustomization::RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder )
{
    sgRegistrationCountDebug++;

    mWeakSequencer = ioBuilder.GetSequencer().AsShared();
    // From 5.3, the registration is only done if the new focused sequence is NOT the same type than the previous one
    // It means that:
    // - going from board to inner board, NOT called
    // - going from board to inner shot, called
    // - going from shot to upper board, called
    mShotSequence = Cast<UShotSequence>( &ioBuilder.GetFocusedSequence() );

    //---

    // Listen for actor/component movement
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw( this, &FShotSequenceCustomization::OnPrePropertyChanged );
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw( this, &FShotSequenceCustomization::OnPostPropertyChanged );

    mShotCommandList = MakeShared<FUICommandList>();

    BindCommands( mShotCommandList );

    mWeakSequencer.Pin()->GetCommandBindings()->Append( mShotCommandList.ToSharedRef() );

    TSharedPtr< ILevelEditor > levelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>( "LevelEditor" ).GetFirstLevelEditor();
    levelEditor->AppendCommands( mShotCommandList.ToSharedRef() );

    //---

    FSequencerCustomizationInfo customization;

    // customization.AddMenuExtender ...

    TSharedRef<FExtender> ToolbarExtender = MakeShared<FExtender>();
    ToolbarExtender->AddToolBarExtension( "CurveEditor", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw( this, &FShotSequenceCustomization::ExtendSequencerToolbar ) );
    customization.ToolbarExtender = ToolbarExtender;

    customization.OnBuildObjectBindingContextMenu = FOnGetSequencerMenuExtender::CreateRaw(this, &FShotSequenceCustomization::CreateObjectBindingContextMenuExtender);
    //customization.OnBuildSidebarMenu = FOnGetSequencerMenuExtender::CreateRaw( this, &FShotSequenceCustomization::CreateObjectBindingSidebarMenuExtender );

    // customization.OnReceivedDragOver ...
    // customization.OnReceivedDrop ...

    customization.OnAssetsDrop.BindRaw( this, &FShotSequenceCustomization::OnSequencerAssetsDrop );
    customization.OnClassesDrop.BindRaw( this, &FShotSequenceCustomization::OnSequencerClassesDrop );
    customization.OnActorsDrop.BindRaw( this, &FShotSequenceCustomization::OnSequencerActorsDrop );

    ioBuilder.AddCustomization( customization );

    UEposSequenceEditorBlueprintLibrary::SetSequencer( mWeakSequencer.Pin().ToSharedRef() );
    UEposNamingConventionBlueprintLibrary::SetSequencer( mWeakSequencer.Pin().ToSharedRef() );

    mSequencerActorAddedDelegates = mWeakSequencer.Pin()->OnActorAddedToSequencer().AddStatic( &ToolkitHelpers::HandleActorAddedToSequencer, mWeakSequencer.Pin().Get() );
    mSequencerActivatedDelegates = mWeakSequencer.Pin()->OnActivateSequence().AddStatic( &ToolkitHelpers::HandleOnActivateSequence, mWeakSequencer.Pin().Get() );
    mSequencerSelectionSectionChangedDelegates = mWeakSequencer.Pin()->GetSelectionChangedSections().AddStatic( &ToolkitHelpers::HandleOnSelectionChangedSections, mWeakSequencer.Pin().Get() );

    mSequencerGlobalTimeChangedDelegates = mWeakSequencer.Pin()->OnGlobalTimeChanged().AddRaw( this, &FShotSequenceCustomization::OnGlobalTimeChanged );

    mMovieSceneDataChangedHandle = mWeakSequencer.Pin()->OnMovieSceneDataChanged().AddRaw( this, &FShotSequenceCustomization::MovieSceneDataChanged );

    FLevelEditorModule& levelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( "LevelEditor" );
    mSelectionChangedDelegates = levelEditorModule.OnActorSelectionChanged().AddRaw( this, &FShotSequenceCustomization::OnObjectSelectedMulti );
    //ULevelEditorSubsystem& subsystem = GEditor->GetSubsystem<ULevelEditorSubsystem>();
}

void
FShotSequenceCustomization::UnregisterSequencerCustomization()
{
    sgRegistrationCountDebug--;
    check( sgRegistrationCountDebug == 0 );

    FCoreUObjectDelegates::OnPreObjectPropertyChanged.RemoveAll( this );
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll( this );

    mShotCommandList = nullptr;

    if( mWeakSequencer.IsValid() )
    {
        mWeakSequencer.Pin()->OnActorAddedToSequencer().Remove( mSequencerActorAddedDelegates );
        mWeakSequencer.Pin()->OnActivateSequence().Remove( mSequencerActivatedDelegates );
        mWeakSequencer.Pin()->GetSelectionChangedSections().Remove( mSequencerSelectionSectionChangedDelegates );

        mWeakSequencer.Pin()->OnGlobalTimeChanged().Remove( mSequencerGlobalTimeChangedDelegates );

        mWeakSequencer.Pin()->OnMovieSceneDataChanged().Remove( mMovieSceneDataChangedHandle );
    }

    FLevelEditorModule& levelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( "LevelEditor" );
    levelEditorModule.OnActorSelectionChanged().Remove( mSelectionChangedDelegates );

    mWeakSequencer = nullptr;
    mShotSequence = nullptr;
}

void
FShotSequenceCustomization::MovieSceneDataChanged( EMovieSceneDataChangeType iType )
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
    if( !sequencer )
        return;

}

//---

void
FShotSequenceCustomization::BindCommands( TSharedPtr<FUICommandList> ioCommandList )
{
    FEposSequenceEditorActionCallbacks::MapActions( ioCommandList );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().StepToNextShot,
        FExecuteAction::CreateStatic( &ShotSequenceTools::StepToNextShot, mWeakSequencer.Pin().Get() )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().StepToPreviousShot,
        FExecuteAction::CreateStatic( &ShotSequenceTools::StepToPreviousShot, mWeakSequencer.Pin().Get() )
    );

    //---

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().CreateCameraAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          FCameraArgs camera_args;
                                          camera_args.mSpawnable = GetDefault<UEposTracksEditorSettings>()->bSpawnable;
                                          FAnimationArgs animation_args;
                                          animation_args.mSpawnable = GetDefault<UEposTracksEditorSettings>()->bSpawnable;
                                          ShotSequenceTools::CreateCameraWithAnimation( sequencer.Get(), camera_args, animation_args );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return ShotSequenceTools::CanCreateCamera( sequencer.Get() );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().SnapCameraToViewportAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          ShotSequenceTools::SnapCameraToViewport( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        // It doesn't work due to strange stuff between FMovieSceneSequenceID and FMovieSceneSequenceIDRef ...
        //FExecuteAction::CreateStatic( &ShotSequenceTools::SnapCameraToViewport, mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID() ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return ShotSequenceTools::CanSnapCameraToViewport( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().PilotCameraAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          ShotSequenceTools::PilotCamera( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return ShotSequenceTools::CanPilotCamera( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().EjectCameraAtCurrentTime,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          ShotSequenceTools::EjectCamera( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return ShotSequenceTools::CanEjectCamera( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoPreviousCameraPosition,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          ShotSequenceTools::GotoPreviousCameraPosition( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return ShotSequenceTools::HasPreviousCameraPosition( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoNextCameraPosition,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          ShotSequenceTools::GotoNextCameraPosition( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return ShotSequenceTools::HasNextCameraPosition( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
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
                                          FAnimationArgs animation_args;
                                          animation_args.mSpawnable = GetDefault<UEposTracksEditorSettings>()->bSpawnable;
                                          ShotSequenceTools::CreateAnimation( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, animation_args );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return ShotSequenceTools::CanCreateAnimation( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
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
                                          int32 animation_count = ShotSequenceTools::GetAttachedAnimations( sequencer.Get(), nullptr, &animation_bindings );
                                          if( animation_count != 1 )
                                              return;
                                          ShotSequenceTools::DetachAnimation( sequencer.Get(), animation_bindings[0] );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return ShotSequenceTools::GetAttachedAnimations( sequencer.Get() ) == 1;
                                         } ),
        FIsActionChecked(),
        FIsActionButtonVisible::CreateLambda( [this]()
                                              {
                                                  TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                  if( !sequencer )
                                                      return false;
                                                  return ShotSequenceTools::GetAttachedAnimations( sequencer.Get() ) <= 1;
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
                                          int32 animation_count = ShotSequenceTools::GetAllAnimations( sequencer.Get(), nullptr, &animation_bindings );
                                          if( animation_count != 1 )
                                              return;
                                          ShotSequenceTools::CreateAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, animation_bindings[0] );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             TArray<FGuid> animation_bindings;
                                             int32 animation_count = ShotSequenceTools::GetAllAnimations( sequencer.Get(), nullptr, &animation_bindings );
                                             if( animation_count != 1 )
                                                 return false;
                                             return ShotSequenceTools::CanCreateAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, animation_bindings[0] );
                                         } ),
        FIsActionChecked(),
        FIsActionButtonVisible::CreateLambda( [this]()
                                              {
                                                  TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                  if( !sequencer )
                                                      return false;
                                                  return ShotSequenceTools::GetAllAnimations( sequencer.Get() ) <= 1;
                                              } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoPreviousAnimationCut,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          ShotSequenceTools::GotoPreviousAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return ShotSequenceTools::HasPreviousAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                         } )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoNextAnimationCut,
        FExecuteAction::CreateLambda( [this]()
                                      {
                                          TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                          if( !sequencer )
                                              return;
                                          ShotSequenceTools::GotoNextAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;
                                             return ShotSequenceTools::HasNextAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber );
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

void
FShotSequenceCustomization::ExtendSequencerToolbar( FToolBarBuilder& ToolbarBuilder )
{
    static const FName sSequencerToolbarStyleName = "SequencerToolbar";

    ToolbarBuilder.BeginStyleOverride( sSequencerToolbarStyleName );

    //ToolbarBuilder.AddSeparator();
    //ToolbarBuilder.AddToolBarButton( FUIAction(
    //    FExecuteAction::CreateLambda( [this]()
    //                                  {
    //                                      TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
    //                                      if( !sequencer )
    //                                          return;

    //                                      //ShotSequenceTools::CloneInnerContent( sequencer.Get(), sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), false );
    //                                  } ) ),
    //    NAME_None,
    //    FText::FromString( TEXT( "DEBUG" ) ),
    //    FText::GetEmpty(),
    //    FSlateIcon( FEposSequenceEditorStyle::Get().GetStyleSetName(), "Settings" )
    //);

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().CreateCameraAtCurrentTime );
    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeCameraMenu ),
        LOCTEXT( "CameraOptions", "Options" ),
        LOCTEXT( "CameraOptionsToolTip", "Camera Options" ),
        TAttribute<FSlateIcon>(),
        true );
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().SnapCameraToViewportAtCurrentTime );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().CreateAnimationAtCurrentTime );
    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeAnimationSettingsMenu ),
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
                                                      return ShotSequenceTools::GetAttachedAnimations( sequencer.Get() ) > 1;
                                                  } )
        ),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeAnimationMenu ),
        FEposSequenceEditorCommands::Get().DetachAnimationAtCurrentTime->GetLabel(),
        FEposSequenceEditorCommands::Get().DetachAnimationAtCurrentTime->GetDescription(),
        FEposSequenceEditorCommands::Get().DetachAnimationAtCurrentTime->GetIcon() );

    auto GetLighttableTooltip = [this]() -> FText
        {
            TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
            if( !sequencer )
                return LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable" );

            TArray<FGuid> animation_bindings;
            int32 animation_count = ShotSequenceTools::GetAllAnimations( sequencer.Get(), nullptr, &animation_bindings );
            //check( animation_count == 1 );
            if( animation_count != 1 )
                return LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable" );

            if( LighttableTools::IsOn( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), animation_bindings[0] ) )
                return LOCTEXT( "disable-lighttable-tooltip", "Disable the lighttable" );
            else
                return LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable" );
        };

    auto GetLighttableIcon = [this]() -> FSlateIcon
        {
            TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
            if( !sequencer )
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );

            TArray<FGuid> animation_bindings;
            int32 animation_count = ShotSequenceTools::GetAllAnimations( sequencer.Get(), nullptr, &animation_bindings );
            //check( animation_count == 1 );
            if( animation_count != 1 )
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );

            if( LighttableTools::IsOn( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), animation_bindings[0] ) )
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
                                          int32 animation_count = ShotSequenceTools::GetAllAnimations( sequencer.Get(), nullptr, &animation_bindings );
                                          if( animation_count != 1 )
                                              return;

                                          if( LighttableTools::IsOn( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), animation_bindings[0] ) )
                                              LighttableTools::Deactivate( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), animation_bindings[0] );
                                          else
                                              LighttableTools::Activate( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), animation_bindings[0] );
                                      } ),
        FCanExecuteAction::CreateLambda( [this]()
                                         {
                                             TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                             if( !sequencer )
                                                 return false;

                                             return ShotSequenceTools::GetAllAnimations( sequencer.Get() ) == 1;
                                         } ),
        FIsActionChecked(),
        FIsActionButtonVisible::CreateLambda( [this]()
                                              {
                                                  TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                  if( !sequencer )
                                                      return false;

                                                  return ShotSequenceTools::GetAllAnimations( sequencer.Get() ) <= 1;
                                              } ) ),
        NAME_None,
        FText::GetEmpty(),
        MakeAttributeLambda( GetLighttableTooltip ),
        MakeAttributeLambda( GetLighttableIcon )
    );
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

                                                      return ShotSequenceTools::GetAllAnimations( sequencer.Get() ) > 1;
                                                  } )
        ),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeLighttableMenu ),
        FText::GetEmpty(),
        LOCTEXT( "LighttableOptionsTooltip", "Activate/Deactivate lighttable on animations" ),
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

                                                      return ShotSequenceTools::GetAllAnimations( sequencer.Get() ) > 1;
                                                  } )
        ),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeAnimationCutMenu ),
        FEposSequenceEditorCommands::Get().CreateAnimationCutAtCurrentTime->GetLabel(),
        FEposSequenceEditorCommands::Get().CreateAnimationCutAtCurrentTime->GetDescription(),
        FEposSequenceEditorCommands::Get().CreateAnimationCutAtCurrentTime->GetIcon() );
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoNextAnimationCut );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeSettingsMenu ),
        LOCTEXT( "Settings", "Settings" ),
        LOCTEXT( "SettingsToolTip", "Set sequence settings" ),
        FSlateIcon( FEposSequenceEditorStyle::Get().GetStyleSetName(), "Settings" ) );

    ToolbarBuilder.EndStyleOverride();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeCameraMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    EposSequenceToolbarHelpers::MakeCameraSettingsEntries( MenuBuilder );

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeLighttableMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    TArray<AOdysseyAnimationActor*> animations;
    TArray<FGuid> animation_bindings;
    int32 animation_count = ShotSequenceTools::GetAllAnimations( sequencer.Get(), &animations, &animation_bindings );
    if( !animation_count )
        return SNullWidget::NullWidget;

    for( int i = 0; i < animation_count; i++ )
    {
        AOdysseyAnimationActor* animation = animations[i];
        FGuid animation_binding = animation_bindings[i];

        FText tooltip;
        if( LighttableTools::IsOn( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), animation_binding ) )
            tooltip = LOCTEXT( "disable-lighttable-tooltip", "Disable the lighttable" );
        else
            tooltip = LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable" );

        FSlateIcon icon;
        if( LighttableTools::IsOn( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), animation_binding ) )
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

                                                  if( LighttableTools::IsOn( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), animation_binding ) )
                                                      LighttableTools::Deactivate( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), animation_binding );
                                                  else
                                                      LighttableTools::Activate( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), animation_binding );
                                              } )
            )
        );
    }

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeAnimationMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    TArray<AOdysseyAnimationActor*> animations;
    TArray<FGuid> animation_bindings;
    int32 animation_count = ShotSequenceTools::GetAttachedAnimations( sequencer.Get(), &animations, &animation_bindings );
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

                                                  ShotSequenceTools::DetachAnimation( sequencer.Get(), animation_binding );
                                              } )
            )
        );
    }

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeAnimationCutMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    TArray<AOdysseyAnimationActor*> animations;
    TArray<FGuid> animation_bindings;
    int32 animation_count = ShotSequenceTools::GetAllAnimations( sequencer.Get(), &animations, &animation_bindings );
    if( !animation_count )
        return SNullWidget::NullWidget;

    for( int i = 0; i < animation_count; i++ )
    {
        AOdysseyAnimationActor* animation = animations[i];
        FGuid animation_binding = animation_bindings[i];

        MenuBuilder.AddMenuEntry(
            FText::FromString( animation->GetActorLabel() ),
            //LOCTEXT( "LockPlayback", "Lock to Display Rate at Runtime" ),
            FText::GetEmpty(),
            //LOCTEXT( "LockPlayback_Description", "When enabled, causes all runtime evaluation and the engine FPS to be locked to the current display frame rate" ),
            FSlateIcon(),
            FUIAction(
                FExecuteAction::CreateLambda( [this, animation_binding]()
                                              {
                                                  TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                  if( !sequencer )
                                                      return;

                                                  ShotSequenceTools::CreateAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, animation_binding );
                                              } ),
                FCanExecuteAction::CreateLambda( [this, animation_binding]()
                                                 {
                                                     TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();
                                                     if( !sequencer )
                                                         return false;

                                                     return ShotSequenceTools::CanCreateAnimationCut( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, animation_binding );
                                                 } )
            )/*,
            NAME_None,
            EUserInterfaceActionType::ToggleButton*/ //TODO: I don't know how, but there should be something to multi-select animations and create animation on them
        );
    }

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeSettingsMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    EposSequenceToolbarHelpers::MakeSettingsEntries( MenuBuilder, sequencer.Get() );

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeAnimationSettingsMenu()
{
    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    FMenuBuilder MenuBuilder( true, sequencer ? sequencer->GetCommandBindings() : nullptr );

    EposSequenceToolbarHelpers::MakeAnimationSettingsEntries( MenuBuilder );

    return MenuBuilder.MakeWidget();
}

//---

TSharedPtr<FExtender>
FShotSequenceCustomization::CreateObjectBindingContextMenuExtender( UE::Sequencer::FViewModelPtr InViewModel )
{
    TSharedRef<FExtender> Extender = MakeShared<FExtender>();
    TSharedPtr<UE::Sequencer::FObjectBindingModel> ObjectBindingModel = InViewModel->CastThisShared<UE::Sequencer::FObjectBindingModel>();
    Extender->AddMenuExtension(
        "ObjectBindingActions", EExtensionHook::Before, nullptr,
        FMenuExtensionDelegate::CreateRaw(this, &FShotSequenceCustomization::ExtendObjectBindingContextMenu, ObjectBindingModel));
    return Extender.ToSharedPtr();
}

void
FShotSequenceCustomization::ExtendObjectBindingContextMenu(FMenuBuilder& MenuBuilder, TSharedPtr<UE::Sequencer::FObjectBindingModel> ObjectBindingModel)
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

    bool bShowConvert = true;

    if( FMovieScenePossessable* Possessable = MovieScene->FindPossessable( ObjectBindingID ) )
    {
        // We can't convert sub-objects to different binding types for now.
        if( Possessable->GetParent().IsValid() )
        {
            bShowConvert = false;
        }
        bool bCustomBinding = false;
        bool bMultipleBindings = false;
        UObject* ResolutionContext = MovieSceneHelpers::GetResolutionContext( Sequence, ObjectBindingID, Sequencer->GetFocusedTemplateID(), Sequencer->GetSharedPlaybackState() );

        if( const FMovieSceneBindingReferences* BindingReferences = Sequence->GetBindingReferences() )
        {
            bCustomBinding = Algo::AnyOf( BindingReferences->GetReferences( ObjectBindingID ), []( const FMovieSceneBindingReference& Reference )
                                          {
                                              return Reference.CustomBinding;
                                          } );
            bMultipleBindings = BindingReferences->GetReferences( ObjectBindingID ).Num() > 1;
            UE::UniversalObjectLocator::FResolveParams LocatorResolveParams( ResolutionContext );
            FMovieSceneBindingResolveParams BindingResolveParams{ Sequence, ObjectBindingID, Sequencer->GetFocusedTemplateID(), ResolutionContext };

            // Can convert to possessable
            int32 BindingIndex = 0;
            bool bAnyValidConversions = false;
            if( Algo::AnyOf( BindingReferences->GetReferences( ObjectBindingID ), [&BindingIndex, Sequencer]( const FMovieSceneBindingReference& BindingReference )
                             {
                                 return FSequencerUtilities::CanConvertToPossessable( Sequencer.ToSharedRef(), BindingReference.ID, BindingIndex++ );
                             } ) )
            {
                bAnyValidConversions = true;
            }
            else
            {
                TArrayView<const TSubclassOf<UMovieSceneCustomBinding>> PrioritySortedCustomBindingTypes = Sequencer->GetSupportedCustomBindingTypes();
                for( const TSubclassOf<UMovieSceneCustomBinding>& CustomBindingType : PrioritySortedCustomBindingTypes )
                {
                    BindingIndex = 0;
                    if( Algo::AllOf( BindingReferences->GetReferences( ObjectBindingID ), [&BindingIndex, &CustomBindingType, Sequencer]( const FMovieSceneBindingReference& BindingReference )
                                     {
                                         return FSequencerUtilities::CanConvertToCustomBinding( Sequencer.ToSharedRef(), BindingReference.ID, CustomBindingType, BindingIndex++ );
                                     } ) )
                    {
                        bAnyValidConversions = true;
                        break;
                    }
                }
            }
            if( !bAnyValidConversions )
            {
                bShowConvert = false;
            }
        }

        // Regular possessable
        if( !bCustomBinding )
        {
            //// Regular possessable
            //// We don't add anything here, but the extension will
            //MenuBuilder.BeginSection( "EposPossessable" );
            //MenuBuilder.EndSection();
        }
        else
        {
            // IObjectBindingExtension is not accessible as not exported as UE_API

            //MenuBuilder.BeginSection( "EposCustomBinding" );
            //bool bCustomSpawnable = MovieSceneHelpers::SupportsObjectTemplate( Sequence, ObjectBindingID, Sequencer->GetSharedPlaybackState() );
            //// Check for custom binding types

            //if( bCustomSpawnable )
            //{
            //    MenuBuilder.AddMenuEntry( FSequencerCommands::Get().SaveCurrentSpawnableState );

            //    if( !bMultipleBindings )
            //    {
            //        MenuBuilder.AddSubMenu(
            //            LOCTEXT( "ChangeClassLabel", "Change Class" ),
            //            LOCTEXT( "ChangeClassTooltip", "Change the class (object template) that this spawns from" ),
            //            FNewMenuDelegate::CreateLambda( [this]( FMenuBuilder& MenuBuilder )
            //                                            {
            //                                                const TSharedPtr<ISequencer> Sequencer = mWeakSequencer.Pin();
            //                                                if( !Sequencer.IsValid() )
            //                                                {
            //                                                    return;
            //                                                }

            //                                                UMovieSceneSequence* Sequence = Sequencer->GetFocusedMovieSceneSequence();

            //                                                TArray<FSequencerChangeBindingInfo> Bindings;
            //                                                const FMovieSceneBindingReferences* BindingReferences = Sequence->GetBindingReferences();
            //                                                for( TViewModelPtr<IObjectBindingExtension> ObjectBindingNode : Sequencer->GetViewModel()->GetSelection()->Outliner.Filter<IObjectBindingExtension>() )
            //                                                {
            //                                                    int32 BindingIndex = 0;
            //                                                    for( const FMovieSceneBindingReference& Reference : BindingReferences->GetReferences( ObjectBindingNode->GetObjectGuid() ) )
            //                                                    {
            //                                                        Bindings.Add( { Reference.ID, BindingIndex++ } );
            //                                                    }
            //                                                }

            //                                                FSequencerUtilities::AddChangeClassMenu( MenuBuilder, Sequencer.ToSharedRef(), Bindings, TFunction<void()>() );
            //                                            } ) );
            //    }
            //}

            //MenuBuilder.EndSection();
        }
    }

    if( bShowConvert )
    {
        // We don't add anything here, but the extension will
        MenuBuilder.BeginSection( "EposConvertBinding" );
        MenuBuilder.EndSection();
    }

    MenuBuilder.BeginSection("Import/Export", LOCTEXT("ImportExportMenuSectionName", "Import/Export"));

    MenuBuilder.AddMenuEntry(
        LOCTEXT("ImportFBX", "Import..."),
        LOCTEXT("ImportFBXTooltip", "Import FBX animation to this object"),
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
        ));

    MenuBuilder.AddMenuEntry(
        LOCTEXT("ExportFBX", "Export..."),
        LOCTEXT("ExportFBXTooltip", "Export FBX animation from this object"),
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
        ));

    MenuBuilder.EndSection();
}

//TSharedPtr<FExtender> FShotSequenceCustomization::CreateObjectBindingSidebarMenuExtender( FViewModelPtr InViewModel )
//{
//    TSharedRef<FExtender> Extender = MakeShared<FExtender>();
//
//    TSharedPtr<FObjectBindingModel> ObjectBindingModel = InViewModel->CastThisShared<FObjectBindingModel>();
//
//    Extender->AddMenuExtension( TEXT( "ObjectBindingActions" ), EExtensionHook::Before, nullptr,
//                                FMenuExtensionDelegate::CreateRaw( this, &FShotSequenceCustomization::ExtendObjectBindingSidebarMenu, ObjectBindingModel ) );
//
//    return Extender.ToSharedPtr();
//}
//
//void FShotSequenceCustomization::ExtendObjectBindingSidebarMenu( FMenuBuilder& MenuBuilder, TSharedPtr<FObjectBindingModel> ObjectBindingModel )
//{
//    ExtendObjectBindingContextMenu( MenuBuilder, ObjectBindingModel );
//}

//---

void
FShotSequenceCustomization::OnObjectSelectedMulti( const TArray<UObject*>& iObjects, bool bForceRefresh )
{
    // Store an history of selected actors to know later (when scrubbing) which actor best fit the auto-selection
    for( UObject* object : iObjects )
    {
        ACineCameraActor* camera = Cast<ACineCameraActor>( object );
        if( camera )
            ShotSequenceTools::AddSelectedActorToHistory( camera );

        AOdysseyAnimationActor* animation = Cast<AOdysseyAnimationActor>( object );
        if( animation )
            ShotSequenceTools::AddSelectedActorToHistory( animation );
    }
}

void
FShotSequenceCustomization::OnGlobalTimeChanged()
{
    if( !mWeakSequencer.IsValid() )
        return;

    TSharedPtr<ISequencer> sequencer = mWeakSequencer.Pin();

    EMovieScenePlayerStatus::Type playback_state = sequencer->GetPlaybackStatus();

    //FString prefix = TEXT( "global time changed" );
    //FQualifiedFrameTime frame_time = sequencer->GetLocalTime();
    //UE_LOG( LogTemp, Warning, TEXT( "%s: frame=%d state=%s" ), *prefix, frame_time.Time.GetFrame().Value,
    //        playback_state == EMovieScenePlayerStatus::Type::Stopped
    //        ? TEXT("Stopped")
    //        : playback_state == EMovieScenePlayerStatus::Type::Playing
    //        ? TEXT( "Playing" )
    //        : playback_state == EMovieScenePlayerStatus::Type::Scrubbing
    //        ? TEXT( "Scrubbing" )
    //        : playback_state == EMovieScenePlayerStatus::Type::Jumping
    //        ? TEXT( "Jumping" )
    //        : playback_state == EMovieScenePlayerStatus::Type::Stepping
    //        ? TEXT( "Stepping" )
    //        : playback_state == EMovieScenePlayerStatus::Type::Paused
    //        ? TEXT( "Paused" )
    //        : TEXT( "Unknown" )
    //);

    // Don't auto-select an actor if currently play or scrub because changing the actor in Odyssey mode is not instant
    // So just change the actor at the end of play/scrub
    if( playback_state != EMovieScenePlayerStatus::Playing && playback_state != EMovieScenePlayerStatus::Scrubbing )
    {
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
        if( nothing_selected )
        {
            FQualifiedFrameTime qframetime = sequencer->GetLocalTime();
            FFrameTime frametime = qframetime.Time;
            FFrameNumber framenumber = frametime.FloorToFrame();
            AActor* actor_to_select = ShotSequenceTools::GuessActorToSelect( sequencer.Get(), framenumber );
            if( actor_to_select )
            {
                sequencer->EmptySelection();
                //sequencer->SelectSection( subsection );

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
}

//---

void
FShotSequenceCustomization::OnPreTransformChanged( UObject& InObject )
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
FShotSequenceCustomization::OnTransformChanged( UObject& InObject )
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

    ShotSequenceTools::StopPilotingCamera( sequencer.Get(), sequencer->GetLocalTime().Time.FrameNumber, Actor, ExistingTransform, NewTransformData );
}

void
FShotSequenceCustomization::OnPrePropertyChanged( UObject* InObject, const FEditPropertyChain& InPropertyChain )
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
FShotSequenceCustomization::OnPostPropertyChanged( UObject* InObject, FPropertyChangedEvent& InPropertyChangedEvent )
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
//FShotSequenceCustomization::OnSequencerReceivedDragOver( const FGeometry& iMyGeometry, const FDragDropEvent& iDragDropEvent, FReply& oReply )
//{
//    bool bIsDragSupported = false;
//
//    TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
//    if( Operation.IsValid() && (
//        ( Operation->IsOfType<FAssetDragDropOp>() && StaticCastSharedPtr<FAssetDragDropOp>( Operation )->GetAssetPaths().Num() <= 1 ) ||
//        ( Operation->IsOfType<FClassDragDropOp>() && StaticCastSharedPtr<FClassDragDropOp>( Operation )->ClassesToDrop.Num() <= 1 ) ||
//        ( Operation->IsOfType<FActorDragDropGraphEdOp>() && StaticCastSharedPtr<FActorDragDropGraphEdOp>( Operation )->Actors.Num() <= 1 ) ) )
//    {
//        bIsDragSupported = true;
//    }
//
//    OutReply = ( bIsDragSupported ? FReply::Handled() : FReply::Unhandled() );
//    return true;
//}

ESequencerDropResult
FShotSequenceCustomization::OnSequencerAssetsDrop( const TArray<UObject*>& iAssets, const FAssetDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::Unhandled; // Process the default behavior for assets
}

ESequencerDropResult
FShotSequenceCustomization::OnSequencerClassesDrop( const TArray<TWeakObjectPtr<UClass>>& iClasses, const FClassDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::DropDenied;    // Don't accept classes
}

ESequencerDropResult
FShotSequenceCustomization::OnSequencerActorsDrop( const TArray<TWeakObjectPtr<AActor>>& iActors, const FActorDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::Unhandled; // Process the default behavior for actors
}

#undef LOCTEXT_NAMESPACE
