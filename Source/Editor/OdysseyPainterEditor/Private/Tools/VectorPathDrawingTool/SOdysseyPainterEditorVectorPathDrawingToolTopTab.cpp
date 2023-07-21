// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/SOdysseyPainterEditorVectorPathDrawingToolTopTab.h"
#include "ISinglePropertyView.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorPathDrawingToolTopTab"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorVectorPathDrawingToolTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction


TSharedPtr<SWidget>
SOdysseyPainterEditorVectorPathDrawingToolTopTab::CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle, const TSharedPtr<ISinglePropertyView> iView)
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
SOdysseyPainterEditorVectorPathDrawingToolTopTab::Construct( const FArguments& InArgs, UOdysseyPainterEditorVectorPathDrawingTool* iTool )
{
    mTool = iTool;

    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;
    const TSharedPtr<ISinglePropertyView> radiusPropertyView = propertyEditorModule.CreateSingleProperty(mTool, "Radius", defaultPropertyParams);
    const TSharedPtr<ISinglePropertyView> opacityPropertyView = propertyEditorModule.CreateSingleProperty(mTool, "Opacity", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> radiusHandle = radiusPropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> opacityHandle = opacityPropertyView->GetPropertyHandle();

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
            CreatePropertyWidget(radiusHandle, radiusPropertyView).ToSharedRef()
        ]
        + SWrapBox::Slot()
        .HAlign(HAlign_Fill)
        [
            CreatePropertyWidget(opacityHandle, opacityPropertyView).ToSharedRef()
        ]
    ];
}

#undef LOCTEXT_NAMESPACE

