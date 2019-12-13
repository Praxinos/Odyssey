// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Framework/MultiBox/MultiBoxBuilder.h" //For FMenuBuilder
#include "FOdysseyLayerStackModel.h"

#include "OdysseyLayerStack.h"

class FExtender;
class SOdysseyLayerStackTreeView;

/**
 * Implements the Layer stack widget
 */
class ODYSSEYWIDGETS_API SOdysseyLayerStackView : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SOdysseyLayerStackView)
        {}
        /** Called to populate the add combo button in the toolbar. */
        //SLATE_EVENT( FOnGetAddMenuContent, OnGetAddMenuContent )
        /** Extender to use for the add menu. */
        SLATE_ARGUMENT( TSharedPtr<FOdysseyLayerStack>, LayerStackData )
        SLATE_ARGUMENT( TSharedPtr<FExtender>, AddMenuExtender )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyLayerStackView();
    void Construct(const FArguments& InArgs);

public:
    //PUBLIC API
    /** Access the tree view */
    TSharedPtr<SOdysseyLayerStackTreeView> GetTreeView() const;

    void RefreshView();

private:
    //PRIVATE API

    /** Get context menu contents. */
    void GetContextMenuContent(FMenuBuilder& MenuBuilder);

    /** Makes add button. */
    TSharedRef<SWidget> MakeAddButton();

    /** Makes the add menu for the toolbar. */
    TSharedRef<SWidget> MakeAddMenu();

private:

    /** The main LayerStack model */
    FOdysseyLayerStackModel* LayerStackModelPtr;

    /** The tree view responsible for the GUI of the layerStackTree */
    TSharedPtr<SOdysseyLayerStackTreeView> TreeView;

    /** Extender to use for the 'add' menu */
    TSharedPtr<FExtender> AddMenuExtender;

    /** CallBack for the creation of the 'add' menu */
    FOnGetAddMenuContent OnGetAddMenuContent;
};
