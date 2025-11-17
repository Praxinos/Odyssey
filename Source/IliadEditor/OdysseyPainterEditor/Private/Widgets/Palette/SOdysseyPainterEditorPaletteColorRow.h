// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorColorType.h"
#include "Widgets/Palette/SOdysseyPainterEditorPaletteEntryRow.h"

class UOdysseyPaletteEntryColor;

/**
 * Implements a layer row widget
 */
class SOdysseyPainterEditorPaletteColorRow
    : public SOdysseyPainterEditorPaletteEntryRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorPaletteColorRow)
        : _IsCurrent( false )
        , _Entry(nullptr)
        , _Set(FGuid())
        , _ColorType(EOdysseyPainterEditorColorType::Raw)
        {}
        SLATE_ATTRIBUTE(bool, IsCurrent)
        SLATE_ATTRIBUTE(UOdysseyPaletteEntryColor*, Entry)
        SLATE_ATTRIBUTE(FGuid, Set)
        SLATE_ATTRIBUTE(EOdysseyPainterEditorColorType, ColorType)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<STableViewBase>& iTreeView);

protected:
    virtual const FSlateBrush* GetIcon() const override;

    virtual TSharedRef<SWidget> GenerateContentWidget() override;

public:
    /** @return True if the corresponding item is selected; false otherwise */
    virtual bool IsItemSelected() const override;

private:
    FLinearColor GetEntryColorAsLinear() const;

private:
    TAttribute<bool> mIsCurrent;
    TAttribute<FGuid> mSet;
    TAttribute<EOdysseyPainterEditorColorType> mColorType;
    TAttribute<UOdysseyPaletteEntryColor*> mEntryColor;
};
