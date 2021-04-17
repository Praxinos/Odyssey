// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Board/BoardSequenceCustomization.h"

#include "ArrangeSectionsType.h"
#include "Board/BoardSequence.h"
#include "Board/BoardSequenceEditorCommands.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "EposTracksEditorHelpers.h"
#include "EposTracksModule.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/EposTracksSettings.h"
#include "Sections/MovieSceneSubSection.h"
#include "Shot/ShotSequence.h"
#include "Shot/ShotSequenceEditorCommands.h"
#include "ShotHelpers/ShotSequenceHelpers.h"

#define LOCTEXT_NAMESPACE "BoardSequenceCustomization"

//---

void
FBoardSequenceCustomization::RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder ) // This is called each time the focused sequence changed (ie. when double-clicking on a section to go inside its subsequence)
{
    mSequencer = &ioBuilder.GetSequencer();
    mBoardSequence = Cast<UBoardSequence>( &ioBuilder.GetFocusedSequence() );

    //---

    mPatchNotifySequencerHandle = FEposTracksModule::GetTracksCustomizationManager().Register( FOnNotifySequencer::CreateLambda( [this](){ mSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::Unknown ); } ) );

    ProcessCommands( mSequencer->GetCommandBindings(), kMap );

    //---

    FSequencerCustomizationInfo customization;

    TSharedRef<FExtender> ToolbarExtender = MakeShared<FExtender>();
    ToolbarExtender->AddToolBarExtension( "Curve Editor", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw( this, &FBoardSequenceCustomization::ExtendSequencerToolbar ) );
    customization.ToolbarExtender = ToolbarExtender;

    //customization.OnReceivedDragOver.BindRaw( this, &FBoardSequenceCustomization::OnSequencerReceiveDragOver );
    //customization.OnReceivedDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerReceiveDrop );

    customization.OnAssetsDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerAssetsDrop );
    customization.OnClassesDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerClassesDrop );
    customization.OnActorsDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerActorsDrop );

    ioBuilder.AddCustomization( customization );
}

void
FBoardSequenceCustomization::UnregisterSequencerCustomization()
{
    FEposTracksModule::GetTracksCustomizationManager().Unregister( mPatchNotifySequencerHandle );

    ProcessCommands( mSequencer->GetCommandBindings(), kUnmap );

    //---

    mSequencer = nullptr;
    mBoardSequence = nullptr;
}

//---

void
FBoardSequenceCustomization::ProcessCommands( TSharedPtr<FUICommandList> CommandList, EMapping iMap )
{
    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().ArrangeShotsManually,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::SetArrangeSections, EArrangeSections::Manually ),
            FCanExecuteAction::CreateLambda([] { return true; }),
            FIsActionChecked::CreateLambda([] { return GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots == EArrangeSections::Manually; } )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().ArrangeShotsManually );

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::SetArrangeSections, EArrangeSections::OnOneRow ),
            FCanExecuteAction::CreateLambda([] { return true; }),
            FIsActionChecked::CreateLambda([] { return GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots == EArrangeSections::OnOneRow; } )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow );

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::SetArrangeSections, EArrangeSections::OnTwoRowsShifted ),
            FCanExecuteAction::CreateLambda([] { return true; }),
            FIsActionChecked::CreateLambda([] { return GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots == EArrangeSections::OnTwoRowsShifted; } )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows );

    //---

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().NewSectionWithBoardAtCurrentFrame,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::NewSectionWithBoardAtCurrentFrame )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().NewSectionWithBoardAtCurrentFrame );

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().NewSectionWithShotAtCurrentFrame,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::NewSectionWithShotAtCurrentFrame )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().NewSectionWithShotAtCurrentFrame );

    //---

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().CreateCamera,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::CreateCamera ),
            FCanExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::CheckNoCamera )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().CreateCamera );

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().SnapCameraToViewport,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::SnapCameraToViewport ),
            FCanExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::CheckCamera )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().SnapCameraToViewport );

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().CreatePlane,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::CreatePlane ),
            FCanExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::CheckCamera )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().CreatePlane );
}

void
FBoardSequenceCustomization::ArrangeSections()
{
    EposTracksEditorHelpers::ArrangeSections( mSequencer );
}

void
FBoardSequenceCustomization::SetArrangeSections( EArrangeSections iArrangeSections )
{
    EposTracksEditorHelpers::SetArrangeSections( mSequencer, iArrangeSections );
}

void
FBoardSequenceCustomization::NewSectionWithBoardAtCurrentFrame()
{
    EposTracksEditorHelpers::InsertBoard( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber );
}
void
FBoardSequenceCustomization::NewSectionWithShotAtCurrentFrame()
{
    EposTracksEditorHelpers::InsertShot( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber );
}

//---

void
FBoardSequenceCustomization::CreateCamera()
{
    FInnerSequenceResult inner_sequence = GetInnerSequence();
    check( inner_sequence.mInnerSequence );

    ShotSequenceHelpers::CreateCamera( *mSequencer, inner_sequence.mInnerSequence, inner_sequence.mInnerSequenceId );
}

void
FBoardSequenceCustomization::SnapCameraToViewport()
{
    FInnerSequenceResult inner_sequence = GetInnerSequence();
    check( inner_sequence.mInnerSequence );

    ShotSequenceHelpers::SnapCameraToViewport( *mSequencer, inner_sequence.mInnerSequence, inner_sequence.mInnerSequenceId );
}

void
FBoardSequenceCustomization::CreatePlane()
{
    FInnerSequenceResult inner_sequence = GetInnerSequence();
    check( inner_sequence.mInnerSequence );

    ShotSequenceHelpers::CreatePlane( *mSequencer, inner_sequence.mInnerSequence, inner_sequence.mInnerSequenceId );
}

FBoardSequenceCustomization::FInnerSequenceResult
FBoardSequenceCustomization::GetInnerSequence()
{
    FInnerSequenceResult result = { nullptr, MovieSceneSequenceID::Invalid };

    UMovieSceneSequence* sequence = mSequencer->GetFocusedMovieSceneSequence();
    UMovieScene* moviescene = sequence ? sequence->GetMovieScene() : nullptr;
    UMovieSceneSection* section = moviescene ? MovieSceneHelpers::FindSectionAtTime( moviescene->GetAllSections(), mSequencer->GetLocalTime().Time.GetFrame() ) : nullptr;
    UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );

    result.mInnerSequence = subsection ? subsection->GetSequence() : nullptr;
    //result.mInnerSequenceId = subsection ? subsection->GetSequenceID() : FMovieSceneSequenceID();

    //---

    if( subsection )
    {
        const FMovieSceneSequenceID             thisSequenceID = mSequencer->GetFocusedTemplateID();
        const FMovieSceneSequenceID             targetSequenceID = subsection->GetSequenceID();
        const FMovieSceneSequenceHierarchy*     hierarchy = mSequencer->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( mSequencer->GetEvaluationTemplate().GetCompiledDataID() );

        if( !hierarchy )
            return result;

        const FMovieSceneSequenceHierarchyNode* thisSequenceNode = hierarchy->FindNode( thisSequenceID );

        check( thisSequenceNode );

        // Find the TargetSequenceID by comparing deterministic sequence IDs for all children of the current node
        const FMovieSceneSequenceID* innerSequenceID = Algo::FindByPredicate( thisSequenceNode->Children,
            [hierarchy, targetSequenceID]( FMovieSceneSequenceID iSequenceID )
            {
                const FMovieSceneSubSequenceData* subData = hierarchy->FindSubData( iSequenceID );
                return subData && subData->DeterministicSequenceID == targetSequenceID;
            }
        );

        result.mInnerSequenceId = innerSequenceID ? *innerSequenceID : FMovieSceneSequenceID();
    }

    return result;
}

bool
FBoardSequenceCustomization::CheckNoCamera()
{
    FInnerSequenceResult inner_sequence = GetInnerSequence();
    if( !inner_sequence.mInnerSequence )
        return false;

    if( !inner_sequence.mInnerSequence->IsA<UShotSequence>() )
        return false;

    return !ShotSequenceHelpers::GetCamera( *mSequencer, inner_sequence.mInnerSequence, inner_sequence.mInnerSequenceId, nullptr );
}

bool
FBoardSequenceCustomization::CheckCamera()
{
    FInnerSequenceResult inner_sequence = GetInnerSequence();
    if( !inner_sequence.mInnerSequence )
        return false;

    if( !inner_sequence.mInnerSequence->IsA<UShotSequence>() )
        return false;

    return ShotSequenceHelpers::GetCamera( *mSequencer, inner_sequence.mInnerSequence, inner_sequence.mInnerSequenceId, nullptr );
}

//---

void
FBoardSequenceCustomization::ExtendSequencerToolbar( FToolBarBuilder& ToolbarBuilder )
{
    ToolbarBuilder.AddSeparator();

    TAttribute<FText> ArrangeShotsName;
    ArrangeShotsName.Bind( TAttribute<FText>::FGetter::CreateLambda( []
    {
        switch( GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots )
        {
            case EArrangeSections::OnOneRow:            return FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow->GetLabel();
            case EArrangeSections::OnTwoRowsShifted:    return FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows->GetLabel();
            default:
            case EArrangeSections::Manually:            return FBoardSequenceEditorCommands::Get().ArrangeShotsManually->GetLabel();
        }
    } ) );

    TAttribute<FSlateIcon> ArrangeShotsIcon;
    ArrangeShotsIcon.Bind( TAttribute<FSlateIcon>::FGetter::CreateLambda( []
    {
        switch( GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots )
        {
            case EArrangeSections::OnOneRow:            return FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow->GetIcon();
            case EArrangeSections::OnTwoRowsShifted:    return FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows->GetIcon();
            default:
            case EArrangeSections::Manually:            return FBoardSequenceEditorCommands::Get().ArrangeShotsManually->GetIcon();
        }
    } ) );

    TAttribute<FText> ArrangeShotsToolTip;
    ArrangeShotsToolTip.Bind( TAttribute<FText>::FGetter::CreateLambda( []
    {
        switch( GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots )
        {
            case EArrangeSections::OnOneRow:            return FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow->GetDescription();
            case EArrangeSections::OnTwoRowsShifted:    return FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows->GetDescription();
            default:
            case EArrangeSections::Manually:            return FBoardSequenceEditorCommands::Get().ArrangeShotsManually->GetDescription();
        }
    } ) );

    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeArrangeShotsMenu ),
        ArrangeShotsName,
        ArrangeShotsToolTip,
        ArrangeShotsIcon );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FBoardSequenceEditorCommands::Get().NewSectionWithBoardAtCurrentFrame );
    ToolbarBuilder.AddToolBarButton( FBoardSequenceEditorCommands::Get().NewSectionWithShotAtCurrentFrame );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().CreateCamera );
    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeCameraMenu ),
        LOCTEXT( "CameraOptions", "Options" ),
        LOCTEXT( "CameraOptionsToolTip", "Camera Options" ),
        TAttribute<FSlateIcon>(),
        true );

    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().SnapCameraToViewport );
    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().CreatePlane );

    ToolbarBuilder.AddSeparator();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeArrangeShotsMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    //MenuBuilder.BeginSection( NAME_None, LOCTEXT( "ArrangeShotsHeader", "Arrange Shots" ) );
    {
        MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().ArrangeShotsManually );
        MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow );
        MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows );
    }
    //MenuBuilder.EndSection();

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeCameraMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    MenuBuilder.BeginSection( NAME_None, LOCTEXT( "CameraSettingsTitle", "Default Camera Settings" ) );
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );

        // Create a detail view
        FDetailsViewArgs Args( false, false, false, FDetailsViewArgs::HideNameArea );
        Args.ColumnWidth = .5f;
        TSharedRef<IDetailsView> DetailView = PropertyModule.CreateDetailView( Args );

        // Filter properties to only get CameraSettings ones
        auto visible_property = []( const FPropertyAndParent& iPropertyChain )
        {
            FName root_name = iPropertyChain.ParentProperties.Num() ? iPropertyChain.ParentProperties.Last()->GetFName() : iPropertyChain.Property.GetFName();
            return root_name == GET_MEMBER_NAME_CHECKED( UEposSequenceEditorSettings, CameraSettings );
        };
        DetailView->GetIsPropertyVisibleDelegate() = FIsPropertyVisible::CreateLambda( visible_property );
        // Set the object to view
        DetailView->SetObject( GetMutableDefault<UEposSequenceEditorSettings>() );

        MenuBuilder.AddWidget( DetailView, FText(), true );
    }
    MenuBuilder.EndSection();

    return MenuBuilder.MakeWidget();
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
    // Maybe we can store data, and then get them inside tracks through (like in sequencer.cpp ?)
    //   ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" );
    //   TSharedPtr<FSequencerCustomizationManager> Manager = SequencerModule.GetSequencerCustomizationManager();
    //   ...
    // because once an asset is dropped on a NOT supported track, it goes through here, where we can store iDragDropOp.X/Y,
    // and get them again when we go inside CinematicBoardTrackEditor:HandleAssetAdded() where the information is not given

    return ESequencerDropResult::Unhandled; // Process the default behavior for assets
}

ESequencerDropResult
FBoardSequenceCustomization::OnSequencerClassesDrop( const TArray<TWeakObjectPtr<UClass>>& iClasses, const FClassDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::DropDenied;    // Don't accept classes
}

ESequencerDropResult
FBoardSequenceCustomization::OnSequencerActorsDrop( const TArray<TWeakObjectPtr<AActor>>& iActors, const FActorDragDropGraphEdOp& iDragDropOp )
{
    return ESequencerDropResult::DropDenied;    // Don't accept actors
}

#undef LOCTEXT_NAMESPACE
