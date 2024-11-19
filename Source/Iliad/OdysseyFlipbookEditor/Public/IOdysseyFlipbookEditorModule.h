// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Toolkits/IToolkitHost.h"

class FOdysseyFlipbookEditorToolkit;
class UPaperFlipbook;

class IOdysseyFlipbookEditorModule
    : public IModuleInterface
{
public:
    virtual void CreateOdysseyFlipbookEditor( TArray<UPaperFlipbook*> iFlipbook ) = 0;
};
