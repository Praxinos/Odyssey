// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyImportTextureScanCleaner.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

// Construction / Destruction
SOdysseyImportTextureScanCleaner::~SOdysseyImportTextureScanCleaner()
{

}

void
SOdysseyImportTextureScanCleaner::Construct(const FArguments& InArgs)
{
    mData = InArgs._Data;
    mOnChanged = InArgs._OnChanged;

    FMargin alignmentButtonPadding(4.0f);

    ChildSlot
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Top)
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 0.f, 4.f))
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("import-texture-dialog.scan-cleaner.activate", "Activate Scan Cleaner"))
            ]

            + SHorizontalBox::Slot()
            [
                SNew(SCheckBox)
                //.Padding(alignmentButtonPadding)
                //.HAlign( HAlign_Center )
                .OnCheckStateChanged( this, &SOdysseyImportTextureScanCleaner::OnActivateCheckBoxStateChanged)
                .IsChecked( this, &SOdysseyImportTextureScanCleaner::IsActivateChecked )
            ]
        ]
    ];
}

void
SOdysseyImportTextureScanCleaner::OnActivateCheckBoxStateChanged(ECheckBoxState InCheckState)
{
    FOdysseyImportTexturesParameters importData = mData.Get();
    importData.SetIsScanCleanerActivated(InCheckState == ECheckBoxState::Checked);
    mOnChanged.ExecuteIfBound(importData);
}

ECheckBoxState
SOdysseyImportTextureScanCleaner::IsActivateChecked() const
{
    return mData.Get().GetIsScanCleanerActivated() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE
