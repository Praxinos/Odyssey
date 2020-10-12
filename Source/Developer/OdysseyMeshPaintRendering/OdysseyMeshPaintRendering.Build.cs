// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyMeshPaintRendering : ModuleRules
{
    public OdysseyMeshPaintRendering(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "RenderCore",
                "Engine",
                "Projects",
                "RHI"
            }
        );
    }
}
