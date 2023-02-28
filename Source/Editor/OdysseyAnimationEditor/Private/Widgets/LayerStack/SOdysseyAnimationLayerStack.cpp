// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/SOdysseyLayerStackAddLayerButton.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerStack"

SLATE_IMPLEMENT_WIDGET(SOdysseyAnimationLayerStack)
void
SOdysseyAnimationLayerStack::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mLayerStack, EInvalidateWidgetReason::Layout)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyAnimationLayerStack&>(Widget).RebuildWidgets();
        }
    ));
}

SOdysseyAnimationLayerStack::~SOdysseyAnimationLayerStack()
{
}

SOdysseyAnimationLayerStack::SOdysseyAnimationLayerStack()
    : mLayerStack(*this, nullptr)
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyAnimationLayerStack::Construct(const FArguments& InArgs)
{
    mLayerStack.Assign(*this, InArgs._LayerStack);

    RebuildWidgets();
}

void
SOdysseyAnimationLayerStack::RebuildWidgets()
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
                SAssignNew(mTreeView, SOdysseyLayerStackTreeView)
                .LayerStack(layerstack)
                .OnGenerateRow(this, &SOdysseyAnimationLayerStack::OnGenerateRow)
            ];
    }
    else
    {
        /**
         * Display a PlaceHolder when no layerstack can be displayed
         * 
         */
        widget = SNew(STextBlock)
        .Text(LOCTEXT("EmptyLayerStackInstructions", "No Layer Stack can be displayed"));
    }

    this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

TSharedRef<ITableRow>
SOdysseyAnimationLayerStack::OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iLayer);

    UClass* layerClass = iLayer->GetClass();
    if (layerClass == UOdysseyAnimationLayerFolder::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerFolderRow, mTreeView.ToSharedRef(), Cast<UOdysseyAnimationLayerFolder>(iLayer));
    }
    else if (layerClass == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageRasterRow, mTreeView.ToSharedRef(), Cast<UOdysseyAnimationLayerImageRaster>(iLayer));
    }

    return SNew(SOdysseyLayerRow, mTreeView.ToSharedRef(), Cast<UOdysseyLayer>(iLayer)); //Default widget
}

#undef LOCTEXT_NAMESPACE
