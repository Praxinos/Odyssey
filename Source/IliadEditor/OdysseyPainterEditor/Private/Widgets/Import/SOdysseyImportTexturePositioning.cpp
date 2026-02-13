// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyImportTexturePositioning.h"

#include "OdysseyStyle.h"
#include "SEnumCombo.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

// Construction / Destruction
SOdysseyImportTexturePositioning::~SOdysseyImportTexturePositioning()
{

}

void
SOdysseyImportTexturePositioning::Construct(const FArguments& InArgs)
{
    mData = InArgs._Data;
    mOnChanged = InArgs._OnChanged;

    FMargin alignmentButtonPadding(4.0f);
    FMargin alignmentGridPadding(4.0f);

    ChildSlot
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Top)
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 0.f, 4.f))
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("import-texture-dialog.alignment.name", "Alignment"))
        ]

        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 0.f, 4.f))
        .AutoHeight()
        [
            SNew(SGridPanel)
            + SGridPanel::Slot(0, 0)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .Padding(alignmentGridPadding)
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToggleButtonCheckbox" )
                .Padding(alignmentButtonPadding)
                .HAlign( HAlign_Center )
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EOdysseyImportTextureAlignment::TopLeft )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EOdysseyImportTextureAlignment::TopLeft )
                [
                    SNew(SImage)
                    .Image(FOdysseyStyle::GetBrush("OdysseyImportTexturePositioning.Alignment.TopLeft"))
                ]
            ]
            + SGridPanel::Slot(1, 0)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .Padding(alignmentGridPadding)
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToggleButtonCheckbox" )
                .Padding(alignmentButtonPadding)
                .HAlign( HAlign_Center )
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EOdysseyImportTextureAlignment::Top )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EOdysseyImportTextureAlignment::Top )
                [
                    SNew(SImage)
                    .Image(FOdysseyStyle::GetBrush("OdysseyImportTexturePositioning.Alignment.Top"))
                ]
            ]
            + SGridPanel::Slot(2, 0)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .Padding(alignmentGridPadding)
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToggleButtonCheckbox" )
                .Padding(alignmentButtonPadding)
                .HAlign( HAlign_Center )
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EOdysseyImportTextureAlignment::TopRight )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EOdysseyImportTextureAlignment::TopRight )
                [
                    SNew(SImage)
                    .Image(FOdysseyStyle::GetBrush("OdysseyImportTexturePositioning.Alignment.TopRight"))
                ]
            ]
            + SGridPanel::Slot(0, 1)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .Padding(alignmentGridPadding)
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToggleButtonCheckbox" )
                .Padding(alignmentButtonPadding)
                .HAlign( HAlign_Center )
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EOdysseyImportTextureAlignment::Left )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EOdysseyImportTextureAlignment::Left )
                [
                    SNew(SImage)
                    .Image(FOdysseyStyle::GetBrush("OdysseyImportTexturePositioning.Alignment.Left"))
                ]
            ]
            + SGridPanel::Slot(1, 1)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .Padding(alignmentGridPadding)
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToggleButtonCheckbox" )
                .Padding(alignmentButtonPadding)
                .HAlign( HAlign_Center )
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EOdysseyImportTextureAlignment::Center )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EOdysseyImportTextureAlignment::Center )
                [
                    SNew(SImage)
                    .Image(FOdysseyStyle::GetBrush("OdysseyImportTexturePositioning.Alignment.Center"))
                ]
            ]
            + SGridPanel::Slot(2, 1)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .Padding(alignmentGridPadding)
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToggleButtonCheckbox" )
                .Padding(alignmentButtonPadding)
                .HAlign( HAlign_Center )
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EOdysseyImportTextureAlignment::Right )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EOdysseyImportTextureAlignment::Right )
                [
                    SNew(SImage)
                    .Image(FOdysseyStyle::GetBrush("OdysseyImportTexturePositioning.Alignment.Right"))
                ]
            ]
            + SGridPanel::Slot(0, 2)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .Padding(alignmentGridPadding)
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToggleButtonCheckbox" )
                .Padding(alignmentButtonPadding)
                .HAlign( HAlign_Center )
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EOdysseyImportTextureAlignment::BottomLeft )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EOdysseyImportTextureAlignment::BottomLeft )
                [
                    SNew(SImage)
                    .Image(FOdysseyStyle::GetBrush("OdysseyImportTexturePositioning.Alignment.BottomLeft"))
                ]
            ]
            + SGridPanel::Slot(1, 2)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .Padding(alignmentGridPadding)
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToggleButtonCheckbox" )
                .Padding(alignmentButtonPadding)
                .HAlign( HAlign_Center )
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EOdysseyImportTextureAlignment::Bottom )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EOdysseyImportTextureAlignment::Bottom )
                [
                    SNew(SImage)
                    .Image(FOdysseyStyle::GetBrush("OdysseyImportTexturePositioning.Alignment.Bottom"))
                ]
            ]
            + SGridPanel::Slot(2, 2)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .Padding(alignmentGridPadding)
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToggleButtonCheckbox" )
                .Padding(alignmentButtonPadding)
                .HAlign( HAlign_Center )
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EOdysseyImportTextureAlignment::BottomRight )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EOdysseyImportTextureAlignment::BottomRight )
                [
                    SNew(SImage)
                    .Image(FOdysseyStyle::GetBrush("OdysseyImportTexturePositioning.Alignment.BottomRight"))
                ]
            ]
        ]
        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 4.f, 4.f))
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("import-texture-dialog.scaling.name", "Scaling"))
                ]
                + SVerticalBox::Slot()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("import-texture-dialog.resampling-method.name", "Resampling Method"))
                ]
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                [
                    SNew( SEnumComboBox, StaticEnum<EOdysseyImportTextureScaling>() )
                    .CurrentValue(this, &SOdysseyImportTexturePositioning::GetScaling)
                    .OnEnumSelectionChanged(this, &SOdysseyImportTexturePositioning::OnScalingEnumSelectionChanged)
                ]
                + SVerticalBox::Slot()
                [
                    SNew( SEnumComboBox, StaticEnum<EOdysseyAntiAliasing>() )
                    .CurrentValue(this, &SOdysseyImportTexturePositioning::GetResamplingMethod)
                    .OnEnumSelectionChanged(this, &SOdysseyImportTexturePositioning::OnResamplingMethodEnumSelectionChanged)
                ]

            ]
        ]
    ];
}

void
SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged(ECheckBoxState InCheckState, EOdysseyImportTextureAlignment iAlignment)
{
    if (InCheckState != ECheckBoxState::Checked)
        return;

    mData->SetAlignment(iAlignment);
    mOnChanged.ExecuteIfBound();
}

ECheckBoxState
SOdysseyImportTexturePositioning::IsAlignmentChecked(EOdysseyImportTextureAlignment iAlignment) const
{
    return mData->GetAlignment() == iAlignment ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

int32
SOdysseyImportTexturePositioning::GetScaling() const
{
    return (int32)mData->GetScaling();
}

void
SOdysseyImportTexturePositioning::OnScalingEnumSelectionChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    mData->SetScaling((EOdysseyImportTextureScaling)iValue);
    mOnChanged.ExecuteIfBound();
}

int32
SOdysseyImportTexturePositioning::GetResamplingMethod() const
{
    return (int32)mData->GetResamplingMethod();
}

void
SOdysseyImportTexturePositioning::OnResamplingMethodEnumSelectionChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    mData->SetResamplingMethod((EOdysseyAntiAliasing)iValue);
    mOnChanged.ExecuteIfBound();
}

#undef LOCTEXT_NAMESPACE
