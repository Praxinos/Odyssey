// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/SOdysseyPainterEditorRasterDrawingToolTopTab.h"
#include "ISinglePropertyView.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorRasterDrawingToolTopTab"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterDrawingToolTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction


TSharedPtr<SWidget>
SOdysseyPainterEditorRasterDrawingToolTopTab::CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle, const TSharedPtr<ISinglePropertyView> iView)
{
    if (!iPropertyHandle)
        return nullptr;

    TSharedRef<SWidget> nameWidget = iPropertyHandle->CreatePropertyNameWidget();
    TSharedRef<SWidget> valueWidget = iPropertyHandle->CreatePropertyValueWidget(false);

    iView->SetVisibility(EVisibility::Collapsed);

    return SNew(SHorizontalBox)
    + SHorizontalBox::Slot()
    .AutoWidth()
    [
        //PATCH:
        iView.ToSharedRef()
    ]
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
    const TSharedPtr<ISinglePropertyView> sizePropertyView = propertyEditorModule.CreateSingleProperty(mTool->GetBrushOptions(), "Size", defaultPropertyParams);
    const TSharedPtr<ISinglePropertyView> flowPropertyView = propertyEditorModule.CreateSingleProperty(mTool->GetBrushOptions(), "Flow", defaultPropertyParams);
    const TSharedPtr<ISinglePropertyView> blendParametersPropertyView = propertyEditorModule.CreateSingleProperty(mTool, "BlendParameters", defaultPropertyParams);

    TSharedPtr<class IPropertyHandle> blendParametersHandle = blendParametersPropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> sizeHandle = sizePropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> flowHandle = flowPropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> opacityHandle = blendParametersHandle->GetChildHandle("Opacity");
    TSharedPtr<class IPropertyHandle> blendModeHandle = blendParametersHandle->GetChildHandle("BlendingMode");
    TSharedPtr<class IPropertyHandle> alphaModeHandle = blendParametersHandle->GetChildHandle("AlphaMode");
    TSharedPtr<class IPropertyHandle> eraserModeHandle = blendParametersHandle->GetChildHandle("bEraserMode");

    ChildSlot
    .VAlign(VAlign_Fill)
    .HAlign(HAlign_Fill)
    [
        SNew(SWrapBox)
        .InnerSlotPadding(FVector2D(10.f, 3.f))
        .UseAllottedSize(true)
        .HAlign(HAlign_Fill)
        + SWrapBox::Slot()
        .HAlign(HAlign_Fill)
        [
            CreatePropertyWidget(sizeHandle, sizePropertyView).ToSharedRef()
        ]
        + SWrapBox::Slot()
        .HAlign(HAlign_Fill)
        [
            CreatePropertyWidget(opacityHandle, blendParametersPropertyView).ToSharedRef()
        ]
        + SWrapBox::Slot()
        .HAlign(HAlign_Fill)
        [
            CreatePropertyWidget(flowHandle, flowPropertyView).ToSharedRef()
        ]
        + SWrapBox::Slot()
        .HAlign(HAlign_Fill)
        [
            CreatePropertyWidget(blendModeHandle, blendParametersPropertyView).ToSharedRef()
        ]
        + SWrapBox::Slot()
        .HAlign(HAlign_Fill)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                CreatePropertyWidget(alphaModeHandle, blendParametersPropertyView).ToSharedRef()
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

