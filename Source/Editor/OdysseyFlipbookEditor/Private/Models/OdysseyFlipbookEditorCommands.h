// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

/**
 * Holds the UI commands for the OdysseyFlipbookEditorToolkit widget.
 */
class FOdysseyFlipbookEditorCommands
    : public TCommands<FOdysseyFlipbookEditorCommands>
{
public:
    /**
     * Default constructor.
     */
    FOdysseyFlipbookEditorCommands();

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

	//TSharedPtr<FUICommandInfo> PickNewSpriteForKeyFrame;
	//TSharedPtr<FUICommandInfo> EditSpriteForKeyFrame;
	TSharedPtr<FUICommandInfo> ShowSpriteInContentBrowser;
};
