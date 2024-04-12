// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "StoryboardViewport/SStoryboardViewportSettings.h"
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
SStoryboardViewportSettings::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SComboButton)
        .ButtonStyle(FAppStyle::Get(), "EditorViewportToolBar.Button")
        .ForegroundColor(FSlateColor::UseStyle())
        .OnGetMenuContent_Static(&SStoryboardViewportSettings::GetMenuContent)
        .ToolTipText(LOCTEXT("storyboard-viewport-settings-tooltip", "Storyboard Viewport Settings."))
        .HasDownArrow(false)
        .ButtonContent()
        [
            SNew(SBox)
            .WidthOverride(16)
            .HeightOverride(16)
            [
                SNew(SImage)
                .Image( FEposSequenceEditorStyle::Get().GetBrush( "Viewport.Settings" ) )
                .ColorAndOpacity(FSlateColor::UseForeground())
            ]
        ]
    ];
}

TSharedRef<SWidget>
SStoryboardViewportSettings::GetMenuContent()
{
    FMenuBuilder menuBuilder( true, nullptr );

    //

    auto ExecuteOverlayToolbar = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.OverlayToolbar = !GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.OverlayToolbar;
    };

    auto IsOverlayToolbar = [=]() -> bool
    {
        return GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.OverlayToolbar;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.overlay-toolbar-label", "Overlay toolbar" ),
                            LOCTEXT( "storyboard-viewport-settings.overlay-toolbar-tooltip", "Diplays the toolbar on top of the 3D scene." ),
                            FSlateIcon(),
                            FUIAction( FExecuteAction::CreateLambda( ExecuteOverlayToolbar ),
                                        FCanExecuteAction(),
                                        FIsActionChecked::CreateLambda( IsOverlayToolbar ) ),
                            NAME_None,
                            EUserInterfaceActionType::ToggleButton );

    //

    auto ExecuteDisplayPlaybackTrack = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackTrack = !GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackTrack;
    };

    auto IsDisplayPlaybackTrack = [=]() -> bool
    {
        return GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackTrack;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-playback-track-label", "Display playback track" ),
                            LOCTEXT( "storyboard-viewport-settings.display-playback-track-tooltip", "Diplays the playback track under the 3D scene." ),
                            FSlateIcon(),
                            FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayPlaybackTrack ),
                                        FCanExecuteAction(),
                                        FIsActionChecked::CreateLambda( IsDisplayPlaybackTrack ) ),
                            NAME_None,
                            EUserInterfaceActionType::ToggleButton );

    //

    auto ExecuteDisplaySequenceInfos = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplaySequenceInfos = !GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplaySequenceInfos;
    };

    auto IsDisplaySequenceInfos = [=]() -> bool
    {
        return GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplaySequenceInfos;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-sequence-infos-label", "Display sequence infos" ),
                            LOCTEXT( "storyboard-viewport-settings.display-sequence-infos-tooltip", "Diplays the sequence infos under the 3D scene." ),
                            FSlateIcon(),
                            FUIAction( FExecuteAction::CreateLambda( ExecuteDisplaySequenceInfos ),
                                        FCanExecuteAction(),
                                        FIsActionChecked::CreateLambda( IsDisplaySequenceInfos ) ),
                            NAME_None,
                            EUserInterfaceActionType::ToggleButton );

    //

    auto ExecuteDisplayActorControls = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayActorControls = !GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayActorControls;
    };

    auto IsDisplayActorControls = [=]() -> bool
    {
        return GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayActorControls;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-actor-controls-label", "Display actor controls" ),
                            LOCTEXT( "storyboard-viewport-settings.display-actor-controls-tooltip", "Diplays the actor controls under the 3D scene." ),
                            FSlateIcon(),
                            FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayActorControls ),
                                        FCanExecuteAction(),
                                        FIsActionChecked::CreateLambda( IsDisplayActorControls ) ),
                            NAME_None,
                            EUserInterfaceActionType::ToggleButton );

    //

    auto ExecuteDisplayPlaybackControls = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackControls = !GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackControls;
    };

    auto IsDisplayPlaybackControls = [=]() -> bool
    {
        return GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackControls;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-playback-controls-label", "Display playback controls" ),
                            LOCTEXT( "storyboard-viewport-settings.display-playback-controls-tooltip", "Diplays the playback controls under the 3D scene." ),
                            FSlateIcon(),
                            FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayPlaybackControls ),
                                        FCanExecuteAction(),
                                        FIsActionChecked::CreateLambda( IsDisplayPlaybackControls ) ),
                            NAME_None,
                            EUserInterfaceActionType::ToggleButton );

    //menuBuilder.BeginSection( TEXT("NoteSettings"), LOCTEXT( "storyboard-viewport-settings.note-settings-section.label", "Note Settings" ) );
    {
        auto ExecuteDisplayNoteInViewport = [=]()
        {
            GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteInViewport = !GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteInViewport;
        };

        auto IsDisplayNoteInViewport = [=]() -> bool
        {
            return GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteInViewport;
        };

        menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-in-viewport-label", "Display notes in viewport" ),
                                LOCTEXT( "storyboard-viewport-settings.display-in-viewport-tooltip", "Display the notes at the current frame under the 3D scene." ),
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

        menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-as-overlay-label", "Display notes as overlay" ),
                                LOCTEXT( "storyboard-viewport-settings.display-as-overlay-tooltip", "Display the notes at the current frame on the 3D scene." ),
                                FSlateIcon(),
                                FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayNoteAsOverlay ),
                                            FCanExecuteAction(),
                                            FIsActionChecked::CreateLambda( IsDisplayNoteAsOverlay ) ),
                                NAME_None,
                                EUserInterfaceActionType::ToggleButton );
    }
    //menuBuilder.EndSection();

    return menuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
