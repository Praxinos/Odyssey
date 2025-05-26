// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/SOdysseyAnimationConfigureWindow.h"

#include "PropertyEditorModule.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SSeparator.h"
#include "SEnumCombo.h"

#include "IStructureDetailsView.h"

/////////////////////////////////////////////////////
// Defines
#define LOCTEXT_NAMESPACE "Animation"

#define MAX_CANVAS_SIZE 8192
#define MIN_CANVAS_SIZE 1

//---

void
SOdysseyAnimationConfigureWindow::Construct( const FArguments& iArgs)
{
    mWindowAnswer = false;

    FStructureDetailsViewArgs structureDetailsViewArgs;
    FDetailsViewArgs detailsViewArgs;
    detailsViewArgs.bAllowSearch = false;
    detailsViewArgs.bShowScrollBar = false;

    FPropertyEditorModule& propertyEditor = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
    TSharedRef<FStructOnScope> structOnScope = MakeShared<FStructOnScope>( FOdysseyAnimationConfiguration::StaticStruct(), reinterpret_cast<uint8*>(&mConfiguration ));
    TSharedPtr<IStructureDetailsView> configurationDetailsView = propertyEditor.CreateStructureDetailView(detailsViewArgs, structureDetailsViewArgs, structOnScope);

    //---

    SWindow::Construct( SWindow::FArguments()
        .Title( LOCTEXT( "configure-window.title", "Create Animation Asset" ) )
        // Remove the empty space at the bottom but doesn't scale anymore
        //.SizingRule( ESizingRule::FixedSize )
        //.ClientSize( FVector2D( 300, 180 ) )
        .SizingRule( ESizingRule::Autosized )
        .SupportsMinimize( false )
        .SupportsMaximize( false )
        [
            SNew(SVerticalBox)
            +SVerticalBox::Slot()
            [
                configurationDetailsView->GetWidget().ToSharedRef()
            ]
            +SVerticalBox::Slot()
            .AutoHeight()
            .HAlign( HAlign_Center )
            .VAlign( VAlign_Center )
            [
                SNew( SUniformGridPanel )
                .MinDesiredSlotHeight( FCoreStyle::Get().GetFloat( "StandardDialog.MinDesiredSlotHeight" ) )
                .MinDesiredSlotWidth( FCoreStyle::Get().GetFloat( "StandardDialog.MinDesiredSlotWidth" ) )
                .SlotPadding( FCoreStyle::Get().GetMargin( "StandardDialog.SlotPadding" ) )

                +SUniformGridPanel::Slot( 0, 0 )
                [
                    SNew( SButton )
                    .ContentPadding( FCoreStyle::Get().GetMargin( "StandardDialog.ContentPadding" ) )
                    .HAlign( HAlign_Center )
                    .Text( LOCTEXT( "configure-window.create-asset", "Create Asset" ) )
                    .OnClicked_Raw( this, &SOdysseyAnimationConfigureWindow::OnAccept )
                ]

                +SUniformGridPanel::Slot( 1, 0 )
                [
                    SNew( SButton )
                    .ContentPadding( FCoreStyle::Get().GetMargin( "StandardDialog.ContentPadding" ) )
                    .HAlign( HAlign_Center )
                    .Text( LOCTEXT( "configure-window.cancel", "Cancel" ) )
                    .OnClicked_Raw( this, &SOdysseyAnimationConfigureWindow::OnCancel )
                ]
            ]
        ]
    );
}

//---

bool
SOdysseyAnimationConfigureWindow::GetWindowAnswer()
{
    return mWindowAnswer;
}

const FOdysseyAnimationConfiguration&
SOdysseyAnimationConfigureWindow::GetConfiguration() const
{
    return mConfiguration;
}

//---

FReply
SOdysseyAnimationConfigureWindow::OnAccept()
{
    mWindowAnswer = true;
    RequestDestroyWindow();

    return FReply::Handled();
}

FReply
SOdysseyAnimationConfigureWindow::OnCancel()
{
    mWindowAnswer = false;
    RequestDestroyWindow();

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
