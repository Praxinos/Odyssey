// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Interfaces/IOdysseyPainterEditorToolkit.h"
#include "Modules/ModuleInterface.h"

class UTexture;

/**
 * Interface for texture editor modules.
 */
class IOdysseyPainterEditorModule
    : public IModuleInterface
    , public IHasMenuExtensibility
{
public:
    virtual TSharedRef<IOdysseyPainterEditorToolkit> CreateOdysseyPainterEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UTexture2D* InTexture ) = 0;
};
