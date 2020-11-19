// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Board/BoardSequenceCustomization.h"

#include "ArrangeSectionsType.h"
#include "Board/BoardSequence.h"
#include "Board/BoardSequenceEditorCommands.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposTracksEditorHelpers.h"
#include "EposTracksModule.h"
#include "Settings/EposTracksEditorSettings.h"

#define LOCTEXT_NAMESPACE "BoardSequenceCustomization"

//---

void
FBoardSequenceCustomization::RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder ) // This is called each time the focused sequence changed (ie. when double-clicking on a section to go inside its subsequence)
{
    mSequencer = &ioBuilder.GetSequencer();
    mBoardSequence = Cast<UBoardSequence>( &ioBuilder.GetFocusedSequence() );

    //---

    mArrangeSectionsHandle = FEposTracksModule::GetTracksCustomizationManager().Register( FOnArrangeSections::CreateRaw( this, &FBoardSequenceCustomization::ArrangeSections ) );

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
    FEposTracksModule::GetTracksCustomizationManager().Unregister( mArrangeSectionsHandle );

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
            FIsActionChecked::CreateLambda([] { return GetDefault<UEposTracksEditorSettings>()->BoardTrackSettings.ArrangeShots == EArrangeSections::Manually; } )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().ArrangeShotsManually );

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::SetArrangeSections, EArrangeSections::OnOneRow ),
            FCanExecuteAction::CreateLambda([] { return true; }),
            FIsActionChecked::CreateLambda([] { return GetDefault<UEposTracksEditorSettings>()->BoardTrackSettings.ArrangeShots == EArrangeSections::OnOneRow; } )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow );

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::SetArrangeSections, EArrangeSections::OnTwoRowsShifted ),
            FCanExecuteAction::CreateLambda([] { return true; }),
            FIsActionChecked::CreateLambda([] { return GetDefault<UEposTracksEditorSettings>()->BoardTrackSettings.ArrangeShots == EArrangeSections::OnTwoRowsShifted; } )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows );

    //---

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().NewSectionWithBoardAtCurrentFrame,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::NewSectionWithBoardAtCurrentFrame ) );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().NewSectionWithBoardAtCurrentFrame );

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().NewSectionWithShotAtCurrentFrame,
            FExecuteAction::CreateRaw( this, &FBoardSequenceCustomization::NewSectionWithShotAtCurrentFrame ) );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().NewSectionWithShotAtCurrentFrame );
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
FBoardSequenceCustomization::ExtendSequencerToolbar( FToolBarBuilder& ToolbarBuilder )
{
    ToolbarBuilder.AddSeparator();

    TAttribute<FText> ArrangeShotsName;
    ArrangeShotsName.Bind( TAttribute<FText>::FGetter::CreateLambda( []
    {
        switch( GetDefault<UEposTracksEditorSettings>()->BoardTrackSettings.ArrangeShots )
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
        switch( GetDefault<UEposTracksEditorSettings>()->BoardTrackSettings.ArrangeShots )
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
        switch( GetDefault<UEposTracksEditorSettings>()->BoardTrackSettings.ArrangeShots )
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
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeArrangeShotsMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    //MenuBuilder.BeginSection( "ArrangeShots", LOCTEXT( "ArrangeShotsHeader", "Arrange Shots" ) );
    {
        MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().ArrangeShotsManually );
        MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow );
        MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows );
    }
    //MenuBuilder.EndSection();

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
