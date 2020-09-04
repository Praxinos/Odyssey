// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

using UnrealBuildTool;

public class OdysseyDrawingState : ModuleRules
{
    public OdysseyDrawingState(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                // "OdysseyStrokePipeline",
                "OdysseyImaging",
                "OdysseyBrush",
                "OdysseyLayer",
                "ULIS",
                "ULISLoader",
            }
        );
    }
}
