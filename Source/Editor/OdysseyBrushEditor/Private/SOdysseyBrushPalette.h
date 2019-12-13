// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000


#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "SGraphPalette.h"
#include "EdGraph/EdGraphNodeUtils.h"

class FOdysseyBrushEditor;
class SSplitter;
class SToolTip;
class UBlueprint;

/*******************************************************************************
* SOdysseyBrushPaletteItem
*******************************************************************************/

/** Widget for displaying a single item  */
class SOdysseyBrushPaletteItem : public SGraphPaletteItem
{
public:
    SLATE_BEGIN_ARGS( SOdysseyBrushPaletteItem )
            : _ShowClassInTooltip(false)
        {}

        SLATE_ARGUMENT(bool, ShowClassInTooltip)
    SLATE_END_ARGS()

    /**
     * Creates the slate widget to be place in a palette.
     *
     * @param  InArgs                A set of slate arguments, defined above.
     * @param  InCreateData            A set of data associated with a FEdGraphSchemaAction that this item represents.
     * @param  InOdysseyBrushEditor    A pointer to the blueprint editor that the palette belongs to.
     */
    void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData, TWeakPtr<FOdysseyBrushEditor> InOdysseyBrushEditor);
    void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData, UBlueprint* InOdysseyBrush);

private:
    void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData, UBlueprint* InOdysseyBrush, TWeakPtr<FOdysseyBrushEditor> InOdysseyBrushEditor);

    // SWidget Interface
    virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    // End of SWidget Interface

    // SGraphPaletteItem Interface
    virtual TSharedRef<SWidget> CreateTextSlotWidget( const FSlateFontInfo& NameFont,  FCreateWidgetForActionData* const InCreateData, TAttribute<bool> bIsReadOnly ) override;
    virtual FText GetDisplayText() const override;
    virtual bool OnNameTextVerifyChanged(const FText& InNewText, FText& OutErrorMessage) override;
    virtual void OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit) override;
    // End of SGraphPaletteItem Interface

    /**
     * Creates a tooltip widget based off the specified action (attempts to
     * mirror the tool-tip that would be found on the node once it's placed).
     *
     * @return A new slate widget to be used as the tool tip for this item's text element.
     */
    TSharedPtr<SToolTip> ConstructToolTipWidget() const;

    /** Returns the up-to-date tooltip for the item */
    FText GetToolTipText() const;
private:
    /** True if the class should be displayed in the tooltip */
    bool bShowClassInTooltip;

    UBlueprint* OdysseyBrush;

    /** Pointer back to the blueprint editor that owns this */
    TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr;

    /** Cache for the MenuDescription to be displayed for this item */
    FNodeTextCache MenuDescriptionCache;
};


/*******************************************************************************
* SOdysseyBrushPalette
*******************************************************************************/

class SOdysseyBrushPalette : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SOdysseyBrushPalette ) {};
    SLATE_END_ARGS()

    /**
     * Creates the slate widget that represents a list of available actions for
     * the specified blueprint.
     *
     * @param  InArgs                A set of slate arguments, defined above.
     * @param  InOdysseyBrushEditor    A pointer to the blueprint editor that this palette belongs to.
     */
    void Construct(const FArguments& InArgs, TWeakPtr<FOdysseyBrushEditor> InOdysseyBrushEditor);

private:
    /**
     * Saves off the user's new sub-palette configuration (so as to not annoy
     * them by reseting it every time they open the blueprint editor).
     */
    void OnSplitterResized() const;

    TSharedPtr<SWidget> FavoritesWrapper;
    TSharedPtr<SSplitter> PaletteSplitter;
    TSharedPtr<SWidget> LibraryWrapper;
};

