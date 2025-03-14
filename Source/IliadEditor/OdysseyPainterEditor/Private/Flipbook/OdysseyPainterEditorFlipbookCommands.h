// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

class FOdysseyPainterEditorFlipbookCommands
    : public TCommands<FOdysseyPainterEditorFlipbookCommands>
{
public:
    /**
     * Default constructor.
     */
    FOdysseyPainterEditorFlipbookCommands();

public:
    // TCommands interface
    virtual void RegisterCommands() override;

public:
    /** Opens the UPaperSprite default editor */
    TSharedPtr<FUICommandInfo> AddNewKeyFrame;
    TSharedPtr<FUICommandInfo> AddNewKeyFrameBefore;
    TSharedPtr<FUICommandInfo> AddNewKeyFrameAfter;

    TSharedPtr<FUICommandInfo> PlayForward;
    TSharedPtr<FUICommandInfo> PlayBackward;
    TSharedPtr<FUICommandInfo> Pause;
    TSharedPtr<FUICommandInfo> Stop;
    TSharedPtr<FUICommandInfo> PreviousFrame;
    TSharedPtr<FUICommandInfo> NextFrame;
    TSharedPtr<FUICommandInfo> PreviousKeyFrame;
    TSharedPtr<FUICommandInfo> NextKeyFrame;
    TSharedPtr<FUICommandInfo> FirstFrame;
    TSharedPtr<FUICommandInfo> LastFrame;
    TSharedPtr<FUICommandInfo> ToggleLooping;

    TSharedPtr<FUICommandInfo> ShowSpriteInContentBrowser;
};
