// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyTransaction : ModuleRules
{
    public OdysseyTransaction(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "Engine"
            }
        );
    }
}
