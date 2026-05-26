// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationTimelineCellNamesHeader.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SNumericEntryBox.h"
//#include "Widgets/Input/NumericTypeInterface.h"
//#include "Widgets/Input/NumericUnitTypeInterface.inl"

#include "OdysseyAnimationLayer.h"
#include "OdysseyStyle.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationTimelineCellNamesHeader::SOdysseyAnimationTimelineCellNamesHeader()
{
}

void
SOdysseyAnimationTimelineCellNamesHeader::Construct(const FArguments& iArgs)
{
    mLayer = iArgs._Layer;

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(0.f, 0.f, 0.f, 2.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("cell-names.timeline-header.name", "Cell Names"))
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SComboButton)
                .ComboButtonStyle(&FOdysseyStyle::GetWidgetStyle<FComboButtonStyle>("Animation.CellNames.Options"))
                .OnGetMenuContent(this, &SOdysseyAnimationTimelineCellNamesHeader::OnOptionsGetMenuContent)
                .HasDownArrow(false)
                .ToolTipText( LOCTEXT( "cell-names.timeline-header.options.tooltip", "No options at this time" ) )

                .IsEnabled( false ) //DEBUG: juste to display the button but as there is no options, disable it. Once removed, change the tooltip as well !!!
            ]
        ]
    ];
}

TSharedRef< SWidget >
SOdysseyAnimationTimelineCellNamesHeader::OnOptionsGetMenuContent()
{
    FMenuBuilder menuBuilder(true, nullptr);

    //menuBuilder.BeginSection("Options", LOCTEXT("lighttable.timeline-header.options-menu.options-section.name", "Options"));
    //{
    //    menuBuilder.AddWidget(
    //        SNew(SHorizontalBox)
    //        + SHorizontalBox::Slot()
    //        [
    //            SNullWidget::NullWidget
    //        ]
    //        + SHorizontalBox::Slot()
    //        .HAlign(HAlign_Right)
    //        .AutoWidth()
    //        [
    //            SNew(SNumericEntryBox<int>)
    //            .Value_Lambda(
    //                [this]()
    //                {
    //                    return (int)(mLayer->GetLighttable().PreviousKeysContrast + 0.5f);
    //                }
    //            )
    //            .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
    //            .AllowSpin(true)
    //            /*.LinearDeltaSensitivity(10)
    //            .Delta(1)*/
    //            .MinDesiredValueWidth(50)
    //            .Justification(ETextJustify::Type::Right)
    //            .OnValueChanged(this, &SOdysseyAnimationTimelineLighttableHeader::OnPreviousKeysContrastValueChanged)
    //            .OnValueCommitted(this, &SOdysseyAnimationTimelineLighttableHeader::OnPreviousKeysContrastValueCommitted)

    //        ]
    //        , LOCTEXT("lighttable.timeline-header.options-menu.previous-keys-contrast.name", "Previous Keys Contrast")
    //        , true
    //    );

    //    menuBuilder.AddWidget(
    //        SNew(SHorizontalBox)
    //        + SHorizontalBox::Slot()
    //        [
    //            SNullWidget::NullWidget
    //        ]
    //        + SHorizontalBox::Slot()
    //        .HAlign(HAlign_Right)
    //        .AutoWidth()
    //        [
    //            SNew(SNumericEntryBox<int>)
    //            .Value_Lambda(
    //                [this]()
    //                {
    //                    return (int)(mLayer->GetLighttable().NextKeysContrast + 0.5f);
    //                }
    //            )
    //            .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
    //            .AllowSpin(true)
    //            .MinDesiredValueWidth(50)
    //            .Justification(ETextJustify::Type::Right)
    //            .OnValueChanged(this, &SOdysseyAnimationTimelineLighttableHeader::OnNextKeysContrastValueChanged)
    //            .OnValueCommitted(this, &SOdysseyAnimationTimelineLighttableHeader::OnNextKeysContrastValueCommitted)
    //        ]
    //        , LOCTEXT("lighttable.timeline-header.options-menu.next-keys-contrast.name", "Next Keys Contrast")
    //        , true
    //    );
    //}
    //menuBuilder.EndSection();

    return menuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
