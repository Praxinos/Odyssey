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
    .HAlign(HAlign_Left)
    .VAlign(VAlign_Top)
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
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
                    .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EAlignment::TopLeft )
                    .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EAlignment::TopLeft )
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
                    .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EAlignment::Top )
                    .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EAlignment::Top )
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
                    .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EAlignment::TopRight )
                    .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EAlignment::TopRight )
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
                    .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EAlignment::Left )
                    .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EAlignment::Left )
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
                    .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EAlignment::Center )
                    .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EAlignment::Center )
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
                    .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EAlignment::Right )
                    .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EAlignment::Right )
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
                    .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EAlignment::BottomLeft )
                    .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EAlignment::BottomLeft )
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
                    .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EAlignment::Bottom )
                    .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EAlignment::Bottom )
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
                    .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged, EAlignment::BottomRight )
                    .IsChecked( this, &SOdysseyImportTexturePositioning::IsAlignmentChecked, EAlignment::BottomRight )
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
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("import-texture-dialog.scaling.name", "Scaling"))
                ]
                + SHorizontalBox::Slot()
                [
                    SNew( SEnumComboBox, StaticEnum<EOdysseyImportTextureScaling>() )
                    .CurrentValue(this, &SOdysseyImportTexturePositioning::GetScaling)
                    .OnEnumSelectionChanged(this, &SOdysseyImportTexturePositioning::OnScalingEnumSelectionChanged)
                ]
            ]
            + SVerticalBox::Slot()
            .Padding(FMargin(0.f, 0.f, 4.f, 4.f))
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("import-texture-dialog.resampling-method.name", "Resampling Method"))
                ]
                + SHorizontalBox::Slot()
                [
                    SNew( SEnumComboBox, StaticEnum<EOdysseyAntiAliasing>() )
                    .CurrentValue(this, &SOdysseyImportTexturePositioning::GetResamplingMethod)
                    .OnEnumSelectionChanged(this, &SOdysseyImportTexturePositioning::OnResamplingMethodEnumSelectionChanged)
                ]
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SBox)
            .WidthOverride(500)
            .HeightOverride(500)
        ]
    ];
}

void
SOdysseyImportTexturePositioning::OnAlignmentCheckBoxStateChanged(ECheckBoxState InCheckState, EAlignment iAlignment)
{
    if (InCheckState != ECheckBoxState::Checked)
        return;

    FData positioning = mData.Get();
    positioning.mAlignment = iAlignment;
    mOnChanged.ExecuteIfBound(positioning);
}

ECheckBoxState
SOdysseyImportTexturePositioning::IsAlignmentChecked(EAlignment iAlignment) const
{
    return mData.Get().mAlignment == iAlignment ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

int32
SOdysseyImportTexturePositioning::GetScaling() const
{
    return (int32)mData.Get().mScaling;
}

void
SOdysseyImportTexturePositioning::OnScalingEnumSelectionChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    FData positioning = mData.Get();
    positioning.mScaling = (EOdysseyImportTextureScaling)iValue;
    mOnChanged.ExecuteIfBound(positioning);
}

int32
SOdysseyImportTexturePositioning::GetResamplingMethod() const
{
    return (int32)mData.Get().mResamplingMethod;
}

void
SOdysseyImportTexturePositioning::OnResamplingMethodEnumSelectionChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    FData positioning = mData.Get();
    positioning.mResamplingMethod = (EOdysseyAntiAliasing)iValue;
    mOnChanged.ExecuteIfBound(positioning);
}

#undef LOCTEXT_NAMESPACE
