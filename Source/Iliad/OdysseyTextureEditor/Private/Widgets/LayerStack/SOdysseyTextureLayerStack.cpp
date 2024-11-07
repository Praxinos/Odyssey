// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyTextureLayerStack.h"
#include "Widgets/SOdysseyLayerStackAddLayerButton.h"
#include "LayerStack/OdysseyTextureLayerFolder.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "Widgets/LayerStack/SOdysseyTextureLayerFolderRow.h"
#include "Widgets/LayerStack/SOdysseyTextureLayerImageRasterRow.h"
#include "Widgets/LayerStack/SOdysseyTextureLayerImageVectorRow.h"
#include "Widgets/LayerStack/SOdysseyTextureLayerStackTreeView.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

SLATE_IMPLEMENT_WIDGET(SOdysseyTextureLayerStack)
void
SOdysseyTextureLayerStack::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mLayerStack, EInvalidateWidgetReason::Layout)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyTextureLayerStack&>(Widget).RebuildWidgets();
        }
    ));
}

SOdysseyTextureLayerStack::~SOdysseyTextureLayerStack()
{
}

SOdysseyTextureLayerStack::SOdysseyTextureLayerStack()
    : mLayerStack(*this, nullptr)
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyTextureLayerStack::Construct(const FArguments& InArgs, FOdysseyTextureEditorExtension* iTextureExtension)
{
    mExtension = iTextureExtension;
    mLayerStack.Assign(*this, InArgs._LayerStack);

    this->RebuildWidgets();
}

void
SOdysseyTextureLayerStack::RebuildWidgets()
{
    this->ChildSlot.DetachWidget();

    TSharedPtr<SWidget> widget = SNullWidget::NullWidget;

    UOdysseyLayerStack* layerstack = mLayerStack.Get();
    if (layerstack)
    {
        widget = SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SOdysseyLayerStackAddLayerButton)
                .LayerStack(layerstack)
            ]
            + SVerticalBox::Slot()
            .FillHeight(1.0)
            [
                SAssignNew(mTreeView, SOdysseyTextureLayerStackTreeView, mExtension)
                .LayerStack(layerstack)
                .OnGenerateRow(this, &SOdysseyTextureLayerStack::OnGenerateRow)
            ];
    }
    else
    {
        /**
         * Display a PlaceHolder when no layerstack can be displayed
         *
         */
        widget = SNew(STextBlock)
        .Text(LOCTEXT("texture-layerstack.no-layerstack", "No Layer Stack can be displayed"));
    }

    this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

TSharedRef<ITableRow>
SOdysseyTextureLayerStack::OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iLayer);

    UClass* layerClass = iLayer->GetClass();
    if (layerClass == UOdysseyTextureLayerFolder::StaticClass())
    {
        return SNew(SOdysseyTextureLayerFolderRow, mTreeView.ToSharedRef(), Cast<UOdysseyTextureLayerFolder>(iLayer));
    }
    else if (layerClass == UOdysseyTextureLayerImageRaster::StaticClass())
    {
        return SNew(SOdysseyTextureLayerImageRasterRow, mTreeView.ToSharedRef(), Cast<UOdysseyTextureLayerImageRaster>(iLayer));
    }
    else if (layerClass == UOdysseyTextureLayerImageVector::StaticClass())
    {
        return SNew(SOdysseyTextureLayerImageVectorRow, mTreeView.ToSharedRef(), Cast<UOdysseyTextureLayerImageVector>(iLayer));
    }

    return SNew(SOdysseyLayerRow, mTreeView.ToSharedRef(), Cast<UOdysseyLayer>(iLayer)); //Default widget
}

#undef LOCTEXT_NAMESPACE
