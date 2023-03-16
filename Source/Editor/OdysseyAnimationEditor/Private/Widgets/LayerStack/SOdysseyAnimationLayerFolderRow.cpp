// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerFolderRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "SEnumCombo.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/OdysseyAnimationLayerFolder.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerFolderRow"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyAnimationLayerFolderRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyAnimationLayerFolder* iAnimationLayerFolder)
{
    ensure(iAnimationLayerFolder);
    mAnimationLayerFolder = iAnimationLayerFolder;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
		iAnimationLayerFolder
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyAnimationLayerFolderRow::GenerateHeaderWidget()
{
	TSharedRef<SWidget> defaultWidget = SOdysseyLayerRow::GenerateHeaderWidget();
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
		[
			//LayerName
			SOdysseyLayerRow::GenerateHeaderWidget()
		]
        +SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            SNew(SNumericEntryBox<int>)
            .Value_Lambda([this]() { return (int)(mAnimationLayerFolder->Opacity * 100.f + 0.5f);})
            .AllowSpin(true)
            .ShiftMouseMovePixelPerDelta(10)
            .Delta(1)
            .MinValue(0)
            .MinSliderValue(0)
            .MaxValue(100)
            .MaxSliderValue(100)
            .OnValueChanged(this, &SOdysseyAnimationLayerFolderRow::OnOpacityValueChanged)
            .OnValueCommitted(this, &SOdysseyAnimationLayerFolderRow::OnOpacityValueCommitted)
            .OnBeginSliderMovement(this, &SOdysseyAnimationLayerFolderRow::OnOpacityBeginSliderMovement)
            .OnEndSliderMovement(this, &SOdysseyAnimationLayerFolderRow::OnOpacityEndSliderMovement)
            //.MinDesiredValueWidth  
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationLayerFolderRow::GenerateOptionsWidget()
{
	return SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("OdysseyLayerFolderBlendingMode", "Blending Mode"))
        ]
		+ SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
		[
			SNew(SEnumComboBox, StaticEnum<EOdysseyBlendingMode>())
			.CurrentValue_Lambda([this]() { return (int32)mAnimationLayerFolder->BlendMode; })
		    .OnEnumSelectionChanged(this, &SOdysseyAnimationLayerFolderRow::OnBlendModeComboBoxChanged)
		];
}

void
SOdysseyAnimationLayerFolderRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerFolder, "BlendMode", iValue, EPropertyChangeType::ValueSet);
}

void
SOdysseyAnimationLayerFolderRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerFolder, "Opacity", iValue / 100.f, EPropertyChangeType::ValueSet);
}

void
SOdysseyAnimationLayerFolderRow::OnOpacityValueChanged(int iValue)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerFolder, "Opacity", iValue / 100.f, EPropertyChangeType::Interactive);
}

void
SOdysseyAnimationLayerFolderRow::OnOpacityBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
}

void
SOdysseyAnimationLayerFolderRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

#undef LOCTEXT_NAMESPACE
