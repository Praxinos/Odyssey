// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

using UnrealBuildTool;

public class OdysseyLayer : ModuleRules
{
    public OdysseyLayer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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
