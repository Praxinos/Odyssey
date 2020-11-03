// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
