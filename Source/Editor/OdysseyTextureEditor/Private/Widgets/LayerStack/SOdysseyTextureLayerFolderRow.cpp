// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyTextureLayerFolderRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "SEnumCombo.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/OdysseyTextureLayerFolder.h"

#define LOCTEXT_NAMESPACE "SOdysseyTextureLayerFolderRow"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyTextureLayerFolderRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyTextureLayerFolder* iTextureLayerFolder)
{
    ensure(iTextureLayerFolder);
    mTextureLayerFolder = iTextureLayerFolder;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
		iTextureLayerFolder
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyTextureLayerFolderRow::GenerateHeaderWidget()
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
            .Value_Lambda([this]() { return (int)(mTextureLayerFolder->Opacity * 100.f + 0.5f);})
            .AllowSpin(true)
            .ShiftMouseMovePixelPerDelta(10)
            .Delta(1)
            .MinValue(0)
            .MinSliderValue(0)
            .MaxValue(100)
            .MaxSliderValue(100)
            .OnValueChanged(this, &SOdysseyTextureLayerFolderRow::OnOpacityValueChanged)
            .OnValueCommitted(this, &SOdysseyTextureLayerFolderRow::OnOpacityValueCommitted)
            .OnBeginSliderMovement(this, &SOdysseyTextureLayerFolderRow::OnOpacityBeginSliderMovement)
            .OnEndSliderMovement(this, &SOdysseyTextureLayerFolderRow::OnOpacityEndSliderMovement)
            //.MinDesiredValueWidth  
        ];
}

TSharedRef<SWidget>
SOdysseyTextureLayerFolderRow::GenerateOptionsWidget()
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
			.CurrentValue_Lambda([this]() { return (int32)mTextureLayerFolder->BlendMode; })
		    .OnEnumSelectionChanged(this, &SOdysseyTextureLayerFolderRow::OnBlendModeComboBoxChanged)
		];
}

void
SOdysseyTextureLayerFolderRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer BlendMode"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerFolder, "BlendMode", iValue, EPropertyChangeType::ValueSet);
}

void
SOdysseyTextureLayerFolderRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerFolder, "Opacity", iValue / 100.f, EPropertyChangeType::ValueSet);
}

void
SOdysseyTextureLayerFolderRow::OnOpacityValueChanged(int iValue)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerFolder, "Opacity", iValue / 100.f, EPropertyChangeType::Interactive);
}

void
SOdysseyTextureLayerFolderRow::OnOpacityBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(LOCTEXT("LayerTransaction", "Change Layer Opacity"));
}

void
SOdysseyTextureLayerFolderRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

#undef LOCTEXT_NAMESPACE
