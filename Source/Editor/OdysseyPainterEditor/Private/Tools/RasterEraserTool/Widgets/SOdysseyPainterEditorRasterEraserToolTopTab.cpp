// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterEraserTool/Widgets/SOdysseyPainterEditorRasterEraserToolTopTab.h"
#include "ISinglePropertyView.h"
#include "PropertyHandle.h"
#include "Tools/RasterEraserTool/OdysseyPainterEditorRasterEraserTool.h"
#include "Widgets/Layout/SUniformWrapPanel.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterEraserToolTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction


TSharedPtr<SWidget>
SOdysseyPainterEditorRasterEraserToolTopTab::CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle)
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
SOdysseyPainterEditorRasterEraserToolTopTab::Construct( const FArguments& InArgs, UOdysseyPainterEditorRasterEraserTool* iTool )
{
    mTool = iTool;

    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    FSinglePropertyParams defaultPropertyParams;
    mSizePropertyView = propertyEditorModule.CreateSingleProperty(mTool, "Size", defaultPropertyParams);
    mFlowPropertyView = propertyEditorModule.CreateSingleProperty(mTool, "Flow", defaultPropertyParams);
    mOpacityPropertyView = propertyEditorModule.CreateSingleProperty(mTool, "Opacity", defaultPropertyParams);

    TSharedPtr<class IPropertyHandle> sizeHandle = mSizePropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> flowHandle = mFlowPropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> opacityHandle = mOpacityPropertyView->GetPropertyHandle();

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
    ];
}

#undef LOCTEXT_NAMESPACE

