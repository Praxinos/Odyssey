// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"

class IOdysseyPainterEditorToolkit;

/**
 * Interface for texture editor modules.
 */
class IOdysseyPainterEditorModule
    : public IModuleInterface
    , public IHasMenuExtensibility
{
public:
    virtual TSharedRef<IOdysseyPainterEditorToolkit> CreateOdysseyPainterEditor( const EToolkitMode::Type iMode, const TSharedPtr< IToolkitHost >& iInitToolkitHost, UTexture2D* iTexture ) = 0;
};
