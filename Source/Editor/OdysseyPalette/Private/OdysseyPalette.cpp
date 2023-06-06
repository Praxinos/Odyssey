// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPalette.h"
#include "Widgets/Colors/SColorBlock.h"

TSharedPtr<SWidget> UOdysseyPalette::CreateWidget()
{
    mPaletteMap.Add( TEXT("Word"), FColor(100,100,100));
    mElementsWidget = SNew(SScrollBox);
    for (auto it = mPaletteMap.CreateConstIterator(); it; ++it)
    {
        mElementsWidget->AddSlot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Left)
            .Padding(FMargin(0.f, 0.f, 4.f, 0.f))
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text( FText::FromName( it->Key ))
            ]
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            [
                SNew(SColorBlock)
                .Color(it->Value)
                //.OnMouseButtonDown(this, &SOdysseyColorPalette::HandleMeshColorBlockMouseButtonDown)
            ]
        ];
    }

    return mElementsWidget;
}

