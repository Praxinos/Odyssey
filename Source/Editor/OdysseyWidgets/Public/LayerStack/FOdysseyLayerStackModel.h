// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h" //For FMenuBuilder
#include "LayerStack/LayersEditor/IOdysseyLayerEditor.h"
#include "IOdysseyLayer.h"

class FOdysseyLayerStackTree;
class SOdysseyLayerStackView;
class FOdysseyLayerStack;
class IOdysseyLayerEditor;

/**
 * Implements the Layer stack model
 */
class FOdysseyLayerStackModel : public TSharedFromThis<FOdysseyLayerStackModel>
{
public:
    //CONSTRUCTOR/DESTRUCTOR
    /** Constructor */
    FOdysseyLayerStackModel( TSharedPtr<SOdysseyLayerStackView> InWidget, TSharedPtr<FOdysseyLayerStack> InLayerStackData );

    /** Virtual destructor */
    virtual ~FOdysseyLayerStackModel();

public:
    //PUBLIC API

    /**
     * Builds up the LayerStack "Add Layer" menu.
     *
     * @param MenuBuilder The menu builder to add things to.
     */
    void BuildAddLayerMenu(FMenuBuilder& MenuBuilder);

    /** Gets the tree of nodes which is used to populate the GUI of the LayerStack */
    TSharedRef<FOdysseyLayerStackTree> GetNodeTree();

    /** Gets the LayerStack data */
    TSharedPtr<FOdysseyLayerStack> GetLayerStackData();

    /**
     * @return Layer tools used by the widget
     */
    const TArray<TSharedPtr<IOdysseyLayerEditor>>& GetLayerEditors() const
    {
        return LayerEditors;
    }

    /** Gets the command bindings for the LayerStack */
    TSharedPtr<FUICommandList> GetCommandBindings() const
    {
        return LayerStackCommandBindings;
    }

    TSharedRef<SOdysseyLayerStackView> GetLayerStackWidget() const;

    /** Handles adding a new folder to the outliner tree. */
    void OnAddFolder();

    /** Handles adding a newly created layer to the outliner tree */
    void OnAddLayer(const IOdysseyLayer& InLayer);

    void OnDeleteLayer( IOdysseyLayer* InLayerToDelete );

    void OnMergeLayerDown( IOdysseyLayer* InLayerToMergeDown );
    
    void OnDuplicateLayer( IOdysseyLayer* InLayerToDuplicate );

    void TestOption();

    /** Generates command bindings for UI commands */
    void BindCommands();

    /** Handles adding a newly created layer to the outliner tree by assigning it into a folder and selecting it. */
    //void OnAddLayer(const TWeakObjectPtr<UOdysseyBaseLayer>& InLayer);

private:
    /** Command list for LayerStack commands and Only (Right-click commands) */
    TSharedRef<FUICommandList> LayerStackCommandBindings;

    /** Command list for LayerStack commands and shared by others (Right-click commands) */
    TSharedRef<FUICommandList> LayerStackSharedBindings;

    /** Represents the tree of nodes to display*/
    TSharedRef<FOdysseyLayerStackTree> NodeTree;

    /** Main LayerStack widget */
    TSharedPtr<SOdysseyLayerStackView> LayerStackWidget;

    /** The true layers data of the layer Stack */
    TSharedPtr<FOdysseyLayerStack> LayerStackData;

    /** List of tools we own */
    TArray<TSharedPtr<IOdysseyLayerEditor>> LayerEditors;

public:
    //Temporary Annecy
    bool bIsOnlyCurrentVisibleLayer;


};


/** A delegate that is executed when adding menu content. */
DECLARE_DELEGATE_TwoParams(FOnGetAddMenuContent, FMenuBuilder& /*MenuBuilder*/, TSharedRef<FOdysseyLayerStackModel>);
