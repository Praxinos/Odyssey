// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyAnimationLightTable.h"

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableMutator.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SSlider.h"


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
SOdysseyAnimationLightTable::Construct(const FArguments& InArgs, FOdysseyAnimationEditorExtension* iExtension)
{
	UOdysseyLayerStack::OnCurrentLayerChanged().AddSP(this, &SOdysseyAnimationLightTable::OnCurrentLayerChanged);

    mExtension = iExtension;
    mLayerStack.Assign(*this, InArgs._LayerStack);
    
	ChildSlot
	[
		SAssignNew(mSlidersBox, SHorizontalBox)
	];

	RequestRebuild();
}

TSharedPtr<FOdysseyAnimationLightTable>
SOdysseyAnimationLightTable::GetLightTable() const
{
	UOdysseyAnimationLayerStack* layerStack = mExtension->LayerStack();
	if (!layerStack)
		return nullptr;

	UOdysseyAnimationLayerImageRaster* currentLayer = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->CurrentLayer.Get());
	if (!currentLayer)
		return nullptr;

	return currentLayer->GetLightTable();
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

	TSharedPtr<FOdysseyAnimationLightTable> lightTable = GetLightTable();
	if (!lightTable)
		return;

	const TArray<FOdysseyAnimationLightTable::FKeyData>& keysData = lightTable->GetKeysData();

	for (int i = 0; i < keysData.Num(); i++)
    {
		FText offsetText = FText::AsNumber(keysData[i].mOffset);

		mSlidersBox->AddSlot()
		[
			SNew(SVerticalBox)
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
				.OnCheckStateChanged(this, &SOdysseyAnimationLightTable::OnKeyIsActivatedCheckStateChanged, i)
				.IsChecked(this, &SOdysseyAnimationLightTable::GetKeyIsActivated, i )
			]
			+SVerticalBox::Slot()
			[
				SNew(SSlider)
				.Orientation(Orient_Vertical)
				.MinValue(0.f)
				.MaxValue(1.f)
				.OnValueChanged(this, &SOdysseyAnimationLightTable::OnKeyOpacitySliderValueChanged, i)
				.Value(this, &SOdysseyAnimationLightTable::GetKeyOpacity, i)
			]
		];
	}
}

void
SOdysseyAnimationLightTable::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	if (mExtension->LayerStack() != iLayerStack)
		return;

	RequestRebuild();
}

void
SOdysseyAnimationLightTable::OnKeyIsActivatedCheckStateChanged( ECheckBoxState iState, int iKeyIndex )
{
	TSharedPtr<FOdysseyAnimationLightTable> lightTable = GetLightTable();
	if (!lightTable)
		return;

	FOdysseyAnimationLightTableMutator mutator(lightTable);
	mutator.SetKeyIsActivated(iKeyIndex, iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyAnimationLightTable::GetKeyIsActivated( int iKeyIndex ) const
{
	TSharedPtr<FOdysseyAnimationLightTable> lightTable = GetLightTable();
	if (!lightTable)
		return ECheckBoxState::Unchecked;

	return lightTable->GetKeyIsActivated(iKeyIndex) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyAnimationLightTable::OnKeyOpacitySliderValueChanged( float iValue, int iKeyIndex)
{
	TSharedPtr<FOdysseyAnimationLightTable> lightTable = GetLightTable();
	if (!lightTable)
		return;

	FOdysseyAnimationLightTableMutator mutator(lightTable);
	mutator.SetKeyOpacity(iKeyIndex, iValue);
}

float
SOdysseyAnimationLightTable::GetKeyOpacity( int iKeyIndex ) const
{
	TSharedPtr<FOdysseyAnimationLightTable> lightTable = GetLightTable();
	if (!lightTable)
		return 0.f;

	return lightTable->GetKeyOpacity(iKeyIndex);
}