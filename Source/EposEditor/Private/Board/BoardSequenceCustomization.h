// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "SequencerCustomizationManager.h"
#include "Widgets/SWidget.h"

#include "ArrangeSectionsType.h"

class UBoardSequence;

/**
 * The sequencer customization for board sequences.
 */
class FBoardSequenceCustomization 
    : public ISequencerCustomization
{
public:
    virtual void RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder ) override;
    virtual void UnregisterSequencerCustomization() override;

private:
    void ExtendSequencerToolbar( FToolBarBuilder& ToolbarBuilder );
    TSharedRef<SWidget> MakeArrangeShotsMenu();

    void BindCommands( TSharedPtr<FUICommandList> CommandList );

private:
    //bool OnSequencerReceiveDragOver( const FGeometry& iGeometry, const FDragDropEvent& iEvent, FReply& oReply );
    //bool OnSequencerReceiveDrop( const FGeometry& iGeometry, const FDragDropEvent& iEvent, FReply& oReply );

    ESequencerDropResult OnSequencerAssetsDrop( const TArray<UObject*>& iAssets, const FAssetDragDropOp& iDragDropOp );
    ESequencerDropResult OnSequencerClassesDrop( const TArray<TWeakObjectPtr<UClass>>& iClasses, const FClassDragDropOp& iDragDropOp );
    ESequencerDropResult OnSequencerActorsDrop( const TArray<TWeakObjectPtr<AActor>>& iActors, const FActorDragDropGraphEdOp& iDragDropOp );

private:
    ISequencer*     mSequencer;
    UBoardSequence* mBoardSequence;

    FDelegateHandle mArrangeSectionsHandle;
};
