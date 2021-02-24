// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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

    enum EMapping
    {
        kMap,
        kUnmap,
    };
    void ProcessCommands( TSharedPtr<FUICommandList> CommandList, EMapping iMap );

    void ArrangeSections();
    void SetArrangeSections( EArrangeSections iArrangeSections );
    void NewSectionWithBoardAtCurrentFrame();
    void NewSectionWithShotAtCurrentFrame();
    void CreateCamera();
    void SnapCameraToViewport();
    void CreatePlane();
    bool CheckNoCamera();
    bool CheckCamera();

    struct FInnerSequenceResult
    {
        UMovieSceneSequence* mInnerSequence;
        FMovieSceneSequenceID mInnerSequenceId;
    };
    FInnerSequenceResult GetInnerSequence();

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
