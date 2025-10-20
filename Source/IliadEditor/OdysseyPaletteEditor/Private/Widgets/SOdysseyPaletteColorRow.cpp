// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyPaletteColorRow.h"

#include "Framework/Application/SlateApplication.h"
#include "Layout/WidgetPath.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyStyle.h"
#include "SOdysseyPaletteEntryRow.h"
#include "SOdysseyPaletteTreeView.h"
#include "Widgets/Colors/SColorPicker.h"
#include "OdysseyPaletteEntryColor.h"
#include "Widgets/Colors/SColorBlock.h"
#include "OdysseyPaletteUndoAlterPaletteEntry.h"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPaletteColorRow::Construct(const FArguments& InArgs
                                      , const TSharedRef<SOdysseyPaletteTreeView>& iTreeView
                                      , UOdysseyPaletteEntryColor* iColorEntry)
{
    ensure(iColorEntry);
    mColorEntry = iColorEntry;
    mSet = InArgs._Set;

    SOdysseyPaletteEntryRow::Construct(
        SOdysseyPaletteEntryRow::FArguments()
            .IsReadOnly(InArgs._IsReadOnly),
        iTreeView,
        iColorEntry
    );
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyPaletteColorRow::GenerateWidgetForColumn( const FName& InColumnName )
{
    if (InColumnName == "Color")
    {
        return GenerateColorWidget();
    }

    return SOdysseyPaletteEntryRow::GenerateWidgetForColumn(InColumnName);
}

TSharedRef<SWidget>
SOdysseyPaletteColorRow::GenerateColorWidget()
{
    return SAssignNew(mColorWidget, SColorBlock )
        .Color(this, &SOdysseyPaletteColorRow::GetEntryColorAsLinear)
        .OnMouseButtonDown(this, &SOdysseyPaletteColorRow::HandleEntryColorMouseButtonDown);
}

FReply SOdysseyPaletteColorRow::HandleEntryColorMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (mIsReadOnly)
        return FReply::Unhandled();

    FColorPickerArgs PickerArgs;
    {
        PickerArgs.bUseAlpha = true;
        PickerArgs.bOnlyRefreshOnMouseUp = false;
        PickerArgs.bOnlyRefreshOnOk = false;
        PickerArgs.sRGBOverride = false;
        PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SOdysseyPaletteColorRow::OnSetColorFromColorPicker);
        PickerArgs.bOnlyRefreshOnOk = true;
        PickerArgs.InitialColor = mColorEntry->GetColor(mSet.Get());
        PickerArgs.ParentWidget = mColorWidget;
        PickerArgs.OptionalOwningDetailsView = mColorWidget;
        FWidgetPath ParentWidgetPath;
        if (FSlateApplication::Get().FindPathToWidget(mColorWidget.ToSharedRef(), ParentWidgetPath))
        {
            PickerArgs.bOpenAsMenu = FSlateApplication::Get().FindMenuInWidgetPath(ParentWidgetPath).IsValid();
        }
    }

    OpenColorPicker(PickerArgs);

    return FReply::Handled();
}

void SOdysseyPaletteColorRow::OnSetColorFromColorPicker(FLinearColor iNewColor)
{
    if (mIsReadOnly)
        return;

    const FScopedTransaction transaction(NSLOCTEXT("Palette", "ChangeColorEntry_Transaction", "Change color entry"));

    FOdysseyObjectEditorUtils::PreChangePropertyValue(mColorEntry, "EntryColors");

    mColorEntry->SetColor( iNewColor.ToFColorSRGB(), mSet.Get() );

    FOdysseyObjectEditorUtils::PostChangePropertyValue(mColorEntry, "EntryColors", EPropertyChangeType::ValueSet);

    // will refresh viewports on undo.
    TArray<FOdysseyVectorCell*> cells = mTreeView.Pin()->GetVectorCellsUsedByEntries({mColorEntry});

    for (FOdysseyVectorCell* cell : cells)
    {
        cell->GetLayer()->RequestRedraw(cell, 0);
    }

    if (GUndo)
    {
        FOdysseyPaletteUndoAlterPaletteEntry* undo = new FOdysseyPaletteUndoAlterPaletteEntry(cells);

        GUndo->StoreUndo(GEditor, TUniquePtr<FChange>(undo));
    }
}

FLinearColor SOdysseyPaletteColorRow::GetEntryColorAsLinear() const
{
    return FLinearColor( mColorEntry->GetColor(mSet.Get()) );
}

const FSlateBrush*
SOdysseyPaletteColorRow::GetIcon() const
{
    return FOdysseyStyle::Get().GetBrush("OdysseyPalette.EntryColor");
}
