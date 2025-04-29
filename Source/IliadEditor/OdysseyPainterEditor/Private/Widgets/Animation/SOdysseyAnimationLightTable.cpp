// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/Animation/SOdysseyAnimationLightTable.h"

#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationLightTable.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SSlider.h"
#include "OdysseyLayerStack.h"
#include "OdysseyAnimationLayerStack.h"

#include "UObject/OdysseyObjectEditorUtils.h"

SLATE_IMPLEMENT_WIDGET(SOdysseyAnimationLightTable)
void
SOdysseyAnimationLightTable::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mLayerStack, EInvalidateWidgetReason::Layout)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyAnimationLightTable&>(Widget).RequestRebuild();
        }
    ));
}

SOdysseyAnimationLightTable::~SOdysseyAnimationLightTable()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
}

SOdysseyAnimationLightTable::SOdysseyAnimationLightTable()
    : mRebuildRequested(false)
    , mLayerStack(*this, nullptr)
{
}

void
SOdysseyAnimationLightTable::Construct(const FArguments& InArgs)
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddSP(this, &SOdysseyAnimationLightTable::OnCurrentLayerChanged);

    mLayerStack.Assign(*this, InArgs._LayerStack);

    ChildSlot
    [
        SAssignNew(mSlidersBox, SHorizontalBox)
    ];

    RequestRebuild();
}

void
SOdysseyAnimationLightTable::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    if (mRebuildRequested)
    {
        Rebuild();
        mRebuildRequested = false;
    }
}

void
SOdysseyAnimationLightTable::RequestRebuild()
{
    mRebuildRequested = true;
}

void
SOdysseyAnimationLightTable::Rebuild()
{
    mSlidersBox->ClearChildren();
    for (int i = 9; i >= 0 ; i--)
    {
        mSlidersBox->AddSlot()
        [
            GeneratePreviousKeyWidget(i)
        ];
    }

    for (int i = 0; i <= 9; i++)
    {
        mSlidersBox->AddSlot()
        [
            GenerateNextKeyWidget(i)
        ];
    }
}

TSharedRef<SWidget>
SOdysseyAnimationLightTable::GeneratePreviousKeyWidget(int iKeyIndex)
{
    FText offsetText = FText::AsNumber(iKeyIndex + 1);
    return SNew(SVerticalBox)
    +SVerticalBox::Slot()
    .AutoHeight()
    [
        SNew(STextBlock)
        .Text(offsetText)
        .Justification(ETextJustify::Center)
    ]
    +SVerticalBox::Slot()
    .AutoHeight()
    .HAlign(HAlign_Center)
    [
        SNew(SCheckBox)
        .OnCheckStateChanged(this, &SOdysseyAnimationLightTable::OnPreviousKeyIsActivatedCheckStateChanged, iKeyIndex)
        .IsChecked(this, &SOdysseyAnimationLightTable::GetPreviousKeyIsActivated, iKeyIndex )
    ]
    +SVerticalBox::Slot()
    [
        SNew(SSlider)
        .Orientation(Orient_Vertical)
        .MinValue(0.f)
        .MaxValue(100.f)
        .OnValueChanged(this, &SOdysseyAnimationLightTable::OnPreviousKeyOpacitySliderValueChanged, iKeyIndex)
        .Value(this, &SOdysseyAnimationLightTable::GetPreviousKeyOpacity, iKeyIndex)
    ];
}

TSharedRef<SWidget>
SOdysseyAnimationLightTable::GenerateNextKeyWidget(int iKeyIndex)
{
    FText offsetText = FText::AsNumber(iKeyIndex + 1);
    return SNew(SVerticalBox)
    +SVerticalBox::Slot()
    .AutoHeight()
    [
        SNew(STextBlock)
        .Text(offsetText)
        .Justification(ETextJustify::Center)
    ]
    +SVerticalBox::Slot()
    .AutoHeight()
    .HAlign(HAlign_Center)
    [
        SNew(SCheckBox)
        .OnCheckStateChanged(this, &SOdysseyAnimationLightTable::OnNextKeyIsActivatedCheckStateChanged, iKeyIndex)
        .IsChecked(this, &SOdysseyAnimationLightTable::GetNextKeyIsActivated, iKeyIndex )
    ]
    +SVerticalBox::Slot()
    [
        SNew(SSlider)
        .Orientation(Orient_Vertical)
        .MinValue(0.f)
        .MaxValue(100.f)
        .OnValueChanged(this, &SOdysseyAnimationLightTable::OnNextKeyOpacitySliderValueChanged, iKeyIndex)
        .Value(this, &SOdysseyAnimationLightTable::GetNextKeyOpacity, iKeyIndex)
    ];
}

void
SOdysseyAnimationLightTable::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
    if (mLayerStack.Get() != iLayerStack)
        return;

    RequestRebuild();
}

void
SOdysseyAnimationLightTable::OnPreviousKeyIsActivatedCheckStateChanged( ECheckBoxState iState, int iKeyIndex )
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    FOdysseyAnimationLightTable lighttable = currentLayer->Lighttable;
    lighttable.PreviousKeys[iKeyIndex].bIsActivated = iState == ECheckBoxState::Checked;
    FOdysseyObjectEditorUtils::SetPropertyValue(currentLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable);
}

void
SOdysseyAnimationLightTable::OnNextKeyIsActivatedCheckStateChanged( ECheckBoxState iState, int iKeyIndex )
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    FOdysseyAnimationLightTable lighttable = currentLayer->Lighttable;
    lighttable.NextKeys[iKeyIndex].bIsActivated = iState == ECheckBoxState::Checked;
    FOdysseyObjectEditorUtils::SetPropertyValue(currentLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable);
}

ECheckBoxState
SOdysseyAnimationLightTable::GetPreviousKeyIsActivated( int iKeyIndex ) const
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return ECheckBoxState::Unchecked;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return ECheckBoxState::Unchecked;

    return currentLayer->Lighttable.PreviousKeys[iKeyIndex].bIsActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState
SOdysseyAnimationLightTable::GetNextKeyIsActivated( int iKeyIndex ) const
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return ECheckBoxState::Unchecked;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return ECheckBoxState::Unchecked;

    return currentLayer->Lighttable.NextKeys[iKeyIndex].bIsActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyAnimationLightTable::OnPreviousKeyOpacitySliderValueChanged( float iValue, int iKeyIndex)
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    FOdysseyAnimationLightTable lighttable = currentLayer->Lighttable;
    lighttable.PreviousKeys[iKeyIndex].Opacity = iValue;
    FOdysseyObjectEditorUtils::SetPropertyValue(currentLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable);
}

void
SOdysseyAnimationLightTable::OnNextKeyOpacitySliderValueChanged( float iValue, int iKeyIndex)
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    FOdysseyAnimationLightTable lighttable = currentLayer->Lighttable;
    lighttable.NextKeys[iKeyIndex].Opacity = iValue;
    FOdysseyObjectEditorUtils::SetPropertyValue(currentLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable);
}

float
SOdysseyAnimationLightTable::GetPreviousKeyOpacity( int iKeyIndex ) const
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return 0.f;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return 0.f;

    return currentLayer->Lighttable.PreviousKeys[iKeyIndex].Opacity;
}

float
SOdysseyAnimationLightTable::GetNextKeyOpacity( int iKeyIndex ) const
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return 0.f;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return 0.f;

    return currentLayer->Lighttable.NextKeys[iKeyIndex].Opacity;
}
