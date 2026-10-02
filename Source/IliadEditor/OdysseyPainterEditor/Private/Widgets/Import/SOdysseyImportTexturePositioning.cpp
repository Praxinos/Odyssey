// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyImportTexturePositioning.h"

#include "Math/UnitConversion.h"
#include "SEnumCombo.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#include "OdysseyStyle.h"

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

    const FMargin alignmentButtonPadding( 4.0f );
    const FMargin alignmentGridPadding( 4.0f );

    TSharedRef<TNumericUnitTypeInterface<float>> TypeInterfacePercent = MakeShareable( new TNumericUnitTypeInterface<float>( EUnit::Percentage ) );

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
        .AutoHeight()
        .Padding( 0.f, 8.f )
        [
            SNew( SSeparator )
            .Thickness( 2.f )
        ]
        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 4.f, 4.f))
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
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
            SNew( SWidgetSwitcher )
            .WidgetIndex( this, &SOdysseyImportTexturePositioning::GetScalingOptionsWidgetIndex )
            .Visibility( this, &SOdysseyImportTexturePositioning::GetScalingOptionsVisibility )
            // 0 == None
            + SWidgetSwitcher::Slot()
            [
                SNullWidget::NullWidget
            ]
            // 1 == Full
            + SWidgetSwitcher::Slot()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("import-texture-dialog.scaling-options.fit.name", "Fit"))
                ]
                + SHorizontalBox::Slot()
                [
                    SNew( SCheckBox )
                    .IsChecked(this, &SOdysseyImportTexturePositioning::IsScalingFullFitChecked)
                    .OnCheckStateChanged(this, &SOdysseyImportTexturePositioning::OnScalingFullFitChanged)
                ]
            ]
            // 2 == Custom
            + SWidgetSwitcher::Slot()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("import-texture-dialog.scaling-options.custom-scale.name", "Scale"))
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    .Padding( 5.f, 0.f )
                    [
                        SNew( SCheckBox )
                        .Style( FAppStyle::Get(), "ToggleButtonCheckbox" )
                        .IsChecked( this, &SOdysseyImportTexturePositioning::IsScalingCustomLockChecked )
                        .OnCheckStateChanged( this, &SOdysseyImportTexturePositioning::OnScalingCustomLockChanged )
                        [
                            SNew( SImage )
                            .Image_Lambda( [this]() -> const FSlateBrush*
                                           {
                                               return ScalingCustomLock
                                                   ? FOdysseyStyle::GetBrush( "OdysseyImportTexturePositioning.Scaling.Lock" )
                                                   : FOdysseyStyle::GetBrush( "OdysseyImportTexturePositioning.Scaling.Unlock" );
                                           } )
                        ]
                    ]
                    + SHorizontalBox::Slot()
                    [
                        SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        [
                            SNew(SHorizontalBox)
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .VAlign( VAlign_Center )
                            .Padding( 5.f, 0.f )
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("import-texture-dialog.scaling-options.custom-scale-x.name", "X"))
                            ]
                            + SHorizontalBox::Slot()
                            [
                                SNew( SSpinBox<float> )
                                .Justification( ETextJustify::Right )
                                //.MinDesiredWidth( 70.f )
                                .MinFractionalDigits( 0 )
                                .MaxFractionalDigits( 2 )
                                .Delta( 1.f )
                                .SliderExponent( 0.8f )
                                .TypeInterface( TypeInterfacePercent )
                                .Value( this, &SOdysseyImportTexturePositioning::GetScalingCustomX )
                                .OnValueChanged( this, &SOdysseyImportTexturePositioning::SetScalingCustomX )
                            ]
                        ]
                        + SVerticalBox::Slot()
                        [
                            SNew( SHorizontalBox )
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .VAlign( VAlign_Center )
                            .Padding( 5.f, 0.f )
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("import-texture-dialog.scaling-options.custom-scale-y.name", "Y"))
                            ]
                            + SHorizontalBox::Slot()
                            [
                                SNew( SSpinBox<float> )
                                .Justification( ETextJustify::Right )
                                //.MinDesiredWidth( 70.f )
                                .MinFractionalDigits( 0 )
                                .MaxFractionalDigits( 2 )
                                .Delta( 1.f )
                                .SliderExponent( 0.8f )
                                .TypeInterface( TypeInterfacePercent )
                                .Value( this, &SOdysseyImportTexturePositioning::GetScalingCustomY )
                                .OnValueChanged( this, &SOdysseyImportTexturePositioning::SetScalingCustomY )
                            ]
                        ]
                    ]
                ]
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 0.f , 8.f )
        [
            SNew( SSeparator )
            .Thickness( 2.f )
        ]
        + SVerticalBox::Slot()
        .Padding( FMargin( 0.f, 0.f, 4.f, 4.f ) )
        .AutoHeight()
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            [
                SNew( STextBlock )
                .Text( LOCTEXT( "import-texture-dialog.resampling-method.name", "Resampling Method" ) )
            ]
            + SHorizontalBox::Slot()
            [
                SNew( SEnumComboBox, StaticEnum<EOdysseyAntiAliasing>() )
                .CurrentValue( this, &SOdysseyImportTexturePositioning::GetResamplingMethod )
                .OnEnumSelectionChanged( this, &SOdysseyImportTexturePositioning::OnResamplingMethodEnumSelectionChanged )
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

//---

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
SOdysseyImportTexturePositioning::GetScalingOptionsWidgetIndex() const
{
    return (int32)mData->GetScaling();
}

EVisibility
SOdysseyImportTexturePositioning::GetScalingOptionsVisibility() const
{
    return mData->GetScaling() != EOdysseyImportTextureScaling::None ? EVisibility::Visible : EVisibility::Collapsed;
}

//-

ECheckBoxState
SOdysseyImportTexturePositioning::IsScalingFullFitChecked() const
{
    return mData->GetScalingFullFit() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}
void
SOdysseyImportTexturePositioning::OnScalingFullFitChanged( ECheckBoxState iNewState )
{
    mData->SetScalingFullFit( iNewState == ECheckBoxState::Checked );
    mOnChanged.ExecuteIfBound();
}

//-

ECheckBoxState
SOdysseyImportTexturePositioning::IsScalingCustomLockChecked() const
{
    return ScalingCustomLock ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}
void
SOdysseyImportTexturePositioning::OnScalingCustomLockChanged( ECheckBoxState iNewState )
{
    ScalingCustomLock = iNewState == ECheckBoxState::Checked;
    mOnChanged.ExecuteIfBound();
}

//-

float
SOdysseyImportTexturePositioning::GetScalingCustomX() const
{
    return mData->GetScalingCustomSize().X * 100.f;
}
void
SOdysseyImportTexturePositioning::SetScalingCustomX( float iNewValue )
{
    float valueXNormalized = FMath::Max( iNewValue / 100.f, 1.f / 100.f );

    if( FMath::IsNearlyEqual( valueXNormalized, mData->GetScalingCustomSize().X ) )
        return;

    float valueYNormalized = mData->GetScalingCustomSize().Y;
    if( ScalingCustomLock )
    {
        float ratio = valueXNormalized / mData->GetScalingCustomSize().X;
        valueYNormalized = FMath::Max( valueYNormalized * ratio, 1.f / 100.f );
    }

    mData->SetScalingCustomSize( FVector2D( valueXNormalized, valueYNormalized ) );
    mOnChanged.ExecuteIfBound();
}

float
SOdysseyImportTexturePositioning::GetScalingCustomY() const
{
    return mData->GetScalingCustomSize().Y * 100.f;
}
void
SOdysseyImportTexturePositioning::SetScalingCustomY( float iNewValue )
{
    float valueYNormalized = FMath::Max( iNewValue / 100.f, 1.f / 100.f );

    if( FMath::IsNearlyEqual( valueYNormalized, mData->GetScalingCustomSize().Y ) )
        return;

    float valueXNormalized = mData->GetScalingCustomSize().X;
    if( ScalingCustomLock )
    {
        float ratio = valueYNormalized / mData->GetScalingCustomSize().Y;
        valueXNormalized = FMath::Max( valueXNormalized * ratio, 1.f / 100.f );
    }

    mData->SetScalingCustomSize( FVector2D( valueXNormalized, valueYNormalized ) );
    mOnChanged.ExecuteIfBound();
}

//---

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
