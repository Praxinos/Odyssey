// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SingleCameraCutTrack/SingleCameraCutTrackEditor.h"

#include "DragAndDrop/ActorDragDropGraphEdOp.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneObjectBindingIDPicker.h"
#include "MovieSceneToolHelpers.h"
#include "SequencerSettings.h"
#include "MVVM/Views/ViewUtilities.h"
#include "MVVM/ViewModels/OutlinerColumns/OutlinerColumnTypes.h"
#include "TrackEditorThumbnail/TrackEditorThumbnailPool.h"
#include "TrackInstances/MovieSceneCameraCutTrackInstance.h"
#include "Tracks/MovieSceneCameraCutTrack.h"

#include "ActorEditorUtils.h"
#include "ActorTreeItem.h"
#include "Application/ThrottleManager.h"
#include "Editor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameFramework/WorldSettings.h"
#include "LevelEditorViewport.h"
#include "LevelSequence.h"
#include "Modules/ModuleManager.h"
#include "SceneOutlinerModule.h"
#include "SceneOutlinerPublicTypes.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"

#include "Shot/ShotSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrackInstance.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "SingleCameraCutTrack/SingleCameraCutSection.h"
#include "Styles/EposTracksEditorStyle.h"

#define LOCTEXT_NAMESPACE "FSingleCameraCutTrackEditor"


class FSingleCameraCutTrackCommands
    : public TCommands<FSingleCameraCutTrackCommands>
{
public:

    FSingleCameraCutTrackCommands()
        : TCommands<FSingleCameraCutTrackCommands>
    (
        "SingleCameraCutTrack",
        NSLOCTEXT("Contexts", "SingleCameraCutTrack", "SingleCameraCutTrack"),
        NAME_None, // "MainFrame" // @todo Fix this crash
        FAppStyle::Get().GetStyleSetName() // Icon Style Set
    )
        , BindingCount(0)
    { }

    /** Toggle the camera lock */
    TSharedPtr< FUICommandInfo > ToggleLockCamera;

    /**
     * Initialize commands
     */
    virtual void RegisterCommands() override;

    mutable uint32 BindingCount;
};


void FSingleCameraCutTrackCommands::RegisterCommands()
{
    UI_COMMAND( ToggleLockCamera, "Toggle Lock Camera", "Toggle locking the viewport to the single camera cut track.", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::L) );
}


/* FSingleCameraCutTrackEditor structors
 *****************************************************************************/

FSingleCameraCutTrackEditor::FSingleCameraCutTrackEditor(TSharedRef<ISequencer> InSequencer)
    : FMovieSceneTrackEditor(InSequencer)
{
    ThumbnailPool = MakeShareable(new FTrackEditorThumbnailPool(InSequencer));

    FSingleCameraCutTrackCommands::Register();
}

void FSingleCameraCutTrackEditor::OnRelease()
{
    const FSingleCameraCutTrackCommands& Commands = FSingleCameraCutTrackCommands::Get();
    Commands.BindingCount--;

    if (Commands.BindingCount < 1)
    {
        FSingleCameraCutTrackCommands::Unregister();
    }
}

TSharedRef<ISequencerTrackEditor> FSingleCameraCutTrackEditor::CreateTrackEditor(TSharedRef<ISequencer> InSequencer)
{
    return MakeShareable(new FSingleCameraCutTrackEditor(InSequencer));
}


/* ISequencerTrackEditor interface
 *****************************************************************************/

void FSingleCameraCutTrackEditor::BindCommands(TSharedRef<FUICommandList> SequencerCommandBindings)
{
    const FSingleCameraCutTrackCommands& Commands = FSingleCameraCutTrackCommands::Get();

    SequencerCommandBindings->MapAction(
        Commands.ToggleLockCamera,
        FExecuteAction::CreateSP( this, &FSingleCameraCutTrackEditor::ToggleLockCamera) );

    TSharedPtr<FUICommandList> CurveEditorSharedBindings = GetSequencer()->GetCommandBindings( ESequencerCommandBindings::CurveEditor );
    if( CurveEditorSharedBindings )
    {
        CurveEditorSharedBindings->MapAction( Commands.ToggleLockCamera, *SequencerCommandBindings->GetActionForCommand( Commands.ToggleLockCamera ) );
    }

    Commands.BindingCount++;
}

void FSingleCameraCutTrackEditor::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.AddMenuEntry(
        LOCTEXT("AddSingleCameraCutTrack", "Single Camera Cut Track"),
        LOCTEXT("AddSingleCameraCutTooltip", "Adds a single camera cut track, as well as a new camera cut at the current scrubber location if a camera is selected."),
        FSlateIcon(FEposTracksEditorStyle::Get().GetStyleSetName(), "Sequencer.Tracks.SingleCameraCut"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &FSingleCameraCutTrackEditor::HandleAddSingleCameraCutTrackMenuEntryExecute),
            FCanExecuteAction::CreateRaw(this, &FSingleCameraCutTrackEditor::HandleAddSingleCameraCutTrackMenuEntryCanExecute),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateRaw( this, &FSingleCameraCutTrackEditor::HandleAddSingleCameraCutTrackMenuEntryIsVisible )
        )
    );
}

void FSingleCameraCutTrackEditor::BuildTrackContextMenu(FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track)
{
    //UMovieSceneSingleCameraCutTrack* SingleCameraCutTrack = Cast<UMovieSceneSingleCameraCutTrack>(Track);
    //
    //MenuBuilder.AddMenuEntry(
    //  LOCTEXT("CanBlendShots", "Can Blend"),
    //  LOCTEXT("CanBlendShotsTooltip", "Enable shot blending on this track, making it possible to overlap sections."),
    //  FSlateIcon(),
    //  FUIAction(
    //      FExecuteAction::CreateSP(this, &FSingleCameraCutTrackEditor::HandleToggleCanBlendExecute, SingleCameraCutTrack),
    //      FCanExecuteAction::CreateLambda([=]() { return SingleCameraCutTrack != nullptr; }),
    //      FIsActionChecked::CreateLambda([=]() { return SingleCameraCutTrack->bCanBlend; })
    //      ),
    //  "Edit",
    //  EUserInterfaceActionType::ToggleButton
    //);

    //MenuBuilder.AddMenuEntry(
    //    LOCTEXT( "AutoArrangeShots", "Auto Arrange" ),
    //    LOCTEXT( "AutoArrangeShotsTooltip", "Auto-arrange and resize sections to fill gaps." ),
    //    FSlateIcon(),
    //    FUIAction(
    //        FExecuteAction::CreateSP( this, &FCameraCutTrackEditor::HandleToggleAutoArrangeSectionsExecute, CameraCutTrack ),
    //        FCanExecuteAction::CreateLambda( [=]() { return CameraCutTrack != nullptr; } ),
    //        FIsActionChecked::CreateLambda( [=]() { return CameraCutTrack->IsAutoManagingSections(); } )
    //    ),
    //    "Edit",
    //    EUserInterfaceActionType::ToggleButton
    //);
}

//void FCameraCutTrackEditor::HandleToggleCanBlendExecute(UMovieSceneCameraCutTrack* CameraCutTrack)
//{
//    const FScopedTransaction Transaction( LOCTEXT( "CameraCutTrackSetCanBlend", "Set Camera Cut Track Can Blend" ) );
//
//    CameraCutTrack->Modify();
//
//    CameraCutTrack->bCanBlend = !CameraCutTrack->bCanBlend;
//
//    if( !CameraCutTrack->bCanBlend )
//    {
//        CameraCutTrack->RearrangeAllSections();
//    }
//}
//
//void FCameraCutTrackEditor::HandleToggleAutoArrangeSectionsExecute( UMovieSceneCameraCutTrack* CameraCutTrack )
//{
//    const FScopedTransaction Transaction( LOCTEXT( "CameraCutTrackSetAutoArrangeSections", "Set Camera Cut Track Auto Arrange" ) );
//
//    CameraCutTrack->Modify();
//
//    CameraCutTrack->SetIsAutoManagingSections( !CameraCutTrack->IsAutoManagingSections() );
//
//    if( CameraCutTrack->IsAutoManagingSections() )
//    {
//        CameraCutTrack->RearrangeAllSections();
//    }
//}

TSharedPtr<SWidget> FSingleCameraCutTrackEditor::BuildOutlinerColumnWidget( const FBuildColumnWidgetParams& Params, const FName& ColumnName )
{
    using namespace UE::Sequencer;

    if( ColumnName == FCommonOutlinerNames::Add )
    {
        return UE::Sequencer::MakeAddButton(
            LOCTEXT( "SingleCameraCutText", "Camera" ),
            FOnGetContent::CreateSP( this, &FSingleCameraCutTrackEditor::HandleAddSingleCameraCutComboButtonGetMenuContent ),
            Params.ViewModel );
    }

    bool bAddCameraLock = false;
    if( ColumnName == FCommonOutlinerNames::Nav )
    {
        bAddCameraLock = true;
    }
    else if( ColumnName == FCommonOutlinerNames::KeyFrame )
    {
        // Add the camera lock button to the keyframe column if Nav is disabled
        bAddCameraLock = Params.TreeViewRow->IsColumnVisible( FCommonOutlinerNames::Nav ) == false;
    }
    else if( ColumnName == FCommonOutlinerNames::Edit )
    {
        // Add the camera lock button to the edit column if both Nav and KeyFrame are disabled
        bAddCameraLock = Params.TreeViewRow->IsColumnVisible( FCommonOutlinerNames::Nav ) == false &&
            Params.TreeViewRow->IsColumnVisible( FCommonOutlinerNames::KeyFrame ) == false;
    }

    if( bAddCameraLock )
    {
        TSharedRef<SWidget> Button = SNew( SCheckBox )
            .Style( FAppStyle::Get(), "Sequencer.Outliner.ToggleButton" )
            .Type( ESlateCheckBoxType::ToggleButton )
            .IsFocusable( false )
            .IsChecked( this, &FSingleCameraCutTrackEditor::IsCameraLocked )
            .OnCheckStateChanged( this, &FSingleCameraCutTrackEditor::OnLockCameraClicked )
            .ToolTipText( this, &FSingleCameraCutTrackEditor::GetLockCameraToolTip )
            [
                SNew( SImage )
                    .Image( FAppStyle::GetBrush( "Sequencer.Outliner.CameraLock" ) )
            ];

        if( ColumnName == FCommonOutlinerNames::Edit )
        {
            // Needs to be left aligned in the edit column because this column slot is set to fill
            return SNew( SBox )
                .HAlign( HAlign_Left )
                .Padding( 4.f, 0.f )
                [
                    Button
                ];
        }
        else
        {
            return Button;
        }
    }

    return FMovieSceneTrackEditor::BuildOutlinerColumnWidget( Params, ColumnName );;
}


TSharedRef<ISequencerSection> FSingleCameraCutTrackEditor::MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding)
{
    check(SupportsType(SectionObject.GetOuter()->GetClass()));

    return MakeShareable(new FSingleCameraCutSection(GetSequencer(), ThumbnailPool, SectionObject));
}


bool FSingleCameraCutTrackEditor::SupportsSequence(UMovieSceneSequence* InSequence) const
{
    ETrackSupport TrackSupported = InSequence ? InSequence->IsTrackSupported(UMovieSceneSingleCameraCutTrack::StaticClass()) : ETrackSupport::NotSupported;
    return ( TrackSupported == ETrackSupport::Supported || InSequence->IsA( ULevelSequence::StaticClass() ) );
}


bool FSingleCameraCutTrackEditor::SupportsType(TSubclassOf<UMovieSceneTrack> Type) const
{
    return (Type == UMovieSceneSingleCameraCutTrack::StaticClass());
}


void FSingleCameraCutTrackEditor::Tick(float DeltaTime)
{
    TSharedPtr<ISequencer> SequencerPin = GetSequencer();
    if (!SequencerPin.IsValid())
    {
        return;
    }

    EMovieScenePlayerStatus::Type PlaybackState = SequencerPin->GetPlaybackStatus();

    if (FSlateThrottleManager::Get().IsAllowingExpensiveTasks() && PlaybackState != EMovieScenePlayerStatus::Playing && PlaybackState != EMovieScenePlayerStatus::Scrubbing)
    {
        SequencerPin->EnterSilentMode();

        FQualifiedFrameTime SavedTime = SequencerPin->GetLocalTime();

        if (DeltaTime > 0.f && ThumbnailPool->DrawThumbnails())
        {
            SequencerPin->SetLocalTimeDirectly(SavedTime.Time);
        }

        SequencerPin->ExitSilentMode();
    }
}


const FSlateBrush* FSingleCameraCutTrackEditor::GetIconBrush() const
{
    return FEposTracksEditorStyle::Get().GetBrush("Sequencer.Tracks.SingleCameraCut");
}


bool FSingleCameraCutTrackEditor::OnAllowDrop(const FDragDropEvent& DragDropEvent, FSequencerDragDropParams& DragDropParams )
{
    if (!DragDropParams.Track.IsValid() || !DragDropParams.Track.Get()->IsA( UMovieSceneSingleCameraCutTrack::StaticClass()))
    {
        return false;
    }

    TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();

    if (!Operation.IsValid() || !Operation->IsOfType<FActorDragDropGraphEdOp>() )
    {
        return false;
    }

    UMovieSceneSingleCameraCutTrack* CameraCutTrack = Cast<UMovieSceneSingleCameraCutTrack>( DragDropParams.Track );

    TSharedPtr<FActorDragDropGraphEdOp> DragDropOp = StaticCastSharedPtr<FActorDragDropGraphEdOp>( Operation );

    for (auto& ActorPtr : DragDropOp->Actors)
    {
        if (ActorPtr.IsValid())
        {
            AActor* Actor = ActorPtr.Get();

            UCameraComponent* CameraComponent = MovieSceneHelpers::CameraComponentFromActor(Actor);
            if (CameraComponent)
            {
                FFrameNumber EndFrameNumber = CameraCutTrack->FindEndTimeForCameraCut( DragDropParams.FrameNumber );
                DragDropParams.FrameRange = TRange<FFrameNumber>( DragDropParams.FrameNumber, EndFrameNumber );
                return true;
            }
        }
    }

    return false;
}


FReply FSingleCameraCutTrackEditor::OnDrop(const FDragDropEvent& DragDropEvent, const FSequencerDragDropParams& DragDropParams)
{
    if( !DragDropParams.Track.IsValid() || !DragDropParams.Track.Get()->IsA( UMovieSceneSingleCameraCutTrack::StaticClass() ) )
    {
        return FReply::Unhandled();
    }

    TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();

    if (!Operation.IsValid() || !Operation->IsOfType<FActorDragDropGraphEdOp>() )
    {
        return FReply::Unhandled();
    }

    TSharedPtr<FActorDragDropGraphEdOp> DragDropOp = StaticCastSharedPtr<FActorDragDropGraphEdOp>( Operation );

    FMovieSceneTrackEditor::BeginKeying( DragDropParams.FrameNumber );

    bool bAnyDropped = false;
    for (auto& ActorPtr : DragDropOp->Actors)
    {
        if (ActorPtr.IsValid())
        {
            AActor* Actor = ActorPtr.Get();

            FGuid ObjectGuid = FindOrCreateHandleToObject(Actor).Handle;

            if (ObjectGuid.IsValid())
            {
                AnimatablePropertyChanged(FOnKeyProperty::CreateRaw(this, &FSingleCameraCutTrackEditor::AddKeyInternal, ObjectGuid));

                bAnyDropped = true;
            }
        }
    }

    FMovieSceneTrackEditor::EndKeying();

    return bAnyDropped ? FReply::Handled() : FReply::Unhandled();
}


/* FCameraCutTrackEditor implementation
 *****************************************************************************/

FKeyPropertyResult FSingleCameraCutTrackEditor::AddKeyInternal( FFrameNumber KeyTime, const FGuid ObjectGuid )
{
    FKeyPropertyResult KeyPropertyResult;

    UMovieSceneSingleCameraCutTrack* SingleCameraCutTrack = FindOrCreateSingleCameraCutTrack();
    const TArray<UMovieSceneSection*>& AllSections = SingleCameraCutTrack->GetAllSections();

    UMovieSceneSingleCameraCutSection* NewSection = SingleCameraCutTrack->AddNewSingleCameraCut( UE::MovieScene::FRelativeObjectBindingID( ObjectGuid ), KeyTime );
    KeyPropertyResult.bTrackModified = true;
    KeyPropertyResult.SectionsCreated.Add(NewSection);

    GetSequencer()->EmptySelection();
    GetSequencer()->SelectSection(NewSection);
    GetSequencer()->ThrobSectionSelection();

    return KeyPropertyResult;
}


UMovieSceneSingleCameraCutTrack* FSingleCameraCutTrackEditor::FindOrCreateSingleCameraCutTrack()
{
    UMovieScene* FocusedMovieScene = GetFocusedMovieScene();
    if (FocusedMovieScene->IsReadOnly())
    {
        return nullptr;
    }

    UMovieSceneTrack* CameraCutTrack = FocusedMovieScene->GetCameraCutTrack();

    if (CameraCutTrack == nullptr)
    {
        const FScopedTransaction Transaction(LOCTEXT("AddCameraCutTrack_Transaction", "Add Camera Cut Track"));
        FocusedMovieScene->Modify();

        CameraCutTrack = FocusedMovieScene->AddCameraCutTrack(UMovieSceneSingleCameraCutTrack::StaticClass());
    }

    return CastChecked<UMovieSceneSingleCameraCutTrack>(CameraCutTrack);
}


/* FCameraCutTrackEditor callbacks
 *****************************************************************************/

bool FSingleCameraCutTrackEditor::HandleAddSingleCameraCutTrackMenuEntryCanExecute() const
{
    UMovieScene* FocusedMovieScene = GetFocusedMovieScene();

    return ((FocusedMovieScene != nullptr) && (FocusedMovieScene->GetCameraCutTrack() == nullptr));
}

void FSingleCameraCutTrackEditor::HandleAddSingleCameraCutTrackMenuEntryExecute()
{
    UMovieSceneSingleCameraCutTrack* CameraCutTrack = FindOrCreateSingleCameraCutTrack();

    if (CameraCutTrack)
    {
        if (GetSequencer().IsValid())
        {
            GetSequencer()->OnAddTrack(CameraCutTrack, FGuid());
        }
    }
}

bool FSingleCameraCutTrackEditor::HandleAddSingleCameraCutTrackMenuEntryIsVisible()
{
    UMovieSceneSequence* FocusedSequence = GetSequencer()->GetFocusedMovieSceneSequence();

    return ( ( FocusedSequence != nullptr ) && ( FocusedSequence->IsA<UShotSequence>() ) );
}

bool FSingleCameraCutTrackEditor::IsCameraPickable(const AActor* const PickableActor)
{
    if (PickableActor->IsListedInSceneOutliner() &&
        !FActorEditorUtils::IsABuilderBrush(PickableActor) &&
        !PickableActor->IsA( AWorldSettings::StaticClass() ) &&
         IsValid(PickableActor))
    {
        UCameraComponent* CameraComponent = MovieSceneHelpers::CameraComponentFromActor(PickableActor);
        if (CameraComponent)
        {
            return true;
        }
    }
    return false;
}

TSharedRef<SWidget> FSingleCameraCutTrackEditor::HandleAddSingleCameraCutComboButtonGetMenuContent()
{
    FMenuBuilder MenuBuilder(true, nullptr);

    auto CreateNewCamera =
        [this](FMenuBuilder& SubMenuBuilder)
        {
            FSceneOutlinerInitializationOptions InitOptions;
            {
                InitOptions.bShowHeaderRow = false;
                InitOptions.bFocusSearchBoxWhenOpened = true;
                InitOptions.bShowTransient = true;
                InitOptions.bShowCreateNewFolder = false;
                // Only want the actor label column
                InitOptions.ColumnMap.Add( FSceneOutlinerBuiltInColumnTypes::Label(), FSceneOutlinerColumnInfo( ESceneOutlinerColumnVisibility::Visible, 0 ) );

                // Only display Actors that we can attach too
                InitOptions.Filters->AddFilterPredicate<FActorTreeItem>( FActorTreeItem::FFilterPredicate::CreateRaw( this, &FSingleCameraCutTrackEditor::IsCameraPickable ) );
            }

            // Actor selector to allow the user to choose a parent actor
            FSceneOutlinerModule& SceneOutlinerModule = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>( "SceneOutliner" );

            TSharedPtr<ISequencer> Sequencer = GetSequencer();
            const float WidthOverride = Sequencer.IsValid() ? Sequencer->GetSequencerSettings()->GetAssetBrowserWidth() : 500.f;
            const float HeightOverride = Sequencer.IsValid() ? Sequencer->GetSequencerSettings()->GetAssetBrowserHeight() : 400.f;

            TSharedRef< SWidget > MenuWidget =
                SNew( SHorizontalBox )

                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SBox )
                    .WidthOverride( WidthOverride )
                    .HeightOverride( HeightOverride )
                    [
                        SceneOutlinerModule.CreateActorPicker(
                            InitOptions,
                            FOnActorPicked::CreateSP(this, &FSingleCameraCutTrackEditor::HandleAddSingleCameraCutComboButtonMenuEntryExecute )
                            )
                    ]
                ];
            SubMenuBuilder.AddWidget(MenuWidget, FText::GetEmpty(), false);
        };

    TSharedPtr<ISequencer> SequencerPtr = GetSequencer();

    // Always recreate the binding picker to ensure we have the correct sequence ID
    BindingIDPicker = MakeShared<FTrackEditorBindingIDPicker>(SequencerPtr->GetFocusedTemplateID(), SequencerPtr);
    BindingIDPicker->OnBindingPicked().AddRaw(this, &FSingleCameraCutTrackEditor::CreateNewSectionFromBinding);

    FText ExistingBindingText = LOCTEXT("ExistingBinding", "Existing Binding");
    FText NewBindingText = LOCTEXT("NewBinding", "New Binding");

    const bool bHasExistingBindings = !BindingIDPicker->IsEmpty();
    if (bHasExistingBindings)
    {
        MenuBuilder.AddSubMenu(
            NewBindingText,
            LOCTEXT("NewBinding_Tip", "Add a new camera cut by creating a new binding to an object in the world."),
            FNewMenuDelegate::CreateLambda(CreateNewCamera)
        );

        MenuBuilder.BeginSection(NAME_None, ExistingBindingText);
        {
            BindingIDPicker->GetPickerMenu(MenuBuilder);
        }
        MenuBuilder.EndSection();
    }
    else
    {
        MenuBuilder.BeginSection(NAME_None, NewBindingText);
        {
            CreateNewCamera(MenuBuilder);
        }
        MenuBuilder.EndSection();
    }

    return MenuBuilder.MakeWidget();
}


void FSingleCameraCutTrackEditor::CreateNewSectionFromBinding(FMovieSceneObjectBindingID InBindingID)
{
    auto CreateNewSection = [this, InBindingID](FFrameNumber KeyTime)
    {
        FKeyPropertyResult KeyPropertyResult;

        UMovieSceneSingleCameraCutSection* NewSection = FindOrCreateSingleCameraCutTrack()->AddNewSingleCameraCut(InBindingID, KeyTime);
        KeyPropertyResult.bTrackModified = true;
        KeyPropertyResult.SectionsCreated.Add(NewSection);

        GetSequencer()->EmptySelection();
        GetSequencer()->SelectSection(NewSection);
        GetSequencer()->ThrobSectionSelection();

        return KeyPropertyResult;
    };

    AnimatablePropertyChanged(FOnKeyProperty::CreateLambda(CreateNewSection));
}


void FSingleCameraCutTrackEditor::HandleAddSingleCameraCutComboButtonMenuEntryExecute(AActor* Camera)
{
    FGuid ObjectGuid = FindOrCreateHandleToObject(Camera).Handle;

    if (ObjectGuid.IsValid())
    {
        AnimatablePropertyChanged(FOnKeyProperty::CreateRaw(this, &FSingleCameraCutTrackEditor::AddKeyInternal, ObjectGuid));
    }
}

ECheckBoxState FSingleCameraCutTrackEditor::IsCameraLocked() const
{
    if (GetSequencer()->IsPerspectiveViewportCameraCutEnabled())
    {
        return ECheckBoxState::Checked;
    }
    else
    {
        return ECheckBoxState::Unchecked;
    }
}


void FSingleCameraCutTrackEditor::OnLockCameraClicked(ECheckBoxState CheckBoxState)
{
    TSharedPtr<ISequencer> SequencerPtr = GetSequencer();

    const bool bEnableCameraCuts = ( CheckBoxState == ECheckBoxState::Checked );
    SequencerPtr->SetPerspectiveViewportCameraCutEnabled( bEnableCameraCuts );

    bool bNeedsRestoreViewport = true;
    if( const USequencerSettings* SequencerSettings = SequencerPtr->GetSequencerSettings() )
    {
        bNeedsRestoreViewport = SequencerSettings->GetRestoreOriginalViewportOnCameraCutUnlock();
    }

    UMovieSceneEntitySystemLinker* Linker = SequencerPtr->GetEvaluationTemplate().GetEntitySystemLinker();
    UMovieSceneSingleCameraCutTrackInstance::ToggleCameraCutLock( Linker, bEnableCameraCuts, bNeedsRestoreViewport );

    SequencerPtr->ForceEvaluate();
}

void FSingleCameraCutTrackEditor::ToggleLockCamera()
{
    OnLockCameraClicked(IsCameraLocked() == ECheckBoxState::Checked ?  ECheckBoxState::Unchecked :  ECheckBoxState::Checked);
}

FText FSingleCameraCutTrackEditor::GetLockCameraToolTip() const
{
    const TSharedRef<const FInputChord> FirstActiveChord = FSingleCameraCutTrackCommands::Get().ToggleLockCamera->GetFirstValidChord();

    FText Tooltip = IsCameraLocked() == ECheckBoxState::Checked ?
        LOCTEXT("UnlockCamera", "Unlock Viewport from Camera Cuts") :
        LOCTEXT("LockCamera", "Lock Viewport to Camera Cuts");

    if( FirstActiveChord->IsValidChord() )
    {
        return FText::Join( FText::FromString( TEXT( " " ) ), Tooltip, FirstActiveChord->GetInputText() );
    }
    return Tooltip;
}

#undef LOCTEXT_NAMESPACE
