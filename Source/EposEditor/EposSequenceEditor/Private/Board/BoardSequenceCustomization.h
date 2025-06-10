// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "MVVM/ViewModelPtr.h"
#include "UObject/ObjectMacros.h"
#include "SequencerCustomizationManager.h"
#include "TransformData.h"
#include "Widgets/SWidget.h"

#include "ArrangeSectionsType.h"

namespace UE::Sequencer
{
    class FObjectBindingModel;
    class FSequencerEditorViewModel;
}
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
    TSharedRef<SWidget> MakeCameraMenu();
    TSharedRef<SWidget> MakeAnimationMenu();
    TSharedRef<SWidget> MakeLighttableMenu();
    TSharedRef<SWidget> MakeAnimationCutMenu();
    TSharedRef<SWidget> MakeSettingsMenu();
    TSharedRef<SWidget> MakeAnimationSettingsMenu();

    void BindCommands( TSharedPtr<FUICommandList> CommandList );

    TSharedPtr<FExtender> CreateObjectBindingContextMenuExtender(UE::Sequencer::FViewModelPtr InViewModel);
    void ExtendObjectBindingContextMenu(FMenuBuilder& MenuBuilder, TSharedPtr<UE::Sequencer::FObjectBindingModel> ObjectBindingModel);

    //// Object binding sidebar menu extensions
    //TSharedPtr<FExtender> CreateObjectBindingSidebarMenuExtender( FViewModelPtr InViewModel );
    //void ExtendObjectBindingSidebarMenu( FMenuBuilder& MenuBuilder, TSharedPtr<FObjectBindingModel> ObjectBindingModel );

    FText CreateInfoText() const;

private:
    void OnGlobalTimeChanged();

    void OnBeginScrubbing();
    void OnEndScrubbing();

private:
    /**
     * Called before an actor or component transform changes
     *
     * @param Object The object whose transform is about to change
     */
    void OnPreTransformChanged( UObject& InObject );

    /**
     * Called when an actor or component transform changes
     *
     * @param Object The object whose transform has changed
     */
    void OnTransformChanged( UObject& InObject );

    /**
     * Called before an actor or component property changes.
     * Forward to OnPreTransformChanged if the property is transform related.
     *
     * @param InObject The object whose property is about to change
     * @param InPropertyChain the property that is about to change
     */
    void OnPrePropertyChanged( UObject* InObject, const class FEditPropertyChain& InPropertyChain );

    /**
     * Called before an actor or component property changes.
     * Forward to OnTransformChanged if the property is transform related.
     *
     * @param InObject The object whose property is about to change
     * @param InPropertyChangedEvent the property that changed
     */
    void OnPostPropertyChanged( UObject* InObject, struct FPropertyChangedEvent& InPropertyChangedEvent );

private:
    //bool OnSequencerReceiveDragOver( const FGeometry& iGeometry, const FDragDropEvent& iEvent, FReply& oReply );
    //bool OnSequencerReceiveDrop( const FGeometry& iGeometry, const FDragDropEvent& iEvent, FReply& oReply );

    ESequencerDropResult OnSequencerAssetsDrop( const TArray<UObject*>& iAssets, const FAssetDragDropOp& iDragDropOp );
    ESequencerDropResult OnSequencerClassesDrop( const TArray<TWeakObjectPtr<UClass>>& iClasses, const FClassDragDropOp& iDragDropOp );
    ESequencerDropResult OnSequencerActorsDrop( const TArray<TWeakObjectPtr<AActor>>& iActors, const FActorDragDropOp& iDragDropOp );

private:
    TWeakPtr<ISequencer> mWeakSequencer;
    FDelegateHandle mSequencerActorAddedDelegates;
    FDelegateHandle mSequencerActivatedDelegates;
    FDelegateHandle mSequencerSelectionSectionChangedDelegates;
    FDelegateHandle mSequencerBeginScrubbingDelegates;
    FDelegateHandle mSequencerEndScrubbingDelegates;
    FDelegateHandle mSequencerGlobalTimeChangedDelegates;
    UBoardSequence* mBoardSequence;

    TSharedPtr<FUICommandList> mBoardCommandList;

    /** Mapping of objects to their existing transform data (for comparing against new transform data) */
    TMap< TWeakObjectPtr<UObject>, FTransformData > mObjectToExistingTransform;
};
