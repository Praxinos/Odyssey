// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerFolder/SOdysseyAnimationLayerFolderRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "SEnumCombo.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolder.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow

SOdysseyAnimationLayerFolderRow::SOdysseyAnimationLayerFolderRow()
    : mSetOpacityTransactionName(LOCTEXT("layer-folder.transaction.set-opacity", "Change Layer Opacity"))
{
    
}

void SOdysseyAnimationLayerFolderRow::Construct(
    const FArguments& iArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    FOdysseyAnimationEditorExtension* iExtension,
    class UOdysseyAnimationLayerFolder* iAnimationLayerFolder
)
{
    ensure(iAnimationLayerFolder);
    mAnimationLayerFolder = iAnimationLayerFolder;

    SOdysseyAnimationLayerRow::Construct(
        SOdysseyAnimationLayerRow::FArguments(),
        iOwnerTableView,
        iExtension,
		iAnimationLayerFolder
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyAnimationLayerFolderRow::GenerateHeaderWidget()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
		[
			//LayerName
			SOdysseyLayerRow::GenerateHeaderWidget()
		]
        +SHorizontalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 2.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            SNew(SNumericEntryBox<int>)
            .Visibility(this, &SOdysseyAnimationLayerFolderRow::GetCollapsedOpacityVisibility)
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
        .Padding(FMargin(0, 0, 1.f, 0))
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
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(1.f, 0, 0, 0))
        .VAlign(VAlign_Center)
        [
            SNew(SEnumComboBox, StaticEnum<EOdysseyBlendingMode>())
            .CurrentValue_Lambda([this](){ return (int32)mAnimationLayerFolder->BlendMode;})
            .ContentPadding(FMargin(0))
            .OnEnumSelectionChanged(this, &SOdysseyAnimationLayerFolderRow::OnBlendModeComboBoxChanged)
        ];
}

void
SOdysseyAnimationLayerFolderRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-folder.transaction.set-blend-mode", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerFolder, "BlendMode", iValue, EPropertyChangeType::ValueSet);
}

void
SOdysseyAnimationLayerFolderRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(mSetOpacityTransactionName);
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
    GEditor->BeginTransaction(mSetOpacityTransactionName);
}

void
SOdysseyAnimationLayerFolderRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

EVisibility
SOdysseyAnimationLayerFolderRow::GetCollapsedOpacityVisibility() const
{
    return IsCollapsed() ? EVisibility::Visible : EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE
