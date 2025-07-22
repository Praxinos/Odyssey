// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLighttableHeader.h"

#include "OdysseyLighttable.h"
#include "OdysseyAnimationLayer.h"
#include "Math/UnitConversion.h"
#include "OdysseyStyle.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLighttableKey.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "SOdysseyAnimationTimelineLighttable.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationTimelineLighttableHeader::SOdysseyAnimationTimelineLighttableHeader()
    : mLighttablePreviousKeysColorBlockWidget(nullptr)
    , mLighttableNextKeysColorBlockWidget(nullptr)
{

}

void
SOdysseyAnimationTimelineLighttableHeader::Construct(const FArguments& iArgs)
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
                .Text(LOCTEXT("lighttable.timeline-header.name", "Lighttable"))
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SComboButton)
                .ComboButtonStyle(&FOdysseyStyle::GetWidgetStyle<FComboButtonStyle>("Animation.Lighttable.Options"))
                .OnGetMenuContent(this, &SOdysseyAnimationTimelineLighttableHeader::OnOptionsGetMenuContent)
                .HasDownArrow(false)
            ]
        ]
        + SVerticalBox::Slot()
        .Padding(0.f, 0.f, 0.f, 2.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .Padding(0.f, 0.f, 1.f, 0.f)
            [
                SAssignNew(mLighttablePreviousKeysColorBlockWidget, SColorBlock)
                .CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
                .Color(this, &SOdysseyAnimationTimelineLighttableHeader::GetLighttablePreviousKeysColor)
                .UseSRGB(true)
                .OnMouseButtonDown(this, &SOdysseyAnimationTimelineLighttableHeader::OnLighttablePreviousKeysColorMouseButtonDown)
                .AlphaDisplayMode(EColorBlockAlphaDisplayMode::Ignore)
            ]
            + SHorizontalBox::Slot()
            .Padding(1.f, 0.f, 0.f, 0.f)
            [
                SAssignNew(mLighttableNextKeysColorBlockWidget, SColorBlock)
                .CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
                .Color(this, &SOdysseyAnimationTimelineLighttableHeader::GetLighttableNextKeysColor)
                .UseSRGB(true)
                .OnMouseButtonDown(this, &SOdysseyAnimationTimelineLighttableHeader::OnLighttableNextKeysColorMouseButtonDown)
                .AlphaDisplayMode(EColorBlockAlphaDisplayMode::Ignore)
            ]
        ]
    ];
}

FLinearColor
SOdysseyAnimationTimelineLighttableHeader::GetLighttablePreviousKeysColor() const
{
    return mLayer->GetLighttable().PreviousKeysColor;
}

FLinearColor
SOdysseyAnimationTimelineLighttableHeader::GetLighttableNextKeysColor() const
{
    return mLayer->GetLighttable().NextKeysColor;
}

FReply
SOdysseyAnimationTimelineLighttableHeader::OnLighttablePreviousKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const
{
    FColorPickerArgs PickerArgs;
    {
        PickerArgs.bUseAlpha = false;
        PickerArgs.bOnlyRefreshOnMouseUp = false;
        PickerArgs.bOnlyRefreshOnOk = false;
        PickerArgs.InitialColor = GetLighttablePreviousKeysColor();
        PickerArgs.ParentWidget = mLighttablePreviousKeysColorBlockWidget;
        PickerArgs.bOpenAsMenu = true;
        PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda(
            [this](FLinearColor iColor)
            {
                FOdysseyLighttable lighttable = mLayer->GetLighttable();
                lighttable.PreviousKeysColor = iColor;
                mLayer->SetLighttableInteractive(lighttable);
            }
        );
        /** A delegate to be called when the color picker window closes. */
        PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateLambda(
            [this](const TSharedRef<SWindow>& iWindow)
            {
                mLayer->SetLighttable(mLayer->GetLighttable());
            }
        );

        PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateLambda(
            [this](FLinearColor iColor)
            {
                FOdysseyLighttable lighttable = mLayer->GetLighttable();
                lighttable.PreviousKeysColor = iColor;
                mLayer->SetLighttable(lighttable);
            }
        );
    }

    OpenColorPicker(PickerArgs);

    return FReply::Handled();
}

FReply
SOdysseyAnimationTimelineLighttableHeader::OnLighttableNextKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const
{
    FColorPickerArgs PickerArgs;
    {
        PickerArgs.bUseAlpha = false;
        PickerArgs.bOnlyRefreshOnMouseUp = false;
        PickerArgs.bOnlyRefreshOnOk = false;
        PickerArgs.InitialColor = GetLighttableNextKeysColor();
        PickerArgs.ParentWidget = mLighttableNextKeysColorBlockWidget;
        PickerArgs.bOpenAsMenu = true;
        PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda(
            [this](FLinearColor iColor)
            {
                FOdysseyLighttable lighttable = mLayer->GetLighttable();
                lighttable.NextKeysColor = iColor;
                mLayer->SetLighttableInteractive(lighttable);
            }
        );
        /** A delegate to be called when the color picker window closes. */
        PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateLambda(
            [this](const TSharedRef<SWindow>& iWindow)
            {
                mLayer->SetLighttable(mLayer->GetLighttable());
            }
        );

        PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateLambda(
            [this](FLinearColor iColor)
            {
                FOdysseyLighttable lighttable = mLayer->GetLighttable();
                lighttable.NextKeysColor = iColor;
                mLayer->SetLighttable(lighttable);
            }
        );
    }

    OpenColorPicker(PickerArgs);

    return FReply::Handled();
}

TSharedRef< SWidget >
SOdysseyAnimationTimelineLighttableHeader::OnOptionsGetMenuContent()
{
    FMenuBuilder menuBuilder(true, nullptr);
    menuBuilder.BeginSection("Display Position", LOCTEXT("lighttable.timeline-header.options-menu.display-position-section.name", "Display Position"));
    {
        menuBuilder.AddMenuEntry(
            LOCTEXT("lighttable.timeline-header.options-menu.display-position-above-layer.name", "Above Layer")
            , LOCTEXT("lighttable.timeline-header.options-menu.display-position-above-layer.tooltip", "Displays the lighttable frames above Layer")
            , FSlateIcon("OdysseyStyle", "Animation.Lighttable.Options.DisplayPosition.AboveLayer")
            , FUIAction(
                FExecuteAction::CreateRaw( this, &SOdysseyAnimationTimelineLighttableHeader::SetDisplayPosition, EOdysseyLighttableDisplayPosition::AboveLayer )
                , FCanExecuteAction::CreateLambda([](){return true;})
                , FIsActionChecked::CreateRaw(this, &SOdysseyAnimationTimelineLighttableHeader::IsDisplayPositionSet, EOdysseyLighttableDisplayPosition::AboveLayer )
            )
            , NAME_None
            , EUserInterfaceActionType::RadioButton
        );

        menuBuilder.AddMenuEntry(
            LOCTEXT("lighttable.timeline-header.options-menu.display-position-under-layer.name", "Under Layer")
            , LOCTEXT("lighttable.timeline-header.options-menu.display-position-under-layer.tooltip", "Displays the lighttable frames under Layer")
            , FSlateIcon("OdysseyStyle", "Animation.Lighttable.Options.DisplayPosition.UnderLayer")
            , FUIAction(
                FExecuteAction::CreateRaw( this, &SOdysseyAnimationTimelineLighttableHeader::SetDisplayPosition, EOdysseyLighttableDisplayPosition::UnderLayer )
                , FCanExecuteAction::CreateLambda([](){return true;})
                , FIsActionChecked::CreateRaw(this, &SOdysseyAnimationTimelineLighttableHeader::IsDisplayPositionSet, EOdysseyLighttableDisplayPosition::UnderLayer )
            )
            , NAME_None
            , EUserInterfaceActionType::RadioButton
        );
    }
    menuBuilder.EndSection();

    menuBuilder.BeginSection("Options", LOCTEXT("lighttable.timeline-header.options-menu.options-section.name", "Options"));
    {
        menuBuilder.AddWidget(
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNullWidget::NullWidget
            ]
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            .AutoWidth()
            [
                SNew(SNumericEntryBox<int>)
                .Value_Lambda(
                    [this]()
                    {
                        return (int)(mLayer->GetLighttable().PreviousKeysContrast + 0.5f);
                    }
                )
                .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
                .AllowSpin(true)
                /*.LinearDeltaSensitivity(10)
                .Delta(1)*/
                .MinDesiredValueWidth(50)
                .Justification(ETextJustify::Type::Right)
                .OnValueChanged(this, &SOdysseyAnimationTimelineLighttableHeader::OnPreviousKeysContrastValueChanged)
                .OnValueCommitted(this, &SOdysseyAnimationTimelineLighttableHeader::OnPreviousKeysContrastValueCommitted)

            ]
            , LOCTEXT("lighttable.timeline-header.options-menu.previous-keys-contrast.name", "Previous Keys Contrast")
            , true
        );

        menuBuilder.AddWidget(
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNullWidget::NullWidget
            ]
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            .AutoWidth()
            [
                SNew(SNumericEntryBox<int>)
                .Value_Lambda(
                    [this]()
                    {
                        return (int)(mLayer->GetLighttable().NextKeysContrast + 0.5f);
                    }
                )
                .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
                .AllowSpin(true)
                .MinDesiredValueWidth(50)
                .Justification(ETextJustify::Type::Right)
                .OnValueChanged(this, &SOdysseyAnimationTimelineLighttableHeader::OnNextKeysContrastValueChanged)
                .OnValueCommitted(this, &SOdysseyAnimationTimelineLighttableHeader::OnNextKeysContrastValueCommitted)
            ]
            , LOCTEXT("lighttable.timeline-header.options-menu.next-keys-contrast.name", "Next Keys Contrast")
            , true
        );
    }
    menuBuilder.EndSection();

    return menuBuilder.MakeWidget();
}

void
SOdysseyAnimationTimelineLighttableHeader::OnPreviousKeysContrastValueCommitted(int iValue, ETextCommit::Type iType)
{
    FOdysseyLighttable lighttable = mLayer->GetLighttable();
    lighttable.PreviousKeysContrast = iValue;
    mLayer->SetLighttable(lighttable);
}

void
SOdysseyAnimationTimelineLighttableHeader::OnPreviousKeysContrastValueChanged(int iValue)
{
    FOdysseyLighttable lighttable = mLayer->GetLighttable();
    lighttable.PreviousKeysContrast = iValue;
    mLayer->SetLighttableInteractive(lighttable);
}

void
SOdysseyAnimationTimelineLighttableHeader::OnNextKeysContrastValueCommitted(int iValue, ETextCommit::Type iType)
{
    FOdysseyLighttable lighttable = mLayer->GetLighttable();
    lighttable.NextKeysContrast = iValue;
    mLayer->SetLighttable(lighttable);
}

void
SOdysseyAnimationTimelineLighttableHeader::OnNextKeysContrastValueChanged(int iValue)
{
    FOdysseyLighttable lighttable = mLayer->GetLighttable();
    lighttable.NextKeysContrast = iValue;
    mLayer->SetLighttableInteractive(lighttable);
}

void
SOdysseyAnimationTimelineLighttableHeader::SetDisplayPosition(EOdysseyLighttableDisplayPosition iPosition)
{
    FOdysseyLighttable lighttable = mLayer->GetLighttable();
    lighttable.DisplayPosition = iPosition;
    mLayer->SetLighttable(lighttable);
}

bool
SOdysseyAnimationTimelineLighttableHeader::IsDisplayPositionSet(EOdysseyLighttableDisplayPosition iPosition) const
{
    return mLayer->GetLighttable().DisplayPosition == iPosition;
}

#undef LOCTEXT_NAMESPACE
