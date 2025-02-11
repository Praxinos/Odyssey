// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyPaletteSetComboBox.h"

#include "OdysseyPalette.h"

#define LOCTEXT_NAMESPACE "Palette"

SLATE_IMPLEMENT_WIDGET(SOdysseyPaletteSetComboBox)
void
SOdysseyPaletteSetComboBox::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mPaletteAttribute, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyPaletteSetComboBox&>(Widget).OnPaletteChanged();
        }
    ));

    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mCurrentSetAttribute, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyPaletteSetComboBox&>(Widget).OnCurrentSetChanged();
        }
    ));
}

SOdysseyPaletteSetComboBox::~SOdysseyPaletteSetComboBox()
{
}

SOdysseyPaletteSetComboBox::SOdysseyPaletteSetComboBox()
    : mPaletteAttribute(*this, nullptr)
    , mPalette(nullptr)
    , mCurrentSetAttribute(*this, 0)
    , mCurrentSet(0)
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void SOdysseyPaletteSetComboBox::Construct(const FArguments& InArgs)
{
    mPaletteAttribute.Assign(*this, InArgs._Palette);
    mPalette = mPaletteAttribute.Get();
    mCurrentSetAttribute.Assign(*this, InArgs._CurrentSet);
    mCurrentSet = mCurrentSetAttribute.Get();
    mOnCurrentSetSelected = InArgs._OnCurrentSetSelected;

    SComboButton::Construct(
        SComboButton::FArguments()
        .OnGetMenuContent(this, &SOdysseyPaletteSetComboBox::GetMenuContent)
        .ButtonContent()
        [
            SNew(STextBlock)
            .Text(this, &SOdysseyPaletteSetComboBox::GetCurrentSetName)
        ]
    );
}

TSharedRef<SWidget>
SOdysseyPaletteSetComboBox::GetMenuContent()
{
    if (!mPalette)
        return SNullWidget::NullWidget;

    FMenuBuilder menuBuilder( true, nullptr );

    for (int i = 0; i < mPalette->GetSets().Num(); i++)
    {
        FName set = mPalette->GetSets()[i];

        FMenuEntryParams params;
        params.LabelOverride = FText::FromName(set);
        params.UserInterfaceActionType = EUserInterfaceActionType::Button;
        params.DirectActions.ExecuteAction = FExecuteAction::CreateLambda(
            [this, i]()
            {
                mOnCurrentSetSelected.ExecuteIfBound(i);
            }
        );

        menuBuilder.AddMenuEntry(params);
    }

    return menuBuilder.MakeWidget();
}

FText
SOdysseyPaletteSetComboBox::GetCurrentSetName() const
{
    if (!mPalette)
        return FText::GetEmpty();

    if (mCurrentSet < 0 || mCurrentSet >= mPalette->GetSets().Num())
        return FText::GetEmpty();

    return FText::FromName(mPalette->GetSets()[mCurrentSet]);
}
void
SOdysseyPaletteSetComboBox::OnPaletteChanged()
{
    mPalette = mPaletteAttribute.Get();
}
void
SOdysseyPaletteSetComboBox::OnCurrentSetChanged()
{
    mCurrentSet = mCurrentSetAttribute.Get();
}
UOdysseyPalette*
SOdysseyPaletteSetComboBox::GetPalette() const
{
    return mPalette;
}

#undef LOCTEXT_NAMESPACE
