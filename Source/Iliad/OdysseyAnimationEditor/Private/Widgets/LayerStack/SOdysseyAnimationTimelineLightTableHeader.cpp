// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableHeader.h"

#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "Math/UnitConversion.h"
#include "OdysseyStyleSet.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "SOdysseyAnimationTimelineLightTable.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationTimelineLightTableHeader::SOdysseyAnimationTimelineLightTableHeader()
    : mLightTablePreviousKeysColorBlockWidget(nullptr)
    , mLightTableNextKeysColorBlockWidget(nullptr)
{

}

void
SOdysseyAnimationTimelineLightTableHeader::Construct(const FArguments& iArgs)
{
    mLayer = iArgs._Layer;

    ChildSlot
    [
        SNew(SBox)
        .HeightOverride(FOptionalSize(SOdysseyAnimationTimelineLightTable::mDesiredHeight))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.f, 2.f, 0.f, 2.f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("lighttable.timeline-header.name", "LightTable"))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SComboButton)
                    .ComboButtonStyle(&FOdysseyStyle::GetWidgetStyle<FComboButtonStyle>("Animation.LightTable.Options"))
                    .OnGetMenuContent(this, &SOdysseyAnimationTimelineLightTableHeader::OnOptionsGetMenuContent)
                    .HasDownArrow(false)
                ]
            ]
            + SVerticalBox::Slot()
            .Padding(0.f, 2.f, 0.f, 0.f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .Padding(0.f, 0.f, 2.f, 0.f)
                [
                    SAssignNew(mLightTablePreviousKeysColorBlockWidget, SColorBlock)
                    .CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
                    .Color(this, &SOdysseyAnimationTimelineLightTableHeader::GetLightTablePreviousKeysColor)
                    .UseSRGB(true)
                    .OnMouseButtonDown(this, &SOdysseyAnimationTimelineLightTableHeader::OnLightTablePreviousKeysColorMouseButtonDown)
                    .IgnoreAlpha(true)
                ]
                + SHorizontalBox::Slot()
                .Padding(2.f, 0.f, 0.f, 0.f)
                [
                    SAssignNew(mLightTableNextKeysColorBlockWidget, SColorBlock)
                    .CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
                    .Color(this, &SOdysseyAnimationTimelineLightTableHeader::GetLightTableNextKeysColor)
                    .UseSRGB(true)
                    .OnMouseButtonDown(this, &SOdysseyAnimationTimelineLightTableHeader::OnLightTableNextKeysColorMouseButtonDown)
                    .IgnoreAlpha(true)
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.f, 2.f, 0.f, 2.f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("lighttable.timeline-header.out-of-pegs.name", "Out Of Pegs"))
                ]
            ]
        ]
    ];
}

FLinearColor
SOdysseyAnimationTimelineLightTableHeader::GetLightTablePreviousKeysColor() const
{
    return mLayer->Lighttable.PreviousKeysColor;
}

FLinearColor
SOdysseyAnimationTimelineLightTableHeader::GetLightTableNextKeysColor() const
{
    return mLayer->Lighttable.NextKeysColor;
}

FReply
SOdysseyAnimationTimelineLightTableHeader::OnLightTablePreviousKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const
{
    FColorPickerArgs PickerArgs;
    {
        PickerArgs.bUseAlpha = false;
        PickerArgs.bOnlyRefreshOnMouseUp = false;
        PickerArgs.bOnlyRefreshOnOk = false;
        PickerArgs.InitialColor = GetLightTablePreviousKeysColor();
        PickerArgs.ParentWidget = mLightTablePreviousKeysColorBlockWidget;
        PickerArgs.bOpenAsMenu = true;
        PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda(
            [this](FLinearColor iColor)
            {
                FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
                lighttable.PreviousKeysColor = iColor;
                FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::Interactive);
            }
        );
        /** A delegate to be called when the color picker window closes. */
        PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateLambda(
            [this](const TSharedRef<SWindow>& iWindow)
            {
                FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), mLayer->Lighttable, EPropertyChangeType::ValueSet);
            }
        );

        PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateLambda(
            [this](FLinearColor iColor)
            {
                FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
                lighttable.PreviousKeysColor = iColor;
                FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::ValueSet);
            }
        );
    }

    OpenColorPicker(PickerArgs);

    return FReply::Handled();
}

FReply
SOdysseyAnimationTimelineLightTableHeader::OnLightTableNextKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const
{
    FColorPickerArgs PickerArgs;
    {
        PickerArgs.bUseAlpha = false;
        PickerArgs.bOnlyRefreshOnMouseUp = false;
        PickerArgs.bOnlyRefreshOnOk = false;
        PickerArgs.InitialColor = GetLightTableNextKeysColor();
        PickerArgs.ParentWidget = mLightTableNextKeysColorBlockWidget;
        PickerArgs.bOpenAsMenu = true;
        PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda(
            [this](FLinearColor iColor)
            {
                FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
                lighttable.NextKeysColor = iColor;
                FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::Interactive);
            }
        );
        /** A delegate to be called when the color picker window closes. */
        PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateLambda(
            [this](const TSharedRef<SWindow>& iWindow)
            {
                FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), mLayer->Lighttable, EPropertyChangeType::ValueSet);
            }
        );

        PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateLambda(
            [this](FLinearColor iColor)
            {
                FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
                lighttable.NextKeysColor = iColor;
                FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::ValueSet);
            }
        );
    }

    OpenColorPicker(PickerArgs);

    return FReply::Handled();
}

TSharedRef< SWidget >
SOdysseyAnimationTimelineLightTableHeader::OnOptionsGetMenuContent()
{
    FMenuBuilder menuBuilder(true, nullptr);
    menuBuilder.BeginSection("Display Position", LOCTEXT("lighttable.timeline-header.options-menu.display-position-section.name", "Display Position"));
    {
        menuBuilder.AddMenuEntry(
            LOCTEXT("lighttable.timeline-header.options-menu.display-position-above-layer.name", "Above Layer")
            , LOCTEXT("lighttable.timeline-header.options-menu.display-position-above-layer.tooltip", "Displays the lightTable frames above Layer")
            , FSlateIcon("OdysseyStyle", "Animation.LightTable.Options.DisplayPosition.AboveLayer")
            , FUIAction(
                FExecuteAction::CreateRaw( this, &SOdysseyAnimationTimelineLightTableHeader::SetDisplayPosition, EOdysseyLightTableDisplayPosition::AboveLayer )
                , FCanExecuteAction::CreateLambda([](){return true;})
                , FIsActionChecked::CreateRaw(this, &SOdysseyAnimationTimelineLightTableHeader::IsDisplayPositionSet, EOdysseyLightTableDisplayPosition::AboveLayer )
            )
            , NAME_None
            , EUserInterfaceActionType::RadioButton
        );

        menuBuilder.AddMenuEntry(
            LOCTEXT("lighttable.timeline-header.options-menu.display-position-under-layer.name", "Under Layer")
            , LOCTEXT("lighttable.timeline-header.options-menu.display-position-under-layer.tooltip", "Displays the lightTable frames under Layer")
            , FSlateIcon("OdysseyStyle", "Animation.LightTable.Options.DisplayPosition.UnderLayer")
            , FUIAction(
                FExecuteAction::CreateRaw( this, &SOdysseyAnimationTimelineLightTableHeader::SetDisplayPosition, EOdysseyLightTableDisplayPosition::UnderLayer )
                , FCanExecuteAction::CreateLambda([](){return true;})
                , FIsActionChecked::CreateRaw(this, &SOdysseyAnimationTimelineLightTableHeader::IsDisplayPositionSet, EOdysseyLightTableDisplayPosition::UnderLayer )
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
                        return (int)(mLayer->Lighttable.PreviousKeysContrast + 0.5f);
                    }
                )
                .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
                .AllowSpin(true)
                /*.LinearDeltaSensitivity(10)
                .Delta(1)*/
                .MinDesiredValueWidth(50)
                .Justification(ETextJustify::Type::Right)
                .OnValueChanged(this, &SOdysseyAnimationTimelineLightTableHeader::OnPreviousKeysContrastValueChanged)
                .OnValueCommitted(this, &SOdysseyAnimationTimelineLightTableHeader::OnPreviousKeysContrastValueCommitted)

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
                        return (int)(mLayer->Lighttable.NextKeysContrast + 0.5f);
                    }
                )
                .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
                .AllowSpin(true)
                .MinDesiredValueWidth(50)
                .Justification(ETextJustify::Type::Right)
                .OnValueChanged(this, &SOdysseyAnimationTimelineLightTableHeader::OnNextKeysContrastValueChanged)
                .OnValueCommitted(this, &SOdysseyAnimationTimelineLightTableHeader::OnNextKeysContrastValueCommitted)
            ]
            , LOCTEXT("lighttable.timeline-header.options-menu.next-keys-contrast.name", "Next Keys Contrast")
            , true
        );
    }
    menuBuilder.EndSection();

    return menuBuilder.MakeWidget();
}

void
SOdysseyAnimationTimelineLightTableHeader::OnPreviousKeysContrastValueCommitted(int iValue, ETextCommit::Type iType)
{
    FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
    lighttable.PreviousKeysContrast = iValue;
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::ValueSet);
}

void
SOdysseyAnimationTimelineLightTableHeader::OnPreviousKeysContrastValueChanged(int iValue)
{
    FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
    lighttable.PreviousKeysContrast = iValue;
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::Interactive);
}

void
SOdysseyAnimationTimelineLightTableHeader::OnNextKeysContrastValueCommitted(int iValue, ETextCommit::Type iType)
{
    FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
    lighttable.NextKeysContrast = iValue;
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::ValueSet);
}

void
SOdysseyAnimationTimelineLightTableHeader::OnNextKeysContrastValueChanged(int iValue)
{
    FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
    lighttable.NextKeysContrast = iValue;
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::Interactive);
}

void
SOdysseyAnimationTimelineLightTableHeader::SetDisplayPosition(EOdysseyLightTableDisplayPosition iPosition)
{
    FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
    lighttable.DisplayPosition = iPosition;
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable);
}

bool
SOdysseyAnimationTimelineLightTableHeader::IsDisplayPositionSet(EOdysseyLightTableDisplayPosition iPosition) const
{
    return mLayer->Lighttable.DisplayPosition == iPosition;
}

#undef LOCTEXT_NAMESPACE
