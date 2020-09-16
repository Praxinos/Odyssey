// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Customization/ShotSequenceCustomization.h"

#include "Shot/ShotSequence.h"

//---

void
FShotSequenceCustomization::RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder )
{
    mSequencer = &ioBuilder.GetSequencer();
    mShotSequence = Cast<UShotSequence>( &ioBuilder.GetFocusedSequence() );

    FSequencerCustomizationInfo customization;

    customization.OnAssetsDrop.BindRaw( this, &FShotSequenceCustomization::OnSequencerAssetsDrop );
    customization.OnClassesDrop.BindRaw( this, &FShotSequenceCustomization::OnSequencerClassesDrop );
    customization.OnActorsDrop.BindRaw( this, &FShotSequenceCustomization::OnSequencerActorsDrop );

    ioBuilder.AddCustomization( customization );
}

void
FShotSequenceCustomization::UnregisterSequencerCustomization()
{
    mSequencer = nullptr;
    mShotSequence = nullptr;
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
    return ESequencerDropResult::Unhandled; // No more used, but left for info
}

ESequencerDropResult
FShotSequenceCustomization::OnSequencerClassesDrop( const TArray<TWeakObjectPtr<UClass>>& iClasses, const FClassDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::Unhandled;
}

ESequencerDropResult
FShotSequenceCustomization::OnSequencerActorsDrop( const TArray<TWeakObjectPtr<AActor>>& iActors, const FActorDragDropGraphEdOp& iDragDropOp )
{
    return ESequencerDropResult::Unhandled;
}
