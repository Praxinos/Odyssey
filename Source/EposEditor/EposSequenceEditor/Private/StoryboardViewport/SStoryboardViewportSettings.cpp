// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "StoryboardViewport/SStoryboardViewportSettings.h"

#include "EditorStyleSet.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Styling/SlateTypes.h"
#include "Textures/SlateIcon.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SHeader.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Input/SCheckBox.h"

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

    menuBuilder.BeginSection( NAME_None, LOCTEXT( "storyboard-viewport-settings.section.display.label", "Display" ) );

    //

    auto ExecuteDisplaySequenceInfos = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplaySequenceInfos = !GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplaySequenceInfos;
        GetMutableDefault<UEposSequenceEditorSettings>()->SaveConfig();
    };

    auto IsDisplaySequenceInfos = [=]() -> bool
    {
        return GetDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplaySequenceInfos;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-sequence-infos-label", "Sequence Infos" ),
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
        GetMutableDefault<UEposSequenceEditorSettings>()->SaveConfig();
    };

    auto IsDisplayActorControls = [=]() -> bool
    {
        return GetDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayActorControls;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-actor-controls-label", "Actor Controls" ),
                            LOCTEXT( "storyboard-viewport-settings.display-actor-controls-tooltip", "Diplays the actor controls under the 3D scene." ),
                            FSlateIcon(),
                            FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayActorControls ),
                                        FCanExecuteAction(),
                                        FIsActionChecked::CreateLambda( IsDisplayActorControls ) ),
                            NAME_None,
                            EUserInterfaceActionType::ToggleButton );

    //

    auto ExecuteDisplayPlaybackTrack = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackTrack = !GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackTrack;
        GetMutableDefault<UEposSequenceEditorSettings>()->SaveConfig();
    };

    auto IsDisplayPlaybackTrack = [=]() -> bool
    {
        return GetDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackTrack;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-playback-track-label", "Playback Track" ),
                            LOCTEXT( "storyboard-viewport-settings.display-playback-track-tooltip", "Diplays the playback track under the 3D scene." ),
                            FSlateIcon(),
                            FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayPlaybackTrack ),
                                        FCanExecuteAction(),
                                        FIsActionChecked::CreateLambda( IsDisplayPlaybackTrack ) ),
                            NAME_None,
                            EUserInterfaceActionType::ToggleButton );

    //

    auto ExecuteDisplayPlaybackControls = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackControls = !GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackControls;
        GetMutableDefault<UEposSequenceEditorSettings>()->SaveConfig();
    };

    auto IsDisplayPlaybackControls = [=]() -> bool
    {
        return GetDefault<UEposSequenceEditorSettings>()->ViewportSettings.DisplayPlaybackControls;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-playback-controls-label", "Playback Controls" ),
                            LOCTEXT( "storyboard-viewport-settings.display-playback-controls-tooltip", "Diplays the playback controls under the 3D scene." ),
                            FSlateIcon(),
                            FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayPlaybackControls ),
                                        FCanExecuteAction(),
                                        FIsActionChecked::CreateLambda( IsDisplayPlaybackControls ) ),
                            NAME_None,
                            EUserInterfaceActionType::ToggleButton );

    auto ExecuteDisplayNoteInViewport = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteInViewport = !GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteInViewport;
        GetMutableDefault<UEposSequenceEditorSettings>()->SaveConfig();
    };

    auto IsDisplayNoteInViewport = [=]() -> bool
    {
        return GetDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteInViewport;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-in-viewport-label", "Notes in Viewport" ),
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
        GetMutableDefault<UEposSequenceEditorSettings>()->SaveConfig();
    };

    auto IsDisplayNoteAsOverlay = [=]() -> bool
    {
        return GetDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteAsOverlay;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-as-overlay-label", "Notes as Overlay" ),
                            LOCTEXT( "storyboard-viewport-settings.display-as-overlay-tooltip", "Display the notes at the current frame on the 3D scene." ),
                            FSlateIcon(),
                            FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayNoteAsOverlay ),
                                        FCanExecuteAction(),
                                        FIsActionChecked::CreateLambda( IsDisplayNoteAsOverlay ) ),
                            NAME_None,
                            EUserInterfaceActionType::ToggleButton );

    //---

    menuBuilder.EndSection();

    menuBuilder.BeginSection( NAME_None, LOCTEXT( "storyboard-viewport-settings.section.camera-bounds.label", "Camera" ) );

    //---

    auto ExecuteDisplayCameraBounds = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.bDisplayCameraBounds = !GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.bDisplayCameraBounds;
        GetMutableDefault<UEposSequenceEditorSettings>()->SaveConfig();
    };

    auto IsDisplayCameraBounds = [=]() -> bool
    {
        return GetDefault<UEposSequenceEditorSettings>()->ViewportSettings.bDisplayCameraBounds;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-camera-bounds-label", "Camera Bounds" ),
                            LOCTEXT( "storyboard-viewport-settings.display-camera-bounds-tooltip", "Display current camera bounds on the 3D scene." ),
                            FSlateIcon(),
                            FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayCameraBounds ),
                                        FCanExecuteAction(),
                                        FIsActionChecked::CreateLambda( IsDisplayCameraBounds ) ),
                            NAME_None,
                            EUserInterfaceActionType::ToggleButton );

    //-

    TSharedPtr<SBox> boxWidget = SNew(SBox);

    boxWidget->SetContent(
        SNew(SColorBlock)
        .AlphaBackgroundBrush(FAppStyle::Get().GetBrush("ColorPicker.RoundedAlphaBackground"))
        .ShowBackgroundForAlpha(true)
        .AlphaDisplayMode(EColorBlockAlphaDisplayMode::Separate)
        .OnMouseButtonDown_Static(&SStoryboardViewportSettings::OnCameraBoundsColorBlockMouseButtonDown, boxWidget)
        .Size(FVector2D(70.0f, 20.0f))
        .CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
        .Color_Lambda(
            []()
            {
                return GetDefault<UEposSequenceEditorSettings>()->ViewportSettings.CameraBoundsShadeColor;
            }
        )
    );

    menuBuilder.AddWidget(
        boxWidget.ToSharedRef(),
        LOCTEXT( "storyboard-viewport-settings.camera-bounds-color-label", "Camera Bounds" ),
        false,
        true,
        LOCTEXT( "storyboard-viewport-settings.camera-bounds-color-tooltip", "Modify the color of the current camera bounds on the 3D scene." )
    );

    //-

    auto ExecuteDisplayPilotingCameraHUD = [=]()
    {
        GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.bDisplayPilotingCameraHUD = !GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.bDisplayPilotingCameraHUD;
        GetMutableDefault<UEposSequenceEditorSettings>()->SaveConfig();
    };

    auto IsDisplayPilotingCameraHUD = [=]() -> bool
    {
        return GetDefault<UEposSequenceEditorSettings>()->ViewportSettings.bDisplayPilotingCameraHUD;
    };

    menuBuilder.AddMenuEntry( LOCTEXT( "storyboard-viewport-settings.display-camera-piloting-label", "Camera Piloting" ),
                            LOCTEXT( "storyboard-viewport-settings.display-camera-piloting-tooltip", "Display camera piloting indication." ),
                            FSlateIcon(),
                            FUIAction( FExecuteAction::CreateLambda( ExecuteDisplayPilotingCameraHUD ),
                                        FCanExecuteAction(),
                                        FIsActionChecked::CreateLambda( IsDisplayPilotingCameraHUD ) ),
                            NAME_None,
                            EUserInterfaceActionType::ToggleButton );

    //---

    menuBuilder.EndSection();

    return menuBuilder.MakeWidget();
}

FReply
SStoryboardViewportSettings::OnCameraBoundsColorBlockMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, TSharedPtr<SBox> iWidget)
{
    if (MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
    {
        return FReply::Unhandled();
    }

    FColorPickerArgs PickerArgs;
    {
        PickerArgs.bUseAlpha = true;
        PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda(
            [](FLinearColor iColor)
            {
                GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.CameraBoundsShadeColor = iColor;
                GetMutableDefault<UEposSequenceEditorSettings>()->SaveConfig();
            }
        );
        PickerArgs.ParentWidget = iWidget;
        PickerArgs.bOpenAsMenu = true;
        PickerArgs.InitialColor = GetMutableDefault<UEposSequenceEditorSettings>()->ViewportSettings.CameraBoundsShadeColor;
    }

    OpenColorPicker(PickerArgs);

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
