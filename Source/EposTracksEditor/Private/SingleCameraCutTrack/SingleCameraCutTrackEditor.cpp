// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SingleCameraCutTrack/SingleCameraCutTrackEditor.h"

#include "Widgets/SBoxPanel.h"
#include "ActorTreeItem.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Modules/ModuleManager.h"
#include "Application/ThrottleManager.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "MovieSceneCommonHelpers.h"
#include "EditorStyleSet.h"
#include "GameFramework/WorldSettings.h"
#include "LevelEditorViewport.h"
#include "SequencerUtilities.h"
#include "Editor.h"
#include "ActorEditorUtils.h"
#include "SceneOutlinerPublicTypes.h"
#include "SceneOutlinerModule.h"
#include "TrackEditorThumbnail/TrackEditorThumbnailPool.h"
#include "MovieSceneObjectBindingIDPicker.h"
#include "MovieSceneToolHelpers.h"
#include "DragAndDrop/ActorDragDropGraphEdOp.h"

#include "Shot/ShotSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
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
}

//void FCameraCutTrackEditor::HandleToggleCanBlendExecute(UMovieSceneCameraCutTrack* CameraCutTrack)
//{
//  CameraCutTrack->bCanBlend = !CameraCutTrack->bCanBlend;
//
//  if (!CameraCutTrack->bCanBlend)
//  {
//      // Reset all easing and remove overlaps.
//      const UMovieScene* FocusedMovieScene = GetFocusedMovieScene();
//      const FFrameRate TickResolution = FocusedMovieScene->GetTickResolution();
//      const FFrameRate DisplayRate = FocusedMovieScene->GetDisplayRate();
//
//      const TArray<UMovieSceneSection*> Sections = CameraCutTrack->GetAllSections();
//      for (int32 Idx = 1; Idx < Sections.Num(); ++Idx)
//      {
//          UMovieSceneSection* CurSection = Sections[Idx];
//          UMovieSceneSection* PrevSection = Sections[Idx - 1];
//
//          CurSection->Modify();
//
//          TRange<FFrameNumber> CurSectionRange = CurSection->GetRange();
//          TRange<FFrameNumber> PrevSectionRange = PrevSection->GetRange();
//          const FFrameNumber OverlapOrGap = (PrevSectionRange.GetUpperBoundValue() - CurSectionRange.GetLowerBoundValue());
//          if (OverlapOrGap > 0)
//          {
//              const FFrameTime TimeAtHalfBlend = CurSectionRange.GetLowerBoundValue() + FMath::FloorToInt(OverlapOrGap.Value / 2.f);
//              const FFrameNumber FrameAtHalfBlend = FFrameRate::Snap(TimeAtHalfBlend, TickResolution, DisplayRate).CeilToFrame();
//
//              PrevSectionRange.SetUpperBoundValue(FrameAtHalfBlend);
//              PrevSection->SetRange(PrevSectionRange);
//
//              CurSectionRange.SetLowerBoundValue(FrameAtHalfBlend);
//              CurSection->SetRange(CurSectionRange);
//          }
//
//          CurSection->Easing.AutoEaseInDuration = 0;
//          PrevSection->Easing.AutoEaseOutDuration = 0;
//      }
//      if (Sections.Num() > 0)
//      {
//          Sections[0]->Modify();
//
//          Sections[0]->Easing.AutoEaseInDuration = 0;
//          Sections[0]->Easing.ManualEaseInDuration = 0;
//          Sections.Last()->Easing.AutoEaseOutDuration = 0;
//          Sections.Last()->Easing.ManualEaseOutDuration = 0;
//      }
//  }
//}

TSharedPtr<SWidget> FSingleCameraCutTrackEditor::BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params)
{
    // Create a container edit box
    return SNew(SHorizontalBox)

    // Add the camera combo box
    + SHorizontalBox::Slot()
    .AutoWidth()
    .VAlign(VAlign_Center)
    [
        FSequencerUtilities::MakeAddButton(LOCTEXT("SingleCameraCutText", "Camera"), FOnGetContent::CreateSP(this, &FSingleCameraCutTrackEditor::HandleAddSingleCameraCutComboButtonGetMenuContent), Params.NodeIsHovered, GetSequencer())
    ]

    + SHorizontalBox::Slot()
    .VAlign(VAlign_Center)
    .HAlign(HAlign_Right)
    .AutoWidth()
    .Padding(4, 0, 0, 0)
    [
        SNew(SCheckBox)
        .Style( &FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "ToggleButtonCheckBoxAlt" ) )
        .Type( ESlateCheckBoxType::CheckBox )
        .Padding( FMargin( 0.f ) )
        .IsFocusable( false )
        .IsChecked( this, &FSingleCameraCutTrackEditor::IsCameraLocked )
        .OnCheckStateChanged( this, &FSingleCameraCutTrackEditor::OnLockCameraClicked )
        .ToolTipText( this, &FSingleCameraCutTrackEditor::GetLockCameraToolTip )
        .CheckedImage( FAppStyle::Get().GetBrush( "Sequencer.LockCamera" ) )
        .CheckedHoveredImage( FAppStyle::Get().GetBrush( "Sequencer.LockCamera" ) )
        .CheckedPressedImage( FAppStyle::Get().GetBrush( "Sequencer.LockCamera" ) )
        .UncheckedImage( FAppStyle::Get().GetBrush( "Sequencer.UnlockCamera" ) )
        .UncheckedHoveredImage( FAppStyle::Get().GetBrush( "Sequencer.UnlockCamera" ) )
        .UncheckedPressedImage( FAppStyle::Get().GetBrush( "Sequencer.UnlockCamera" ) )
    ];
}


TSharedRef<ISequencerSection> FSingleCameraCutTrackEditor::MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding)
{
    check(SupportsType(SectionObject.GetOuter()->GetClass()));

    return MakeShareable(new FSingleCameraCutSection(GetSequencer(), ThumbnailPool, SectionObject));
}


bool FSingleCameraCutTrackEditor::SupportsSequence(UMovieSceneSequence* InSequence) const
{
    ETrackSupport TrackSupported = InSequence ? InSequence->IsTrackSupported(UMovieSceneSingleCameraCutTrack::StaticClass()) : ETrackSupport::NotSupported;
    return TrackSupported == ETrackSupport::Supported;
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

            TSharedRef< SWidget > MenuWidget =
                SNew( SHorizontalBox )

                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SBox )
                    .MaxDesiredHeight( 400.0f )
                    .WidthOverride( 300.0f )
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
    if (CheckBoxState == ECheckBoxState::Checked)
    {
        for(FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
        {
            if (LevelVC && LevelVC->AllowsCinematicControl() && LevelVC->GetViewMode() != VMI_Unknown)
            {
                LevelVC->SetActorLock(nullptr);
                LevelVC->bLockedCameraView = false;
                LevelVC->UpdateViewForLockedActor();
                LevelVC->Invalidate();
            }
        }
        GetSequencer()->SetPerspectiveViewportCameraCutEnabled(true);
    }
    else
    {
        GetSequencer()->UpdateCameraCut(nullptr, EMovieSceneCameraCutParams());
        GetSequencer()->SetPerspectiveViewportCameraCutEnabled(false);
    }

    GetSequencer()->ForceEvaluate();
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
