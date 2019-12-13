// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SOdysseyBrushSubPalette.h"

class FOdysseyBrushEditor;
class FMenuBuilder;
class FUICommandList;

/*******************************************************************************
* SOdysseyBrushFavoritesPalette
*******************************************************************************/

class SOdysseyBrushFavoritesPalette : public SOdysseyBrushSubPalette
{
public:
    SLATE_BEGIN_ARGS(SOdysseyBrushFavoritesPalette) {}
    SLATE_END_ARGS()

    /** Unsubscribes this from events before it is destroyed */
    virtual ~SOdysseyBrushFavoritesPalette();

    /**
     * Creates a sub-palette widget for the blueprint palette UI (this
     * contains a subset of the library palette, specifically the user's
     * favorites and most used nodes)
     *
     * @param  InArgs                A set of slate arguments, defined above.
     * @param  InOdysseyBrushEditor    A pointer to the blueprint editor that this palette belongs to.
     */
    void Construct(const FArguments& InArgs, TWeakPtr<FOdysseyBrushEditor> InOdysseyBrushEditor);

private:
    // SGraphPalette Interface
    virtual void CollectAllActions(FGraphActionListBuilderBase& OutAllActions) override;
    // End SGraphPalette Interface

    // SOdysseyBrushSubPalette Interface
    virtual void BindCommands(TSharedPtr<FUICommandList> CommandListIn) const override;
    virtual void GenerateContextMenuEntries(FMenuBuilder& MenuBuilder) const override;
    // End SOdysseyBrushSubPalette Interface

    /** Flags weather we should add the "frequently used" list to the user's favorites */
    bool bShowFrequentlyUsed;
};
