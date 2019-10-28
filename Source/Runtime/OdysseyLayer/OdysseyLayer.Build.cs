// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyLayer : ModuleRules
{
    public OdysseyLayer(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "Engine",
                "OdysseyImaging",
                "OdysseyTransaction",
                "ULIS"
            }
        );

    }
}
