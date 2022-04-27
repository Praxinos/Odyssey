// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FEposSequenceEditorCommands
    : public TCommands<FEposSequenceEditorCommands>
{
public:
    /** Default constructor. */
    FEposSequenceEditorCommands();

    /** Initialize commands */
    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> NewStoryboardWithSettings;
    TSharedPtr<FUICommandInfo> ToggleStoryboardViewportCommand;

    //---

    TSharedPtr<FUICommandInfo> StoryboardViewportAdd10Rotate;
    TSharedPtr<FUICommandInfo> StoryboardViewportSubstract10Rotate;
    TMap<int32, TSharedPtr<FUICommandInfo>> StoryboardViewportSetRotationX;

    //---

    TSharedPtr<FUICommandInfo> CreateCameraAtCurrentTime;
    TSharedPtr<FUICommandInfo> SnapCameraToViewportAtCurrentTime;
    TSharedPtr<FUICommandInfo> PilotCameraAtCurrentTime;
    TSharedPtr<FUICommandInfo> EjectCameraAtCurrentTime;
    TSharedPtr<FUICommandInfo> GotoPreviousCameraPosition;
    TSharedPtr<FUICommandInfo> GotoNextCameraPosition;

    TSharedPtr<FUICommandInfo> CreatePlaneAtCurrentTime;
    TSharedPtr<FUICommandInfo> DetachPlaneAtCurrentTime;

    TSharedPtr<FUICommandInfo> CreateDrawingAtCurrentTime;
    TSharedPtr<FUICommandInfo> GotoPreviousDrawing;
    TSharedPtr<FUICommandInfo> GotoNextDrawing;

    //---

    TSharedPtr<FUICommandInfo> OpenSequenceEditorSettings;
    TSharedPtr<FUICommandInfo> OpenTrackEditorSettings;
    TSharedPtr<FUICommandInfo> OpenNamingConventionEditorSettings;

    TSharedPtr<FUICommandInfo> GotoPraxinos;
    TSharedPtr<FUICommandInfo> GotoForum;
    TSharedPtr<FUICommandInfo> GotoDiscord;
    TSharedPtr<FUICommandInfo> GotoUserDocumentation;
    TSharedPtr<FUICommandInfo> GotoProjects;
    TSharedPtr<FUICommandInfo> OpenAboutWindow;
};

//---

class FEposSequenceEditorActionCallbacks
{
public:
    static void OpenSequenceEditorSettings();
    static void OpenTrackEditorSettings();
    static void OpenNamingConventionEditorSettings();

    static void GotoPraxinos();
    static void GotoForum();
    static void GotoDiscord();
    static void GotoUserDocumentation();
    static void GotoProjects();
    static void OpenAboutWindow();
};
