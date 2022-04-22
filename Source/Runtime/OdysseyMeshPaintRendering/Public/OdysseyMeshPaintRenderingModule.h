// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/**
 * FOdysseyMeshPaintRenderingModule module interface
 */
class FOdysseyMeshPaintRenderingModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

