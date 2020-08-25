// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
    /** If enabled, render in real time the 3D props on which the Flipbook is used */
    TSharedPtr<FUICommandInfo> Render3DInRealTime;
    /** Opens the UPaperSprite default editor */
    TSharedPtr<FUICommandInfo> OpenUPaperSpriteEditor;
    TSharedPtr<FUICommandInfo> AddNewKeyFrame;
	TSharedPtr<FUICommandInfo> AddNewKeyFrameBefore;
	TSharedPtr<FUICommandInfo> AddNewKeyFrameAfter;

	TSharedPtr<FUICommandInfo> PickNewSpriteForKeyFrame;
	TSharedPtr<FUICommandInfo> EditSpriteForKeyFrame;
	TSharedPtr<FUICommandInfo> ShowSpriteInContentBrowser;
};
