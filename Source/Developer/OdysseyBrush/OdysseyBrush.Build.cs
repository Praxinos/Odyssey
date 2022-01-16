// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using UnrealBuildTool;

public class OdysseyBrush : ModuleRules
{
    public OdysseyBrush(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "InputCore",
                "Engine",
                "OdysseyStrokePipeline",
                "OdysseyImaging",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "OdysseyStrokePipeline",
                "OdysseyImaging",
                "ULIS",
                "ULISLoader",
            }
        );
    }
}
