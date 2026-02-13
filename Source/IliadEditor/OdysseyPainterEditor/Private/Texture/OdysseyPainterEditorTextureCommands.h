// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

class FOdysseyPainterEditorTextureCommands
    : public TCommands<FOdysseyPainterEditorTextureCommands>
{
public:
    /**
     * Default constructor.
     */
    FOdysseyPainterEditorTextureCommands();

public:
    // TCommands interface
    virtual void RegisterCommands() override;

public:
    //Import Export

    /** Action used to export Layers as Textures */
    TSharedPtr<FUICommandInfo> ExportLayersAsTextures;

    /** Action used to export Layers as Images */
    TSharedPtr<FUICommandInfo> ExportLayersAsImages;

    /** Action used to import Textures as Layers */
    TSharedPtr<FUICommandInfo> ImportTextures;

    /** Action used to import Images as Layers */
    TSharedPtr<FUICommandInfo> ImportImages;
};
