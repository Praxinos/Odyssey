// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SStylusInputDebugWidget.h"

#include "IStylusState.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SCheckBox.h"

#define LOCTEXT_NAMESPACE "StylusInput"

SStylusInputDebugWidget::SStylusInputDebugWidget()
{
}

SStylusInputDebugWidget::~SStylusInputDebugWidget()
{
    InputSubsystem->RemoveMessageHandler(*this);
}

void SStylusInputDebugWidget::Construct(const FArguments& InArgs, UOdysseyStylusInputSubsystem& InSubsystem)
{
    InputSubsystem = &InSubsystem;
    InputSubsystem->AddMessageHandler(*this);

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.most-recent-index", "Most Recent Index"))
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetIndexText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled( this, &SStylusInputDebugWidget::IsPositionAvailable )
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.position", "Position"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetPositionText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled( this, &SStylusInputDebugWidget::IsPressureAvailable )
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.normal-pressure", "Normal Pressure"))
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetPressureText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled( this, &SStylusInputDebugWidget::IsPressureTangentAvailable )
            + SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.tangent-pressure", "Tangent Pressure"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetTangentPressureText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled( this, &SStylusInputDebugWidget::IsZAvailable )
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.z", "Z"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetZText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled( this, &SStylusInputDebugWidget::IsTwistAvailable )
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.twist", "Twist"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetTwistText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled( this, &SStylusInputDebugWidget::IsAzimuthAvailable )
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.azimuth", "Azimuth"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetAzimuthText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled( this, &SStylusInputDebugWidget::IsAltitudeAvailable )
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.altitude", "Altitude"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetAltitudeText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled( this, &SStylusInputDebugWidget::IsTiltAvailable )
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.tilt", "Tilt"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetTiltText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled( this, &SStylusInputDebugWidget::IsTiltXAvailable )
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.tilt-x", "Tilt X"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetTiltXText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled( this, &SStylusInputDebugWidget::IsTiltYAvailable )
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.tilt-y", "Tilt Y"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetTiltYText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled( this, &SStylusInputDebugWidget::IsSizeAvailable )
            + SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.size", "Size"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(this, &SStylusInputDebugWidget::GetSizeText)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.is-touching", "Is Touching?"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(SCheckBox)
                .IsFocusable(false)
                .IsChecked(this, &SStylusInputDebugWidget::IsTouching)
            ]
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("debug-tab.is-inverted", "Is Inverted?"))
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(SCheckBox)
                .IsFocusable(false)
                .IsChecked(this, &SStylusInputDebugWidget::IsInverted)
            ]
        ]
    ];
}

FText SStylusInputDebugWidget::GetVector2Text(FVector2D Value)
{
    return FText::FromString(FString::Format(TEXT("{0}, {1}"), { Value.X, Value.Y }));
}

FText SStylusInputDebugWidget::GetFloatText(float Value)
{
    return FText::FromString(FString::Format(TEXT("{0}"), { Value }));
}

ECheckBoxState SStylusInputDebugWidget::IsTouching() const
{
    return State.IsStylusDown() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState SStylusInputDebugWidget::IsInverted() const
{
    return State.IsStylusInverted() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}


#undef LOCTEXT_NAMESPACE
