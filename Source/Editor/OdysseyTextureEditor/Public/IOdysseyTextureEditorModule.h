// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Toolkits/IToolkitHost.h"

class FOdysseyTextureEditorToolkit;

class IOdysseyTextureEditorModule
	: public IModuleInterface
	, public IHasMenuExtensibility
{
public:
    virtual TSharedRef<FOdysseyTextureEditorToolkit> CreateOdysseyTextureEditor( const EToolkitMode::Type iMode, const TSharedPtr< IToolkitHost >& iInitToolkitHost, UTexture2D* iTexture ) = 0;
};
