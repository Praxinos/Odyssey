// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h" //For FMenuBuilder
#include "LayerStack/FOdysseyLayerAddMenu.h"

class FOdysseyLayerStackTree;
class SOdysseyLayerStackView;
class FOdysseyLayerStack;
class FOdysseyLayerAddMenu;
class IOdysseyLayer;

/**
 * Implements the Layer stack model
 */
class FOdysseyLayerStackModel : public TSharedFromThis<FOdysseyLayerStackModel>
{
public:
	DECLARE_MULTICAST_DELEGATE(FOnUpdated);

public:
    //CONSTRUCTOR/DESTRUCTOR
    /** Constructor */
    FOdysseyLayerStackModel( TSharedPtr<SOdysseyLayerStackView> iWidget, const TAttribute< FOdysseyLayerStack* >& iLayerStackData);

    /** Virtual destructor */
    virtual ~FOdysseyLayerStackModel();

public:
    //PUBLIC API

    /**
     * Builds up the LayerStack "Add Layer" menu.
     *
     * @param MenuBuilder The menu builder to add things to.
     */
    void BuildAddLayerMenu(FMenuBuilder& iMenuBuilder);

    /** Gets the tree of nodes which is used to populate the GUI of the LayerStack */
    TSharedPtr<FOdysseyLayerStackTree> GetNodeTree() const;

    /** Gets the LayerStack data */
    FOdysseyLayerStack* GetLayerStackData();

    const TSharedRef< FOdysseyLayerAddMenu > GetLayerAddMenu() const;

    TSharedPtr<FUICommandList> GetCommandBindings() const;

    TSharedRef<SOdysseyLayerStackView> GetLayerStackView() const;

public: //DELEGATES
	/** Gets a multicast delegate which is called whenever the node tree has been updated.*/
	FOnUpdated& OnUpdated() { return mOnUpdatedDelegate; }

public:
    //HANDLES

    void OnDeleteLayer(TSharedPtr<IOdysseyLayer> iLayerToDelete );

    void OnMergeLayerDown(TSharedPtr<IOdysseyLayer> iLayerToMergeDown );
    
    void OnFlattenLayer(TSharedPtr<IOdysseyLayer> iLayerToMergeDown );
    
    void OnDuplicateLayer(TSharedPtr<IOdysseyLayer> iLayerToDuplicate );

private:
    /** Command list for general LayerStack commands (Right-click commands) */
    TSharedRef<FUICommandList> mLayerStackCommandBindings;

    /** Represents the tree of nodes to display*/
    TSharedRef<FOdysseyLayerStackTree> mNodeTree;

    /** Main LayerStack widget */
    TSharedPtr<SOdysseyLayerStackView> mLayerStackView;

    /** The true layers data of the layer Stack */
    const TAttribute< FOdysseyLayerStack* > mLayerStackData;

    /** List of tools we own */
    TSharedRef<FOdysseyLayerAddMenu> mLayerStackAddMenu;

	/** A multicast delegate which is called whenever the node tree has been updated. */
	FOnUpdated mOnUpdatedDelegate;

};


/** A delegate that is executed when adding menu content. */
DECLARE_DELEGATE_TwoParams(FOnGetAddMenuContent, FMenuBuilder&, TSharedRef<FOdysseyLayerStackModel>);
