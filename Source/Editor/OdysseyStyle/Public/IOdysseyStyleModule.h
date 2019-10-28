// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class IOdysseyStyleModule : public IModuleInterface
{
public:

    virtual TSharedRef< class FSlateStyleSet > CreateOdysseyStyleInstance() const = 0;
};
