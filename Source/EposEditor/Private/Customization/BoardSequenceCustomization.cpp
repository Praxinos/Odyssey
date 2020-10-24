// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Customization/BoardSequenceCustomization.h"

#include "Board/BoardSequence.h"
#include "BoardSequenceEditorCommands.h"
#include "Settings/EposEditorSettings.h"

#define LOCTEXT_NAMESPACE "BoardSequenceCustomization"

//---

void
FBoardSequenceCustomization::RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder ) // This is called each time the focused sequence changed (ie. when double-clicking on a section to go inside its subsequence)
{
    mSequencer = &ioBuilder.GetSequencer();
    mBoardSequence = Cast<UBoardSequence>( &ioBuilder.GetFocusedSequence() );

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
    mSequencer = nullptr;
    mBoardSequence = nullptr;
}

//---

void
FBoardSequenceCustomization::ExtendSequencerToolbar( FToolBarBuilder& ToolbarBuilder )
{
    ToolbarBuilder.AddSeparator();

    TAttribute<FText> ArrangeShotsName;
    ArrangeShotsName.Bind( TAttribute<FText>::FGetter::CreateLambda( []
    {
        switch( GetDefault<UEposEditorSettings>()->BoardTrackSettings.ArrangeShots )
        {
            case EArrangeShots::OnOneRow:
                return FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow->GetLabel();
            default:
            case EArrangeShots::OnTwoRowsShifted:
                return FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows->GetLabel();
        }
    } ) );

    TAttribute<FSlateIcon> ArrangeShotsIcon;
    ArrangeShotsIcon.Bind( TAttribute<FSlateIcon>::FGetter::CreateLambda( []
    {
        switch( GetDefault<UEposEditorSettings>()->BoardTrackSettings.ArrangeShots )
        {
            case EArrangeShots::OnOneRow:
                return FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow->GetIcon();
            default:
            case EArrangeShots::OnTwoRowsShifted:
                return FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows->GetIcon();
        }
    } ) );

    TAttribute<FText> ArrangeShotsToolTip;
    ArrangeShotsToolTip.Bind( TAttribute<FText>::FGetter::CreateLambda( []
    {
        switch( GetDefault<UEposEditorSettings>()->BoardTrackSettings.ArrangeShots )
        {
            case EArrangeShots::OnOneRow:
                return FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow->GetDescription();
            default:
            case EArrangeShots::OnTwoRowsShifted:
                return FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows->GetDescription();
        }
    } ) );

    ToolbarBuilder.AddToolBarButton( 
        FBoardSequenceEditorCommands::Get().ArrangeShots,
        NAME_None,
        ArrangeShotsName,
        ArrangeShotsToolTip,
        ArrangeShotsIcon );

    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeArrangeShotsMenu ),
        LOCTEXT( "ArrangeShots", "Arrange Shots" ),
        LOCTEXT( "ArrangeShotsToolTip", "Arrange Shots Options" ),
        TAttribute<FSlateIcon>(),
        true );

    ToolbarBuilder.AddSeparator();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeArrangeShotsMenu()
{
    FMenuBuilder MenuBuilder( false, mSequencer->GetCommandBindings() );

    MenuBuilder.BeginSection( "ArrangeShots", LOCTEXT( "ArrangeShotsHeader", "Arrange Shots" ) );
    {
        MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow );
        MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows );
    }
    MenuBuilder.EndSection();

    return MenuBuilder.MakeWidget();
}

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
