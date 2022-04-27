// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "StoryboardViewport/SNoteSettings.h"
#include "Styling/SlateTypes.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SHeader.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"

#include "Settings/EposSequenceEditorSettings.h"
#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorNoteSettings"

void
SNoteSettings::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SComboButton)
        .ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
        .ForegroundColor(FAppStyle::Get().GetColor("InvertedForeground"))
        .OnGetMenuContent(this, &SNoteSettings::GetMenuContent)
        .ButtonContent()
        [
            SNew(SBox)
            .WidthOverride(24)
            .HeightOverride(24)
            .ToolTipText(LOCTEXT("note-settings-tooltip", "Notes Settings."))
            [
                SNew(SImage)
                .Image( FEposSequenceEditorStyle::Get().GetBrush( "Notes.Viewport.Settings" ) )
            ]
        ]
    ];
}

TSharedRef<SWidget>
SNoteSettings::GetMenuContent()
{
    FMenuBuilder menuBuilder( true, nullptr );

    auto ExecuteDisplayNoteInViewport = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteInViewport = !GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteInViewport;
    };

    auto IsDisplayNoteInViewport = [=]() -> bool
    {
        return GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteInViewport;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "note-settings.display-in-viewport-label", "Display notes in viewport" ),
                              LOCTEXT( "note-settings.display-in-viewport-tooltip", "Display the notes at the current frame under the 3D scene." ),
                              FSlateIcon(),
                              FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayNoteInViewport ),
                                         FCanExecuteAction(),
                                         FIsActionChecked::CreateLambda( IsDisplayNoteInViewport ) ),
                              NAME_None,
                              EUserInterfaceActionType::ToggleButton );

    //

    auto ExecuteDisplayNoteAsOverlay = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteAsOverlay = !GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteAsOverlay;
    };

    auto IsDisplayNoteAsOverlay = [=]() -> bool
    {
        return GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteAsOverlay;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "note-settings.display-as-overlay-label", "Display notes as overlay" ),
                              LOCTEXT( "note-settings.display-as-overlay-tooltip", "Display the notes at the current frame on the 3D scene." ),
                              FSlateIcon(),
                              FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayNoteAsOverlay ),
                                         FCanExecuteAction(),
                                         FIsActionChecked::CreateLambda( IsDisplayNoteAsOverlay ) ),
                              NAME_None,
                              EUserInterfaceActionType::ToggleButton );

    return menuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
