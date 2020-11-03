// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Framework/MultiBox/MultiBoxBuilder.h" //For FMenuBuilder
#include "FOdysseyLayerStackModel.h"

// #include "OdysseyLayerStack.h"

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
        SLATE_ATTRIBUTE( FOdysseyLayerStack*, LayerStackData )
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
    void GetContextMenuContent(FMenuBuilder& ioMenuBuilder);

    /** Makes add button. */
    TSharedRef<SWidget> MakeAddButton();

    /** Makes the add menu for the toolbar. */
    TSharedRef<SWidget> MakeAddMenu();

	/** Returns the visibility of the warning message for missing layer stack */
	EVisibility LayerIsMissingWarningVisibility() const;

	/** Returns the visibility of the layerstack */
	EVisibility LayerStackVisibility() const;

private:

    /** The main LayerStack model */
    FOdysseyLayerStackModel* mLayerStackModelPtr;

    /** The tree view responsible for the GUI of the layerStackTree */
    TSharedPtr<SOdysseyLayerStackTreeView> mTreeView;

    /** Extender to use for the 'add' menu */
    TSharedPtr<FExtender> mAddMenuExtender;

    /** CallBack for the creation of the 'add' menu */
    FOnGetAddMenuContent mOnGetAddMenuContent;
};
