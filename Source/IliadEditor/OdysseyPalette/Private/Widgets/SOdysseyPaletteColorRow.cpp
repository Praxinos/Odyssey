// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaletteColorRow.h"

#include "Framework/Application/SlateApplication.h"
#include "Layout/WidgetPath.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "SOdysseyPaletteEntryRow.h"
#include "Widgets/Colors/SColorPicker.h"
#include "OdysseyPaletteEntryColor.h"
#include "Widgets/Colors/SColorBlock.h"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPaletteColorRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyPaletteTreeView>& iTreeView, UOdysseyPaletteEntryColor* iColorEntry)
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

    mColorEntry->SetColor( iNewColor.ToFColorSRGB(), mSet.Get() );
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
