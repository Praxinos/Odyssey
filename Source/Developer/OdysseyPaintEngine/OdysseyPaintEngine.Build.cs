// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using UnrealBuildTool;

public class OdysseyPaintEngine : ModuleRules
{
    public OdysseyPaintEngine(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
             new string[] {
                "Core",
                "CoreUObject",
                "InputCore",
                "Engine",
                "OdysseyBrush",
                "OdysseyImaging",
                "OdysseyLayer",
                "OdysseyStrokePipeline",
                "OdysseyMaths",
                "OdysseyTransaction",
                "MeshPaint"
             }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "ULIS",
                "ULISLoader",
             }
        );

    }
}
