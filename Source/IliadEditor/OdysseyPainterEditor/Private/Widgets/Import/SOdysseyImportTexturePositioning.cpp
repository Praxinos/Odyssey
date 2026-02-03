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
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, FOdysseyImportTexturesData::EAlignment::TopLeft )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, FOdysseyImportTexturesData::EAlignment::TopLeft )
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
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, FOdysseyImportTexturesData::EAlignment::Top )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, FOdysseyImportTexturesData::EAlignment::Top )
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
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, FOdysseyImportTexturesData::EAlignment::TopRight )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, FOdysseyImportTexturesData::EAlignment::TopRight )
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
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, FOdysseyImportTexturesData::EAlignment::Left )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, FOdysseyImportTexturesData::EAlignment::Left )
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
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, FOdysseyImportTexturesData::EAlignment::Center )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, FOdysseyImportTexturesData::EAlignment::Center )
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
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, FOdysseyImportTexturesData::EAlignment::Right )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, FOdysseyImportTexturesData::EAlignment::Right )
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
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, FOdysseyImportTexturesData::EAlignment::BottomLeft )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, FOdysseyImportTexturesData::EAlignment::BottomLeft )
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
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, FOdysseyImportTexturesData::EAlignment::Bottom )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, FOdysseyImportTexturesData::EAlignment::Bottom )
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
                .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, FOdysseyImportTexturesData::EAlignment::BottomRight )
                .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, FOdysseyImportTexturesData::EAlignment::BottomRight )
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
SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged(ECheckBoxState InCheckState, FOdysseyImportTexturesData::EAlignment iAlignment)
{
    if (InCheckState != ECheckBoxState::Checked)
        return;

    FOdysseyImportTexturesData importData = mData.Get();
    importData.SetAlignment(iAlignment);
    mOnChanged.ExecuteIfBound(importData);
}

ECheckBoxState
SOdysseyImportTexturePositioning::IsAlignmentChecked(FOdysseyImportTexturesData::EAlignment iAlignment) const
{
    return mData.Get().GetAlignment() == iAlignment ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

int32
SOdysseyImportTexturePositioning::GetScaling() const
{
    return (int32)mData.Get().GetScaling();
}

void
SOdysseyImportTexturePositioning::OnScalingEnumSelectionChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    FOdysseyImportTexturesData importData = mData.Get();
    importData.SetScaling((EOdysseyImportTextureScaling)iValue);
    mOnChanged.ExecuteIfBound(importData);
}

int32
SOdysseyImportTexturePositioning::GetResamplingMethod() const
{
    return (int32)mData.Get().GetResamplingMethod();
}

void
SOdysseyImportTexturePositioning::OnResamplingMethodEnumSelectionChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    FOdysseyImportTexturesData importData = mData.Get();
    importData.SetResamplingMethod((EOdysseyAntiAliasing)iValue);
    mOnChanged.ExecuteIfBound(importData);
}

#undef LOCTEXT_NAMESPACE
