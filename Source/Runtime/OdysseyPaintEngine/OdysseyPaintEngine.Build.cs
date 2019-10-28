// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyPaintEngine : ModuleRules
{
    public OdysseyPaintEngine(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(
             new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "OdysseyBrush",
                "OdysseyImaging",
                "OdysseyLayer",
                "OdysseyStrokePipeline",
                "OdysseyMaths",
                "OdysseyTransaction"
             }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "ULIS",
             }
        );

    }
}
