// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaletteColorRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "SOdysseyPaletteEntryRow.h"
#include "Widgets/SOdysseyPaletteTreeView.h"
#include "Widgets/Colors/SColorPicker.h"

#define LOCTEXT_NAMESPACE "SOdysseyPaletteColorRow"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPaletteColorRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyPaletteTreeView>& iOwnerTableView, UOdysseyPaletteEntryColor* iColorEntry)
{
    ensure(iColorEntry);
    mColorEntry = iColorEntry;

    SOdysseyPaletteEntryRow::Construct(
        SOdysseyPaletteEntryRow::FArguments(),
        iOwnerTableView,
		iColorEntry
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyPaletteColorRow::GenerateHeaderWidget()
{
	TSharedRef<SWidget> defaultWidget = SOdysseyPaletteEntryRow::GenerateHeaderWidget();
    return SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        [
            //LayerName
            SOdysseyPaletteEntryRow::GenerateHeaderWidget()
        ]
        + SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        [
             SAssignNew(mColorWidget, SColorBlock )
            .Color(this, &SOdysseyPaletteColorRow::GetEntryColor)
            .OnMouseButtonDown(this, &SOdysseyPaletteColorRow::HandleEntryColorMouseButtonDown)
        ];

}

FReply SOdysseyPaletteColorRow::HandleEntryColorMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    FColorPickerArgs PickerArgs;
    {
        PickerArgs.bUseAlpha = true;
        PickerArgs.bOnlyRefreshOnMouseUp = false;
        PickerArgs.bOnlyRefreshOnOk = false;
        PickerArgs.sRGBOverride = false;
        PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SOdysseyPaletteColorRow::OnSetColorFromColorPicker);
        PickerArgs.InitialColorOverride = mColorEntry->EntryColor;
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
    mColorEntry->EntryColor = iNewColor;
}

FLinearColor SOdysseyPaletteColorRow::GetEntryColor() const
{
    return mColorEntry->EntryColor;
}

#undef LOCTEXT_NAMESPACE
