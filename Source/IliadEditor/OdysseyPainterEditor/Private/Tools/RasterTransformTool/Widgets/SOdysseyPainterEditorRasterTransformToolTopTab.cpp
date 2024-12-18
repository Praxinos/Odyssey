// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/RasterTransformTool/Widgets/SOdysseyPainterEditorRasterTransformToolTopTab.h"
#include "ISinglePropertyView.h"
#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/Layout/SUniformWrapPanel.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterTransformToolTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction


TSharedPtr<SWidget>
SOdysseyPainterEditorRasterTransformToolTopTab::CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle)
{
    if (!iPropertyHandle)
        return nullptr;

    TSharedRef<SWidget> nameWidget = iPropertyHandle->CreatePropertyNameWidget();
    TSharedRef<SWidget> valueWidget = iPropertyHandle->CreatePropertyValueWidget(false);

    return SNew(SHorizontalBox)
    + SHorizontalBox::Slot()
    .AutoWidth()
    .Padding(0.f, 0.f, 3.f, 0.f)
    [
        nameWidget
    ]
    + SHorizontalBox::Slot()
    [
        valueWidget
    ];
}

void
SOdysseyPainterEditorRasterTransformToolTopTab::Construct( const FArguments& InArgs, UOdysseyPainterEditorRasterTransformTool* iTool )
{
    mTool = iTool;

    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    ChildSlot
    .HAlign(HAlign_Fill)
    [
        SNew(SUniformWrapPanel)
        .SlotPadding(FVector2D(3.f, 3.f))
        .EvenRowDistribution(true)
        .HAlign(HAlign_Fill)
        + SUniformWrapPanel::Slot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew( SButton )
                .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                .ToolTipText( LOCTEXT("raster-transform-tool.top-tab.flip_horizontal", "Flip the selection horizontally") )
                .VAlign( VAlign_Center )
                .ContentPadding( FMargin( 0.0, 0.0 ) )
                .OnClicked( this, &SOdysseyPainterEditorRasterTransformToolTopTab::OnFlipHorizontalClicked )
                [
                    SNew( SImage )
                    .Image( FOdysseyStyle::GetBrush( "PainterEditor.FlipHorizontal32" ) )
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew( SButton )
                .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                .ToolTipText(LOCTEXT("raster-transform-tool.top-tab.flip_vertical", "Flip the selection vertically"))
                .VAlign(VAlign_Center)
                .ContentPadding( FMargin( 0.0, 0.0 ) )
                .OnClicked( this, &SOdysseyPainterEditorRasterTransformToolTopTab::OnFlipVerticalClicked )
                [
                    SNew( SImage )
                    .Image( FOdysseyStyle::GetBrush( "PainterEditor.FlipVertical32" ) )
                ]
            ]
        ]
    ];
}

FReply
SOdysseyPainterEditorRasterTransformToolTopTab::OnFlipHorizontalClicked()
{
    mTool->FlipHorizontal();
    return FReply::Handled();
}

FReply SOdysseyPainterEditorRasterTransformToolTopTab::OnFlipVerticalClicked()
{
    mTool->FlipVertical();
    return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE
