// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/SOdysseyPaletteSetComboBox.h"

#include "Dialogs/Dialogs.h"
#include "ScopedTransaction.h"
#include "Widgets/Input/SEditableTextBox.h"

#include "OdysseyPalette.h"
#include "UObject/OdysseyObjectEditorUtils.h"

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
    , mCurrentSetAttribute(*this, FGuid())
    , mCurrentSet(FGuid())
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
    mIsReadOnly = InArgs._IsReadOnly;

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

void
SOdysseyPaletteSetComboBox::BuildMenu(FMenuBuilder& iMenuBuilder, UOdysseyPalette* iPalette, FGuid iCurrentSet, bool iIsReadOnly, FOnCurrentSetSelected iOnCurrentSetSelected)
{
    if (!iIsReadOnly)
    {
        iMenuBuilder.BeginSection("Actions", LOCTEXT("palette-set-combobox.section.actions", "Actions"));
            FMenuEntryParams addSetParams;
            addSetParams.LabelOverride = LOCTEXT("palette-set-combobox.add-set", "Add Set");
            addSetParams.UserInterfaceActionType = EUserInterfaceActionType::Button;
            addSetParams.DirectActions.ExecuteAction = FExecuteAction::CreateLambda(
                [iOnCurrentSetSelected, iPalette, iCurrentSet]()
                {
                    FText setName = LOCTEXT("palette-set-combobox.add-set.dialog.default-name", "New Set");
                    SGenericDialogWidget::OpenDialog(
                        LOCTEXT("palette-set-combobox.add-set.dialog.title", "Add Set"),
                        SNew(SEditableTextBox)
                        .Text_Lambda(
                            [&setName]()
                            {
                                return setName;
                            }
                        )
                        .OnTextCommitted_Lambda(
                            [&setName](const FText& iText, ETextCommit::Type iCommitType)
                            {
                                setName = iText;
                            }
                        ),
                        SGenericDialogWidget::FArguments()
                        .OnOkPressed_Lambda(
                            [iPalette, iCurrentSet, &setName, iOnCurrentSetSelected]()
                            {
                                const FScopedTransaction transaction(LOCTEXT("palette-set-combobox.add-set.transaction", "Add Palette Set"));
                                FOdysseyObjectEditorUtils::PreChangePropertyValue(iPalette, "SetsIDs");
                                FGuid newId = iPalette->DuplicateSet(iCurrentSet, FName(*setName.ToString()));
                                iOnCurrentSetSelected.ExecuteIfBound(newId);
                                FOdysseyObjectEditorUtils::PostChangePropertyValue(iPalette, "SetsIDs", EPropertyChangeType::ArrayAdd);
                            }
                        ),
                        true
                    );
                }
            );

            iMenuBuilder.AddMenuEntry(addSetParams);

            FMenuEntryParams removeSetParams;
            removeSetParams.LabelOverride = LOCTEXT("palette-set-combobox.remove-set", "Remove Current Set");
            removeSetParams.UserInterfaceActionType = EUserInterfaceActionType::Button;
            removeSetParams.DirectActions.ExecuteAction = FExecuteAction::CreateLambda(
                [iOnCurrentSetSelected, iPalette, iCurrentSet]()
                {
                    const FScopedTransaction transaction(LOCTEXT("palette-set-combobox.remove-set.transaction", "Remove Palette Set"));
                    FOdysseyObjectEditorUtils::PreChangePropertyValue(iPalette, "SetsIDs");
                    iPalette->RemoveSet(iCurrentSet);
                    iOnCurrentSetSelected.ExecuteIfBound(iPalette->GetDefaultSetID());
                    FOdysseyObjectEditorUtils::PostChangePropertyValue(iPalette, "SetsIDs", EPropertyChangeType::ArrayRemove);
                }
            );
            removeSetParams.DirectActions.CanExecuteAction = FCanExecuteAction::CreateLambda(
                [iPalette]()
                {
                    return iPalette->GetSetsIDs().Num() > 1;
                }
            );

            iMenuBuilder.AddMenuEntry(removeSetParams);

            FMenuEntryParams renameSetParams;
            renameSetParams.LabelOverride = LOCTEXT("palette-set-combobox.rename-set", "Rename Current Set");
            renameSetParams.UserInterfaceActionType = EUserInterfaceActionType::Button;
            renameSetParams.DirectActions.ExecuteAction = FExecuteAction::CreateLambda(
                [iOnCurrentSetSelected, iPalette, iCurrentSet]()
                {
                    FText setName = FText::FromName(iPalette->GetSetsIDs()[iCurrentSet]);
                    SGenericDialogWidget::OpenDialog(
                        LOCTEXT("palette-set-combobox.rename-set.dialog.title", "Rename Current Set"),
                        SNew(SEditableTextBox)
                        .Text_Lambda(
                            [&setName]()
                            {
                                return setName;
                            }
                        )
                        .OnTextCommitted_Lambda(
                            [&setName](const FText& iText, ETextCommit::Type iCommitType)
                            {
                                setName = iText;
                            }
                        ),
                        SGenericDialogWidget::FArguments()
                        .OnOkPressed_Lambda(
                            [iPalette, iCurrentSet, &setName]()
                            {
                                const FScopedTransaction transaction(LOCTEXT("palette-set-combobox.rename-set.transaction", "Rename Palette Set"));
                                FOdysseyObjectEditorUtils::PreChangePropertyValue(iPalette, "SetsIDs");
                                iPalette->RenameSet(iCurrentSet, FName(*setName.ToString()));
                                FOdysseyObjectEditorUtils::PostChangePropertyValue(iPalette, "SetsIDs", EPropertyChangeType::ValueSet);
                            }
                        ),
                        true
                    );
                }
            );

            iMenuBuilder.AddMenuEntry(renameSetParams);
        iMenuBuilder.EndSection();
    }

    iMenuBuilder.BeginSection("Sets", LOCTEXT("palette-set-combobox.section.sets", "Sets"));
    for ( auto pair : iPalette->GetSetsIDs())
    {
        FName setName = pair.Value;

        FMenuEntryParams params;
        params.LabelOverride = FText::FromName(setName);
        params.UserInterfaceActionType = EUserInterfaceActionType::Check;
        params.DirectActions.ExecuteAction = FExecuteAction::CreateLambda(
            [pair, iOnCurrentSetSelected]()
            {
                iOnCurrentSetSelected.ExecuteIfBound(pair.Key);
            }
        );
        params.DirectActions.GetActionCheckState = FGetActionCheckState::CreateLambda(
            [pair, iCurrentSet]()
            {
                return iCurrentSet == pair.Key ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
            }
        );

        iMenuBuilder.AddMenuEntry(params);
    }
    iMenuBuilder.EndSection();
}

TSharedRef<SWidget>
SOdysseyPaletteSetComboBox::GetMenuContent()
{
    if (!mPalette)
        return SNullWidget::NullWidget;

    FMenuBuilder menuBuilder( true, nullptr );

    BuildMenu(menuBuilder, mPalette, mCurrentSet, mIsReadOnly, mOnCurrentSetSelected);

    return menuBuilder.MakeWidget();
}

FText
SOdysseyPaletteSetComboBox::GetCurrentSetName() const
{
    if (!mPalette)
        return FText::GetEmpty();

    if (!mPalette->GetSetsIDs().Contains(mCurrentSet))
        return FText::GetEmpty();

    const FName* currentSetName = mPalette->GetSetsIDs().Find(mCurrentSet);

    return FText::FromName(*currentSetName);
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
