// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

/**
 * Implements the Layer stack widget
 */
class FOdysseyLayerStackCommands : public TCommands<FOdysseyLayerStackCommands>
{
public:
    FOdysseyLayerStackCommands() : TCommands<FOdysseyLayerStackCommands>
    (
        "LayerStack",
        NSLOCTEXT("Contexts", "LayerStack", "LayerStack"),
        NAME_None,
        FEditorStyle::GetStyleSetName() // Icon Style Set
    )
    {}

    /** Test option */
    TSharedPtr< FUICommandInfo > TestOption;

    TSharedPtr< FUICommandInfo > MergeDownLayer;

    TSharedPtr< FUICommandInfo > DeleteLayer;


    /**
     * Initialize commands
     */
    virtual void RegisterCommands() override;
};
