// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableHeader.h"

#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableMutator.h"

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
			[
				SNew(STextBlock)
				.Text(LOCTEXT("lighttable.timeline-header.name", "LightTable"))
			]
			+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SAssignNew(mLightTablePreviousKeysColorBlockWidget, SColorBlock)
					.CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
					.Color(this, &SOdysseyAnimationTimelineLightTableHeader::GetLightTablePreviousKeysColor)
					.UseSRGB(true)
					.OnMouseButtonDown(this, &SOdysseyAnimationTimelineLightTableHeader::OnLightTablePreviousKeysColorMouseButtonDown)
					.ShowBackgroundForAlpha(true)
					//.IgnoreAlpha(true)
				]
				+ SHorizontalBox::Slot()
				[
					SAssignNew(mLightTableNextKeysColorBlockWidget, SColorBlock)
					.CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
					.Color(this, &SOdysseyAnimationTimelineLightTableHeader::GetLightTableNextKeysColor)
					.UseSRGB(true)
					.OnMouseButtonDown(this, &SOdysseyAnimationTimelineLightTableHeader::OnLightTableNextKeysColorMouseButtonDown)
					.ShowBackgroundForAlpha(true)
					//.IgnoreAlpha(true)
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
		PickerArgs.bUseAlpha = true;
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
		PickerArgs.bUseAlpha = true;
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

#undef LOCTEXT_NAMESPACE
