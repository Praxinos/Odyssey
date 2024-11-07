// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Toolkits/IToolkitHost.h"

class FOdysseyAnimationEditorToolkit;
class UOdysseyAnimation;

class IOdysseyAnimationEditorModule
    : public IModuleInterface
{
public:
    virtual TSharedRef<FOdysseyAnimationEditorToolkit> CreateOdysseyAnimationEditor( UOdysseyAnimation* iAnimation ) = 0;
};
