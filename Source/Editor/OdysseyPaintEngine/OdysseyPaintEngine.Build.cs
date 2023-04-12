// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
                "UnrealEd",
                "OdysseyCore",
                "OdysseyImaging",
                "OdysseyLayerStack",
                "OdysseyMaths",
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
