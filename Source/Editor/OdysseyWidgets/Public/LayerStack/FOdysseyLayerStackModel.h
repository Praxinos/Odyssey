// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
    //CONSTRUCTOR/DESTRUCTOR
    /** Constructor */
    FOdysseyLayerStackModel( TSharedPtr<SOdysseyLayerStackView> iWidget, TSharedPtr<FOdysseyLayerStack> iLayerStackData );

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
    TSharedRef<FOdysseyLayerStackTree> GetNodeTree();

    /** Gets the LayerStack data */
    TSharedPtr<FOdysseyLayerStack> GetLayerStackData();

    const TSharedRef< FOdysseyLayerAddMenu > GetLayerAddMenu() const;

    TSharedPtr<FUICommandList> GetCommandBindings() const;

    TSharedRef<SOdysseyLayerStackView> GetLayerStackView() const;
    
public:
    //HANDLES

    void OnDeleteLayer( IOdysseyLayer* iLayerToDelete );

    void OnMergeLayerDown( IOdysseyLayer* iLayerToMergeDown );
    
    void OnFlattenLayer( IOdysseyLayer* iLayerToMergeDown );
    
    void OnDuplicateLayer( IOdysseyLayer* iLayerToDuplicate );

private:
    /** Command list for general LayerStack commands (Right-click commands) */
    TSharedRef<FUICommandList> mLayerStackCommandBindings;

    /** Represents the tree of nodes to display*/
    TSharedRef<FOdysseyLayerStackTree> mNodeTree;

    /** Main LayerStack widget */
    TSharedPtr<SOdysseyLayerStackView> mLayerStackView;

    /** The true layers data of the layer Stack */
    TSharedPtr<FOdysseyLayerStack> mLayerStackData;

    /** List of tools we own */
    TSharedRef<FOdysseyLayerAddMenu> mLayerStackAddMenu;

};


/** A delegate that is executed when adding menu content. */
DECLARE_DELEGATE_TwoParams(FOnGetAddMenuContent, FMenuBuilder&, TSharedRef<FOdysseyLayerStackModel>);
