// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/Animation/SOdysseyLighttable.h"

#include "OdysseyAnimationLayer.h"
#include "OdysseyLighttable.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SSlider.h"
#include "OdysseyLayerStack.h"
#include "OdysseyAnimationLayerStack.h"

#include "UObject/OdysseyObjectEditorUtils.h"

SLATE_IMPLEMENT_WIDGET(SOdysseyLighttable)
void
SOdysseyLighttable::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mLayerStack, EInvalidateWidgetReason::Layout)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyLighttable&>(Widget).RequestRebuild();
        }
    ));
}

SOdysseyLighttable::~SOdysseyLighttable()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
}

SOdysseyLighttable::SOdysseyLighttable()
    : mRebuildRequested(false)
    , mLayerStack(*this, nullptr)
{
}

void
SOdysseyLighttable::Construct(const FArguments& InArgs)
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddSP(this, &SOdysseyLighttable::OnCurrentLayerChanged);

    mLayerStack.Assign(*this, InArgs._LayerStack);

    ChildSlot
    [
        SAssignNew(mSlidersBox, SHorizontalBox)
    ];

    RequestRebuild();
}

void
SOdysseyLighttable::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    if (mRebuildRequested)
    {
        Rebuild();
        mRebuildRequested = false;
    }
}

void
SOdysseyLighttable::RequestRebuild()
{
    mRebuildRequested = true;
}

void
SOdysseyLighttable::Rebuild()
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
SOdysseyLighttable::GeneratePreviousKeyWidget(int iKeyIndex)
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
        .OnCheckStateChanged(this, &SOdysseyLighttable::OnPreviousKeyIsActivatedCheckStateChanged, iKeyIndex)
        .IsChecked(this, &SOdysseyLighttable::GetPreviousKeyIsActivated, iKeyIndex )
    ]
    +SVerticalBox::Slot()
    [
        SNew(SSlider)
        .Orientation(Orient_Vertical)
        .MinValue(0.f)
        .MaxValue(100.f)
        .OnValueChanged(this, &SOdysseyLighttable::OnPreviousKeyOpacitySliderValueChanged, iKeyIndex)
        .Value(this, &SOdysseyLighttable::GetPreviousKeyOpacity, iKeyIndex)
    ];
}

TSharedRef<SWidget>
SOdysseyLighttable::GenerateNextKeyWidget(int iKeyIndex)
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
        .OnCheckStateChanged(this, &SOdysseyLighttable::OnNextKeyIsActivatedCheckStateChanged, iKeyIndex)
        .IsChecked(this, &SOdysseyLighttable::GetNextKeyIsActivated, iKeyIndex )
    ]
    +SVerticalBox::Slot()
    [
        SNew(SSlider)
        .Orientation(Orient_Vertical)
        .MinValue(0.f)
        .MaxValue(100.f)
        .OnValueChanged(this, &SOdysseyLighttable::OnNextKeyOpacitySliderValueChanged, iKeyIndex)
        .Value(this, &SOdysseyLighttable::GetNextKeyOpacity, iKeyIndex)
    ];
}

void
SOdysseyLighttable::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
    if (mLayerStack.Get() != iLayerStack)
        return;

    RequestRebuild();
}

void
SOdysseyLighttable::OnPreviousKeyIsActivatedCheckStateChanged( ECheckBoxState iState, int iKeyIndex )
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    FOdysseyLighttable lighttable = currentLayer->GetLighttable();
    lighttable.PreviousKeys[iKeyIndex].bIsActivated = iState == ECheckBoxState::Checked;
    currentLayer->SetLighttable(lighttable);
}

void
SOdysseyLighttable::OnNextKeyIsActivatedCheckStateChanged( ECheckBoxState iState, int iKeyIndex )
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    FOdysseyLighttable lighttable = currentLayer->GetLighttable();
    lighttable.NextKeys[iKeyIndex].bIsActivated = iState == ECheckBoxState::Checked;
    currentLayer->SetLighttable(lighttable);
}

ECheckBoxState
SOdysseyLighttable::GetPreviousKeyIsActivated( int iKeyIndex ) const
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return ECheckBoxState::Unchecked;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return ECheckBoxState::Unchecked;

    return currentLayer->GetLighttable().PreviousKeys[iKeyIndex].bIsActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState
SOdysseyLighttable::GetNextKeyIsActivated( int iKeyIndex ) const
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return ECheckBoxState::Unchecked;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return ECheckBoxState::Unchecked;

    return currentLayer->GetLighttable().NextKeys[iKeyIndex].bIsActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyLighttable::OnPreviousKeyOpacitySliderValueChanged( float iValue, int iKeyIndex)
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    FOdysseyLighttable lighttable = currentLayer->GetLighttable();
    lighttable.PreviousKeys[iKeyIndex].Opacity = iValue;
    currentLayer->SetLighttable(lighttable);
}

void
SOdysseyLighttable::OnNextKeyOpacitySliderValueChanged( float iValue, int iKeyIndex)
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    FOdysseyLighttable lighttable = currentLayer->GetLighttable();
    lighttable.NextKeys[iKeyIndex].Opacity = iValue;
    currentLayer->SetLighttable(lighttable);
}

float
SOdysseyLighttable::GetPreviousKeyOpacity( int iKeyIndex ) const
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return 0.f;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return 0.f;

    return currentLayer->GetLighttable().PreviousKeys[iKeyIndex].Opacity;
}

float
SOdysseyLighttable::GetNextKeyOpacity( int iKeyIndex ) const
{
    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return 0.f;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return 0.f;

    return currentLayer->GetLighttable().NextKeys[iKeyIndex].Opacity;
}
