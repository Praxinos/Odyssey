// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyStrokePipeline : ModuleRules
{
    public OdysseyStrokePipeline(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(
             new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "OdysseyMaths",
             }
        );
    }
}
