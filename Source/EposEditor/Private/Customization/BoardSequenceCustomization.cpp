// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Customization/BoardSequenceCustomization.h"

#include "TrackEditors/AudioTrackEditor.h"
#include "Tracks/MovieSceneAudioTrack.h"
#include "Sound/SoundBase.h"

#include "Board/BoardSequence.h"
#include "Shot/ShotSequence.h"

//---

void
FBoardSequenceCustomization::RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder )
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
    //ToolbarBuilder.AddSeparator();

    //ToolbarBuilder.AddToolBarButton( FBoardSequenceCustomization::Get().CreateCamera );

    //TSharedRef<SHorizontalBox> Widget = SNew(SHorizontalBox)
    //	+SHorizontalBox::Slot()
    //	.AutoWidth()
    //	.VAlign(VAlign_Center)
    //	[
    //		SNew(STextBlock)
    //		.Text(LOCTEXT("BoundActorClassPicker", "Bound Actor Class"))
        //]
        //+SHorizontalBox::Slot()
        //.AutoWidth()
        //.VAlign(VAlign_Center)
        //[
        //	SNew(SComboButton)
        //	.OnGetMenuContent_Raw(this, &FTemplateSequenceCustomization::GetBoundActorClassMenuContent)
        //	.ButtonContent()
        //	[
        //		SNew(STextBlock)
        //		.Text_Raw(this, &FTemplateSequenceCustomization::GetBoundActorClassName)
        //	]
        //];

    //ToolbarBuilder.AddWidget(Widget);
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
