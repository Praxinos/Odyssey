// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableHeader.h"

#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableMutator.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Math/UnitConversion.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationTimelineLightTableHeader::SOdysseyAnimationTimelineLightTableHeader()
    : mLightTablePreviousKeysColorBlockWidget(nullptr)
    , mLightTableNextKeysColorBlockWidget(nullptr)
{
	
}

void
SOdysseyAnimationTimelineLightTableHeader::Construct(const FArguments& iArgs)
{
	mLightTable = iArgs._LightTable;

    ChildSlot
	[
		SNew(SBox)
		.HeightOverride(FOptionalSize(SOdysseyAnimationTimelineLightTableKey::mDesiredHeight))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 2.f, 0.f, 2.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("lighttable.timeline-header.name", "LightTable"))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SComboButton)
					.ComboButtonStyle(&FOdysseyStyle::GetWidgetStyle<FComboButtonStyle>("Animation.LightTable.Options"))
					.OnGetMenuContent(this, &SOdysseyAnimationTimelineLightTableHeader::OnOptionsGetMenuContent)
					.HasDownArrow(false)
				]
			]
			+ SVerticalBox::Slot()
			.Padding(0.f, 2.f, 0.f, 0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(0.f, 0.f, 2.f, 0.f)
				[
					SAssignNew(mLightTablePreviousKeysColorBlockWidget, SColorBlock)
					.CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
					.Color(this, &SOdysseyAnimationTimelineLightTableHeader::GetLightTablePreviousKeysColor)
					.UseSRGB(true)
					.OnMouseButtonDown(this, &SOdysseyAnimationTimelineLightTableHeader::OnLightTablePreviousKeysColorMouseButtonDown)
					.IgnoreAlpha(true)
				]
				+ SHorizontalBox::Slot()
				.Padding(2.f, 0.f, 0.f, 0.f)
				[
					SAssignNew(mLightTableNextKeysColorBlockWidget, SColorBlock)
					.CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
					.Color(this, &SOdysseyAnimationTimelineLightTableHeader::GetLightTableNextKeysColor)
					.UseSRGB(true)
					.OnMouseButtonDown(this, &SOdysseyAnimationTimelineLightTableHeader::OnLightTableNextKeysColorMouseButtonDown)
					.IgnoreAlpha(true)
				]
			]
		]
	];
}

FLinearColor
SOdysseyAnimationTimelineLightTableHeader::GetLightTablePreviousKeysColor() const
{
    TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
	if (!lightTable)
		return FLinearColor();
    return lightTable->GetPreviousKeysColor();
}

FLinearColor
SOdysseyAnimationTimelineLightTableHeader::GetLightTableNextKeysColor() const
{
    TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
	if (!lightTable)
		return FLinearColor();
    return lightTable->GetNextKeysColor();
}

FReply
SOdysseyAnimationTimelineLightTableHeader::OnLightTablePreviousKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const
{
    FColorPickerArgs PickerArgs;
	{
		PickerArgs.bUseAlpha = false;
		PickerArgs.bOnlyRefreshOnMouseUp = false;
		PickerArgs.bOnlyRefreshOnOk = false;
		PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda(
            [this](FLinearColor iColor)
            {
				TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
				if (!lightTable)
					return;
                FOdysseyAnimationLightTableMutator mutator(lightTable);
                mutator.SetPreviousKeysColor(iColor);
            }
        );
		PickerArgs.InitialColor = GetLightTablePreviousKeysColor();
		PickerArgs.ParentWidget = mLightTablePreviousKeysColorBlockWidget;
		PickerArgs.bOpenAsMenu = true;
	}

	OpenColorPicker(PickerArgs);

    return FReply::Handled();
}

FReply
SOdysseyAnimationTimelineLightTableHeader::OnLightTableNextKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const
{
    FColorPickerArgs PickerArgs;
	{
		PickerArgs.bUseAlpha = false;
		PickerArgs.bOnlyRefreshOnMouseUp = false;
		PickerArgs.bOnlyRefreshOnOk = false;
		PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda(
            [this](FLinearColor iColor)
            {
                TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
				if (!lightTable)
					return;
                FOdysseyAnimationLightTableMutator mutator(lightTable);
                mutator.SetNextKeysColor(iColor);
            }
        );
		PickerArgs.InitialColor = GetLightTableNextKeysColor();
		PickerArgs.ParentWidget = mLightTableNextKeysColorBlockWidget;
		PickerArgs.bOpenAsMenu = true;
	}

	OpenColorPicker(PickerArgs);

    return FReply::Handled();
}

TSharedRef< SWidget >
SOdysseyAnimationTimelineLightTableHeader::OnOptionsGetMenuContent()
{
    FMenuBuilder menuBuilder(true, nullptr);
    menuBuilder.BeginSection("Options", LOCTEXT("lighttable.timeline-header.options-menu.options-section.name", "Options"));
    {
		menuBuilder.AddWidget(
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNullWidget::NullWidget
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SNumericEntryBox<int>)
				.Value_Lambda(
					[this]()
					{
						TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
						if (!lightTable)
							return 0;
						return (int)(lightTable->GetPreviousKeysContrast() * 100.f + 0.5f);
					}
				)
				.TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
				.AllowSpin(true)
				/*.LinearDeltaSensitivity(10)
				.Delta(1)*/
				.MinDesiredValueWidth(50)
				.Justification(ETextJustify::Type::Right)
				.OnValueChanged(this, &SOdysseyAnimationTimelineLightTableHeader::OnPreviousKeysContrastValueChanged)
				.OnValueCommitted(this, &SOdysseyAnimationTimelineLightTableHeader::OnPreviousKeysContrastValueCommitted)
				
			]
			, LOCTEXT("lighttable.timeline-header.options-menu.previous-keys-contrast.name", "Previous Keys Contrast")
			, true
		);

		menuBuilder.AddWidget(
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNullWidget::NullWidget
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SNumericEntryBox<int>)
				.Value_Lambda(
					[this]()
					{
						TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
						if (!lightTable)
							return 0;
						return (int)(lightTable->GetNextKeysContrast() * 100.f + 0.5f);
					}
				)
				.TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
				.AllowSpin(true)
				/*.ShiftMouseMovePixelPerDelta(10)
				.LinearDeltaSensitivity(10)
				.Delta(1)*/
				/* .MinValue(0)
				.MinSliderValue(0)
				.MaxValue(100)
				.MaxSliderValue(100) */
				.MinDesiredValueWidth(50)
				.Justification(ETextJustify::Type::Right)
				.OnValueChanged(this, &SOdysseyAnimationTimelineLightTableHeader::OnNextKeysContrastValueChanged)
				.OnValueCommitted(this, &SOdysseyAnimationTimelineLightTableHeader::OnNextKeysContrastValueCommitted)
			]
			, LOCTEXT("lighttable.timeline-header.options-menu.next-keys-contrast.name", "Next Keys Contrast")
			, true
			//bool bNoIndent = false, bool bSearchable = true, const TAttribute<FText>& InToolTipText = FText());
		);
    }
    menuBuilder.EndSection();

    return menuBuilder.MakeWidget();
}

void
SOdysseyAnimationTimelineLightTableHeader::OnPreviousKeysContrastValueCommitted(int iValue, ETextCommit::Type iType)
{
	TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
	if (!lightTable)
		return;

	FOdysseyAnimationLightTableMutator mutator(lightTable);
	mutator.SetPreviousKeysContrast(iValue / 100.f);
}

void
SOdysseyAnimationTimelineLightTableHeader::OnPreviousKeysContrastValueChanged(int iValue)
{
    TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
	if (!lightTable)
		return;
	 
	FOdysseyAnimationLightTableMutator mutator(lightTable);
	mutator.SetPreviousKeysContrast(iValue / 100.f);
}

void
SOdysseyAnimationTimelineLightTableHeader::OnNextKeysContrastValueCommitted(int iValue, ETextCommit::Type iType)
{
	TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
	if (!lightTable)
		return;

	FOdysseyAnimationLightTableMutator mutator(lightTable);
	mutator.SetNextKeysContrast(iValue / 100.f);
}

void
SOdysseyAnimationTimelineLightTableHeader::OnNextKeysContrastValueChanged(int iValue)
{
    TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
	if (!lightTable)
		return;
	 
	FOdysseyAnimationLightTableMutator mutator(lightTable);
	mutator.SetNextKeysContrast(iValue / 100.f);
}

#undef LOCTEXT_NAMESPACE
