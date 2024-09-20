// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterPaintBucketTool/Widgets/SOdysseyPainterEditorRasterPaintBucketToolTopTab.h"
#include "ISinglePropertyView.h"
#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"
#include "Widgets/Layout/SUniformWrapPanel.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterPaintBucketToolTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction


TSharedPtr<SWidget>
SOdysseyPainterEditorRasterPaintBucketToolTopTab::CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle)
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
SOdysseyPainterEditorRasterPaintBucketToolTopTab::Construct( const FArguments& InArgs, UOdysseyPainterEditorRasterPaintBucketTool* iTool )
{
    mTool = iTool;

    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    FSinglePropertyParams defaultPropertyParams;
    mBlendParametersPropertyView = propertyEditorModule.CreateSingleProperty(mTool, "BlendParameters", defaultPropertyParams);

    TSharedPtr<class IPropertyHandle> blendParametersHandle = mBlendParametersPropertyView->GetPropertyHandle();
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
            CreatePropertyWidget(opacityHandle).ToSharedRef()
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
                .ToolTipText( LOCTEXT("raster-drawing-tool.top-tab.eraser", "Switch the current tool to Eraser mode.") )
                .VAlign( VAlign_Center )
                .ContentPadding( FMargin( 0.0, 0.0 ) )
                .OnClicked( this, &SOdysseyPainterEditorRasterPaintBucketToolTopTab::OnEraserButtonClicked )
                [
                    SNew( SImage )
                    .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Eraser32" ) )
                    .ColorAndOpacity( this, &SOdysseyPainterEditorRasterPaintBucketToolTopTab::GetEraserButtonColorAndOpacity )
                ]
            ]
        ]
    ];
}

FReply
SOdysseyPainterEditorRasterPaintBucketToolTopTab::OnEraserButtonClicked()
{
    FOdysseyBlendParameters blendParameters = mTool->GetBlendParameters();
    blendParameters.bEraserMode = !blendParameters.bEraserMode;
    FOdysseyObjectEditorUtils::SetPropertyValue(mTool, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterPaintBucketTool, BlendParameters), blendParameters);
    return FReply::Handled();
}

FSlateColor
SOdysseyPainterEditorRasterPaintBucketToolTopTab::GetEraserButtonColorAndOpacity() const
{
    return ( mTool->GetBlendParameters().bEraserMode ? FSlateColor( FLinearColor( 1.0f, 0.5f, 0.0f, 1.0f ) ) : FSlateColor( FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
}

#undef LOCTEXT_NAMESPACE

