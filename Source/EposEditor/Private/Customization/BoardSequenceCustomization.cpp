// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Customization/BoardSequenceCustomization.h"

#include "TrackEditors/AudioTrackEditor.h"
#include "Tracks/MovieSceneAudioTrack.h"
#include "Sound/SoundBase.h"

#include "Board/BoardSequence.h"
#include "Shot/ShotSequence.h"
#include "Shot/ShotSequence.h"

//---

void
FBoardSequenceCustomization::RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder )
{
    mSequencer = &ioBuilder.GetSequencer();
    mBoardSequence = Cast<UBoardSequence>( &ioBuilder.GetFocusedSequence() );

    FSequencerCustomizationInfo customization;

    // customization.AddMenuExtender ...
    // customization.ToolbarExtender ...

    // customization.OnReceivedDragOver ...
    // customization.OnReceivedDrop ...

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

//bool
//FBoardSequenceCustomization::OnSequencerReceivedDragOver( const FGeometry& iMyGeometry, const FDragDropEvent& iDragDropEvent, FReply& oReply )
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
FBoardSequenceCustomization::OnSequencerAssetsDrop( const TArray<UObject*>& iAssets, const FAssetDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::Unhandled; // Process the default behavior for assets

#if 0
    if( !iAssets.Num() )
        return ESequencerDropResult::Unhandled;

    TArray<UObject*> valid_assets = iAssets.FilterByPredicate(
        []( const UObject* iAsset )
        {
            return iAsset->IsA( UBoardSequence::StaticClass() )
                    || iAsset->IsA( UShotSequence::StaticClass() )
                    /*|| iAsset->IsA( USoundBase::StaticClass() )*/;
        }
    );

    if( valid_assets.Num() == iAssets.Num() )   // If every assets are authorized in the board track
        return ESequencerDropResult::Unhandled; // let's the default behavior of the sequencer processes them

    return ESequencerDropResult::DropDenied;    // Don't process the default behavior of the sequencer
#endif
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
