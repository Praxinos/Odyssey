// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Toolkits/IToolkitHost.h"

class FOdysseyFlipbookEditorToolkit;

class IOdysseyFlipbookEditorModule
	: public IModuleInterface
{
public:
    virtual TSharedRef<FOdysseyFlipbookEditorToolkit> CreateOdysseyFlipbookEditor( UPaperFlipbook* iFlipbook ) = 0;
};
