// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyBrush : ModuleRules
{
    public OdysseyBrush(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "OdysseyCore",
                "Engine",
                "OdysseyLayer",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "OdysseyStrokePipeline",
                "OdysseyImaging",
                "ULIS",
            }
        );
    }
}
