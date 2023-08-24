// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/SOdysseyPainterEditorRasterDrawingToolTopTab.h"
#include "ISinglePropertyView.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Widgets/Layout/SUniformWrapPanel.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorRasterDrawingToolTopTab"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterDrawingToolTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction


TSharedPtr<SWidget>
SOdysseyPainterEditorRasterDrawingToolTopTab::CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle)
{
    if (!iPropertyHandle)
        return nullptr;

    TSharedRef<SWidget> nameWidget = iPropertyHandle->CreatePropertyNameWidget();
    TSharedRef<SWidget> valueWidget = iPropertyHandle->CreatePropertyValueWidget(false);

    /* return SNew(SVerticalBox)
    + SVerticalBox::Slot()
    .AutoHeight()
    //.Padding(0.f, 0.f, 3.f, 0.f)
    [
        nameWidget
    ]
    + SVerticalBox::Slot()
    .AutoHeight()
    [
        valueWidget
    ]; */

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
SOdysseyPainterEditorRasterDrawingToolTopTab::Construct( const FArguments& InArgs, UOdysseyPainterEditorRasterDrawingTool* iTool )
{
    mTool = iTool;

    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    FSinglePropertyParams defaultPropertyParams;
    mSizePropertyView = propertyEditorModule.CreateSingleProperty(mTool->GetBrushOptions(), "Size", defaultPropertyParams);
    mFlowPropertyView = propertyEditorModule.CreateSingleProperty(mTool->GetBrushOptions(), "Flow", defaultPropertyParams);
    mBlendParametersPropertyView = propertyEditorModule.CreateSingleProperty(mTool, "BlendParameters", defaultPropertyParams);

    TSharedPtr<class IPropertyHandle> blendParametersHandle = mBlendParametersPropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> sizeHandle = mSizePropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> flowHandle = mSizePropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> opacityHandle = blendParametersHandle->GetChildHandle("Opacity");
    TSharedPtr<class IPropertyHandle> blendModeHandle = blendParametersHandle->GetChildHandle("BlendingMode");

    ChildSlot
    .HAlign(HAlign_Fill)
    //.VAlign(VAlign_Center)
    [
        SNew(SUniformWrapPanel)
        .SlotPadding(FVector2D(3.f, 3.f))
        .EvenRowDistribution(true)
        .HAlign(HAlign_Fill)
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(sizeHandle).ToSharedRef()
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(opacityHandle).ToSharedRef()
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(flowHandle).ToSharedRef()
        ]
        + SUniformWrapPanel::Slot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                CreatePropertyWidget(blendModeHandle).ToSharedRef()
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew( SButton )
                .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                .ToolTipText( LOCTEXT("EraserButton", "Switch the current tool to Eraser mode.") )
                .VAlign( VAlign_Center )
                .ContentPadding( FMargin( 0.0, 0.0 ) )
                .OnClicked( this, &SOdysseyPainterEditorRasterDrawingToolTopTab::OnEraserButtonClicked )
                [
                    SNew( SImage )
                    .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Eraser32" ) )
                    .ColorAndOpacity( this, &SOdysseyPainterEditorRasterDrawingToolTopTab::GetEraserButtonColorAndOpacity )
                ]
            ]
        ]
    ];
}

FReply
SOdysseyPainterEditorRasterDrawingToolTopTab::OnEraserButtonClicked()
{
    FOdysseyBlendParameters blendParameters = mTool->GetBlendParameters();
    blendParameters.bEraserMode = !blendParameters.bEraserMode;
    FOdysseyObjectEditorUtils::SetPropertyValue(mTool, "BlendParameters", blendParameters);
    return FReply::Handled();
}

FSlateColor
SOdysseyPainterEditorRasterDrawingToolTopTab::GetEraserButtonColorAndOpacity() const
{
    return ( mTool->GetBlendParameters().bEraserMode ? FSlateColor( FLinearColor( 1.0f, 0.5f, 0.0f, 1.0f ) ) : FSlateColor( FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
}

#undef LOCTEXT_NAMESPACE

