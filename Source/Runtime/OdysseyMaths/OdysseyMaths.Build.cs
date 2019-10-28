// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyMaths : ModuleRules
{
    public OdysseyMaths(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "OdysseyCore",
                "Engine"
            }
        );
    }
}
